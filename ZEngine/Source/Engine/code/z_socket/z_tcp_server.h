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

#include "z_socket.h"

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

/*
    single session tcp server. Only one client is allowed to connect at a time.
    SetEndpoint -> Listen -> Accept -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSingleSessionServer : public ZObject {
public:
    enum State_ {
        ZTCPSingleSessionServerState_Idle,
        ZTCPSingleSessionServerState_Listen,
        ZTCPSingleSessionServerState_Connect,
        ZTCPSingleSessionServerState_Error
    };

    ZTCPSingleSessionServer() noexcept;

    ~ZTCPSingleSessionServer() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

    /*
        Sets the address and port. Call before Listen() or after Close().
    */
    NODISCARD ReturnType SetEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

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
    ZTCPSingleSessionServer(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer(ZTCPSingleSessionServer&&) = delete;
    ZTCPSingleSessionServer& operator=(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer& operator=(ZTCPSingleSessionServer&&) = delete;

private:
    TUniquePointer<internal::ZTCPSingleSessionServerData> data_ptr_;
    ZTCPSocket socket_;
    State_ state_;
};

/*
    Multiple session tcp server. Multiple clients are allowed to connect at a time.
    SetEndpoint -> Listen -> Accept -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPMultipleSessionServer : public ZObject {
public:
    enum State_ {
        ZTCPMultipleSessionServerState_Idle,
        ZTCPMultipleSessionServerState_Execute,
        ZTCPMultipleSessionServerState_Error
    };

    ZTCPMultipleSessionServer() noexcept;

    ~ZTCPMultipleSessionServer() noexcept;

    NODISCARD State_ State() noexcept;

    /*
        Sets the address and port. Call before Listen() or after Close().
    */
    NODISCARD ReturnType SetEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

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
        Allow a client connection.
    */
    NODISCARD ReturnType AsyncAccept(const TFunction<Void()>& _handle_func) noexcept;

    /*
        Allow a client connection.
    */
    NODISCARD ReturnType AsyncAccept(const TSimpleFunction<Void()>& _handle_func) noexcept;

    /*
        Read a message from the client. Will suspend the current thread until a message is received.
    */
    NODISCARD ReturnType Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr = nullptr) noexcept;

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
    NODISCARD ReturnType Write(const Void* _data_ptr, SizeType _date_size) noexcept;

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

private:
    ZTCPMultipleSessionServer(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer(ZTCPMultipleSessionServer&&) = delete;
    ZTCPMultipleSessionServer& operator=(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer& operator=(ZTCPMultipleSessionServer&&) = delete;

private:
    TUniquePointer<internal::ZTCPMultipleSessionServerData> data_ptr_;
    ZTCPSocket socket_;
    State_ state_;

};

}//socket
}//zengine