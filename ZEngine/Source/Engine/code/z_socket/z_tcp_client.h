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
#include "z_core/t_smart_pointer.h"
#include "z_core/z_object.h"

#include "z_tcp_socket.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPClientData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Simple one-on-one socket tcp client.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPClient : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

    enum State_ {
        ZTCPClientState_Idle,
        ZTCPClientState_Connect,
        ZTCPClientState_Error
    };

    ZTCPClient() noexcept;

    ~ZTCPClient() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

    /*
        Resolve domain and set endpoint. Call before Connect() or after Close().
    */
    NODISCARD ReturnType SetEndpoints(const Char* _domain_str) noexcept;

    /*
        Resolve address and port and set endpoint. Call before Connect() or after Close().
    */
    NODISCARD ReturnType SetEndpoints(const Char* _address_str, const Char* _port_str) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

    /*
        Close the connection.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the client to idle.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Coonect to server.
    */
    NODISCARD ReturnType Connect(Int32 _repeat_times = kConnectRetryForever) noexcept;

    /*
        Get socket ptr.
        WARNING: Moving the socket data might cause fatal errors.
    */
    NODISCARD FORCEINLINE ZTCPSocket* GetSocketPtr() noexcept { return &socket_; }

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
        _handle_func(Void* _data_buffer, SizeType _read_length)
    */
    NODISCARD ReturnType AsyncRead(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TFunction<Void(Void*, SizeType)>& _handle_func
    ) noexcept;
    /*
        Read a message from the client. Will not suspend the current thread.
        _handle_func only needs to handle the message recieved.
        _handle_func(Void* _data_buffer, SizeType _read_length)
    */
    NODISCARD ReturnType AsyncRead(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TSimpleFunction<Void(Void*, SizeType)>& _handle_func
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
        _handle_func(Void* _data_buffer, SizeType _read_length)
    */
    NODISCARD ReturnType AsyncWrite(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TFunction<Void(Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Send a message to the client. Will not suspend the current thread.
        _handle_func will be called after the message send.
        _handle_func(Void* _data_buffer, SizeType _read_length)
    */
    NODISCARD ReturnType AsyncWrite(
        Void* _data_buffer,
        Int32 _buffer_size,
        const TSimpleFunction<Void(Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Starts to deal with async operation until server closed or client disconnnected.
    */
    NODISCARD ReturnType AsyncRun() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;

private:
    ZTCPClient(const ZTCPClient&) = delete;
    ZTCPClient(ZTCPClient&&) = delete;
    ZTCPClient& operator=(const ZTCPClient&) = delete;
    ZTCPClient& operator=(ZTCPClient&&) = delete;

private:
    TUniquePointer<internal::ZTCPClientData> data_ptr_;
    ZTCPSocket socket_;
    State_ state_;
};

}//socket
}//zengine