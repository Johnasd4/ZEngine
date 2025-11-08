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
*/
class SOCKET_DLLAPI ZTLSStream : public ZObject {
public:
    enum State_ : Int32 {
        ZTLSStreamTCPClientState_Uninitialized,
        ZTLSStreamTCPClientState_Idle,
        ZTLSStreamTCPClientState_HandShaked,
        ZTLSStreamTCPClientState_Shutdown,
        ZTLSStreamTCPClientState_Error
    };
    
    /*
        Socket mush be connected.
    */
    ZTLSStream(ZTCPSocket* _socket_ptr, ZTLSContext* _tls_context_ptr) noexcept;
    ZTLSStream(ZTLSStream&& _stream) noexcept;

    ~ZTLSStream() noexcept;

    ZTLSStream& operator=(ZTLSStream&& _stream) noexcept;

    /*
       Initialize tls stream.
   */
    NODISCARD ReturnType Initialize(ZTCPSocket* _socket_ptr, ZTLSContext* _tls_context_ptr) noexcept;

    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE ZTLSContext* TLSContextPtr() const noexcept { return tls_context_ptr_; }
    NODISCARD FORCEINLINE TLSTypeEnum TLSType() const noexcept { return tls_type_; }

    /*
        Conect to target socket. Will suspend the current thread.
    */
    NODISCARD ReturnType Handshake(
        const Char* _address_str,
        const Char* _port_str
    ) noexcept;

    /*
        Conect to target socket. Will not suspend the current thread.
        _handle_func(Bool _connect_success)
    */
    NODISCARD ReturnType AsyncHandshake(
        const Char* _address_str,
        const Char* _port_str,
        const TFunction<Void(Bool)>& _handle_func
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
        Write data. Will suspend the current thread until data write.
    */
    NODISCARD ReturnType Shutdown(
        const Void* _data_ptr,
        SizeType _data_size
    ) noexcept;

    /*
        Write data. Will not suspend the current thread.
        _handle_func will be called after the data send.
        _handle_func(ZTCPSocket* _socket_ptr, const Void* _data_ptr, SizeType _data_size)
    */
    NODISCARD ReturnType AsyncShutdown(
        const Void* _data_ptr,
        SizeType _data_size,
        const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
    ) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZTLSStream(const ZTLSStream&) = delete;
    ZTLSStream& operator=(const ZTLSStream&) = delete;

private:
    TUniquePointer<internal::ZTLSStreamData> data_ptr_;
    ZTLSContext* tls_context_ptr_;
    TLSTypeEnum tls_type_;
    State_ state_;
};

}//socket
}//zengine