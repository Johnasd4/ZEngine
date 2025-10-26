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
        ZTCPSingleSessionClientState_Uninitialized,
        ZTCPSingleSessionClientState_Idle,
        ZTCPSingleSessionClientState_Connect,
        ZTCPSingleSessionClientState_Error
    };

    ZTCPSingleSessionClient(ZIOContext* _io_context_ptr) noexcept;

    ~ZTCPSingleSessionClient() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() noexcept { return io_context_ptr_; }

    /*
        Bind endpoint by address and port. Call before connected.
    */
    NODISCARD ReturnType BindEndpoint(const Char* _address_str, Int32 _port) noexcept;

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
    ZTCPSingleSessionClient(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient(ZTCPSingleSessionClient&&) = delete;
    ZTCPSingleSessionClient& operator=(const ZTCPSingleSessionClient&) = delete;
    ZTCPSingleSessionClient& operator=(ZTCPSingleSessionClient&&) = delete;

private:
    TUniquePointer<internal::ZTCPSingleSessionClientData> data_ptr_;
    ZTCPSocket socket_;
    ZIOContext* io_context_ptr_;
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
        ZTCPMultipleSessionClientState_Uninitialized,
        ZTCPMultipleSessionClientState_Idle,
        ZTCPMultipleSessionClientState_Error
    };

    ZTCPMultipleSessionClient(ZIOContext* _context_ptr) noexcept;

    ~ZTCPMultipleSessionClient() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() noexcept { return io_context_ptr_; }

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
        Write data to all client. Will not suspend the current thread.
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
    ZTCPMultipleSessionClient(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient(ZTCPMultipleSessionClient&&) = delete;
    ZTCPMultipleSessionClient& operator=(const ZTCPMultipleSessionClient&) = delete;
    ZTCPMultipleSessionClient& operator=(ZTCPMultipleSessionClient&&) = delete;

private:
    TUniquePointer<internal::ZTCPMultipleSessionClientData> data_ptr_;
    TPoolListSafe<ZTCPSocket> socket_pool_list_;
    ZIOContext* io_context_ptr_;
    State_ state_;
};

}//socket
}//zengine