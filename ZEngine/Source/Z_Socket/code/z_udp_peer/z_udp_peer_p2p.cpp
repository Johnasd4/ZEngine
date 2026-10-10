/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_udp_peer.h"

#include "z_kcp_session.h"

namespace zengine {
namespace socket {

ZUDPPeerP2P::ZUDPPeerP2P(
    ZIOContext& _io_context,
    TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _receive_message_data_handle_func
) noexcept
    : SuperType_()
    , state_(StateEnum_::kClosed)
    , socket_(_io_context)
    , kcp_session_()
    , keep_alive_interval_ms_(kDefaultKeepAliveIntervalMs)
    , output_thread_()
    , output_thread_cv_()
    , output_thread_mutex_()
    , input_thread_()
    , input_thread_cv_()
    , input_thread_mutex_()
    , message_data_handle_thread_()
    , message_data_handle_thread_cv_()
    , message_data_handle_thread_mutex_()
    , receive_packet_buffer_()
    , receive_message_data_handle_func_(std::forward<TFunction<Void(ZUDPPeerP2P&, ZMemory&)>>(_receive_message_data_handle_func))
    , last_message_receive_time_ms_(0U)
    , disconnect_time_ms_(kDefaultDisconnectTimeMs)
    , output_message_queue_(kOutputMessageQueueSize)
    , input_message_data_queue_(kInputMessageQueueSize)
{}

ZUDPPeerP2P::~ZUDPPeerP2P() noexcept {
    Close();
}

NODISCARD ReturnType ZUDPPeerP2P::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Peer state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kClosed)
    );

    link_code = socket_.Open(_ip_type);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::Open() link error!"
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZUDPPeerP2P::BindEndpoint(const ZUDPEndpoint& _endpoint) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Peer state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    link_code = socket_.BindEndpoint(_endpoint);
    if (link_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::BindEndpoint() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPPeerP2P::Connect(
    const ZUDPEndpoint& _endpoint,
    UInt32 _conv_id,
    ModeEnum_ _mode,
    TimeType _max_connect_time_ms,
    TimeType _retry_interval_ms
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Peer state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    state_ = StateEnum_::kConnecting;

    // Bind the target endpoint to the socket. Only receive messages from this endpoint after binding.
    link_code = socket_.Connect(_endpoint);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::Connect() link error!"
        );
        return ret_val;
    }

    // Start kcp with the given conversation ID.
    kcp_session_.Open(_conv_id, this, KCPOutputCallbackP);
    // Set the maximum number of dead link times to disable the kcp disconnect failure.
    kcp_session_.SetDeadLinkTimes(kUInt32Max);

    switch (_mode) {
        // Default/Conservative mode: TCP-like behavior, bandwidth-friendly.
        case ModeEnum_::kNormal:
            // Set Delay mode with the specified parameters for balanced performance.
            kcp_session_.SetNoDelay(0, 40, 0, 0);
            // Smaller window size suitable for low-throughput or background sync (default KCP is 32)
            kcp_session_.SetWindowSize(64, 64);
            // Safe MTU to avoid IP fragmentation
            kcp_session_.SetMaxTransmissionUnitSize(ZKCPSession::kMaxPacketSize);
            // Message mode (packet-oriented, preserves message boundaries)
            kcp_session_.SetStreamMode(0);
            // Backlog threshold: safe buffer limit
            kcp_session_.SetMaxPendingPacketNum(128);
            break;
        // Balanced mode: Reasonable latency without excessive redundant retransmissions.
        case ModeEnum_::kBalance:
            // Set Delay mode with the specified parameters for balanced performance.
            kcp_session_.SetNoDelay(0, 20, 2, 1);
            // Medium window capacity
            kcp_session_.SetWindowSize(128, 128);
            // Safe MTU to avoid IP fragmentation
            kcp_session_.SetMaxTransmissionUnitSize(ZKCPSession::kMaxPacketSize);
            // Message mode (packet-oriented, preserves message boundaries)
            kcp_session_.SetStreamMode(0);
            // Backlog threshold
            kcp_session_.SetMaxPendingPacketNum(256);
            break;
        // Fast mode: Aggressive retransmission and low-latency settings for real-time applications.
        case ModeEnum_::kFast:
            // Set KCP to nodelay mode with the specified parameters for low-latency communication.
            kcp_session_.SetNoDelay(1, 10, 2, 1);
            // Set the KCP window size to allow for a maximum of 256 packets in both send and receive windows.
            kcp_session_.SetWindowSize(256, 256);
            // Set the KCP maximum transmission unit (MTU) to 1400 bytes to avoid fragmentation.
            kcp_session_.SetMaxTransmissionUnitSize(ZKCPSession::kMaxPacketSize);
            // Set KCP to message mode (stream mode off) for reliable message delivery.
            kcp_session_.SetStreamMode(0);
            // Backlog threshold: generous limit for burst traffic
            kcp_session_.SetMaxPendingPacketNum(1024);
            break;
        default:
            Z_LOG_ERROR(
                error_code::kSocketErrorCode_ParamOutOfRange, 0,
                "ZUDPPeerP2P::Connect() mode out of range! mode: {}",
                static_cast<Int32>(_mode)
            );
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_ParamOutOfRange;
            return ret_val;
    }

    // Start the hole punching process to establish a P2P connection with the target endpoint.
    link_code = PuchHoleP(_endpoint, _conv_id, _max_connect_time_ms, _retry_interval_ms);
    if (link_code != kOK) {
        // Close the KCP session if the hole punching process fails to establish a connection.
        kcp_session_.Close();
        

        // If the hole punching process times out, transition back to the opened state and close the KCP session.
        if (link_code == error_code::kZUDPPeerErrorCode_P2PConnectTimeout) {
            state_ = StateEnum_::kOpened;   
            // Return timeout error code to indicate that the connection attempt timed out.
            ret_val = error_code::kZUDPPeerErrorCode_P2PConnectTimeout;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::PuchHoleP() link error!"
            );
        }
        // If any other error occurs during the hole punching process, 
        // the state is already set to error, close the KCP session. Log the error and return the link code.
        else {           
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPPeerP2P::PuchHoleP() link error!"
            );
        }

        return ret_val;
    }

    // Update the last message receive time to the current time after a successful connection.
    last_message_receive_time_ms_ = ZKCPSession::GetCurrentTimeMs();

    // Start the command thread and receive thread to handle commands in the background.
    output_thread_ = ZThread(OutputThreadFuncP, this);
    input_thread_ = ZThread(InputThreadFuncP, this);
    message_data_handle_thread_ = ZThread(MessageDataHandleThreadFuncP, this);

    return ret_val;
}

NODISCARD ReturnType ZUDPPeerP2P::Reconnect(
    const ZUDPEndpoint& _endpoint,
    TimeType _max_connect_time_ms = kDefaultConnectMaxTimeMs,
    TimeType _retry_interval_ms = kDefaultConnectReTryIntervalMs
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kDisconnected,
        error_code::kSocketErrorCode_StateError,
        "Peer state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kDisconnected)
    );

    state_ = StateEnum_::kConnecting;

    // Bind the target endpoint to the socket. Only receive messages from this endpoint after binding.
    link_code = socket_.Connect(_endpoint);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::Connect() link error!"
        );
        return ret_val;
    }

    // Start the hole punching process to establish a P2P connection with the target endpoint.
    link_code = PuchHoleP(_endpoint, kcp_session_.GetConversationID(), _max_connect_time_ms, _retry_interval_ms);
    if (link_code != kOK) {
        // If the hole punching process times out, transition back to the disconnected state.
        if (link_code == error_code::kZUDPPeerErrorCode_P2PConnectTimeout) {
            state_ = StateEnum_::kDisconnected;
            // Return timeout error code to indicate that the connection attempt timed out.
            ret_val = error_code::kZUDPPeerErrorCode_P2PConnectTimeout;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::PuchHoleP() link error!"
            );
        }
        // If any other error occurs during the hole punching process, 
        // the state is already set to error, close the KCP session. Log the error and return the link code.
        else {
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPPeerP2P::PuchHoleP() link error!"
            );
        }
        return ret_val;
    }

    // Update the last message receive time to the current time after a successful connection.
    last_message_receive_time_ms_ = ZKCPSession::GetCurrentTimeMs();

    // Notify the output thread, input thread, and message data handle thread to wake up and continue processing.
    output_thread_cv_.NotifyOne();
    input_thread_cv_.NotifyOne();
    message_data_handle_thread_cv_.NotifyOne();

    return ret_val;
}

Void ZUDPPeerP2P::Close() noexcept {
    // Transition to the closed state only if the current state is connected.
    StateEnum_ old_state = state_.Exchange(StateEnum_::kClosed);
    if (old_state == StateEnum_::kClosed) {
        return;
    }

    // Cancel any pending operations on the socket.
    socket_.Shutdown();

    // Stop the output thread and notify it to exit.
    output_thread_cv_.NotifyOne();
    if (output_thread_.IsJoinable()) {
        output_thread_.Join();
    }
    
    // Stop the input thread and notify it to exit.
    if (input_thread_.IsJoinable()) {
        input_thread_.Join();
    }

    // stop the message handle thread and notify it to exit.
    message_data_handle_thread_cv_.NotifyOne();
    if (message_data_handle_thread_.IsJoinable()) {
        // If the current thread is not the message handle thread, join it to wait for its completion.
        if (ZThread::GetCurrentThreadID() != message_data_handle_thread_.GetID()) {
            message_data_handle_thread_.Join();
        }
    }

    // Close the socket and KCP session to release resources.
    socket_.Close();
    {
        TLockGuard<ZMutex> lock(kcp_session_mutex_);
        kcp_session_.Close();
    }
}

NODISCARD ReturnType ZUDPPeerP2P::Send(TUniquePointer<ZUDPMessage> _msg_ptr) noexcept {
    ReturnType ret_val = kOK;
    // If the peer is in the disconnected state, return a disconnected error code.
    // Reconnecting is required to send messages again.
    if (state_ == StateEnum_::kDisconnected) {
        ret_val = error_code::kSocketErrorCode_Disconnected;
        return ret_val;
    }
    
    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Peer state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    output_message_queue_.PushBack(std::move(_msg_ptr));
    output_thread_cv_.NotifyOne();

    return kOK;
}

Void ZUDPPeerP2P::OutputThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept {
    ReturnType link_code = kOK;
    TUniquePointer<ZUDPMessage> message_ptr;
    ZMemory message_buffer;
    Bool message_buffer_message_exist = false;
    Bool message_get = false;
    UInt32 last_message_time_ms = ZKCPSession::GetCurrentTimeMs();

    // Pre-serialize a keep-alive message to avoid repeated serialization during the output loop.
    static ZConstBuffer keep_alive_message_buffer = []() {
        static ZMemory keep_alive_message_buffer;
        ZUDPMessage keep_alive_message(0, ZUDPMessage::kKeepAliveMessageType);
        ReturnType link_code = keep_alive_message.Serialize(keep_alive_message_buffer);
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZUDPPeerErrorCode_LinkError, link_code,
                "ZUDPMessage::Serialize() link error!"
            );
        }
        return ZConstBuffer(keep_alive_message_buffer.GetDataPtr<Void>(), keep_alive_message_buffer.GetSize());
    }();

    while (true) {
        message_get = false;

        // Try to pop a message from the output message queue. 
        // If a message is available, serialize it and send it through the KCP session.
        if (message_buffer_message_exist == false && _peer_ptr->output_message_queue_.TryPopFront(message_ptr)) {
            // Serialize the message to a buffer before sending it through KCP.
            link_code = message_ptr->Serialize(message_buffer);
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZUDPPeerErrorCode_LinkError, link_code,
                    "ZUDPMessage::Serialize() link error!"
                );
            }
            // If the message was successfully serialized, 
            // set flags to indicate that there is a message ready to be sent.
            else {
                message_buffer_message_exist = true;
                message_get = true;
                last_message_time_ms = ZKCPSession::GetCurrentTimeMs();
            }
        }
        else {
            // If no message was retrieved from the output queue, calculate the time since the last message was sent.
            Int32 last_message_interval_ms = 
                static_cast<Int32>(ZKCPSession::GetCurrentTimeMs() - last_message_time_ms);

            // If the time since the last message exceeds the keep-alive interval, send a keep-alive message.
            if (last_message_interval_ms >= _peer_ptr->keep_alive_interval_ms_) {
                {
                    TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                    link_code = _peer_ptr->kcp_session_.Send(keep_alive_message_buffer);
                    if (link_code == kOK) {
                        _peer_ptr->kcp_session_.Flush();
                        last_message_time_ms = ZKCPSession::GetCurrentTimeMs();
                    }
                }
                if (link_code != kOK) {
                    // If the send buffer is full, 
                    // log a debug message and set a flag to indicate that there are messages waiting to be sent.
                    if (link_code == error_code::kZKCPSessionErrorCode_WaitSend) {
                        Z_DEBUG_LOG_MESSAGE("ZKCPSession::Send() send buffer full!");
                    }
                    // If the send buffer is not full but another error occurs, 
                    // transition to the error state and log an error message.
                    else {
                        _peer_ptr->state_ = StateEnum_::kError;
                        Z_LOG_ERROR(
                            error_code::kZUDPPeerErrorCode_LinkError, link_code,
                            "ZKCPSession::Send() link error!"
                        );
                        return;
                    }
                }
            }
        }

        // If there is a serialized message in the buffer, send it through the KCP session.
        if(message_buffer_message_exist) {
            // Send the serialized message buffer through the KCP session for reliable delivery.
            {
                TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                link_code = _peer_ptr->kcp_session_.Send(ZConstBuffer(
                    message_buffer.GetDataPtr<Void>(),
                    message_buffer.At<SizeType>(ZUDPMessage::KMessageSizeOffset)
                ));
                // If the message was successfully sent through KCP, 
                // flush the KCP session to ensure that the message is transmitted promptly.
                if (link_code == kOK) {
                    _peer_ptr->kcp_session_.Flush();
                    message_buffer_message_exist = false;
                }
            }

            if (link_code != kOK) {
                // If the send buffer is full, 
                // log a debug message and set a flag to indicate that there are messages waiting to be sent.
                if (link_code == error_code::kZKCPSessionErrorCode_WaitSend) {
                    Z_DEBUG_LOG_MESSAGE("ZKCPSession::Send() send buffer full!");
                    
                }
                // If the send buffer is not full but another error occurs, 
                // transition to the error state and log an error message.
                else {
                    _peer_ptr->state_ = StateEnum_::kError;
                    Z_LOG_ERROR(
                        error_code::kZUDPPeerErrorCode_LinkError, link_code,
                        "ZKCPSession::Send() link error!"
                    );
                    return;
                }
            }  
        }

        UInt32 update_interval_ms = _peer_ptr->kcp_session_.GetUpdateIntervalMs();
        UInt32 next_update_time_ms;
        Int32 wait_time_ms;
        {
            TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
            next_update_time_ms = _peer_ptr->kcp_session_.GetNextUpdateTimeMs();
            // Calculate the time to wait until the next KCP session update is needed.
            wait_time_ms = Min(
                static_cast<Int32>(next_update_time_ms - ZKCPSession::GetCurrentTimeMs()),
                static_cast<Int32>(update_interval_ms)
            );
            // Update the KCP session if the next update time has been reached or exceeded.
            if (wait_time_ms <= 0) {
                _peer_ptr->kcp_session_.Update();
            }
        }

        // If the kcp buffer is full, flush the KCP session to send out any pending packets.
        if (wait_time_ms > 0) {
            if (message_buffer_message_exist == true) {
                {
                    TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                    _peer_ptr->kcp_session_.Flush();
                }
                {
                    TUniqueLock<ZMutex> lock(_peer_ptr->output_thread_mutex_);
                    _peer_ptr->output_thread_cv_.WaitForMs(
                        lock,
                        [_peer_ptr] {
                            return
                                _peer_ptr->state_ != StateEnum_::kConnected ||
                                _peer_ptr->output_message_queue_.GetSize() != 0;
                        },
                        update_interval_ms
                    );
                }
            }
            else {
                // If no message was retrieved from the output queue, wait for a signal or timeout before checking again.
                if (message_get == false) {
                    TUniqueLock<ZMutex> lock(_peer_ptr->output_thread_mutex_);
                    _peer_ptr->output_thread_cv_.WaitForMs(
                        lock,
                        [_peer_ptr] {
                            return
                                _peer_ptr->state_ != StateEnum_::kConnected ||
                                _peer_ptr->output_message_queue_.GetSize() != 0;
                        },
                        wait_time_ms
                    );
                }
            }
        }

        // Check if the time since the last received message exceeds the disconnect threshold.
        UInt32 last_msg_recv_time_diff_ms = ZKCPSession::GetCurrentTimeMs() - _peer_ptr->last_message_receive_time_ms_;
        if (last_msg_recv_time_diff_ms > _peer_ptr->disconnect_time_ms_) {
            _peer_ptr->state_ = StateEnum_::kDisconnected;
            // Lock the output thread until close or reconnect.
            {
                TUniqueLock<ZMutex> lock(_peer_ptr->output_thread_mutex_);
                _peer_ptr->output_thread_cv_.Wait(
                    lock,
                    [_peer_ptr] {
                        return _peer_ptr->state_ != StateEnum_::kDisconnected;
                    }
                );
            }
        }

        // If the peer state is no longer connected, break out of the output thread loop to exit the thread.
        if (_peer_ptr->state_ != StateEnum_::kConnected) {
            break;
        }
    }
}

Void ZUDPPeerP2P::InputThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept {
    ReturnType link_code = kOK;
    SizeType packet_size;

    while (true) {
        // Receive a packet from the socket into the packet buffer.
        link_code = _peer_ptr->socket_.Receive(
            ZBuffer(_peer_ptr->receive_packet_buffer_.GetDataPtr<Void>(), _peer_ptr->receive_packet_buffer_.GetSize()),
            &packet_size
        );
        if (link_code != kOK) {
            // If the socket was shutdown, exit the input thread gracefully.
            if (link_code == error_code::kSocketErrorCode_SocketShutdown) {
                return;
            }
            // If another error occurred during receive, transition to the error state and log an error message.
            else {
                _peer_ptr->state_ = StateEnum_::kError;
                Z_LOG_ERROR(
                    error_code::kSocketErrorCode_LinkError, link_code,
                    "ZUDPSocket::Receive() link error!"
                );
                return;
            }
        }       

        // Resolve the received packet using the KCP session.
        {
            TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
            link_code = _peer_ptr->kcp_session_.ResolveReceivedPacket(ZConstBuffer(
                _peer_ptr->receive_packet_buffer_.GetDataPtr<Void>(),
                packet_size
            ));
        }
        if (link_code != kOK) {
            if (link_code == error_code::kZKCPSessionErrorCode_NotKCPPacket) {
                continue;
            }
            else {
                _peer_ptr->state_ = StateEnum_::kError;
                Z_LOG_ERROR(
                    error_code::kZUDPPeerErrorCode_LinkError, link_code,
                    "ZKCPSession::ResolveReceivedPacket() link error!"
                );
                return;
            }
        }

        // Update the last message receive time to the current time after successfully receiving a packet.
        _peer_ptr->last_message_receive_time_ms_ = ZKCPSession::GetCurrentTimeMs();

        // Deal with the received messages from the KCP session.
        while (true) {
            // Check if there is a complete message available in the KCP session to be read.
            Int32 next_message_size;
            {
                TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                next_message_size = _peer_ptr->kcp_session_.GetNextMessageSize();
            }
          
            // If the next message size is negative, it indicates that there are no complete messages available, 
            // so break out of the loop.
            if (next_message_size < 0) {
                break;
            }

            // If the next message size is less than the size of the message header, 
            // skip the message and continue to the next iteration.
            if(next_message_size < ZUDPMessage::kMessageHeadSize) {
                {
                    TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                    _peer_ptr->kcp_session_.SkipNextMessage();
                }        
                continue;
            }

            // Create a memory buffer to hold the next complete message from the KCP session.
            ZMemory message_buffer(static_cast<SizeType>(next_message_size));

            // Retrieve the next complete message from the KCP session into the message buffer.
            {
                TLockGuard<ZMutex> lock(_peer_ptr->kcp_session_mutex_);
                link_code = _peer_ptr->kcp_session_.GetNextMessage(
                    ZBuffer(message_buffer.GetDataPtr<Void>(), message_buffer.GetSize())
                );
            }

            if (link_code != kOK) {
                _peer_ptr->state_ = StateEnum_::kError;
                Z_LOG_ERROR(
                    error_code::kZUDPPeerErrorCode_LinkError, link_code,
                    "ZKCPSession::GetNextMessage() link error!"
                );
                return;
            }

            // Check if the received message is a keep-alive message. 
            // If it is, skip processing and continue to the next iteration.
            if (message_buffer.At<UInt32>(ZUDPMessage::KMessageTypeOffset) == ZUDPMessage::kKeepAliveMessageType) {
                continue;
            }

            // Push the received message buffer into the input message data queue for further processing.
            _peer_ptr->input_message_data_queue_.PushBack(std::move(message_buffer));
            // Notify the message handle thread that a new message is available for processing.
            _peer_ptr->message_data_handle_thread_cv_.NotifyOne();
        }

        if (_peer_ptr->state_ != StateEnum_::kConnected) {
            // Lock the output thread until close or reconnect.
            if (_peer_ptr->state_ == StateEnum_::kDisconnected) {
                TUniqueLock<ZMutex> lock(_peer_ptr->input_thread_mutex_);
                _peer_ptr->input_thread_cv_.Wait(
                    lock,
                    [_peer_ptr] {
                        return _peer_ptr->state_ != StateEnum_::kDisconnected;
                    }
                );
            }
            // If the peer state is no longer connected and not disconnected, 
            // break out of the input thread loop to exit the thread.
            else {
                break;
            }
        }
    }
}

Void ZUDPPeerP2P::MessageDataHandleThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept {
    ReturnType link_code = kOK;
    ZMemory message_buffer;

    while (_peer_ptr->state_ == StateEnum_::kConnected) {
        // Wait for next message or end.
        {
            TUniqueLock<ZMutex> lock(_peer_ptr->message_data_handle_thread_mutex_);
            _peer_ptr->message_data_handle_thread_cv_.Wait(lock, [_peer_ptr] {
                return 
                    _peer_ptr->state_ != StateEnum_::kConnected ||
                    _peer_ptr->input_message_data_queue_.GetSize() != 0;
                });
        }

        // Process all available messages in the input message data queue.
        while (_peer_ptr->input_message_data_queue_.TryPopFront(message_buffer)) {
            // Call the user-defined message data handle function to process the received message buffer.
            _peer_ptr->receive_message_data_handle_func_(*_peer_ptr, message_buffer);
        }

        // If the peer state is no longer connected, break out of the output thread loop to exit the thread.
        if (_peer_ptr->state_ != StateEnum_::kConnected) {
            // Lock the output thread until close or reconnect.
            if (_peer_ptr->state_ == StateEnum_::kDisconnected) {
                TUniqueLock<ZMutex> lock(_peer_ptr->message_data_handle_thread_mutex_);
                _peer_ptr->message_data_handle_thread_cv_.Wait(
                    lock,
                    [_peer_ptr] {
                        return _peer_ptr->state_ != StateEnum_::kDisconnected;
                    }
                );
            }
            // If the peer state is no longer connected and not disconnected, 
            // break out of the input thread loop to exit the thread.
            else {
                break;
            }
        }

    }
}

Int32 ZUDPPeerP2P::KCPOutputCallbackP(const Char* _buf, Int32 _size, ikcpcb* _kcp_ptr, Void* _peer_ptr) noexcept {
    ZUDPSocket& socket = static_cast<ZUDPPeerP2P*>(_peer_ptr)->socket_;
    // Return result does not matter, get the return code to ignore the warning.
    ReturnType link_code = socket.Send(ZConstBuffer(_buf, _size));
    return 0;
}

NODISCARD ReturnType ZUDPPeerP2P::PuchHoleP(
    const ZUDPEndpoint& _endpoint,
    UInt32 _conv_id,
    TimeType _max_connect_time_ms,
    TimeType _retry_interval_ms
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // "P2P!" in ASCII
    constexpr UInt32 kHandshakeMagicNumber = 0x50325021;
#pragma pack(push, 1)
    struct P2PHandshakeContext {
        UInt32 magic_number_ = kHandshakeMagicNumber;
        UInt32 conv_id_;
        Bool if_received_packet_ = false;
        Byte dummy_[7];
    };
#pragma pack(pop)

    struct HandshakeHandlerFunction {
        void operator()(ReturnType _error_code, ZUDPSocket* _socket_ptr, ZConstBuffer _data_buffer) {
            ReturnType link_code = kOK;

            if (_error_code != kOK) {
                if (_error_code == error_code::kSocketErrorCode_OperationCanceled) {}
                else {
                    peer_ptr_->state_ = StateEnum_::kError;
                    peer_ptr_->state_.NotifyOne();
                    Z_LOG_ERROR(
                        error_code::kSocketErrorCode_LinkError, _error_code,
                        "ZUDPSocket::AsyncReceive() link error!"
                    );
                }
                return;
            }

            // If the received packet is a handshake packet, check the magic number to validate it.
            if (
                _data_buffer.GetSize() == sizeof(P2PHandshakeContext) &&
                _data_buffer.GetDataPtr<P2PHandshakeContext>()->magic_number_ == kHandshakeMagicNumber
            ) {
                // If the conversation ID in the handshake packet does not match the expected conversation ID, 
                // transition to the error state.
                if (_data_buffer.GetDataPtr<P2PHandshakeContext>()->conv_id_ != conv_id_) {
                    peer_ptr_->state_ = StateEnum_::kError;
                    peer_ptr_->state_.NotifyOne();
                    Z_LOG_ERROR(
                        error_code::kZUDPPeerErrorCode_P2PConvIDDifferent, 0,
                        "P2P handshake error! conv_id: {} expect conv_id: {}",
                        _data_buffer.GetDataPtr<P2PHandshakeContext>()->conv_id_,
                        conv_id_
                    );
                    return;
                }
                // If target peer has already received a handshake packet, transition to the connected state.
                else if (_data_buffer.GetDataPtr<P2PHandshakeContext>()->if_received_packet_ == true) {
                    peer_ptr_->state_ = StateEnum_::kConnected;
                    peer_ptr_->state_.NotifyOne();
                    return;
                }
                // If target peer has not received a handshake packet, set the flag and continue listening.
                else {
                    if (context_ptr_->if_received_packet_ == false) {
                        context_ptr_->if_received_packet_ = true;
                    }
                    link_code = peer_ptr_->socket_.AsyncReceive(
                        ZBuffer(
                            peer_ptr_->receive_packet_buffer_.GetDataPtr<Void>(),
                            peer_ptr_->receive_packet_buffer_.GetSize()
                        ),
                        *this
                    );
                    if (link_code != kOK) {
                        peer_ptr_->state_ = StateEnum_::kError;
                        peer_ptr_->state_.NotifyOne();
                        Z_LOG_ERROR(
                            error_code::kSocketErrorCode_LinkError, link_code,
                            "ZUDPSocket::AsyncReceive() link error!"
                        );
                        return;
                    }
                }
            }
            // Resolve the received packet using the KCP session. 
            // If the packet is valid and processed, then the handshake is considered successful.
            else if (
                context_ptr_->if_received_packet_ == true &&
                peer_ptr_->kcp_session_.ResolveReceivedPacket(_data_buffer)
                ) {
                peer_ptr_->state_ = StateEnum_::kConnected;
                peer_ptr_->state_.NotifyOne();
                return;
            }
            // Unknown packet received, continue listening for valid packets.
            else {
                link_code = peer_ptr_->socket_.AsyncReceive(
                    ZBuffer(
                        peer_ptr_->receive_packet_buffer_.GetDataPtr<Void>(),
                        peer_ptr_->receive_packet_buffer_.GetSize()
                    ),
                    *this
                );
                if (link_code != kOK) {
                    peer_ptr_->state_ = StateEnum_::kError;
                    peer_ptr_->state_.NotifyOne();
                    Z_LOG_ERROR(
                        error_code::kSocketErrorCode_LinkError, link_code,
                        "ZUDPSocket::AsyncReceive() link error!"
                    );
                    return;
                }

            }
        }

        ZUDPPeerP2P* peer_ptr_;
        TSharedPointer<P2PHandshakeContext> context_ptr_;
        UInt32 conv_id_;
    };
    auto context_ptr = MakeShared<P2PHandshakeContext>();
    context_ptr->conv_id_ = _conv_id;
    HandshakeHandlerFunction handler_func{ this, context_ptr, _conv_id };

    // Start an asynchronous receive operation to listen for incoming packets from the connected peer.
    link_code = socket_.AsyncReceive(
        ZBuffer(receive_packet_buffer_.GetDataPtr<Void>(), receive_packet_buffer_.GetSize()),
        handler_func
    );
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::AsyncReceive() link error!"
        );
        return ret_val;
    }

    // Retry sending the connect command until the maximum connection time is reached.
    TimeType timeout_time_ms = ZSystemTime::GetSteadyTimeMs() + _max_connect_time_ms;
    do {
        link_code = socket_.Send(ZConstBuffer(context_ptr.GetPtr(), sizeof(P2PHandshakeContext)));
        if (link_code != kOK) {
            socket_.Cancel();
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::Send() link error!"
            );
            return ret_val;
        }
    } while (
        !state_.WaitForMs(StateEnum_::kConnecting, static_cast<UInt32>(_retry_interval_ms)) &&
        ZSystemTime::GetSteadyTimeMs() < timeout_time_ms
    );

    // Not connected after retrying, clean up the command thread and queue, and return a timeout error.
    if (state_ != StateEnum_::kConnected) {
        // Cancel any pending operations on the socket to stop the asynchronous receive operation.
        socket_.Cancel();
        if (state_ == StateEnum_::kConnecting) {
            ret_val = error_code::kZUDPPeerErrorCode_P2PConnectTimeout;
            Z_LOG_ERROR(
                ret_val, 0,
                "P2P connect timeout! target ip: {} target port: {}",
                _endpoint.GenerateIPString(), _endpoint.GetPort()
            );
            return ret_val;
        }
        else {
            ret_val = error_code::kZUDPPeerErrorCode_SocketError;
            Z_LOG_ERROR(
                ret_val, 0,
                "Socket error! target ip: {} target port: {}",
                _endpoint.GenerateIPString(), _endpoint.GetPort()
            );
            return ret_val;
        }
    }

    // Update the last message receive time to the current time after a successful connection.
    last_message_receive_time_ms_ = ZKCPSession::GetCurrentTimeMs();

    return ret_val;
}

} // socket
} // zengine