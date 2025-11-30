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
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTLSStreamData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    TLS stream.
    Initialize with a connected tcp socket.
    Socket connected -> SetSNI(optional) -> Handshake -> Read/Write -> Shutdown -> Socket shutdown
*/
class SOCKET_DLLAPI ZTLSStream : public ZObject {
public:
    enum class StateEnum_ : Int32 {
        kUninitialized,
        kWaitForHandShake,
        kHandShaked,
        kShutdown,
        kError
    };
    
    /*
        Socket mush be connected.
    */
    ZTLSStream(
        ZTCPSocket* _socket_ptr, 
        ZTLSContext* _tls_context_ptr
    ) noexcept;
    ZTLSStream(ZTLSStream&& _stream) noexcept;

    ~ZTLSStream() noexcept;

    ZTLSStream& operator=(ZTLSStream&& _stream) noexcept;

    /*
       Initialize tls stream.
   */
    NODISCARD ReturnType Initialize(
        ZTCPSocket* _socket_ptr, 
        ZTLSContext* _tls_context_ptr
    ) noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_.Value(); }
    NODISCARD FORCEINLINE ZTLSContext* TLSContextPtr() const noexcept { return tls_context_ptr_; }

    NODISCARD ReturnType SetDNI(const Char* host_name) noexcept;   

    /*
        Handshake.
    */
    NODISCARD ReturnType Handshake() noexcept;

    /*
        Handshake. Will not suspend the current thread.
        _handle_func(ReturnType _error_code)
    */
    NODISCARD ReturnType AsyncHandshake(
        const TFunction<Void(ReturnType, ZTLSStream*)>& _handle_func
    ) noexcept;

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
        _handle_func(ReturnType _error_code, ZTLSStream* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncRead(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZTLSStream*, const ZConstBuffer)>& _handle_func
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
        _handle_func(ReturnType _error_code, ZTLSStream* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZSocketBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ReturnType, ZTLSStream*, ZSocketBufferStream*)>& _handle_func
    ) noexcept;

    /*
        Read data until match string. Will not suspend the current thread.
        _handle_func only needs to handle the read data.
        _handle_func(ReturnType _error_code, ZTLSStream* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
    */
    NODISCARD ReturnType AsyncReadUntil(
        ZSocketBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ReturnType, ZTLSStream*, ZSocketBufferStream*)>& _handle_func
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
        _handle_func(ReturnType _error_code, ZTLSStream* _socket_ptr, const ZConstBuffer _buffer)
    */
    NODISCARD ReturnType AsyncWrite(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTLSStream*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /*
        Shutdown stream. Will suspend the current thread until stream shutdown.
    */
    NODISCARD ReturnType Shutdown() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZTLSStream(const ZTLSStream&) = delete;
    ZTLSStream& operator=(const ZTLSStream&) = delete;

    /*
        Called when handshake success.
    */
    Void OnHandshakeP() noexcept;

private:
    TUniquePointer<internal::ZTLSStreamData> data_ptr_;
    ZTCPSocket* tcp_socket_ptr_;
    ZTLSContext* tls_context_ptr_;
    TAtom<StateEnum_> state_;
};

}//socket
}//zengine