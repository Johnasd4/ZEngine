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

#include "drive.h"

#include "../z_core/t_function.h"
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

#include "z_tcp_endpoint.h"
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
    enum State_ : Int32 {
        ZTCPSingleSessionServerState_Uninitialized,
        ZTCPSingleSessionServerState_Idle,
        ZTCPSingleSessionServerState_Listen,
        ZTCPSingleSessionServerState_Connected,
        ZTCPSingleSessionServerState_Error
    };

    ZTCPSingleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPSingleSessionServer() noexcept;

    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }

    /*
        Bind endpoint by endpoint. Call before Listen() or after Close().
    */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept;

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
        ZBuffer _buffer,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncRead(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Read data until match char. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match string. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match char. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until match string. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until close. Will suspend the current thread until close.
    */
    NODISCARD ReturnType ReadUntilClose(
        ZBufferStream* _buffer_ptr,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Write data. Will suspend the current thread until data write.
    */
    NODISCARD ReturnType Write(
        ZConstBuffer _buffer
    ) noexcept;

    /*
        Write data. Will not suspend the current thread.
        _handle_func will be called after the data send.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncWrite(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
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
    enum State_ : Int32 {
        ZTCPMultipleSessionServerState_Uninitialized,
        ZTCPMultipleSessionServerState_Idle,
        ZTCPMultipleSessionServerState_Listen,
        ZTCPMultipleSessionServerState_Error
    };

    ZTCPMultipleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPMultipleSessionServer() noexcept;

    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }

    /*
        Bind endpoint. Call before Listen() or after Close().
    */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept;

    /*
        Starts the server.
    */
    NODISCARD ReturnType Listen() noexcept;

    /*
        Close the server.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Wait for a client to connected. Will not suspend the current thread.
        _handle_func will be called after a client is connected.
        _handle_func(ReturnType _error_code, ZTCPMultipleSessionServer* _server_ptr, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncAccept(
        const TFunction<Void(ReturnType, ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
    ) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE TPoolListSafe<ZTCPSocket>& GetSocketPoolList() noexcept { return socket_pool_list_; }

    /*
        Write data to all server. Will not suspend the current thread.
        _handle_func will be called after the data send.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncBroadcast(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
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