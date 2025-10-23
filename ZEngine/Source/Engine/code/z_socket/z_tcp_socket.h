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
    TCP socket type.
    Contains a extra ptr that can be linked to any object. Call LinkObjectPtr<_ObjectType>() to get the object ptr.
*/
class SOCKET_DLLAPI ZTCPSocket : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

public:
    enum State_ {
        ZTCPSocketState_Uninitialized,
        ZTCPSocketState_Idle,
        ZTCPSocketState_Connect,
        ZTCPSocketState_Error
    };

    ZTCPSocket() noexcept;
    ZTCPSocket(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket(ZTCPSocket&& _socket) noexcept;
    ZTCPSocket(ZIOContext* _context_ptr) noexcept;

    ~ZTCPSocket() noexcept;

    ZTCPSocket& operator=(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket& operator=(ZTCPSocket&& _socket) noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() noexcept { return link_object_ptr_; }

    NODISCARD const ZString& RemoteAddress() noexcept;
    NODISCARD const Int32 RemotePort() noexcept;

    /*
        Initialize socket.
    */
    NODISCARD ReturnType Initialize(ZIOContext* _io_context_ptr) noexcept;

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
        const Char* _port_str,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Conect to target socket. Will not suspend the current thread.
        _handle_func(Bool _connect_success)
    */
    NODISCARD ReturnType AsyncConnect(
        const Char* _address_str,
        const Char* _port_str,
        const TFunction<Void(Bool)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

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
    NODISCARD SizeType ReadableDataSize() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSingleSessionClient;
    friend class ZTCPMultipleSessionClient;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:
    Void MoveP(ZTCPSocket&& _socket) noexcept;

    /*
        Called when socket connected.
    */
    Void OnConnectP() noexcept;

    /*
        Async connect execute func.
    */
    NODISCARD ReturnType AsyncConnectExecuteP(
        ZString&& _address_str,
        ZString&& _port_str,
        Void* _endpoints_ptr,
        Void* _endpoint_iterator,
        const TFunction<Void(Bool)>& _handle_func,
        Int32 _repeat_times,
        Int32 _reconnect_times
    ) noexcept;

private:    
    TUniquePointer<internal::ZTCPSocketData> data_ptr_;
    ZIOContext* io_context_ptr_;
    Void* link_object_ptr_;
    State_ state_;
};

}//socket
}//zengine