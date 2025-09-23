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
#include "z_core/z_string.h"

namespace zengine {
namespace socket {
namespace internal {

class ZTCPServerData : public ZObject {
public:
    ZTCPServerData() noexcept 
        : end_point_set_(false)
        , endpoint_()
        , io_context_()
        , acceptor_(io_context_)
        , socket_(io_context_) {}

public:
    Bool end_point_set_;
    boost::asio::ip::tcp::endpoint endpoint_;

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};

}//internal
}//socket
}//zengine

namespace zengine {
    namespace socket {

ZTCPServer::ZTCPServer() noexcept 
    : state_(ZTCPServerState_Idle)
    , data_ptr_(new internal::ZTCPServerData())
{}

ZTCPServer::~ZTCPServer() noexcept {
    if (data_ptr_ != nullptr) {
        delete data_ptr_;
    }
}

NODISCARD ZTCPServer::State_ ZTCPServer::State() noexcept {
    return state_;
}

NODISCARD ReturnType ZTCPServer::SetEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Idle
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
    data_ptr_->endpoint_.address(address);
    data_ptr_->endpoint_.port(_port);
    data_ptr_->end_point_set_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::SetSocketBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Connect
    );

    boost::system::error_code error_code;
    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_UnknownError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"Unknown error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPServerState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::Listen() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        !data_ptr_->end_point_set_,
        error_code::kZSocketErrorCode_ServerEndpointNotSet,
        L"Endpoint not set! Can not open!"
    );
    Z_CHECK(
        state_ != ZTCPServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Idle
    );

    try {
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4());
        data_ptr_->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        data_ptr_->acceptor_.bind(data_ptr_->endpoint_);
        data_ptr_->acceptor_.listen();
        state_ = ZTCPServerState_Listen;
    }
    catch (boost::system::error_code error_code) {
        ret_val = error_code::kZSocketErrorCode_UnknownError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"Unknown error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPServerState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::Accept() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPServerState_Listen,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Listen
    );

    data_ptr_->acceptor_.accept(data_ptr_->socket_, error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_UnknownError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"Unknown error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPServerState_Error;
        return ret_val;
    }
    state_ = ZTCPServerState_Connect;

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ == ZTCPServerState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server not open!"
    );

    try {
        data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        data_ptr_->socket_.close(error_code);
        data_ptr_->acceptor_.close(error_code);
        state_ = ZTCPServerState_Idle;
    }
    catch (boost::system::error_code error_code) {
        ret_val = error_code::kZSocketErrorCode_UnknownError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"Unknown error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPServerState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Connect
    );

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_ptr_->socket_.read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
    }
    else {
        data_ptr_->socket_.read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
    }

    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_.close(error_code);
            ret_val = error_code::kZSocketErrorCode_ClientDisconnected;
            Z_LOG_MESSAGE(
                L"TCP client disconnected! Server IP: %ls, Server Port: %d",
                string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
                data_ptr_->endpoint_.port()
            );
            state_ = ZTCPServerState_Listen;
            return ret_val;
        } 
        else {
            ret_val = error_code::kZSocketErrorCode_UnknownError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Unknown error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPServerState_Error;
            return ret_val;
        }
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPServer::Write(const Void* _data_ptr, SizeType _date_size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPServerState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPServerState_Connect
    );

    SizeType length = data_ptr_->socket_.write_some(boost::asio::buffer(_data_ptr, _date_size), error_code);

    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_.close(error_code);
            ret_val = error_code::kZSocketErrorCode_ClientDisconnected;
            Z_LOG_MESSAGE(
                L"TCP client disconnected! Server IP: %ls, Server Port: %d",
                string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
                data_ptr_->endpoint_.port()
            );
            state_ = ZTCPServerState_Listen;
            return ret_val;
        }
        else {
            ret_val = error_code::kZSocketErrorCode_UnknownError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Unknown error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPServerState_Error;
            return ret_val;
        }
        return ret_val;
    }

    return ret_val;
}

}//socket
}//zengine