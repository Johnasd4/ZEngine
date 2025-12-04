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
    Open -> BindEndpoint -> Listen -> Accept -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSingleSessionServer : public ZObject {
public:
    enum class StateEnum_ : Int32 {
        kUninitialized,
        kClosed,
        kOpened,
        kEndpointBind,
        kListen,
        kConnected,
        kError
    };

    static constexpr SizeType kDefaultMaxWaitConnectClientNum = 128ULL;

    ZTCPSingleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPSingleSessionServer() noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return socket_.LinkObjectPtr(); }
    NODISCARD FORCEINLINE ZTCPSocket* SocketPtr() noexcept { return &socket_; }
    template<typename _ObjectType>
    NODISCARD Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        socket_.SetLinkObjectPtr(_obj_ptr);
    }

    /*
        Open the server.
    */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

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
        Set if address is reuseable. If true, can bind multiple sockets to the same address.
        Call before binding endpoint. Must be called on all sockets that bind to the same address.
    */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /*
        Starts listening, client can connect.
    */
    NODISCARD ReturnType Listen(Int32 _max_wait_connect_client_num = kDefaultMaxWaitConnectClientNum) noexcept;

    /*
        Cancel async operation.
    */
    Void Cancel() noexcept;

    /*
        Close the server.
    */
    Void Close() noexcept;

    /*
        Suspend the current thread until a client is connected.
    */
    NODISCARD ReturnType Accept() noexcept;

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
        ZSocketBufferStream* _buffer_ptr,
        Char _match_char,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match string. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType ReadUntil(
        ZSocketBufferStream* _buffer_ptr,
        const Char* _match_str,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match char. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZSocketBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until match string. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZSocketBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until close. Will suspend the current thread until close.
    */
    NODISCARD ReturnType ReadUntilClose(
        ZSocketBufferStream* _buffer_ptr,
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
    StateEnum_ state_;
};

/*
    Multiple session tcp server. Multiple clients are allowed to connect at a time.
    SetEndpoint -> Listen -> Async accept -> Handle socket -> Close
*/
class SOCKET_DLLAPI ZTCPMultipleSessionServer : public ZObject {
public:
    enum class StateEnum_ : Int32 {
        kUninitialized,
        kClosed,
        kOpened,
        kEndpointBind,
        kListen,
        kError
    };

    static constexpr SizeType kDefaultMaxWaitConnectClientNum = 128ULL;

    ZTCPMultipleSessionServer(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPMultipleSessionServer() noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }

    /*
        Open the server.
    */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /*
        Bind endpoint. Call before Listen() or after Close().
    */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept;

    /*
        Set if address is reuseable. If true, can bind multiple sockets to the same address.
        Call before binding endpoint. Must be called on all sockets that bind to the same address.
    */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /*
        Starts the server.
    */
    NODISCARD ReturnType Listen(Int32 _max_wait_connect_client_num = kDefaultMaxWaitConnectClientNum) noexcept;

    /*
        Close the server.
    */
    Void Close() noexcept;

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
    StateEnum_ state_;

};

}//socket
}//zengine