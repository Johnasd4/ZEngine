///*
//    Copyright (c) YuLin Zhu
//
//    This code file is licensed under the Creative Commons
//    Attribution-NonCommercial 4.0 International License.
//
//    You may obtain a copy of the License at
//    https://creativecommons.org/licenses/by-nc/4.0/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    Author: YuLin Zhu
//    Contact: 1152325286@qq.com
//*/
//#pragma once
//
//#include "internal/z_drive.h"
//
//#include "z_core/t_function.h"
//#include "z_core/t_smart_pointer.h"
//#include "z_core/z_object.h"
//
//namespace zengine {
//namespace socket {
//namespace internal {
//
//struct ZUDPSocketData;
//
//}//internal
//}//socket
//}//zengine
//
//namespace zengine {
//namespace socket {
//
///*
//    TCP socket type.
//    Contains a extra ptr that can be linked to any object. Call LinkObjectPtr<_ObjectType>() to get the object ptr.
//*/
//class SOCKET_DLLAPI ZUDPSocket : public ZObject {
//public:
//    enum State_ {
//        ZUDPSocketState_Uninitialized,
//        ZUDPSocketState_Idle,
//        ZUDPSocketState_Connect,
//        ZUDPSocketState_Error
//    };
//
//    ZUDPSocket() noexcept;
//    ZUDPSocket(const ZUDPSocket& _socket) noexcept;
//    ZUDPSocket(ZUDPSocket&& _socket) noexcept;
//    ZUDPSocket(ZSocketContext* _context_ptr) noexcept;
//
//    ~ZUDPSocket() noexcept;
//
//    ZUDPSocket& operator=(const ZUDPSocket& _socket) noexcept;
//    ZUDPSocket& operator=(ZUDPSocket&& _socket) noexcept;
//
//    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
//    template<typename _ObjectType>
//    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() noexcept { return link_object_ptr_; }
//    NODISCARD const ZString& RemoteAddressString() noexcept;
//    NODISCARD const ZString& RemotePortString() noexcept;
//
//    /*
//        Initialize socket.
//    */
//    NODISCARD ReturnType Initialize(ZSocketContext* _context_ptr) noexcept;
//
//    /*
//        Sets os write buffer size. Call after connected.
//    */
//    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;
//    /*
//        Sets os read buffer size. Call after connected.
//    */
//    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;
//
//    /*
//        Close the socket.
//    */
//    NODISCARD ReturnType Close() noexcept;
//
//    /*
//        Reset the socket.
//    */
//    NODISCARD ReturnType Reset() noexcept;
//
//    /*
//        Receive message. Will suspend the current thread until a message received.
//    */
//    NODISCARD ReturnType Receive(
//        Void* _data_buffer, 
//        Int32 _buffer_size, 
//        SizeType* _message_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Receive message. Will not suspend the current thread.
//        _handle_func only needs to handle the message recieved.
//        _handle_func(ZUDPSocket* _socket_ptr, Void* _data_buffer, SizeType _read_length)
//    */
//    NODISCARD ReturnType AsyncReceive(
//        Void* _data_buffer,
//        Int32 _buffer_size,
//        const TFunction<Void(ZUDPSocket*, Void*, SizeType)>& _handle_func
//    ) noexcept;
//
//    /*
//        Send message.
//    */
//    NODISCARD ReturnType Send(
//        const Void* _data_buffer,
//        SizeType _date_size
//    ) noexcept;
//
//    /*
//        Send message. Will not suspend the current thread.
//        _handle_func will be called after the message send.
//        _handle_func(ZUDPSocket* _socket_ptr, Void* _data_buffer, SizeType _write_length)
//    */
//    NODISCARD ReturnType AsyncSend(
//        Void* _data_buffer,
//        Int32 _buffer_size,
//        const TFunction<Void(ZUDPSocket*, Void*, SizeType)>& _handle_func
//    ) noexcept;
//
//protected:
//    using SuperType_ = ZObject;
//    friend class ZTCPSingleSessionClient;
//    friend class ZTCPMultipleSessionClient;
//    friend class ZTCPSingleSessionServer;
//    friend class ZTCPMultipleSessionServer;
//
//private:
//    Void MoveP(ZUDPSocket&& _socket) noexcept;
//
//    /*
//        Sets the aysnc error handle func, called when aysnc error happens.
//    */
//    NODISCARD Void SetAsyncErrorHandleFunctionP(TFunction<Void()>&& _handle_func) noexcept;
//
//private:
//    TUniquePointer<internal::ZUDPSocketData> data_ptr_;
//    State_ state_;
//    Void* link_object_ptr_;
//};
//
//}//socket
//}//zengine