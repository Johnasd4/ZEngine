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

#include "z_tcp_client.h"

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/z_string.h"

namespace zengine {
namespace socket {
namespace internal {

class ZTCPClientData : public ZObject {
public:
    ZTCPClientData() noexcept 
        : end_point_set_(false)
        , endpoint_()
        , io_context_()
        , resolver_(io_context_)
        , socket_(io_context_) {}

public:
    Bool end_point_set_;
    boost::asio::ip::tcp::endpoint endpoint_;

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
};

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

ZTCPClient::ZTCPClient() noexcept 
    : state_(ZTCPClientState_Idle)
    , data_ptr_(new internal::ZTCPClientData())
{}

ZTCPClient::~ZTCPClient() noexcept {
    if (data_ptr_ != nullptr) {
        delete data_ptr_;
    }
}

NODISCARD ZTCPClient::State_ ZTCPClient::State() noexcept {
    return state_;
}

NODISCARD ReturnType ZTCPClient::SetEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPClientState_Idle
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

NODISCARD ReturnType ZTCPClient::SetSocketBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPClientState_Connect
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
        state_ = ZTCPClientState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPClient::Connect(Int32 _repeat_times) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        !data_ptr_->end_point_set_,
        error_code::kZSocketErrorCode_ServerEndpointNotSet,
        L"Endpoint not set! Can not open!"
    );
    Z_CHECK(
        state_ != ZTCPClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPClientState_Idle
    );

    boost::system::error_code error_code;
    Int32 reconnect_times = 0;
    do {
        boost::asio::connect(data_ptr_->socket_, data_ptr_->resolver_.resolve(data_ptr_->endpoint_), error_code);
        if (error_code) {
            if (error_code == boost::asio::error::connection_refused) {
                reconnect_times += 1;
                Z_LOG_MESSAGE(
                    L"Retry to connect server! Repeat num: %d Server IP: %ls Server Port: %d",
                    reconnect_times,
                    string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
                    data_ptr_->endpoint_.port()
                );
            }
            else {
                ret_val = error_code::kZSocketErrorCode_UnknownError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"Unknown error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                state_ = ZTCPClientState_Error;
                return ret_val;
            }
        }
        else {
            state_ = ZTCPClientState_Connect;
            break;
        }
    } while (_repeat_times > reconnect_times);

    if (state_ != ZTCPClientState_Connect) {
        ret_val = error_code::kZSocketErrorCode_ClientConnectServerFailed;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Connect server failed! Server IP: %ls Server Port: %d",
            string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
            data_ptr_->endpoint_.port()
        );
        return ret_val;    
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPClient::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ == ZTCPClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server not open!"
    );

    try {
        data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        data_ptr_->socket_.close(error_code);
        state_ = ZTCPClientState_Idle;
    }
    catch (boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_UnknownError;
        printf("%s", error.what());
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"Unknown error! error info: %ls",
            string::String2WString(error.what()).String()
        );
        state_ = ZTCPClientState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPClient::Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPClientState_Connect
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
                L"TCP server disconnected! Server IP: %ls, Server Port: %d",
                string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
                data_ptr_->endpoint_.port()
            );
            state_ = ZTCPClientState_Idle;
            return ret_val;
        } 
        else {
            ret_val = error_code::kZSocketErrorCode_UnknownError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Unknown error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPClientState_Error;
            return ret_val;
        }
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPClient::Write(const Void* _data_ptr, SizeType _date_size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPClientState_Connect
    );

    SizeType length = data_ptr_->socket_.write_some(boost::asio::buffer(_data_ptr, _date_size), error_code);

    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_.close(error_code);
            ret_val = error_code::kZSocketErrorCode_ClientDisconnected;
            Z_LOG_MESSAGE(
                L"TCP server disconnected! Server IP: %ls, Server Port: %d",
                string::String2WString(data_ptr_->endpoint_.address().to_string().c_str()).String(),
                data_ptr_->endpoint_.port()
            );
            state_ = ZTCPClientState_Idle;
            return ret_val;
        }
        else {
            ret_val = error_code::kZSocketErrorCode_UnknownError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"Unknown error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPClientState_Error;
            return ret_val;
        }
        return ret_val;
    }

    return ret_val;
}

}//socket
}//zengine