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

#include "../z_core/t_fixed_queue_spsc.h"
#include "../z_core/t_function.h"
#include "../z_core/t_quest_queue.h"
#include "../z_core/z_condition_variable.h"
#include "../z_core/z_mutex.h"

#include "z_kcp_session.h"
#include "z_io_context.h"
#include "z_udp_message.h"
#include "z_udp_socket.h"

namespace zengine {
namespace error_code {

enum ZUDPPeerErrorCodeEnum : ReturnType::ValueType {
    kZUDPPeerErrorCode_LinkError = kErrorCodeBase_ZUDPPeer,
    kZUDPPeerErrorCode_SystemOrLibraryError,
    kZUDPPeerErrorCode_NullptrParam,
    kZUDPPeerErrorCode_ParamOutOfRange,
    kZUDPPeerErrorCode_UnknownCommand,
    kZUDPPeerErrorCode_P2PConnectTimeout,
    kZUDPPeerErrorCode_P2PConvIDDifferent,
    kZUDPPeerErrorCode_SocketError,

};

} // zengine
} // error_code

namespace zengine {
namespace socket {

/*
    UDP peer for communication. Can only connect to one endpoint.
    Open -> BindEndpoint -> SetReceiveMessageHandleFunction -> Connect -> Send -> Close
*/
class SOCKET_DLLAPI CACHE_ALIGN ZUDPPeerP2P : public ZObject {
public:
    using SuperType_ = ZObject;

    static inline constexpr TimeType kDefaultConnectMaxTimeMs = 30000;
    static inline constexpr TimeType kDefaultConnectReTryIntervalMs = 30;
    static inline constexpr TimeType kDefaultKeepAliveIntervalMs = 2000;
    static inline constexpr TimeType kDefaultDisconnectTimeMs = 5000;

    static inline constexpr SizeType kOutputMessageQueueSize = 4096ULL;
    static inline constexpr SizeType kInputMessageQueueSize = 4096ULL;

    enum class StateEnum_ : Int32 {
        kClosed,
        kOpened,
        kConnecting,
        kConnected,
        kDisconnected,
        kError
    };

    enum class ModeEnum_ : Int32 {
        kNormal,
        kBalance,
        kFast
    };

public:
    ZUDPPeerP2P(
        ZIOContext& _io_context, 
        TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _receive_message_data_handle_func
    ) noexcept;

    ZUDPPeerP2P(const ZUDPPeerP2P&) = delete;
    ZUDPPeerP2P(ZUDPPeerP2P&&) = delete;

    ~ZUDPPeerP2P() noexcept;

    ZUDPPeerP2P& operator=(const ZUDPPeerP2P&) = delete;
    ZUDPPeerP2P& operator=(ZUDPPeerP2P&&) = delete;

    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_.Load(); }

    NODISCARD FORCEINLINE ZUDPEndpoint GetLocalEndpoint() const noexcept { return socket_.GetLocalEndpoint(); }

    NODISCARD FORCEINLINE SizeType GetReadableDataSize() noexcept { return socket_.GetReadableDataSize(); }

    NODISCARD FORCEINLINE ZUDPSocket& GetSocket() noexcept { return socket_; }
    NODISCARD FORCEINLINE const ZUDPSocket& GetSocket() const noexcept { return socket_; }
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return socket_.GetIOContext(); }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetLinkObjectPtr() const noexcept { return socket_.GetLinkObjectPtr(); }

    NODISCARD FORCEINLINE ReturnType SetOSWriteBufferSize(Int32 _size) noexcept { 
        return socket_.SetOSWriteBufferSize(_size); 
    }

    NODISCARD FORCEINLINE ReturnType SetOSReadBufferSize(Int32 _size) noexcept {
        return socket_.SetOSReadBufferSize(_size);

    }

    NODISCARD FORCEINLINE ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept {
        return socket_.SetIfReuseAddress(_if_reuse);
    }

    FORCEINLINE Void SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
        socket_.SetAsyncErrorHandleFunction(std::forward<TFunction<Void(ReturnType)>>(_handle_func));
    }

    FORCEINLINE Void SetReceiveMessagedDataHandleFunction(
        TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _handle_func
    ) noexcept {
        receive_message_data_handle_func_ = std::forward<TFunction<Void(ZUDPPeerP2P&, ZMemory&)>>(_handle_func);
    }

    template<typename _ObjectType>
    FORCEINLINE Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        socket_.SetLinkObjectPtr(_obj_ptr);
    }

    FORCEINLINE Void SetKeepAliveInterval(Int32 _interval_ms) noexcept {
        keep_alive_interval_ms_ = _interval_ms;
    }

    FORCEINLINE Void SetDisconnectTimeMs(Int32 _time_ms) noexcept {
        disconnect_time_ms_ = _time_ms;
    }

    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /*
        Bind endpoint. Call before connected.
    */
    NODISCARD ReturnType BindEndpoint(const ZUDPEndpoint& _endpoint) noexcept;

    /*
        Connect to peer or server. Return after connected or timeout.
    */
    NODISCARD ReturnType Connect(
        const ZUDPEndpoint& _endpoint,
        UInt32 _conv_id,
        ModeEnum_ _mode,
        TimeType _max_connect_time_ms = kDefaultConnectMaxTimeMs,
        TimeType _retry_interval_ms = kDefaultConnectReTryIntervalMs
    ) noexcept;

    NODISCARD ReturnType Reconnect(
        const ZUDPEndpoint& _endpoint,
        TimeType _max_connect_time_ms = kDefaultConnectMaxTimeMs,
        TimeType _retry_interval_ms = kDefaultConnectReTryIntervalMs
    ) noexcept;


    /*
        Close the connection.
    */
    Void Close() noexcept;

    NODISCARD ReturnType Send(TUniquePointer<ZUDPMessage> _msg_ptr) noexcept;

private:
    static Void OutputThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept;
    static Void InputThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept;   
    static Void MessageDataHandleThreadFuncP(ZUDPPeerP2P* _peer_ptr) noexcept;

    static Int32 KCPOutputCallbackP(const Char* _buf, Int32 _size, ikcpcb* _kcp_ptr, Void* _peer_ptr) noexcept;

    NODISCARD ReturnType PuchHoleP(
        const ZUDPEndpoint& _endpoint,
        UInt32 _conv_id, 
        TimeType _max_connect_time_ms,
        TimeType _retry_interval_ms
    ) noexcept;
private:
    TAtom<StateEnum_> state_;

    ZUDPSocket socket_;
    ZKCPSession kcp_session_;
    ZMutex kcp_session_mutex_;
    Int32 keep_alive_interval_ms_;
    

    ZThread output_thread_;
    ZConditionVariable output_thread_cv_;
    ZMutex output_thread_mutex_;

    ZThread input_thread_;
    ZConditionVariable input_thread_cv_;
    ZMutex input_thread_mutex_;

    ZThread message_data_handle_thread_;
    ZConditionVariable message_data_handle_thread_cv_;
    ZMutex message_data_handle_thread_mutex_;


    TFixedMemory<ZKCPSession::kMaxPacketSize> receive_packet_buffer_;
    TFunction<Void(ZUDPPeerP2P&, ZMemory&)> receive_message_data_handle_func_;
    CACHE_ALIGN TAtom<UInt32> last_message_receive_time_ms_;
    UInt32 disconnect_time_ms_;

    TQueueSafe<TUniquePointer<ZUDPMessage>> output_message_queue_;
    TQueueSafe<ZMemory> input_message_data_queue_;
};

} // socket
} // zengine