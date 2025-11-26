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
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "z_buffer.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZUDPSocketData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    UDP socket type.
    Contains a extra ptr that can be linked to any object. Call LinkObjectPtr<_ObjectType>() to get the object ptr.
*/
class SOCKET_DLLAPI ZUDPSocket : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

public:
    enum State_ : Int32 {
        ZUDPSocketState_Uninitialized,
        ZUDPSocketState_Idle,
        ZUDPSocketState_Error
    };

    ZUDPSocket() noexcept;
    ZUDPSocket(const ZUDPSocket& _socket) noexcept;
    ZUDPSocket(ZUDPSocket&& _socket) noexcept;
    ZUDPSocket(ZIOContext* _context_ptr) noexcept;

    ~ZUDPSocket() noexcept;

    ZUDPSocket& operator=(const ZUDPSocket& _socket) noexcept;
    ZUDPSocket& operator=(ZUDPSocket&& _socket) noexcept;


    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return link_object_ptr_; }

    /*
        Initialize socket.
    */
    NODISCARD ReturnType Initialize(ZIOContext* _context_ptr) noexcept;

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
        Sets the aysnc error handle func, called when aysnc error happens.
    */
    NODISCARD Void SetAsyncErrorHandleFunction(TFunction<Void()>&& _handle_func) noexcept;

    /*
        Cancel async operation.
    */
    NODISCARD ReturnType Cancel() noexcept;

    /*
        Close the socket.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Conect to target socket. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        ZStringView _address_str,
        ZStringView _port_str
    ) noexcept;

    /*
        Receive message. Will suspend the current thread until a message received.
    */
    NODISCARD ReturnType ReceiveFrom(
        ZBuffer _buffer,
        SizeType* _message_size_ptr = nullptr,
        ZString* _address_str_ptr = nullptr,
        Int32* _port_ptr = nullptr
    ) noexcept;

    /*
        Receive message. Will not suspend the current thread.
        _handle_func only needs to handle the message recieved.
        _handle_func(
            ZUDPSocket* _socket_ptr, 
            const ZConstBuffer _buffer,
            const Char* _address_str,
            Int32 _port
        )
    */
    NODISCARD ReturnType AsyncReceiveFrom(
        ZBuffer _buffer,
        const TFunction<Void(ZUDPSocket*, const ZConstBuffer, const Char*, Int32)>& _handle_func
    ) noexcept;

    /*
        Receive message. Will suspend the current thread until a message received.
        Call Connect() before this function.
    */
    NODISCARD ReturnType Receive(
        ZBuffer _buffer,
        SizeType* _message_size_ptr = nullptr
    ) noexcept;

    /*
        Receive message. Will not suspend the current thread.
        Call Connect() before this function.
        _handle_func only needs to handle the message recieved.
        _handle_func(ZUDPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncReceive(
        ZBuffer _buffer,
        const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Send message. Will suspend the current thread until the message send.
        Call Connect() before this function.
    */
    NODISCARD ReturnType SendTo(
        ZStringView _address_str,
        Int32 _port,
        ZConstBuffer _buffer
    ) noexcept;

    /*
        Send message. Will not suspend the current thread.
        _handle_func will be called after the message send.
        _handle_func(ZUDPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncSendTo(
        ZStringView _address_str,
        Int32 _port,
        ZConstBuffer _buffer,
        const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Send message. Will suspend the current thread until the message send.
        Call Connect() before this function.
    */
    NODISCARD ReturnType Send(
        ZConstBuffer _buffer
    ) noexcept;

    /*
        Send message. Will not suspend the current thread.
        Call Connect() before this function.
        _handle_func will be called after the message send.
        _handle_func(ZUDPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncSend(
        ZConstBuffer _buffer,
        const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Return the size of the data can be read.
    */
    NODISCARD SizeType ReadableDataSize() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSingleSessionClient;
    friend class ZTCPMultipleSessionClient;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:
    Void MoveP(ZUDPSocket&& _socket) noexcept;

private:
    TUniquePointer<internal::ZUDPSocketData> data_ptr_;
    ZIOContext* io_context_ptr_;
    Void* link_object_ptr_;
    State_ state_;
};

}//socket
}//zengine