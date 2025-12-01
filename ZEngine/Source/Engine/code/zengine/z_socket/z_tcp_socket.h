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
#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "z_tcp_endpoint.h"

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
    Open -> Bind(Optional) -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPSocket : public ZObject {
public:
    static inline constexpr Int32 kConnectRetryForever = kInt32Max;

public:
    enum class StateEnum_ : Int32 {
        kUninitialized,
        kClosed,
        kOpened,
        kConnecting,
        kConnected,
        kError
    };

    ZTCPSocket() noexcept;
    ZTCPSocket(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket(ZTCPSocket&& _socket) noexcept;
    ZTCPSocket(ZIOContext* _context_ptr) noexcept;

    ~ZTCPSocket() noexcept;

    ZTCPSocket& operator=(const ZTCPSocket& _socket) noexcept;
    ZTCPSocket& operator=(ZTCPSocket&& _socket) noexcept;

    NODISCARD FORCEINLINE StateEnum_ State()const  noexcept { return state_.Value(); }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return link_object_ptr_; }

    NODISCARD ZTCPEndpoint LocalEndpoint() const noexcept;
    NODISCARD ZTCPEndpoint RemoteEndpoint() const noexcept;

    /*
        Initialize socket.
    */
    NODISCARD ReturnType Initialize(ZIOContext* _io_context_ptr) noexcept;

    /*
        Open the socket.
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
        Call before open. Must be called on all sockets that bind to the same address.
        Default is true.
    */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /*
        Sets the aysnc error handle func, called when aysnc error happens.
    */
    NODISCARD Void SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept;

    /*
        Cancel async operation.
    */
    Void Cancel() noexcept;

    /*
        Close the socket.
    */
    Void Close() noexcept;

    /*
        Conect to target socket. Will suspend the current thread.
    */
    NODISCARD ReturnType Connect(
        const ZTCPEndpoint& _tcp_endpoint,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /*
        Conect to target socket. Will not suspend the current thread.
        _handle_func(ReturnType _error_code, ZTCPSocket* _socket_ptr)
    */
    NODISCARD ReturnType AsyncConnect(
        const ZTCPEndpoint& _tcp_endpoint,
        const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
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
        const ZTCPEndpoint& _tcp_endpoint,
        const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times,
        Int32 _reconnect_times
    ) noexcept;

private:    
    TUniquePointer<internal::ZTCPSocketData> data_ptr_;
    ZIOContext* io_context_ptr_;
    Void* link_object_ptr_;
    TAtom<StateEnum_> state_;
};

}//socket
}//zengine