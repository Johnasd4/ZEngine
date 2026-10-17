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
#pragma once

#include "drive.h"

#include "../z_core/t_function.h"
#include "../z_core/t_queue_spsc.h"
#include "../z_core/t_queue_mpmc.h"
#include "../z_core/z_condition_variable.h"
#include "../z_core/z_recursive_mutex.h"

#include "z_kcp_session.h"
#include "z_io_context.h"
#include "z_udp_message.h"
#include "z_udp_socket.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to ZUDPPeer operations and lifecycle states.
 */
enum ZUDPPeerErrorCodeEnum : ReturnType::ValueType {
    /** @brief Underlying link or sub-component execution error. */
    kZUDPPeerErrorCode_LinkError = kErrorCodeBase_ZUDPPeer,
    /** @brief Operating system or external library error. */
    kZUDPPeerErrorCode_SystemOrLibraryError,
    /** @brief Required pointer argument passed as nullptr. */
    kZUDPPeerErrorCode_NullptrParam,
    /** @brief Argument value passed is outside the valid range. */
    kZUDPPeerErrorCode_ParamOutOfRange,
    /** @brief Unrecognized or unsupported command identifier. */
    kZUDPPeerErrorCode_UnknownCommand,
    /** @brief P2P hole punching and handshake negotiation timed out. */
    kZUDPPeerErrorCode_P2PConnectTimeout,
    /** @brief Conversation ID in probe packet does not match expected session ID. */
    kZUDPPeerErrorCode_P2PConvIDDifferent,
    /** @brief Socket level system I/O operation error. */
    kZUDPPeerErrorCode_SocketError,
};

} // error_code
} // zengine

namespace zengine {
namespace socket {

/**
 * @brief Peer-to-point UDP peer supporting reliable communication over KCP and UDP hole punching.
 *
 * Implementation Principles:
 * 1. Transport & Reliability: Builds an ARQ reliable communication channel by layering a KCP session on top of
 *    an asynchronous ZUDPSocket, ensuring ordered and non-duplicated message delivery with tunable transmission modes.
 * 2. Hole Punching & Handshake: Uses a lightweight probe packet state machine to establish bidirectional NAT traversal
 *    between two endpoints before activating normal KCP data exchange.
 * 3. Concurrency & Decoupling: Decouples socket async I/O and user logic via thread-safe queues (an MPMC queue for
 *    outbound user messages and an SPSC queue for inbound socket datagrams), synchronized under a recursive mutex.
 * 4. Liveness & Heartbeat: Tracks periodic keep-alive packet transmission and receive intervals, triggering automatic
 *    disconnect state transition and socket cancellation upon elapsed timeout.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZUDPPeerP2P : public ZObject {
public:
    using SuperType_ = ZObject;
    using SendToken_ = TQueueMPMC<TUniquePointer<ZUDPMessage>>::ProducerToken_;

    /** @brief Default maximum duration in milliseconds to wait for a connection attempt to complete. */
    static inline constexpr TimeType kDefaultConnectMaxTimeMs = 30000;
    /** @brief Default interval in milliseconds between handshake probe packet retransmissions. */
    static inline constexpr TimeType kDefaultConnectReTryIntervalMs = 30;
    /** @brief Default timeout in milliseconds of silent reception before declaring connection lost. */
    static inline constexpr TimeType kDefaultDisconnectTimeMs = 5000;
    /** @brief Default interval in milliseconds between sending heartbeat keep-alive messages. */
    static inline constexpr Int32 kDefaultKeepAliveIntervalMs = 2000;
    /** @brief Capacity limit of the internal SPSC raw packet receive queue. */
    static inline constexpr SizeType kReceivePacketQueueSize = 4096ULL;
    /** @brief Capacity limit of the internal MPMC user message send queue. */
    static inline constexpr SizeType kSendMessageQueueSize = 4096ULL;

    /**
     * @brief Lifecycle connection state of the P2P peer.
     */
    enum class StateEnum_ : Int32 {
        /** @brief Peer is closed and non-operational. */
        kClosed,
        /** @brief Socket is opened and ready to bind or connect. */
        kOpened,
        /** @brief Performing NAT hole punching and handshake negotiation. */
        kConnecting,
        /** @brief Handshake succeeded and KCP session is established. */
        kConnected,
        /** @brief Connection timed out or disconnected by remote peer. */
        kDisconnected,
        /** @brief An unrecoverable socket or protocol error occurred. */
        kError
    };

    /**
     * @brief KCP transmission mode configuration presets.
     */
    enum class ModeEnum_ : Int32 {
        /** @brief Conservative TCP-like transmission mode with standard window and delays. */
        kNormal,
        /** @brief Balanced transmission mode optimizing both latency and bandwidth consumption. */
        kBalance,
        /** @brief Aggressive low-latency transmission mode with fast retransmission enabled. */
        kFast
    };

public:
    ZUDPPeerP2P(const ZUDPPeerP2P&) = delete;
    ZUDPPeerP2P(ZUDPPeerP2P&&) = delete;

    /**
     * @brief Destructor. Cleans up socket handles, sessions, and active queues.
     */
    ~ZUDPPeerP2P() noexcept;

    ZUDPPeerP2P& operator=(const ZUDPPeerP2P&) = delete;
    ZUDPPeerP2P& operator=(ZUDPPeerP2P&&) = delete;

    /**
     * @brief Retrieves the current atomic lifecycle state of the peer.
     * @return Current StateEnum_ value.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_.Load(); }

    /**
     * @brief Retrieves the local UDP endpoint bound to the underlying socket.
     * @return Local ZUDPEndpoint structure.
     */
    NODISCARD FORCEINLINE ZUDPEndpoint GetLocalEndpoint() const noexcept { return socket_.GetLocalEndpoint(); }

    /**
     * @brief Queries the total size of pending readable bytes in the OS socket buffer.
     * @return Readable data byte count.
     */
    NODISCARD FORCEINLINE SizeType GetReadableDataSize() noexcept { return socket_.GetReadableDataSize(); }

    /**
     * @brief Retrieves a mutable reference to the underlying UDP socket.
     * @return Reference to the internal ZUDPSocket object.
     */
    NODISCARD FORCEINLINE ZUDPSocket& GetSocket() noexcept { return socket_; }

    /**
     * @brief Retrieves a const reference to the underlying UDP socket.
     * @return Const reference to the internal ZUDPSocket object.
     */
    NODISCARD FORCEINLINE const ZUDPSocket& GetSocket() const noexcept { return socket_; }

    /**
     * @brief Retrieves the I/O context driving the asynchronous socket events.
     * @return Const reference to the associated ZIOContext object.
     */
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return socket_.GetIOContext(); }

    /**
     * @brief Retrieves the user-defined custom link context object pointer.
     * @tparam _ObjectType Target type of the bound object.
     * @return Pointer to the bound context object, or nullptr if unset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetLinkObjectPtr() const noexcept { return socket_.GetLinkObjectPtr(); }

    /**
     * @brief Sets the operating system socket write buffer size.
     * @param _size Desired buffer capacity in bytes.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD FORCEINLINE ReturnType SetOSWriteBufferSize(Int32 _size) noexcept { 
        return socket_.SetOSWriteBufferSize(_size); 
    }

    /**
     * @brief Sets the operating system socket read buffer size.
     * @param _size Desired buffer capacity in bytes.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD FORCEINLINE ReturnType SetOSReadBufferSize(Int32 _size) noexcept {
        return socket_.SetOSReadBufferSize(_size);
    }

    /**
     * @brief Configures whether the socket allows address reuse.
     * @param _if_reuse True to enable SO_REUSEADDR option, false to disable.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD FORCEINLINE ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept {
        return socket_.SetIfReuseAddress(_if_reuse);
    }

    /**
     * @brief Sets the callback handler invoked when an asynchronous socket error occurs.
     * @param _handle_func Rvalue reference to the error handling function object.
     * @return None.
     */
    FORCEINLINE Void SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
        socket_.SetAsyncErrorHandleFunction(std::forward<TFunction<Void(ReturnType)>>(_handle_func));
    }

    /**
     * @brief Sets the callback handler invoked when complete message data is received and extracted.
     * @param _handle_func Rvalue reference to the message handling function object.
     * @return None.
     */
    FORCEINLINE Void SetReceiveMessagedDataHandleFunction(
        TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _handle_func
    ) noexcept {
        receive_message_data_handle_func_ = std::forward<TFunction<Void(ZUDPPeerP2P&, ZMemory&)>>(_handle_func);
    }

    /**
     * @brief Associates a user-defined custom link context object pointer with the socket.
     * @tparam _ObjectType Target type of the context object.
     * @param _obj_ptr Pointer to the custom context object.
     * @return None.
     */
    template<typename _ObjectType>
    FORCEINLINE Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        socket_.SetLinkObjectPtr(_obj_ptr);
    }

    /**
     * @brief Configures the interval for sending heartbeat keep-alive messages.
     * @param _interval_ms Heartbeat interval duration in milliseconds.
     * @return None.
     */
    FORCEINLINE Void SetKeepAliveInterval(Int32 _interval_ms) noexcept {
        keep_alive_interval_ms_ = _interval_ms;
    }

    /**
     * @brief Configures the maximum silence duration before disconnecting due to timeout.
     * @param _time_ms Disconnection timeout threshold in milliseconds.
     * @return None.
     */
    FORCEINLINE Void SetDisconnectTimeMs(Int32 _time_ms) noexcept {
        disconnect_time_ms_ = _time_ms;
    }

    /**
     * @brief Opens the underlying socket with the specified IP address protocol family.
     * @param _ip_type Protocol type (IPv4 or IPv6).
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /**
     * @brief Binds the socket to a specific local network endpoint before connecting.
     * @param _endpoint Local UDP endpoint to bind.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType BindEndpoint(const ZUDPEndpoint& _endpoint) noexcept;

    /**
     * @brief Initiates P2P hole punching, negotiates handshake, and activates the KCP session.
     * @param _endpoint Remote target UDP endpoint to connect to.
     * @param _conv_id Unique conversation identifier for the KCP protocol.
     * @param _mode KCP transmission tuning mode preset.
     * @param _max_connect_time_ms Maximum duration in milliseconds to attempt connection.
     * @param _retry_interval_ms Interval in milliseconds between probe retransmissions.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Connect(
        const ZUDPEndpoint& _endpoint,
        UInt32 _conv_id,
        ModeEnum_ _mode,
        TimeType _max_connect_time_ms = kDefaultConnectMaxTimeMs,
        TimeType _retry_interval_ms = kDefaultConnectReTryIntervalMs
    ) noexcept;

    /**
     * @brief Re-establishes connection to the target endpoint from a disconnected state.
     * @param _endpoint Remote target UDP endpoint to reconnect to.
     * @param _max_connect_time_ms Maximum duration in milliseconds to attempt reconnection.
     * @param _retry_interval_ms Interval in milliseconds between probe retransmissions.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Reconnect(
        const ZUDPEndpoint& _endpoint,
        TimeType _max_connect_time_ms = kDefaultConnectMaxTimeMs,
        TimeType _retry_interval_ms = kDefaultConnectReTryIntervalMs
    ) noexcept;

    /**
     * @brief Closes the connection, releases socket and KCP sessions, and resets message queues.
     * @return None.
     */
    Void Close() noexcept;

    /**
     * @brief Enqueues a message into the thread-safe send queue for asynchronous transmission.
     * @param _msg_ptr Reference to unique pointer holding the message to send.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Send(TUniquePointer<ZUDPMessage>& _msg_ptr) noexcept;

    /**
     * @brief Fast-path enqueues a message into the send queue utilizing a producer token.
     * @param _token Pre-allocated producer token dedicated to the caller thread.
     * @param _msg_ptr Reference to unique pointer holding the message to send.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType SendFast(SendToken_& _token, TUniquePointer<ZUDPMessage>& _msg_ptr) noexcept;

    /**
     * @brief Creates a producer token tied to the internal send queue for fast-path enqueueing.
     * @return Newly generated producer token instance.
     */
    NODISCARD FORCEINLINE SendToken_ MakeSendToken() noexcept {
        return send_message_queue_.MakeProducerToken();
    }

private:
    /**
     * @brief Private constructor called by friend class ZUDPPeerManagerP2P.
     * @param _io_context Reference to driving asynchronous I/O context.
     * @param _receive_message_data_handle_func Message reception callback handler.
     */
    ZUDPPeerP2P(
        ZIOContext& _io_context,
        TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _receive_message_data_handle_func
    ) noexcept;

private:
    /**
     * @brief Callback invoked by KCP when lower-layer protocol packets need transmission via UDP.
     * @param _buf Pointer to buffer containing serialized KCP packet data.
     * @param _size Byte count of data to be transmitted.
     * @param _kcp_ptr Pointer to the originating KCP control block.
     * @param _peer_ptr User pointer pointing to the current ZUDPPeerP2P instance.
     * @return Integer status code required by KCP callback interface.
     */
    static Int32 KCPOutputCallbackP(const Char* _buf, Int32 _size, ikcpcb* _kcp_ptr, Void* _peer_ptr) noexcept;

    /**
     * @brief Performs UDP hole punching probe loop and completes mutual connection handshake.
     * @param _endpoint Remote endpoint destination for handshake packets.
     * @param _conv_id Conversation identifier verified during handshake negotiation.
     * @param _max_connect_time_ms Maximum allowable connection handshake duration in milliseconds.
     * @param _retry_interval_ms Probe transmission retry interval in milliseconds.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType PunchHoleP(
        const ZUDPEndpoint& _endpoint,
        UInt32 _conv_id, 
        TimeType _max_connect_time_ms,
        TimeType _retry_interval_ms
    ) noexcept;

    /**
     * @brief Initiates the continuous asynchronous receive loop on the underlying UDP socket.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType StartReceiveP() noexcept;

    /**
     * @brief Completion callback invoked when an asynchronous packet read finishes on the socket.
     * @param _error_code Completion error code reported by the socket I/O operation.
     * @param _socket_ptr Pointer to the socket triggering the completion event.
     * @param _data_buffer Buffer view containing the received raw packet data.
     * @return None.
     */
    Void OnReceivePacketP(
        ReturnType _error_code,
        ZUDPSocket* _socket_ptr,
        ZConstBuffer _data_buffer
    ) noexcept;

    /**
     * @brief Drains received datagrams, resolves them through KCP, and dispatches complete messages.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType HandleReceivePacketsP() noexcept;

    /**
     * @brief Serializes queued outbound messages, pushes them to KCP, and updates heartbeat timers.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType SendMessagesP() noexcept;

    /**
     * @brief Checks elapsed time since last reception and transitions state if timeout exceeded.
     * @return None.
     */
    Void ConnectionTimeoutCheckP() noexcept;

private:
    friend class ZUDPPeerManagerP2P;

private:
    /** @brief Atomic lifecycle state variable of the peer. */
    TAtom<StateEnum_> state_;
    /** @brief Underlying asynchronous UDP socket handle. */
    CACHE_ALIGN ZUDPSocket socket_;
    /** @brief KCP protocol session instance managing reliability and flow control. */
    ZKCPSession kcp_session_;
    /** @brief Recursive mutex synchronizing peer operations during worker iteration. */
    ZRecursiveMutex work_mutex_;

    /** @brief SPSC lock-free queue storing incoming raw datagram buffers from socket callbacks. */
    CACHE_ALIGN TQueueSPSC<ZMemory> receive_packet_queue_;
    /** @brief Reusable memory buffer holding incoming datagram payload during async reception. */
    ZMemory receive_packet_buffer_;
    /** @brief Atomic timestamp in milliseconds recording when the latest message was received. */
    TAtom<UInt32> last_message_receive_time_ms_;

    /** @brief Timestamp in milliseconds recording when the latest outbound message was sent. */
    CACHE_ALIGN UInt32 last_message_send_time_ms_;
    /** @brief Inactivity duration threshold in milliseconds before triggering disconnection. */
    UInt32 disconnect_time_ms_;
    /** @brief Configured interval in milliseconds for periodic keep-alive packet dispatch. */
    Int32 keep_alive_interval_ms_;

    /** @brief MPMC lock-free queue buffering user messages awaiting serialization and dispatch. */
    CACHE_ALIGN TQueueMPMC<TUniquePointer<ZUDPMessage>> send_message_queue_;
    /** @brief Pre-allocated consumer token assigned to the worker thread for dequeuing messages. */
    TQueueMPMC<TUniquePointer<ZUDPMessage>>::ConsumerToken_ send_message_queue_consumer_token_;

    /** @brief Reusable memory buffer for message serialization before KCP transmission. */
    CACHE_ALIGN ZMemory send_message_serialize_buffer_;
    /** @brief Flag indicating whether the serialization buffer holds an unsent message slice. */
    Bool if_send_message_serialize_buffer_empty_;
    /** @brief User-registered callback handler for dispatching parsed message payloads. */
    TFunction<Void(ZUDPPeerP2P&, ZMemory&)> receive_message_data_handle_func_;
};

} // socket
} // zengine