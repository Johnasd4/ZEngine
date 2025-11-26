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

#include "z_tcp_server.h"

#include "z_core/f_string.h"
#include "z_core/m_log.h"
#include "z_core/t_atom.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

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
    , state_(ZTCPSingleSessionServerState_Uninitialized)
{
    if (_io_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    ReturnType link_code = kOK;
    data_ptr_ = MakeUnique<internal::ZTCPSingleSessionServerData>(&_io_context_ptr->data_ptr_->io_context_);
    link_code = socket_.Initialize(_io_context_ptr);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Initialize() link error!"
        );
        return;
    }

    socket_.SetAsyncErrorHandleFunction(
        [this]() {
            //disconnect
            if (socket_.State() == ZTCPSocket::ZTCPSocketState_Idle) {
                //connect->listen
                if (state_ == ZTCPSingleSessionServerState_Connected) {
                    state_ = ZTCPSingleSessionServerState_Listen;
                    Z_DEBUG_LOG_FINISH(L"Client disconnected!");
                }
            }
        }
    );

    state_ = ZTCPSingleSessionServerState_Idle;
}

ZTCPSingleSessionServer::~ZTCPSingleSessionServer() noexcept {
    ReturnType link_code = kOK;
    link_code = Close();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSingleSessionServer::Close() link error!"
        );
        return;
    }
}

NODISCARD ReturnType ZTCPSingleSessionServer::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Idle
    );

    data_ptr_->server_endpoint_ = *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint*>();
    data_ptr_->if_endpoint_bind_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::SetOSWriteBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::SetOSReadBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Listen() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        !data_ptr_->if_endpoint_bind_,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Endpoint not bind! Can not open!"
    );
    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Idle
    );

    try {
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4());
        data_ptr_->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        data_ptr_->acceptor_.bind(data_ptr_->server_endpoint_);
        data_ptr_->acceptor_.listen();
        state_ = ZTCPSingleSessionServerState_Listen;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        state_ = ZTCPSingleSessionServerState_Error;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPSingleSessionServerState_Uninitialized || state_ == ZTCPSingleSessionServerState_Idle) {
        return ret_val;
    }

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    try {
        data_ptr_->acceptor_.close();
        state_ = ZTCPSingleSessionServerState_Idle;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZTCPSingleSessionServerState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Accept() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Listen,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Listen
    );

    Z_DEBUG_LOG_START(L"Wait for client connect...");

    data_ptr_->acceptor_.accept(socket_.data_ptr_->socket_, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        return ret_val;
    }

    socket_.OnConnectP();
    state_ = ZTCPSingleSessionServerState_Connected;
    Z_DEBUG_LOG_SUCCESS(L"Client connected!");

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.Read(_buffer, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_DEBUG_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Read() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncRead(
    ZBuffer _buffer,
    const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.AsyncRead(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_char, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_DEBUG_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_str, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_DEBUG_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_char, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_str, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntilClose(
    ZBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.ReadUntilClose(_buffer_ptr, _data_size_ptr);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::ReadUntilClose() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.Write(_buffer);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_DEBUG_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Write() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncWrite(
    ZConstBuffer _buffer,
    const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connected
    );

    link_code = socket_.AsyncWrite(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

}//socket
}//zengine