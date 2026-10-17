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
	, work_mutex_()
    , receive_packet_queue_(kReceivePacketQueueSize)
    , receive_packet_buffer_(ZKCPSession::kMaxPacketSize)
    , last_message_send_time_ms_(0U)
    , last_message_receive_time_ms_(0U)
	, disconnect_time_ms_(kDefaultDisconnectTimeMs)
    , keep_alive_interval_ms_(kDefaultKeepAliveIntervalMs)
    , send_message_queue_(kSendMessageQueueSize)
    , send_message_queue_consumer_token_(send_message_queue_.MakeConsumerToken())
    , send_message_serialize_buffer_()
	, if_send_message_serialize_buffer_empty_(true)
    , receive_message_data_handle_func_(
        std::forward<TFunction<Void(ZUDPPeerP2P&, ZMemory&)>>(_receive_message_data_handle_func)
    )
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
    link_code = PunchHoleP(_endpoint, _conv_id, _max_connect_time_ms, _retry_interval_ms);
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
                "ZUDPSocket::PunchHoleP() link error!"
            );
        }
        // If any other error occurs during the hole punching process, 
        // the state is already set to error, close the KCP session. Log the error and return the link code.
        else {           
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPPeerP2P::PunchHoleP() link error!"
            );
        }

        return ret_val;
    }

    // Start receiving messages from the socket.
    link_code = StartReceiveP();
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::StartReceiveP() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPPeerP2P::Reconnect(
    const ZUDPEndpoint& _endpoint,
    TimeType _max_connect_time_ms,
    TimeType _retry_interval_ms
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
    link_code = PunchHoleP(_endpoint, kcp_session_.GetConversationID(), _max_connect_time_ms, _retry_interval_ms);
    if (link_code != kOK) {
        // If the hole punching process times out, transition back to the disconnected state.
        if (link_code == error_code::kZUDPPeerErrorCode_P2PConnectTimeout) {
            state_ = StateEnum_::kDisconnected;
            // Return timeout error code to indicate that the connection attempt timed out.
            ret_val = error_code::kZUDPPeerErrorCode_P2PConnectTimeout;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::PunchHoleP() link error!"
            );
        }
        // If any other error occurs during the hole punching process, 
        // the state is already set to error, close the KCP session. Log the error and return the link code.
        else {
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPPeerP2P::PunchHoleP() link error!"
            );
        }
        return ret_val;
    }

    // Continue receiving messages from the socket.
    link_code = StartReceiveP();
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::StartReceiveP() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

Void ZUDPPeerP2P::Close() noexcept {
    TLockGuard lock_guard(work_mutex_);

    // Transition to the closed state only if the current state is connected.
    StateEnum_ old_state = state_.Exchange(StateEnum_::kClosed);
    if (old_state == StateEnum_::kClosed) {
        return;
    }

	// Close the underlying UDP socket and KCP session to release resources.
    socket_.Close();
    kcp_session_.Close();

	// Clear the send message queue and receive packet queue to free up memory and reset the state.
    send_message_queue_.Clear();
    receive_packet_queue_.Clear();
    if_send_message_serialize_buffer_empty_ = true;
}

NODISCARD ReturnType ZUDPPeerP2P::Send(TUniquePointer<ZUDPMessage>& _msg_ptr) noexcept {
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

    send_message_queue_.PushBack(std::move(_msg_ptr));

    return kOK;
}

NODISCARD ReturnType ZUDPPeerP2P::SendFast(SendToken_& _token, TUniquePointer<ZUDPMessage>& _msg_ptr) noexcept {
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

    send_message_queue_.PushBack(_token, std::move(_msg_ptr));

    return kOK;
}

NODISCARD ReturnType ZUDPPeerP2P::HandleReceivePacketsP() noexcept {
	ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

	ZMemory buffer;

	// Process all received packets in the receive packet queue.
    while (receive_packet_queue_.TryPopFront(buffer)) {
        link_code = kcp_session_.ResolveReceivedPacket(ZConstBuffer(
            buffer.GetDataPtr<Void>(),
            buffer.GetSize()
        ));
        if (link_code != kOK) {
            if (link_code == error_code::kZKCPSessionErrorCode_NotKCPPacket) {
                continue;
            }
            else {
                state_ = StateEnum_::kError;
                ret_val = error_code::kZUDPPeerErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    "ZKCPSession::ResolveReceivedPacket() link error!"
                );
                return ret_val;
            }
        }
    }

	Int32 next_message_size = 0;

	// Process all complete messages available in the KCP session.
    while ((next_message_size = kcp_session_.GetNextMessageSize()) > 0) {

		// Resize the message buffer to accommodate the next complete message.
        buffer.Resize(static_cast<SizeType>(next_message_size));

		// Retrieve the next complete message from the KCP session into the message buffer.
        link_code = kcp_session_.GetNextMessage(
            ZBuffer(buffer.GetDataPtr<Void>(), buffer.GetSize())
        );
        if (link_code != kOK) {
            state_ = StateEnum_::kError;
			ret_val = error_code::kZUDPPeerErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZKCPSession::GetNextMessage() link error!"
            );
            return ret_val;
        }

		// Check if the received message is smaller than the minimum message header size.
        if (buffer.GetSize() < ZUDPMessage::kMessageHeadSize) {
            continue;
        }

		// Check if the received message is a keep-alive message. If so, skip processing it.
        if (buffer.At<UInt32>(ZUDPMessage::kMessageTypeOffset) == ZUDPMessage::kKeepAliveMessageType) {
            continue;
        }

		// Invoke the user-defined callback function to handle the received message data.
        receive_message_data_handle_func_(*this, buffer);

		// If kcp is closed during message processing, it means the peer has been closed, 
        // so break out of the loop to stop further processing.
		if (kcp_session_.IsClosed()) {
            break;
		}
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPPeerP2P::SendMessagesP() noexcept {
	ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // If kcp is closed during message processing, it means the peer has been closed, 
    // so break out of the loop to stop further processing.
    if (kcp_session_.IsClosed()) {
        return ret_val;
    }

    TUniquePointer<ZUDPMessage> message_ptr;
	Bool if_message_send = false;

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

	// If the output message serialize buffer is not empty, send the serialized message buffer through the KCP session.
    if (if_send_message_serialize_buffer_empty_ == false) {
        // Send the serialized message buffer through the KCP session.
        link_code = kcp_session_.Send(ZConstBuffer(
            send_message_serialize_buffer_.GetDataPtr<Void>(), 
            send_message_serialize_buffer_.GetSize()
        ));
        if (link_code != kOK) {
            // If the send buffer is full, 
            // log a debug message and set a flag to indicate that there are messages waiting to be sent.
            if (link_code == error_code::kZKCPSessionErrorCode_WaitSend) {
                Z_DEBUG_LOG_MESSAGE("ZKCPSession::Send() send buffer full!");
            }
            // If the send buffer is not full but another error occurs, 
            // transition to the error state and log an error message.
            else {
                state_ = StateEnum_::kError;
                ret_val = error_code::kSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    "ZKCPSession::Send() link error!"
                );
                return ret_val;
            }
        }
        else {
            if_send_message_serialize_buffer_empty_ = true;
            if_message_send = true;
        }
    }

	// Continuously pop messages from the output message queue and serialize them for sending through KCP.
    while (
        if_send_message_serialize_buffer_empty_ == true &&
        send_message_queue_.TryPopFront(send_message_queue_consumer_token_, message_ptr)
    ) {
        // Serialize the message to a buffer before sending it through KCP.
        link_code = message_ptr->Serialize(send_message_serialize_buffer_);
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZUDPPeerErrorCode_LinkError, link_code,
                "ZUDPMessage::Serialize() link error!"
            );
            continue;
        }
     
        

        // Send the serialized message buffer through the KCP session.
        link_code = kcp_session_.Send(ZConstBuffer(
            send_message_serialize_buffer_.GetDataPtr<Void>(),
            send_message_serialize_buffer_.GetSize()
        ));
        if (link_code != kOK) {
            // If the send buffer is full, 
            // log a debug message and set a flag to indicate that there are messages waiting to be sent.
            if (link_code == error_code::kZKCPSessionErrorCode_WaitSend) {
                if_send_message_serialize_buffer_empty_ = false;
                Z_DEBUG_LOG_MESSAGE("ZKCPSession::Send() send buffer full!");
                break;
            }
            // If the send buffer is not full but another error occurs, 
            // transition to the error state and log an error message.
            else {
                state_ = StateEnum_::kError;
                ret_val = error_code::kSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    "ZKCPSession::Send() link error!"
                );
                return ret_val;
            }
        }
        if_message_send = true;
    }

	UInt32 current_time_ms = ZKCPSession::GetCurrentTimeMs();
	if (if_message_send == true) {
		// Update the last message send time to the current time after sending a message.
		last_message_send_time_ms_ = current_time_ms;
	}

	// If the time since the last message exceeds the keep-alive interval, send a keep-alive message.
    Int32 last_message_interval_ms =
        static_cast<Int32>(current_time_ms - last_message_send_time_ms_);
	if (
        if_send_message_serialize_buffer_empty_ == true && 
        last_message_interval_ms >= keep_alive_interval_ms_
    ) {
        link_code = kcp_session_.Send(keep_alive_message_buffer);
		if (link_code != kOK) {
			// If the send buffer is full, 
			// log a debug message and set a flag to indicate that there are messages waiting to be sent.
			if (link_code == error_code::kZKCPSessionErrorCode_WaitSend) {
				Z_DEBUG_LOG_MESSAGE("ZKCPSession::Send() send buffer full!");
			}
			// If the send buffer is not full but another error occurs, 
			// transition to the error state and log an error message.
			else {
				state_ = StateEnum_::kError;
				ret_val = error_code::kSocketErrorCode_LinkError;
				Z_LOG_ERROR(
                    ret_val, link_code,
					"ZKCPSession::Send() link error!"
				);
				return ret_val;
			}
		}
        else {
            last_message_send_time_ms_ = current_time_ms;
            if_message_send = true;
        }
	}
        
	// Update the KCP session if a message was sent or if the output message serialize buffer is not empty.
    if (if_message_send || if_send_message_serialize_buffer_empty_ == false) {
        kcp_session_.Update();
        kcp_session_.Flush();
    }
    else {
		// Update the KCP session only if the next update time has been reached.
        if (static_cast<Int32>(current_time_ms - kcp_session_.GetNextUpdateTimeMs()) >= 0) {
            kcp_session_.Update();
        }   
    }

    return ret_val;
}

Void ZUDPPeerP2P::ConnectionTimeoutCheckP() noexcept {
    UInt32 receive_message_interval_ms = ZKCPSession::GetCurrentTimeMs() - last_message_receive_time_ms_;

    if (receive_message_interval_ms >= disconnect_time_ms_) {
        StateEnum_ expected = StateEnum_::kConnected;
        if (state_.CompareExchangeStrong(expected, StateEnum_::kDisconnected)) {
            socket_.Cancel();
        }
    }
}

Int32 ZUDPPeerP2P::KCPOutputCallbackP(const Char* _buf, Int32 _size, ikcpcb* _kcp_ptr, Void* _peer_ptr) noexcept {
    ZUDPSocket& socket = static_cast<ZUDPPeerP2P*>(_peer_ptr)->socket_;
    // Return result does not matter, get the return code to ignore the warning.
    ReturnType link_code = socket.Send(ZConstBuffer(_buf, _size));
    return 0;
}

NODISCARD ReturnType ZUDPPeerP2P::PunchHoleP(
    const ZUDPEndpoint& _endpoint,
    UInt32 _conv_id,
    TimeType _max_connect_time_ms,
    TimeType _retry_interval_ms
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    constexpr UInt32 kHandshakeMagicNumber = 0x50325021;

#pragma pack(push, 1)
    struct HandshakePacket {
        UInt32 magic_number_ = kHandshakeMagicNumber;
        UInt32 conv_id_;
        UInt32 if_received_packet_;
    };
#pragma pack(pop)

    struct HandshakeContext {
        TAtom<UInt32> if_received_packet_ = 0;
        TFixedMemory<ZKCPSession::kMaxPacketSize> receive_memory_;
    };

    struct HandshakeHandleFunc {
        Void operator()(ReturnType _error_code, ZUDPSocket* _socket_ptr, ZConstBuffer _data_buffer) {

            // If an error occurs during the asynchronous receive operation, handle the error accordingly.
            if (_error_code != kOK) {
                if (_error_code == error_code::kSocketErrorCode_OperationCanceled) {
                    return;
                }
                peer_ptr_->state_ = StateEnum_::kError;
                peer_ptr_->state_.NotifyOne();
                Z_LOG_ERROR(
                    error_code::kSocketErrorCode_LinkError, _error_code,
                    "ZUDPSocket::AsyncReceive() link error!"
                );
                return;
            }

            // If the received packet is a handshake packet, check the magic number to validate it.
            if (
                _data_buffer.GetSize() == sizeof(HandshakePacket) &&
                _data_buffer.GetDataPtr<HandshakePacket>()->magic_number_ == kHandshakeMagicNumber
            ) {
                const auto* packet_ptr = _data_buffer.GetDataPtr<HandshakePacket>();

                // If the conversation ID in the handshake packet does not match the expected conversation ID,
                if (packet_ptr->conv_id_ != conv_id_) {
                    peer_ptr_->state_ = StateEnum_::kError;
                    peer_ptr_->state_.NotifyOne();
                    Z_LOG_ERROR(
                        error_code::kZUDPPeerErrorCode_P2PConvIDDifferent, 0,
                        "P2P handshake error! conv_id: {} expect conv_id: {}",
                        packet_ptr->conv_id_, conv_id_
                    );
                    return;
                }

                // If the peer has confirmed receipt of our probe,
                // complete the handshake and transition to the Connected state
                if (packet_ptr->if_received_packet_ != 0) {
                    peer_ptr_->last_message_send_time_ms_ = ZKCPSession::GetCurrentTimeMs();
                    peer_ptr_->last_message_receive_time_ms_ = peer_ptr_->last_message_send_time_ms_;
                    peer_ptr_->state_ = StateEnum_::kConnected;
                    peer_ptr_->state_.NotifyOne();
                    return;
                }

                // Set the flag to true and continue receiving packets.
                context_ptr_->if_received_packet_ = 1;
                ContinueReceive();
                return;
            }

            // If the peer has already received a handshake packet and the received packet is a valid KCP packet,
            if (
                context_ptr_->if_received_packet_ != 0 &&
                peer_ptr_->kcp_session_.ResolveReceivedPacket(_data_buffer) == kOK
            ) {
                peer_ptr_->last_message_send_time_ms_ = ZKCPSession::GetCurrentTimeMs();
                peer_ptr_->last_message_receive_time_ms_ = peer_ptr_->last_message_send_time_ms_;
                peer_ptr_->state_ = StateEnum_::kConnected;
                peer_ptr_->state_.NotifyOne();
                return;
            }

            // If an unknown packet is received, ignore it and continue listening
            ContinueReceive();
        }

        Void ContinueReceive() {
            ZBuffer receive_buffer(
                context_ptr_->receive_memory_.GetDataPtr<Void>(),
                context_ptr_->receive_memory_.GetSize()
            );
            ReturnType link_code = peer_ptr_->socket_.AsyncReceive(receive_buffer, *this);
            if (link_code != kOK) {
                peer_ptr_->state_ = StateEnum_::kError;
                peer_ptr_->state_.NotifyOne();
                Z_LOG_ERROR(
                    error_code::kSocketErrorCode_LinkError, link_code,
                    "ZUDPSocket::AsyncReceive() link error!"
                );
            }
        }

        ZUDPPeerP2P* peer_ptr_;
        UInt32 conv_id_;
        TSharedPointer<HandshakeContext> context_ptr_;
    };

    // Initialize the handshake context and handler to manage the handshake process.
    auto context_ptr = MakeShared<HandshakeContext>();
    HandshakeHandleFunc handle_func{ this, _conv_id, context_ptr };


    // Start an asynchronous receive operation to listen for incoming packets from the connected peer.
    ZBuffer receive_buffer(
        context_ptr->receive_memory_.GetDataPtr<Void>(),
        context_ptr->receive_memory_.GetSize()
    );
    link_code = socket_.AsyncReceive(receive_buffer, handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPSocket::AsyncReceive() link error!"
        );
        return ret_val;
    }

    // Prepare the handshake packet to send.
    HandshakePacket handshake_packet;
    handshake_packet.conv_id_ = _conv_id;

    // Retry sending the connect command until the maximum connection time is reached.
    TimeType timeout_time_ms = ZSystemTime::GetSteadyTimeMs() + _max_connect_time_ms;
    do {
        // Update if_received_packet_.
        handshake_packet.if_received_packet_ = context_ptr->if_received_packet_;
        // Send the handshake context to the connected peer to initiate the hole punching process.
        link_code = socket_.Send(ZConstBuffer(&handshake_packet, sizeof(HandshakePacket)));
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

NODISCARD ReturnType ZUDPPeerP2P::StartReceiveP() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // Resize the receive packet buffer to the maximum packet size defined by the KCP session.
    receive_packet_buffer_.Resize(ZKCPSession::kMaxPacketSize);

    // Start an asynchronous receive operation to listen for incoming packets from the connected peer.
    ZBuffer receive_buffer(
        receive_packet_buffer_.GetDataPtr<Void>(),
        receive_packet_buffer_.GetSize()
    );
    link_code = socket_.AsyncReceive(
        receive_buffer, 
        [this](ReturnType _error_code, ZUDPSocket* _socket_ptr, ZConstBuffer _data_buffer) {
            OnReceivePacketP(_error_code, _socket_ptr, _data_buffer);
        }     
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

    return ret_val;
}

Void ZUDPPeerP2P::OnReceivePacketP(
    ReturnType _error_code,
    ZUDPSocket* _socket_ptr,
    ZConstBuffer _data_buffer
) noexcept {
    ReturnType link_code = kOK;

    // If an error occurs during the asynchronous receive operation, handle the error accordingly.
    if (_error_code != kOK) {
        if (_error_code == error_code::kSocketErrorCode_OperationCanceled) {
            return;
        }
        state_ = StateEnum_::kError;
        state_.NotifyOne();
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_LinkError, _error_code,
            "ZUDPSocket::AsyncReceive() link error!"
        );
        return;
    }

	// If the received data buffer size is greater than the minimum KCP packet size, process the received packet.
    if (_data_buffer.GetSize() >= ZKCPSession::kPacketMinSize) {
        // Update the last message receive time to the current time after successfully receiving a packet.
        last_message_receive_time_ms_ = ZKCPSession::GetCurrentTimeMs();

        // Resize the receive packet buffer to the size of the received data buffer for further processing.
        receive_packet_buffer_.Resize(_data_buffer.GetSize());

        // Push the received packet buffer into the receive packet pointer queue for further processing.
        receive_packet_queue_.PushBack(std::move(receive_packet_buffer_));

        // Reset the receive packet buffer to prepare for the next asynchronous receive operation.
        receive_packet_buffer_.Resize(ZKCPSession::kMaxPacketSize);
    }
	// Other cases, such as receiving a packet smaller than the minimum KCP packet size, are ignored.
    else {}

	// Start another asynchronous receive operation to continue listening for incoming packets from the connected peer.
    ZBuffer receive_buffer(
        receive_packet_buffer_.GetDataPtr<Void>(),
        receive_packet_buffer_.GetSize()
    );
    link_code = socket_.AsyncReceive(
        receive_buffer,
        [this](ReturnType _error_code, ZUDPSocket* _socket_ptr, ZConstBuffer _data_buffer) {
            OnReceivePacketP(_error_code, _socket_ptr, _data_buffer);
        }
    );
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_StateError && state_ == StateEnum_::kDisconnected) {}
        else {
            state_ = StateEnum_::kError;
            state_.NotifyOne();
            Z_LOG_ERROR(
                error_code::kSocketErrorCode_LinkError, link_code,
                "ZUDPSocket::AsyncReceive() link error!"
            );
            return;
        }
    }
}

} // socket
} // zengine