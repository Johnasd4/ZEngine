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
#include "z_core/z_object.h"

#include "z_tcp_socket.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSingleSessionServerData;
struct ZTCPMultipleSessionServerData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

using ZTCPServer = ZTCPSingleSessionServer;

/*
    Single session tcp server. Only one client is allowed to connect at a time.
    SetEndpoint -> Listen -> Accept -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSingleSessionServer : public ZObject {
public:
    enum State_ {
        ZTCPSingleSessionServerState_Uninitialized,
        ZTCPSingleSessionServerState_Idle,
        ZTCPSingleSessionServerState_Listen,
        ZTCPSingleSessionServerState_Connect,
        ZTCPSingleSessionServerState_Error
    };

    ZTCPSingleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPSingleSessionServer() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() noexcept { return io_context_ptr_; }

    /*
        Bind endpoint by address and port. Call before Listen() or after Close().
    */
    NODISCARD ReturnType BindEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;
    /*
        Sets os read buffer size. Call after connected.
    */
    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;

    /*
        Starts the server.
    */
    NODISCARD ReturnType Listen() noexcept;

    /*
        Close the server.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the server to idle.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Suspend the current thread until a client is connected.
    */
    NODISCARD ReturnType Accept() noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE ZTCPSocket& GetSocket() noexcept { return socket_; }

    /*
        Read data. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType Read(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ZTCPSocket* _socket_ptr, const Void* _buffer_ptr, SizeType _data_size)
    */
    NODISCARD ReturnType AsyncRead(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Write data. Will suspend the current thread until data write.
    */
    NODISCARD ReturnType Write(
        const Void* _data_ptr,
        SizeType _data_size
    ) noexcept;

    /*
        Write data. Will not suspend the current thread.
        _handle_func will be called after the data send.
        _handle_func(ZTCPSocket* _socket_ptr, const Void* _data_ptr, SizeType _data_size)
    */
    NODISCARD ReturnType AsyncWrite(
        const Void* _data_ptr,
        SizeType _data_size,
        const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Return the size of the data can be read.
    */
    NODISCARD FORCEINLINE SizeType ReadableDataSize() noexcept {
        return socket_.ReadableDataSize();
    }

protected:
    using SuperType_ = ZObject;

private:
    ZTCPSingleSessionServer(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer(ZTCPSingleSessionServer&&) = delete;
    ZTCPSingleSessionServer& operator=(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer& operator=(ZTCPSingleSessionServer&&) = delete;

private:
    TUniquePointer<internal::ZTCPSingleSessionServerData> data_ptr_;
    ZTCPSocket socket_;
    ZIOContext* io_context_ptr_;
    State_ state_;
};

/*
    Multiple session tcp server. Multiple clients are allowed to connect at a time.
    SetEndpoint -> Listen -> Async accept -> Handle socket -> Close
*/
class SOCKET_DLLAPI ZTCPMultipleSessionServer : public ZObject {
public:
    enum State_ {
        ZTCPMultipleSessionServerState_Uninitialized,
        ZTCPMultipleSessionServerState_Idle,
        ZTCPMultipleSessionServerState_Listen,
        ZTCPMultipleSessionServerState_Error
    };

    ZTCPMultipleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPMultipleSessionServer() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() noexcept { return io_context_ptr_; }

    /*
        Bind endpoint by address and port. Call before Listen() or after Close().
    */
    NODISCARD ReturnType BindEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Starts the server.
    */
    NODISCARD ReturnType Listen() noexcept;

    /*
        Close the server.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the server to idle.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Wait for a client to connected. Will not suspend the current thread.
        _handle_func will be called after a client is connected.
        _handle_func(ZTCPMultipleSessionServer* _server_ptr, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncAccept(
        const TFunction<Void(ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
    ) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE TPoolListSafe<ZTCPSocket>& GetSocketPoolList() noexcept { return socket_pool_list_; }

    /*
        Write data to all server. Will not suspend the current thread.
        _handle_func will be called after the data send.
        _handle_func(ZTCPSocket* _socket_ptr, const Void* _data_ptr, SizeType _data_size)
    */
    NODISCARD ReturnType AsyncBroadcast(
        const Void* _data_ptr,
        SizeType _data_size,
        const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZTCPMultipleSessionServer(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer(ZTCPMultipleSessionServer&&) = delete;
    ZTCPMultipleSessionServer& operator=(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer& operator=(ZTCPMultipleSessionServer&&) = delete;

private:
    TUniquePointer<internal::ZTCPMultipleSessionServerData> data_ptr_;
    TPoolListSafe<ZTCPSocket> socket_pool_list_;
    ZIOContext* io_context_ptr_;
    State_ state_;

};

}//socket
}//zengine