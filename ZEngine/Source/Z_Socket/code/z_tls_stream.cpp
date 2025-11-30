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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_buffer_stream.h"
#include "z_socket.h"
#include "z_socket_allocator.h"
#include "z_tls_context.h"
#include "z_tls_stream.h"

#include "data/z_buffer_stream_data.h"
#include "data/z_tcp_socket_data.h"
#include "data/z_tls_context_data.h"
#include "data/z_tls_stream_data.h"


namespace zengine {
namespace socket {

ZTLSStream::ZTLSStream(
    ZTCPSocket* _socket_ptr, 
    ZTLSContext* _tls_context_ptr
) noexcept
    : SuperType_()
    , data_ptr_()
    , tcp_socket_ptr_(_socket_ptr)
    , tls_context_ptr_(_tls_context_ptr)
    , state_(StateEnum_::kUninitialized)
{
    if (_socket_ptr->State() != ZTCPSocket::StateEnum_::kConnected) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_StateError, 0,
            L"Socket state error! state: %d expect state: %d",
            _socket_ptr->State(), ZTCPSocket::StateEnum_::kConnected
        );
        return;
    }

    if (_tls_context_ptr->State() != ZTLSContext::StateEnum_::kInitialized) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_StateError, 0,
            L"TLS context state error! state: %d expect state: %d",
            _tls_context_ptr->State(), ZTLSContext::StateEnum_::kInitialized
        );
        return;
    }

    if (_tls_context_ptr->TLSType() == TLSTypeEnum::kServer) {
        if (_tls_context_ptr->CertificateLoaded() == false) {
            Z_LOG_ERROR(
                error_code::kPSocketErrorCode_ServerCertificateNotLoaded, 0,
                L"Server certificate not loaded!"
            );
            return;
        }
        if (_tls_context_ptr->PrivateKeyLoaded() == false) {
            Z_LOG_ERROR(
                error_code::kPSocketErrorCode_ServerPrivateKeyNotLoaded, 0,
                L"Server private key not loaded!"
            );
            return;
        }
    }

    data_ptr_ = MakeUnique<internal::ZTLSStreamData>(
        &_socket_ptr->data_ptr_->socket_, &_tls_context_ptr->data_ptr_->tls_context_
    );
    state_ = StateEnum_::kWaitForHandShake;
}

ZTLSStream::ZTLSStream(ZTLSStream&& _stream) noexcept
    : SuperType_()
    , data_ptr_(std::move(_stream.data_ptr_))
    , tcp_socket_ptr_(_stream.tcp_socket_ptr_)
    , tls_context_ptr_(_stream.tls_context_ptr_)
    , state_(_stream.state_)
{
    _stream.tls_context_ptr_ = nullptr;
    _stream.state_ = StateEnum_::kUninitialized;
}

ZTLSStream::~ZTLSStream() noexcept {
    ReturnType link_code = kOK;
    link_code = Shutdown();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTLSStream::Shutdown() link error!"
        );
        return;
    }
}

ZTLSStream& ZTLSStream::operator=(ZTLSStream&& _stream) noexcept {
    data_ptr_ = std::move(_stream.data_ptr_);
    tcp_socket_ptr_ = _stream.tcp_socket_ptr_;
    tls_context_ptr_ = _stream.tls_context_ptr_;
    state_ = _stream.state_;
    _stream.tcp_socket_ptr_ = nullptr;
    _stream.tls_context_ptr_ = nullptr;
    _stream.state_ = StateEnum_::kUninitialized;
    return *this;
}

NODISCARD ReturnType ZTLSStream::Initialize(
    ZTCPSocket* _socket_ptr,
    ZTLSContext* _tls_context_ptr
) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != StateEnum_::kUninitialized,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kUninitialized
    );
    Z_CHECK(
        _socket_ptr->State() != ZTCPSocket::StateEnum_::kConnected,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        _socket_ptr->State(), ZTCPSocket::StateEnum_::kConnected
    );
    Z_CHECK(
        _tls_context_ptr->State() != ZTLSContext::StateEnum_::kInitialized,
        error_code::kPSocketErrorCode_StateError,
        L"TLS context state error! state: %d expect state: %d",
        _tls_context_ptr->State(), ZTLSContext::StateEnum_::kInitialized
    );

    if (_tls_context_ptr->TLSType() == TLSTypeEnum::kServer) {
        if (_tls_context_ptr->CertificateLoaded() == false) {
            ret_val = error_code::kPSocketErrorCode_ServerCertificateNotLoaded;
            Z_LOG_ERROR(
                ret_val, 0,
                L"Server certificate not loaded!"
            );
            return ret_val;
        }
        if (_tls_context_ptr->PrivateKeyLoaded() == false) {
            ret_val = error_code::kPSocketErrorCode_ServerPrivateKeyNotLoaded;
            Z_LOG_ERROR(
                ret_val, 0,
                L"Server private key not loaded!"
            );
            return ret_val;
        }
    }

    data_ptr_ = MakeUnique<internal::ZTLSStreamData>(
        &_socket_ptr->data_ptr_->socket_, &_tls_context_ptr->data_ptr_->tls_context_
    );
    tcp_socket_ptr_ = _socket_ptr;
    tls_context_ptr_ = _tls_context_ptr;
    state_ = StateEnum_::kWaitForHandShake;

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::SetDNI(const Char* host_name) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != StateEnum_::kWaitForHandShake,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kWaitForHandShake
    );

    SSL_set_tlsext_host_name(data_ptr_->stream_.native_handle(), host_name);

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Handshake() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kWaitForHandShake,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kWaitForHandShake
    );

    Z_DEBUG_LOG_START(
        L"Start handshake... ip: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
        tcp_socket_ptr_->RemoteEndpoint().Port()
    );

    boost::asio::ssl::stream_base::handshake_type tls_type;
    switch (tls_context_ptr_->TLSType()) {
    case TLSTypeEnum::kClient:
        tls_type = boost::asio::ssl::stream_base::client;
        break;
    case TLSTypeEnum::kServer:
        tls_type = boost::asio::ssl::stream_base::server;
        break;
    default:
        Z_DEBUG_LOG_FAILURE(
            L"Handshake failed! ip: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
            tcp_socket_ptr_->RemoteEndpoint().Port()
        );
        ret_val = error_code::kPSocketErrorCode_TLSTypeNotValid;
        Z_LOG_ERROR(ret_val, 0, L"TLS Type not valid! tls_type: %d", tls_context_ptr_->TLSType());
        return ret_val;
    }

    data_ptr_->stream_.handshake(tls_type, error_code);
    if (error_code) {

        Z_DEBUG_LOG_FAILURE(
            L"Handshake failed! ip: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
            tcp_socket_ptr_->RemoteEndpoint().Port()
        );

        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                tcp_socket_ptr_->RemoteEndpoint().Port()
            );
            return ret_val;
        } 
        else {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    OnHandshakeP();

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::AsyncHandshake(
    const TFunction<Void(ReturnType, ZTLSStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kWaitForHandShake,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kWaitForHandShake
    );

    Z_DEBUG_LOG_START(
        L"Start handshake... ip: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
        tcp_socket_ptr_->RemoteEndpoint().Port()
    );

    boost::asio::ssl::stream_base::handshake_type tls_type;
    switch (tls_context_ptr_->TLSType()) {
    case TLSTypeEnum::kClient:
        tls_type = boost::asio::ssl::stream_base::client;
        break;
    case TLSTypeEnum::kServer:
        tls_type = boost::asio::ssl::stream_base::server;
        break;
    default:
        Z_DEBUG_LOG_FAILURE(
            L"Handshake failed! ip: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
            tcp_socket_ptr_->RemoteEndpoint().Port()
        );
        ret_val = error_code::kPSocketErrorCode_TLSTypeNotValid;
        Z_LOG_ERROR(ret_val, 0, L"TLS Type not valid! tls_type: %d", tls_context_ptr_->TLSType());
        return ret_val;
        break;
    }

    data_ptr_->stream_.async_handshake(
        tls_type, 
        MakeSocketHandlerAllocator([this, _handle_func](const boost::system::error_code& _error_code) {
            ReturnType ret_val = kOK;
            ReturnType link_code = kOK;
            if (_error_code) {

                Z_DEBUG_LOG_FAILURE(
                    L"Handshake failed! ip: %ls port: %d",
                    string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                    tcp_socket_ptr_->RemoteEndpoint().Port()
                );

                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                        tcp_socket_ptr_->RemoteEndpoint().Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }
            }
            else {
                OnHandshakeP();
            }

            if (_handle_func) {
                _handle_func(ret_val, this);
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }
    
    SizeType data_size = data_ptr_->stream_.read_some(
        boost::asio::buffer(_buffer.DataPtr<Void>(), _buffer.Size()), error_code
    );
    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                tcp_socket_ptr_->RemoteEndpoint().Port()
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    if (_data_size_ptr != nullptr) {
        *_data_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::AsyncRead(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZTLSStream*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->stream_.async_read_some(
        boost::asio::buffer(_buffer.DataPtr<Void>(), _buffer.Size()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.DataPtr<Void>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;
            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                        tcp_socket_ptr_->RemoteEndpoint().Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                    tcp_socket_ptr_->data_ptr_->async_error_handle_func_(ret_val);
                }
            }

            //handle read message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = boost::asio::read_until(
        data_ptr_->stream_, _buffer_ptr->data_ptr_->buffer_, _match_char, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                tcp_socket_ptr_->RemoteEndpoint().Port()
            );
            return ret_val;
        }
        else if (error_code == boost::asio::error::not_found) {
            ret_val = error_code::kPSocketErrorCode_ReadUntilSymbolNotFound;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Match char not found!"
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    if (_data_size_ptr != nullptr) {
        *_data_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = boost::asio::read_until(
        data_ptr_->stream_, _buffer_ptr->data_ptr_->buffer_, _match_str, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                tcp_socket_ptr_->RemoteEndpoint().Port()
            );
            return ret_val;
        }
        else if (error_code == boost::asio::error::not_found) {
            ret_val = error_code::kPSocketErrorCode_ReadUntilSymbolNotFound;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Match string not found!"
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    if (_data_size_ptr != nullptr) {
        *_data_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ReturnType, ZTLSStream*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    boost::asio::async_read_until(
        data_ptr_->stream_, _buffer_ptr->data_ptr_->buffer_, _match_char,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
            ) {
                ReturnType ret_val = kOK;

                //handle error
                if (_error_code) {
                    if (IS_DICONNECT_ERROR(_error_code)) {
                        state_ = StateEnum_::kError;
                        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                        Z_DEBUG_LOG_FINISH(
                            L"Socket disconnected! ip: %ls port: %d",
                            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                            tcp_socket_ptr_->RemoteEndpoint().Port()
                        );
                    }
                    else if (_error_code == boost::asio::error::operation_aborted) {
                        ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                    }
                    else if (_error_code == boost::asio::error::not_found) {
                        ret_val = error_code::kPSocketErrorCode_ReadUntilSymbolNotFound;
                        Z_LOG_ERROR(
                            ret_val, _error_code.value(),
                            L"Match char not found!"
                        );
                    }
                    else {
                        state_ = StateEnum_::kError;
                        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                        ret_val = error_code::kPSocketErrorCode_SystemError;
                        Z_LOG_ERROR(
                            ret_val, _error_code.value(),
                            L"System error! error info: %ls",
                            string::String2WString(_error_code.message().c_str()).String()
                        );
                    }

                    //call hook error handle func
                    if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                        tcp_socket_ptr_->data_ptr_->async_error_handle_func_(ret_val);
                    }

                }

                //handle read message
                if (_handle_func) {
                    _handle_func(ret_val, this, _buffer_ptr);
                }
            })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ReturnType, ZTLSStream*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    boost::asio::async_read_until(
        data_ptr_->stream_, _buffer_ptr->data_ptr_->buffer_, _match_str,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
            ) {
                ReturnType ret_val = kOK;

                //handle error
                if (_error_code) {
                    if (IS_DICONNECT_ERROR(_error_code)) {
                        state_ = StateEnum_::kError;
                        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                        Z_DEBUG_LOG_FINISH(
                            L"Socket disconnected! ip: %ls port: %d",
                            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                            tcp_socket_ptr_->RemoteEndpoint().Port()
                        );
                    }
                    else if (_error_code == boost::asio::error::operation_aborted) {
                        ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                    }
                    else if (_error_code == boost::asio::error::not_found) {
                        ret_val = error_code::kPSocketErrorCode_ReadUntilSymbolNotFound;
                        Z_LOG_ERROR(
                            ret_val, _error_code.value(),
                            L"Match string not found!"
                        );
                    }
                    else {
                        state_ = StateEnum_::kError;
                        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                        ret_val = error_code::kPSocketErrorCode_SystemError;
                        Z_LOG_ERROR(
                            ret_val, _error_code.value(),
                            L"System error! error info: %ls",
                            string::String2WString(_error_code.message().c_str()).String()
                        );
                    }

                    //call hook error handle func
                    if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                        tcp_socket_ptr_->data_ptr_->async_error_handle_func_(ret_val);
                    }

                }

                //handle read message
                if (_handle_func) {
                    _handle_func(ret_val, this, _buffer_ptr);
                }
            })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::ReadUntilClose(
    ZSocketBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = 0ULL;
    do {
        data_size += boost::asio::read(
            data_ptr_->stream_,
            _buffer_ptr->data_ptr_->buffer_,
            boost::asio::transfer_at_least(1),
            error_code
        );
    } while (!error_code);

    if (IS_DICONNECT_ERROR(error_code)) {
        state_ = StateEnum_::kError;
        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
        ret_val = error_code::kPSocketErrorCode_Disconnected;
        Z_DEBUG_LOG_FINISH(
            L"Socket disconnected! ip: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
            tcp_socket_ptr_->RemoteEndpoint().Port()
        );
        return ret_val;
    }
    else {
        state_ = StateEnum_::kError;
        tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        return ret_val;
    }

    if (_data_size_ptr != nullptr) {
        *_data_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = data_ptr_->stream_.write_some(
        boost::asio::buffer(_buffer.DataPtr<const Void>(), _buffer.Size()), error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                tcp_socket_ptr_->RemoteEndpoint().Port()
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::AsyncWrite(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTLSStream*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;

    if (state_ != StateEnum_::kHandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->stream_.async_write_some(
        boost::asio::buffer(_buffer.DataPtr<const Void>(), _buffer.Size()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.DataPtr<const Void>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;
            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
                        tcp_socket_ptr_->RemoteEndpoint().Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    tcp_socket_ptr_->state_ = ZTCPSocket::StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                    tcp_socket_ptr_->data_ptr_->async_error_handle_func_(ret_val);
                }
            }

            //handle read message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Shutdown(
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kWaitForHandShake) {
        return ret_val;
    }

    data_ptr_->stream_.shutdown(error_code);
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        return ret_val;
    }

    state_ = StateEnum_::kShutdown;

    return ret_val;
}

Void ZTLSStream::OnHandshakeP() noexcept {
    state_ = StateEnum_::kHandShaked;

    Z_DEBUG_LOG_SUCCESS(
        L"Handshake success! ip: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->RemoteEndpoint().IPString().String()).String(),
        tcp_socket_ptr_->RemoteEndpoint().Port()
    );
}

}//socket
}//zengine