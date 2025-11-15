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

#include "z_tls_stream.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "z_tls_context.h"
#include "z_socket.h"

#include "data/z_tcp_socket_data.h"
#include "data/z_tls_context_data.h"
#include "data/z_tls_stream_data.h"
#include "z_socket_allocator.h"

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
    , state_(ZTLSStreamState_Uninitialized)
{
    if (_socket_ptr->State() != ZTCPSocket::ZTCPSocketState_Connected) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_StateError, 0,
            L"Socket state error! state: %d expect state: %d",
            _socket_ptr->State(), ZTCPSocket::ZTCPSocketState_Connected
        );
        return;
    }

    if (_tls_context_ptr->State() != ZTLSContext::ZTLSContextState_Initialized) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_StateError, 0,
            L"TLS context state error! state: %d expect state: %d",
            _tls_context_ptr->State(), ZTLSContext::ZTLSContextState_Initialized
        );
        return;
    }

    if (_tls_context_ptr->TLSType() == TLSTypeEnum::kTLSType_Server) {
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
    state_ = ZTLSStreamState_Idle;
}

ZTLSStream::ZTLSStream(ZTLSStream&& _stream) noexcept
    : SuperType_()
    , data_ptr_(std::move(_stream.data_ptr_))
    , tcp_socket_ptr_(_stream.tcp_socket_ptr_)
    , tls_context_ptr_(_stream.tls_context_ptr_)
    , state_(_stream.state_)
{
    _stream.tls_context_ptr_ = nullptr;
    _stream.state_ = ZTLSStreamState_Uninitialized;
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
    _stream.state_ = ZTLSStreamState_Uninitialized;
    return *this;
}

NODISCARD ReturnType ZTLSStream::Initialize(
    ZTCPSocket* _socket_ptr,
    ZTLSContext* _tls_context_ptr
) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTLSStreamState_Uninitialized,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), ZTLSStreamState_Uninitialized
    );
    Z_CHECK(
        _socket_ptr->State() != ZTCPSocket::ZTCPSocketState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        _socket_ptr->State(), ZTCPSocket::ZTCPSocketState_Connected
    );
    Z_CHECK(
        _tls_context_ptr->State() != ZTLSContext::ZTLSContextState_Initialized,
        error_code::kPSocketErrorCode_StateError,
        L"TLS context state error! state: %d expect state: %d",
        _tls_context_ptr->State(), ZTLSContext::ZTLSContextState_Initialized
    );

    if (_tls_context_ptr->TLSType() == TLSTypeEnum::kTLSType_Server) {
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
    state_ = ZTLSStreamState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::SetDNI(const Char* host_name) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTLSStreamState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), ZTLSStreamState_Idle
    );

    SSL_set_tlsext_host_name(data_ptr_->stream_.native_handle(), host_name);

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Handshake() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTLSStreamState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), ZTLSStreamState_Idle
    );

    Z_LOG_START(
        L"Start handshake... address: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
        tcp_socket_ptr_->data_ptr_->port_
    );

    boost::asio::ssl::stream_base::handshake_type tls_type;
    switch (tls_context_ptr_->TLSType()) {
    case TLSTypeEnum::kTLSType_Client:
        tls_type = boost::asio::ssl::stream_base::client;
        break;
    case TLSTypeEnum::kTLSType_Server:
        tls_type = boost::asio::ssl::stream_base::server;
        break;
    default:
        Z_LOG_FAILURE(
            L"Handshake failed! address: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
            tcp_socket_ptr_->data_ptr_->port_
        );
        ret_val = error_code::kPSocketErrorCode_TLSTypeNotValid;
        Z_LOG_ERROR(ret_val, 0, L"TLS Type not valid! tls_type: %d", tls_context_ptr_->TLSType());
        return ret_val;
    }

    data_ptr_->stream_.handshake(tls_type, error_code);
    if (error_code) {

        Z_LOG_FAILURE(
            L"Handshake failed! address: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
            tcp_socket_ptr_->data_ptr_->port_
        );

        if (IS_DICONNECT_ERROR(error_code)) {
            link_code = Shutdown();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTLSStream::Shutdown() link error!"
                );
                return ret_val;
            }
            link_code = tcp_socket_ptr_->Close();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTCPSocket::Close() link error!"
                );
                return ret_val;
            }
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                tcp_socket_ptr_->data_ptr_->port_
            );
            return ret_val;
        } 
        else {
            state_ = ZTLSStreamState_Error;
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
    const TFunction<Void(ZTLSStream*, Bool)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTLSStreamState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"TLS stream state error! state: %d expect state: %d",
        state_.Value(), ZTLSStreamState_Idle
    );

    Z_LOG_START(
        L"Start handshake... address: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
        tcp_socket_ptr_->data_ptr_->port_
    );

    boost::asio::ssl::stream_base::handshake_type tls_type;
    switch (tls_context_ptr_->TLSType()) {
    case TLSTypeEnum::kTLSType_Client:
        tls_type = boost::asio::ssl::stream_base::client;
        break;
    case TLSTypeEnum::kTLSType_Server:
        tls_type = boost::asio::ssl::stream_base::server;
        break;
    default:
        Z_LOG_FAILURE(
            L"Handshake failed! address: %ls port: %d",
            string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
            tcp_socket_ptr_->data_ptr_->port_
        );
        ret_val = error_code::kPSocketErrorCode_TLSTypeNotValid;
        Z_LOG_ERROR(ret_val, 0, L"TLS Type not valid! tls_type: %d", tls_context_ptr_->TLSType());
        return ret_val;
        break;
    }

    data_ptr_->stream_.async_handshake(
        tls_type, 
        MakeSocketHandlerAllocator([this, _handle_func](const boost::system::error_code& _error_code) {
            ReturnType link_code = kOK;
            if (_error_code) {

                Z_LOG_FAILURE(
                    L"Handshake failed! address: %ls port: %d",
                    string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                    tcp_socket_ptr_->data_ptr_->port_
                );

                if (IS_DICONNECT_ERROR(_error_code)) {
                    link_code = Shutdown();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTLSStream::Shutdown() link error!"
                        );
                    }
                    link_code = tcp_socket_ptr_->Close();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTCPSocket::Close() link error!"
                        );
                    }
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                        tcp_socket_ptr_->data_ptr_->port_
                    );
                }
                else {
                    state_ = ZTLSStreamState_Error;
                    Z_LOG_ERROR(
                        error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                if (_handle_func) {
                    _handle_func(this, false);
                }

                return;
            }

            OnHandshakeP();

            if (_handle_func) {
                _handle_func(this, true);
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Read(
    Void* _buffer_ptr,
    SizeType _buffer_size,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTLSStreamState_HandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = data_ptr_->stream_.read_some(boost::asio::buffer(_buffer_ptr, _buffer_size), error_code);
    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            link_code = Shutdown();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTLSStream::Shutdown() link error!"
                );
                return ret_val;
            }
            link_code = tcp_socket_ptr_->Close();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTCPSocket::Close() link error!"
                );
                return ret_val;
            }
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                tcp_socket_ptr_->data_ptr_->port_
            );
            return ret_val;
        }
        else {
            state_ = ZTLSStreamState_Error;
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
    Void* _buffer_ptr,
    SizeType _buffer_size,
    const TFunction<Void(ZTLSStream*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;

    if (state_ != ZTLSStreamState_HandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->stream_.async_read_some(
        boost::asio::buffer(_buffer_ptr, _buffer_size), 
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    ReturnType link_code = kOK;
                    link_code = Shutdown();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTLSStream::Shutdown() link error!"
                        );
                    }
                    link_code = tcp_socket_ptr_->Close();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTCPSocket::Close() link error!"
                        );
                    }
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                        tcp_socket_ptr_->data_ptr_->port_
                    );
                }
                else {
                    state_ = ZTLSStreamState_Error;
                    Z_LOG_ERROR(
                        error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                    tcp_socket_ptr_->data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle read message
            if (_handle_func) {
                _handle_func(this, _buffer_ptr, _data_size);
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Write(
    const Void* _data_ptr,
    SizeType _data_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTLSStreamState_HandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = data_ptr_->stream_.write_some(boost::asio::buffer(_data_ptr, _data_size), error_code);

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            link_code = Shutdown();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTLSStream::Shutdown() link error!"
                );
                return ret_val;
            }
            link_code = tcp_socket_ptr_->Close();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZTCPSocket::Close() link error!"
                );
                return ret_val;
            }
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %d",
                string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                tcp_socket_ptr_->data_ptr_->port_
            );
            return ret_val;
        }
        else {
            state_ = ZTLSStreamState_Error;
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
    const Void* _data_ptr,
    SizeType _data_size,
    const TFunction<Void(ZTLSStream*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;

    if (state_ != ZTLSStreamState_HandShaked) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->stream_.async_write_some(
        boost::asio::buffer(_data_ptr, _data_size),
        MakeSocketHandlerAllocator([this, _data_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    ReturnType link_code = kOK;
                    link_code = Shutdown();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTLSStream::Shutdown() link error!"
                        );
                    }
                    link_code = tcp_socket_ptr_->Close();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kPSocketErrorCode_LinkError, link_code,
                            L"ZTCPSocket::Close() link error!"
                        );
                    }
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %d",
                        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
                        tcp_socket_ptr_->data_ptr_->port_
                    );
                }
                else {
                    state_ = ZTLSStreamState_Error;
                    Z_LOG_ERROR(
                        error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (tcp_socket_ptr_->data_ptr_->async_error_handle_func_) {
                    tcp_socket_ptr_->data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle read message
            if (_handle_func) {
                _handle_func(this, _data_ptr, _data_size);
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTLSStream::Shutdown(
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    if (state_ == ZTLSStreamState_Uninitialized || state_ == ZTLSStreamState_Idle) {
        return ret_val;
    }

    data_ptr_->stream_.shutdown(error_code);
    if (error_code) {
        state_ = ZTLSStreamState_Error;
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        return ret_val;
    }

    state_ = ZTLSStreamState_Shutdown;

    return ret_val;
}

Void ZTLSStream::OnHandshakeP() noexcept {
    state_ = ZTLSStreamState_HandShaked;

    Z_LOG_SUCCESS(
        L"Handshake success! address: %ls port: %d",
        string::String2WString(tcp_socket_ptr_->data_ptr_->address_.String()).String(),
        tcp_socket_ptr_->data_ptr_->port_
    );
}

}//socket
}//zengine