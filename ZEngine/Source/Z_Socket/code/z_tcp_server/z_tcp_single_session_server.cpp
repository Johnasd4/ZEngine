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

#include "z_tcp_server.h"

#include "z_io_context.h"

#include "data/z_io_context_data.h"
#include "data/z_tcp_server_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPSingleSessionServer::ZTCPSingleSessionServer(ZIOContext* _io_context_ptr) noexcept
    : data_ptr_()
    , socket_()
    , io_context_ptr_(_io_context_ptr)
    , state_(StateEnum_::kUninitialized)
{
    if (_io_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_NullptrParam, 0,
            "_io_context_ptr is nullptr!"
        );
        return;
    }

    ReturnType link_code = kOK;
    data_ptr_ = MakeUnique<internal::ZTCPSingleSessionServerData>(&_io_context_ptr->data_ptr_->io_context_);
    link_code = socket_.Initialize(_io_context_ptr);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_LinkError, link_code,
            "ZTCPSocket::Initialize() link error!"
        );
        return;
    }

    state_ = StateEnum_::kClosed;
}

ZTCPSingleSessionServer::~ZTCPSingleSessionServer() noexcept {
    Close();
}

NODISCARD ReturnType ZTCPSingleSessionServer::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v6(), error_code);
        break;
    default:
        ret_val = error_code::kSocketErrorCode_ParamOutOfRange;
        Z_LOG_ERROR(
            ret_val, 0,
            "Enum out of range! _ip_type: %d",
            _ip_type
        );
        return ret_val;
    }

    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->acceptor_.bind(
        *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint>(), 
        error_code
    );
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    state_ = StateEnum_::kEndpointBind;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::SetOSWriteBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::SetOSReadBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );
    data_ptr_->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(_if_reuse));
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Listen(Int32 _max_wait_connect_client_num) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kEndpointBind,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kEndpointBind
    );

    data_ptr_->acceptor_.listen(_max_wait_connect_client_num, error_code);
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    state_ = StateEnum_::kListen;

    return ret_val;
}

Void ZTCPSingleSessionServer::Cancel() noexcept {
    socket_.Cancel();
}

Void ZTCPSingleSessionServer::Close() noexcept {
    boost::system::error_code error_code;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kClosed) {
        return;
    }

    socket_.Close();
    data_ptr_->acceptor_.close(error_code);

    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Accept() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kListen,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kListen
    );

    Z_DEBUG_LOG_START("Wait for client connect...");

    data_ptr_->acceptor_.accept(socket_.data_ptr_->socket_, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    socket_.SetAsyncErrorHandleFunction(
        [this](ReturnType error_code) {
            //disconnect
            if (socket_.State() == ZTCPSocket::StateEnum_::kError) {
                socket_.Close();

                //connect->listen
                if (state_ == StateEnum_::kConnected) {
                    state_ = StateEnum_::kListen;
                    Z_DEBUG_LOG_FINISH("Client disconnected!");
                }
            }
        }
    );

    socket_.OnConnectP();
    state_ = StateEnum_::kConnected;
    Z_DEBUG_LOG_SUCCESS("Client connected!");

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.Read(_buffer, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_.Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::Read() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncRead(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.AsyncRead(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_char, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_.Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_str, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_.Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_char, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_str, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntilClose(
    ZSocketBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.ReadUntilClose(_buffer_ptr, _data_size_ptr);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::ReadUntilClose() link error!"
        );
    }

    socket_.Close();

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.Write(_buffer);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_.Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::Write() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncWrite(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kConnected
    );

    link_code = socket_.AsyncWrite(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

}//socket
}//zengine