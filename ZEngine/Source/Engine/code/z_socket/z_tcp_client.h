/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "internal/z_drive.h"

#include "z_core/t_function.h"
#include "z_core/t_pool_list.h"
#include "z_core/t_smart_pointer.h"
#include "z_core/t_unordered_map.h"
#include "z_core/z_object.h"
#include "z_core/z_string.h"

#include "z_tcp_socket.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSingleSessionClientData;
struct ZTCPMultipleSessionClientData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

using ZTCPClient = ZTCPSingleSessionClient;

/*
    Single session tcp client. Can connect one server at a time.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSingleSessionClient : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

    enum State_ {
        ZTCPSingleSessionClientState_Idle,
        ZTCPSingleSessionClientState_Connect,
        ZTCPSingleSessionClientState_Error
    };

    ZTCPSingleSessionClient() noexcept;

    ~ZTCPSingleSessionClient() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

    /*
        Sets os write buffer size. Call after connected.
    */
    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;
    /*
        Sets os read buffer size. Call after connected.
    */
    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;

    /*
        Close the connection.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the client to idle.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Coonect to server. Will suspend the current thread.
        Use ':' to split address and port
    */
    NODISCARD ReturnType Connect(
        const Char* _domain_str,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Coonect to server. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        const Char* _address_str, 
        const Char* _port_str, 
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE ZTCPSocket& GetSocket() noexcept { return socket_; }

    /*
        Read a message from the client. Will suspend the current thread until a message is received.
    */
    NODISCARD ReturnType Read(
        Void* _data_buffer, 
        Int32 _buffer_size, 
        SizeType* _message_size_ptr = nullptr
    ) noexcept;

    /*
        Read a message from the client. Will not suspend the current thread.
        _handle_func only needs to handle the message recieved.
        _handle_func(ZTCPSocket* _socket_ptr, Void* _data_buffer, SizeType _read_length)
    */
    NODISCARD ReturnType AsyncRead(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Send a message to the client.
    */
    NODISCARD ReturnType Write(
        const Void* _data_buffer,
        SizeType _date_size
    ) noexcept;

    /*
        Send a message to the client. Will not suspend the current thread.
        _handle_func will be called after the message send.
        _handle_func(ZTCPSocket* _socket_ptr, Void* _data_buffer, SizeType _write_length)
    */
    NODISCARD ReturnType AsyncWrite(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Deal with async operation until all operation finished.
        Suspend the current thread, returns until finish.
    */
    NODISCARD ReturnType Run() noexcept;

    /*
        Deal with async operation until all operation finished.
        Starts a new thread and returns immediately.
    */
    NODISCARD ReturnType AsyncRun() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;

private:
    ZTCPSingleSessionClient(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient(ZTCPSingleSessionClient&&) = delete;
    ZTCPSingleSessionClient& operator=(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient& operator=(ZTCPSingleSessionClient&&) = delete;

private:
    TUniquePointer<internal::ZTCPSingleSessionClientData> data_ptr_;
    ZTCPSocket socket_;
    State_ state_;
};

/*
    Multiple session tcp client. Can connect multiple server at a time.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPMultipleSessionClient : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

    enum State_ {
        ZTCPMultipleSessionClientState_Idle,
        ZTCPMultipleSessionClientState_Error
    };

    ZTCPMultipleSessionClient() noexcept;

    ~ZTCPMultipleSessionClient() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

    /*
        Close the connection.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the client to idle.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Coonect to server. Will not suspend the current thread.
        _handle_func will be called after connected.
        _handle_func(ZTCPMultipleSessionClient* _server_ptr, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncConnect(
        const Char* _domain_str,
        const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Coonect to server. Will not suspend the current thread.
        _handle_func will be called after connected.
        _handle_func(ZTCPMultipleSessionClient* _server_ptr, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncConnect(
        const Char* _address_str, 
        const Char* _port_str,
        const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE TPoolListSafe<ZTCPSocket>& GetSocketPoolList() noexcept { return socket_pool_list_; }

    /*
        Send a message to all clients. Will not suspend the current thread.
        _handle_func will be called after the message send.
        _handle_func(ZTCPSocket* _socket_ptr, Void* _data_buffer, SizeType _write_length)
    */
    NODISCARD ReturnType AsyncBroadcast(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Deal with async operation until all operation finished.
        Suspend the current thread, returns until finish.
    */
    NODISCARD ReturnType Run() noexcept;

    /*
        Deal with async operation until all operation finished.
        Starts a new thread and returns immediately.
    */
    NODISCARD ReturnType AsyncRun() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;

private:
    ZTCPMultipleSessionClient(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient(ZTCPMultipleSessionClient&&) = delete;
    ZTCPMultipleSessionClient& operator=(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient& operator=(ZTCPMultipleSessionClient&&) = delete;

    /*
        Async connect execute func.
    */
    NODISCARD ReturnType AsyncConnectExecuteP(
        ZString&& _address_str,
        ZString&& _port_str,
        Void* _endpoints_ptr,
        const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times,
        Int32 _reconnect_times
    ) noexcept;

private:
    TUniquePointer<internal::ZTCPMultipleSessionClientData> data_ptr_;
    TPoolListSafe<ZTCPSocket> socket_pool_list_;
    State_ state_;
};

}//socket
}//zengine