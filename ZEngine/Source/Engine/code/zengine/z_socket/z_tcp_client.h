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
#include "../z_core/t_hash_map.h"
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string.h"
#include "../z_core/z_string_view.h"

#include "z_tcp_endpoint.h"
#include "z_tcp_socket.h"

namespace zengine {
namespace socket {

using ZTCPClient = ZTCPSingleSessionClient;

/*
    Single session tcp client. Can connect one server at a time.
    Open -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSingleSessionClient : public ZObject {
public:
    using StateEnum_ = ZTCPSocket::StateEnum_;

    static inline constexpr Int32 kConnectRetryForever = kInt32Max;
    
    ZTCPSingleSessionClient(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPSingleSessionClient() noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return socket_.State(); }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return socket_.LinkObjectPtr(); }
    NODISCARD FORCEINLINE ZTCPSocket* SocketPtr() noexcept { return &socket_; }

    /*
        Open the client.
    */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /*
        Bind endpoint. Call before connected.
    */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept;

    /*
        Sets os write buffer size. Call after connected.
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
        Cancel async operation.
    */
    Void Cancel() noexcept;

    /*
        Close the connection.
    */
    Void Close() noexcept;

    /*
        Connect to server. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        const ZTCPEndpoint& _tcp_endpoint,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Stops connecting.
    */
    NODISCARD Void StopConnect() noexcept;

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
    ZTCPSingleSessionClient(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient(ZTCPSingleSessionClient&&) = delete;
    ZTCPSingleSessionClient& operator=(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient& operator=(ZTCPSingleSessionClient&&) = delete;

private:
    ZTCPSocket socket_;
    ZIOContext* io_context_ptr_;
};

/*
    Multiple session tcp client. Can connect multiple server at a time.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPMultipleSessionClient : public ZObject {
public:
    static inline constexpr Int32 kConnectRetryForever = kInt32Max;

    ZTCPMultipleSessionClient(ZIOContext* _context_ptr) noexcept;

    ~ZTCPMultipleSessionClient() noexcept;

    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }

    /*
        Close all connections.
    */
    Void Close() noexcept;

    /*
        Connect to server. Will not suspend the current thread.
        _handle_func will be called after connected.
        _handle_func(ReturnType _error_code, ZTCPMultipleSessionClient* _server_ptr, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncConnect(
        const ZTCPEndpoint& _tcp_endpoint,
        const TFunction<Void(ReturnType, ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever,
        ZTCPSocket** _tcp_socket_ptr_ptr = nullptr
    ) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE TPoolListSafe<ZTCPSocket>& GetSocketPoolList() noexcept { return socket_pool_list_; }

    /*
        Write data to all client. Will not suspend the current thread.
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
    ZTCPMultipleSessionClient(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient(ZTCPMultipleSessionClient&&) = delete;
    ZTCPMultipleSessionClient& operator=(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient& operator=(ZTCPMultipleSessionClient&&) = delete;

private:
    TPoolListSafe<ZTCPSocket> socket_pool_list_;
    ZIOContext* io_context_ptr_;
};

}//socket
}//zengine