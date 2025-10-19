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

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/t_atom.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

#include "data/z_tcp_server_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPSingleSessionServer::ZTCPSingleSessionServer() noexcept 
    : data_ptr_(MakeUnique<internal::ZTCPSingleSessionServerData>())
    , socket_(this)
    , state_(ZTCPSingleSessionServerState_Idle)
{
    socket_.SetAsyncErrorHandleFunction(
        [this]() {
            //disconnect
            if (socket_.State() == ZTCPSocket::ZTCPSocketState_Idle) {
                //connect->listen
                if (state_ == ZTCPSingleSessionServerState_Connect) {
                    state_ = ZTCPSingleSessionServerState_Listen;
                    Z_LOG_FINISH(L"Client disconnected!");
                }
            }
        }
    );
}

ZTCPSingleSessionServer::~ZTCPSingleSessionServer() noexcept {
    ReturnType link_code = kOK;
    link_code = Reset();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSingleSessionServer::Reset() link error!"
        );
        return;
    }
    if (data_ptr_->aysnc_thread_.Joinable()) {
        data_ptr_->aysnc_thread_.Join();
    }
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Idle
    );
    Z_CHECK(
        _port < 0 || _port > 65535,
        error_code::kZSocketErrorCode_PortNotVaild,
        L"Expect port 0 ~ 65535! port: %d",
        _port
    );

    boost::system::error_code error_code;
    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Address not vaild! address: %ls", 
            string::String2WString(_address_str).String()
        );
        return ret_val;
    }
    data_ptr_->server_endpoint_.address(address);
    data_ptr_->server_endpoint_.port(_port);

    data_ptr_->endpoint_set_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
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
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
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
        !data_ptr_->endpoint_set_,
        error_code::kZSocketErrorCode_EndpointNotSet,
        L"Endpoint not set! Can not open!"
    );
    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
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
        ret_val = error_code::kZSocketErrorCode_SystemError;
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

    Z_CHECK(
        state_ == ZTCPSingleSessionServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server not open!"
    );

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    try {
        data_ptr_->acceptor_.close();
        state_ = ZTCPSingleSessionServerState_Idle;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
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

NODISCARD ReturnType ZTCPSingleSessionServer::Reset() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPSingleSessionServerState_Idle) {
        return ret_val;
    }

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    try {
        data_ptr_->acceptor_.close();
        state_ = ZTCPSingleSessionServerState_Idle;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
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
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Listen
    );

    Z_LOG_START(L"Wait for client connect...");

    data_ptr_->acceptor_.accept(*socket_.data_ptr_->socket_ptr_, error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        return ret_val;
    }

    socket_.OnConnectP();
    state_ = ZTCPSingleSessionServerState_Connect;
    Z_LOG_SUCCESS(L"Client connected!");

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Read(
    Void* _data_buffer, 
    Int32 _buffer_size, 
    SizeType* _message_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.Read(_data_buffer, _buffer_size, _message_size_ptr);
    if (link_code != kOK) {
        if (ret_val == error_code::kZSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kZSocketErrorCode_LinkError;
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
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.AsyncRead(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncRead(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TSimpleFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.AsyncRead(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Write(
    const Void* _data_buffer, 
    SizeType _date_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.Write(_data_buffer, _date_size);
    if (link_code != kOK) {
        if (ret_val == error_code::kZSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionServerState_Listen;
            Z_LOG_FINISH(L"Client disconnected!");
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionServerState_Error;
            ret_val = error_code::kZSocketErrorCode_LinkError;
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
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.AsyncWrite(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncWrite(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TSimpleFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    link_code = socket_.AsyncWrite(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionServerState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Run() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    data_ptr_->io_context_.run();
    data_ptr_->io_context_.restart();

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncRun() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionServerState_Connect
    );

    //start dealing with async operation.
    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            data_ptr_->io_context_.run();
            data_ptr_->io_context_.restart();
        }
    );

    return ret_val;
}

}//socket
}//zengine