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

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSocketData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Simple one-on-one socket tcp client.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSocket : public ZObject {
public:
    enum State_ {
        ZTCPSocketState_Uninitialized,
        ZTCPSocketState_Idle,
        ZTCPSocketState_Connect,
        ZTCPSocketState_Error
    };

    ZTCPSocket() noexcept;
    ZTCPSocket(ZTCPSocket&& _socket) noexcept;

    ~ZTCPSocket() noexcept;

    ZTCPSocket& operator=(ZTCPSocket&& _socket) noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

    /*
        Sets the socket buffer size. Call after connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

    /*
        Close the socket.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Reset the socket.
    */
    NODISCARD ReturnType Reset() noexcept;

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

protected:
    using SuperType_ = ZObject;
    friend class ZTCPClient;
    friend class ZTCPSingleSessionServer;

private:
    ZTCPSocket(const ZTCPSocket&) = delete;
    ZTCPSocket& operator=(const ZTCPSocket&) = delete;

    ZTCPSocket(ZTCPClient* _client_ptr) noexcept;
    ZTCPSocket(ZTCPSingleSessionServer* _server_ptr) noexcept;

    /*
        Initialize socket.
    */
    NODISCARD ReturnType Initialize(ZTCPClient* _client_ptr) noexcept;
    /*
        Initialize socket.
    */
    NODISCARD ReturnType Initialize(ZTCPSingleSessionServer* _server_ptr) noexcept;

    Void MoveP(ZTCPSocket&& _socket) noexcept;

private:
    TUniquePointer<internal::ZTCPSocketData> data_ptr_;
    State_ state_;
};

}//socket
}//zengine