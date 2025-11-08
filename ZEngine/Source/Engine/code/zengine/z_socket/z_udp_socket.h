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
        Reset the socket.
    */
    NODISCARD ReturnType Reset() noexcept;

    /*
        Conect to target socket. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        const Char* _address_str,
        Int32 _port,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Receive message. Will suspend the current thread until a message received.
    */
    NODISCARD ReturnType ReceiveFrom(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        ZString* _address_str_ptr = nullptr,
        Int32* _port_ptr = nullptr,
        SizeType* _message_size_ptr = nullptr
    ) noexcept;

    /*
        Receive message. Will not suspend the current thread.
        _handle_func only needs to handle the message recieved.
        _handle_func(
            ZUDPSocket* _socket_ptr, 
            const Void* _message_ptr, 
            SizeType _message_size,
            const Char* _address_str,
            Int32 _port
        )
    */
    NODISCARD ReturnType AsyncReceiveFrom(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        const TFunction<Void(ZUDPSocket*, const Void*, SizeType, const Char*, Int32)>& _handle_func
    ) noexcept;

    /*
        Receive message. Will suspend the current thread until a message received.
        Call Connect() before this function.
    */
    NODISCARD ReturnType Receive(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        SizeType* _message_size_ptr = nullptr
    ) noexcept;

    /*
        Receive message. Will not suspend the current thread.
        Call Connect() before this function.
        _handle_func only needs to handle the message recieved.
        _handle_func(ZUDPSocket* _socket_ptr, const Void* _message_ptr, SizeType _message_size)
    */
    NODISCARD ReturnType AsyncReceive(
        Void* _buffer_ptr,
        SizeType _buffer_size,
        const TFunction<Void(ZUDPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Send message. Will suspend the current thread until the message send.
        Call Connect() before this function.
    */
    NODISCARD ReturnType SendTo(
        const Void* _data_ptr,
        SizeType _data_size,
        const Char* _address_str,
        Int32 _port
    ) noexcept;

    /*
        Send message. Will not suspend the current thread.
        _handle_func will be called after the message send.
        _handle_func(ZUDPSocket* _socket_ptr, const Void* _message_ptr, SizeType _message_size)
    */
    NODISCARD ReturnType AsyncSendTo(
        const Void* _data_ptr,
        SizeType _data_size,
        const Char* _address_str,
        Int32 _port,
        const TFunction<Void(ZUDPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

    /*
        Send message. Will suspend the current thread until the message send.
        Call Connect() before this function.
    */
    NODISCARD ReturnType Send(
        const Void* _data_ptr,
        SizeType _data_size
    ) noexcept;

    /*
        Send message. Will not suspend the current thread.
        Call Connect() before this function.
        _handle_func will be called after the message send.
        _handle_func(ZUDPSocket* _socket_ptr, const Void* _message_ptr, SizeType _message_size)
    */
    NODISCARD ReturnType AsyncSend(
        const Void* _data_ptr,
        SizeType _data_size,
        const TFunction<Void(ZUDPSocket*, const Void*, SizeType)>& _handle_func
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