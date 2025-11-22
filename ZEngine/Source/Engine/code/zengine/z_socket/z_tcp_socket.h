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

#include "../z_core/t_atom.h"
#include "../z_core/t_function.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "z_buffer.h"

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
    enum State_ : Int32 {
        ZTCPSocketState_Uninitialized,
        ZTCPSocketState_Idle,
        ZTCPSocketState_Connecting,
        ZTCPSocketState_Connected,
        ZTCPSocketState_Error
    };

    ZTCPSocket() noexcept;
    ZTCPSocket(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket(ZTCPSocket&& _socket) noexcept;
    ZTCPSocket(ZIOContext* _context_ptr) noexcept;

    ~ZTCPSocket() noexcept;

    ZTCPSocket& operator=(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket& operator=(ZTCPSocket&& _socket) noexcept;

    NODISCARD FORCEINLINE State_ State()const  noexcept { return state_.Value(); }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return link_object_ptr_; }

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
        Conect to target socket. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        ZStringView _address_str,
        ZStringView _port_str,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Conect to target socket. Will not suspend the current thread.
        _handle_func(Bool _connect_success)
    */
    NODISCARD ReturnType AsyncConnect(
        ZStringView _address_str,
        ZStringView _port_str,
        const TFunction<Void(ZTCPSocket*, Bool)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Stops connecting.
    */
    NODISCARD Void StopConnect() noexcept;

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
        _handle_func(ZTCPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncRead(
        ZBuffer _buffer,
        const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Read data until match char. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match string. Will suspend the current thread until data read.
    */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /*
        Read data until match char. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ZTCPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until match string. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ZTCPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until close. Will suspend the current thread until close.
    */
    NODISCARD ReturnType ReadUntilClose(
        ZBufferStream* _buffer_ptr,
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
        _handle_func(ZTCPSocket* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncWrite(
        ZConstBuffer _buffer,
        const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
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
    friend class ZTLSStream;

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
        const TFunction<Void(ZTCPSocket*, Bool)>& _handle_func,
        Int32 _repeat_times,
        Int32 _reconnect_times
    ) noexcept;

private:    
    TUniquePointer<internal::ZTCPSocketData> data_ptr_;
    ZIOContext* io_context_ptr_;
    Void* link_object_ptr_;
    TAtom<State_> state_;
};

}//socket
}//zengine