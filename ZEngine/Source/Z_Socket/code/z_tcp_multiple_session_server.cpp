///*
//    Copyright (c) YuLin Zhu
//
//    This code file is licensed under the Creative Commons
//    Attribution-NonCommercial 4.0 International License.
//
//    You may obtain a copy of the License at
//    https://creativecommons.org/licenses/by-nc/4.0/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    Author: YuLin Zhu
//    Contact: 1152325286@qq.com
//*/
//#define SOCKET_DLLFILE
//
//#include "z_tcp_server.h"
//
//#include <boost/asio.hpp>
//
//#include "z_core/m_log.h"
//#include "z_core/t_atom.h"
//#include "z_core/z_string.h"
//#include "z_core/z_thread.h"
//
//namespace zengine {
//namespace socket {
//namespace internal {
//
//class ZTCPMultiSessionServerData : public ZObject {
//public:
//    ZTCPMultiSessionServerData() noexcept 
//        : end_point_set_(false)
//        , server_endpoint_()
//        , io_context_()
//        , acceptor_(io_context_)
//        , socket_(io_context_)
//        , state_(ZTCPMultiSessionServer::ZTCPMultiSessionServerState_Idle) {}
//
//public:
//    Bool end_point_set_;
//    boost::asio::ip::tcp::endpoint server_endpoint_;
//    boost::asio::ip::tcp::endpoint client_endpoint_;
//
//    boost::asio::io_context io_context_;
//    boost::asio::ip::tcp::tcp::acceptor acceptor_;
//    boost::asio::ip::tcp::socket socket_;
//    TAtom<ZTCPMultiSessionServer::State_> state_;
//    ZThread aysnc_thread_;
//};
//
//}//internal
//}//socket
//}//zengine
//
//namespace zengine {
//    namespace socket {
//
//ZTCPMultiSessionServer::ZTCPMultiSessionServer() noexcept 
//    : data_ptr_(new internal::ZTCPMultiSessionServerData())
//{}
//
//ZTCPMultiSessionServer::~ZTCPMultiSessionServer() noexcept {
//    ReturnType link_code = kOK;
//    link_code = Reset();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(
//            error_code::kZSocketErrorCode_LinkError, link_code,
//            L"ZTCPServer::Reset() link error!"
//        );
//        return;
//    }
//    if (data_ptr_->aysnc_thread_.Joinable()) {
//        data_ptr_->aysnc_thread_.Join();
//    }
//    if (data_ptr_ != nullptr) {
//        delete data_ptr_;
//    }
//}
//
//NODISCARD ZTCPMultiSessionServer::State_ ZTCPMultiSessionServer::State() noexcept {
//    return data_ptr_->state_.Value();
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::SetEndpoint(const Char* _address_str, Int32 _port) noexcept {
//    ReturnType ret_val = kOK;
//
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Idle,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Idle
//    );
//    Z_CHECK(
//        _port < 0 || _port > 65535,
//        error_code::kZSocketErrorCode_PortNotVaild,
//        L"Expect port 0 ~ 65535! port: %d",
//        _port
//    );
//
//    boost::system::error_code error_code;
//    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
//    if (error_code) {
//        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
//        Z_LOG_ERROR(
//            ret_val, 0, 
//            L"Address not vaild! address: %ls", 
//            string::String2WString(_address_str).String()
//        );
//        return ret_val;
//    }
//    data_ptr_->server_endpoint_.address(address);
//    data_ptr_->server_endpoint_.port(_port);
//    data_ptr_->end_point_set_ = true;
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::SetSocketBufferSize(Int32 _size) noexcept {
//    ReturnType ret_val = kOK;
//
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    boost::system::error_code error_code;
//    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
//    if (error_code) {
//        ret_val = error_code::kZSocketErrorCode_SystemError;
//        Z_LOG_ERROR(
//            ret_val, error_code.value(),
//            L"Unknown error! error info: %ls",
//            string::String2WString(error_code.message().c_str()).String()
//        );
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Listen() noexcept {
//    ReturnType ret_val = kOK;
//    boost::system::error_code error_code;
//
//    Z_CHECK(
//        !data_ptr_->end_point_set_,
//        error_code::kZSocketErrorCode_EndpointNotSet,
//        L"Endpoint not set! Can not open!"
//    );
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Idle,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Idle
//    );
//
//    try {
//        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4());
//        data_ptr_->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
//        data_ptr_->acceptor_.bind(data_ptr_->server_endpoint_);
//        data_ptr_->acceptor_.listen();
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//    }
//    catch (const boost::system::system_error& error) {
//        ret_val = error_code::kZSocketErrorCode_SystemError;
//        Z_LOG_ERROR(
//            ret_val, error.code().value(),
//            L"System error! error info: %ls",
//            string::String2WString(error.code().message().c_str()).String()
//        );
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Close() noexcept {
//    ReturnType ret_val = kOK;
//    boost::system::error_code error_code;
//
//    Z_CHECK(
//        data_ptr_->state_ == ZTCPMultiSessionServerState_Idle,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server not open!"
//    );
//
//    try {
//        data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
//        data_ptr_->socket_.close(error_code);
//        data_ptr_->acceptor_.close(error_code);
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Idle;
//    }
//    catch (const boost::system::system_error& error) {
//        ret_val = error_code::kZSocketErrorCode_SystemError;
//        Z_LOG_ERROR(
//            ret_val, error.code().value(),
//            L"System error! error info: %ls",
//            string::String2WString(error.code().message().c_str()).String()
//        );
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Reset() noexcept {
//    ReturnType ret_val = kOK;
//    boost::system::error_code error_code;
//
//    if (data_ptr_->state_ == ZTCPMultiSessionServerState_Idle) {
//        return ret_val;
//    }
//
//    try {
//        data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
//        data_ptr_->socket_.close(error_code);
//        data_ptr_->acceptor_.close(error_code);
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Idle;
//    }
//    catch (const boost::system::system_error& error) {
//        ret_val = error_code::kZSocketErrorCode_SystemError;
//        Z_LOG_ERROR(
//            ret_val, error.code().value(),
//            L"System error! error info: %ls",
//            string::String2WString(error.code().message().c_str()).String()
//        );
//        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Accept() noexcept {
//    ReturnType ret_val = kOK;
//    boost::system::error_code error_code;
//
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Listen,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Listen
//    );
//
//    data_ptr_->acceptor_.accept(data_ptr_->socket_, error_code);
//    if (error_code) {
//        ret_val = error_code::kZSocketErrorCode_SystemError;
//        Z_LOG_ERROR(
//            ret_val, error_code.value(),
//            L"Unknown error! error info: %ls",
//            string::String2WString(error_code.message().c_str()).String()
//        );
//        return ret_val;
//    }
//
//    data_ptr_->client_endpoint_ = data_ptr_->socket_.remote_endpoint();
//    Z_LOG_MESSAGE(
//        L"Client connected! address: %ls port: %d",
//        string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//        data_ptr_->client_endpoint_.port()
//    );
//
//    data_ptr_->state_ = ZTCPMultiSessionServerState_Connect;
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    boost::system::error_code error_code;
//
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    if (_message_size_ptr != nullptr) {
//        *_message_size_ptr = data_ptr_->socket_.read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
//    }
//    else {
//        data_ptr_->socket_.read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
//    }
//
//    if (error_code) {
//        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
//            data_ptr_->socket_.close(error_code);
//            ret_val = error_code::kZSocketErrorCode_Disconnected;
//            Z_LOG_MESSAGE(
//                L"Client disconnected! address: %ls port: %d",
//                string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                data_ptr_->client_endpoint_.port()
//            );
//            data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//            return ret_val;
//        } 
//        else {
//            ret_val = error_code::kZSocketErrorCode_SystemError;
//            Z_LOG_ERROR(
//                ret_val, error_code.value(),
//                L"Unknown error! error info: %ls",
//                string::String2WString(error_code.message().c_str()).String()
//            );
//            data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//            return ret_val;
//        }
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::AsyncRead(
//    Void* _data_buffer,
//    Int32 _buffer_size,
//    const TFunction<Void(Void*, SizeType)>& _handle_func
//) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    data_ptr_->socket_.async_read_some(
//        boost::asio::buffer(_data_buffer, _buffer_size), 
//        [this, _data_buffer, &_handle_func](
//            const std::error_code& _error_code,
//            SizeType _read_length
//            ) {
//                //handle error
//                if (_error_code) {
//                    if (
//                        _error_code.value() == boost::asio::error::connection_reset ||
//                        _error_code.value() == ERROR_FILE_NOT_FOUND
//                        ) {
//                        boost::system::error_code error_code;
//                        data_ptr_->socket_.close(error_code);
//                        Z_LOG_MESSAGE(
//                            L"Client disconnected! address: %ls port: %d",
//                            string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                            data_ptr_->client_endpoint_.port()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//                    }
//                    else {
//                        Z_LOG_ERROR(
//                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
//                            L"Unknown error! error info: %ls",
//                            string::String2WString(_error_code.message().c_str()).String()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//                    }
//                }
//
//                //handle read message
//                _handle_func(_data_buffer, _read_length);
//        }
//    );
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::AsyncRead(
//    Void* _data_buffer,
//    Int32 _buffer_size,
//    const TSimpleFunction<Void(Void*, SizeType)>& _handle_func
//) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    data_ptr_->socket_.async_read_some(
//        boost::asio::buffer(_data_buffer, _buffer_size),
//        [this, _data_buffer, &_handle_func](
//            const std::error_code& _error_code,
//            SizeType _read_length
//            ) {
//                //handle error
//                if (_error_code) {
//                    if (
//                        _error_code.value() == boost::asio::error::connection_reset ||
//                        _error_code.value() == ERROR_FILE_NOT_FOUND
//                        ) {
//                        boost::system::error_code error_code;
//                        data_ptr_->socket_.close(error_code);
//                        Z_LOG_MESSAGE(
//                            L"Client disconnected! address: %ls port: %d",
//                            string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                            data_ptr_->client_endpoint_.port()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//                    }
//                    else {
//                        Z_LOG_ERROR(
//                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
//                            L"Unknown error! error info: %ls",
//                            string::String2WString(_error_code.message().c_str()).String()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//                    }
//                }
//
//                //handle read message
//                _handle_func(_data_buffer, _read_length);
//        }
//    );
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::Write(const Void* _data_ptr, SizeType _date_size) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    boost::system::error_code error_code;
//
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    SizeType length = data_ptr_->socket_.write_some(boost::asio::buffer(_data_ptr, _date_size), error_code);
//
//    if (error_code) {
//        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
//            data_ptr_->socket_.close(error_code);
//            ret_val = error_code::kZSocketErrorCode_Disconnected;
//            Z_LOG_MESSAGE(
//                L"Client disconnected! address: %ls port: %d",
//                string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                data_ptr_->client_endpoint_.port()
//            );
//            data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//            return ret_val;
//        }
//        else {
//            ret_val = error_code::kZSocketErrorCode_SystemError;
//            Z_LOG_ERROR(
//                ret_val, error_code.value(),
//                L"Unknown error! error info: %ls",
//                string::String2WString(error_code.message().c_str()).String()
//            );
//            data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//            return ret_val;
//        }
//        return ret_val;
//    }
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::AsyncWrite(
//    Void* _data_buffer,
//    Int32 _buffer_size,
//    const TFunction<Void(Void*, SizeType)>& _handle_func
//) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    data_ptr_->socket_.async_write_some(
//        boost::asio::buffer(_data_buffer, _buffer_size),
//        [this, _data_buffer, &_handle_func](
//            const std::error_code& _error_code,
//            SizeType _write_length
//            ) {
//                //handle error
//                if (_error_code) {
//                    if (
//                        _error_code.value() == boost::asio::error::connection_reset ||
//                        _error_code.value() == ERROR_FILE_NOT_FOUND
//                        ) {
//                        boost::system::error_code error_code;
//                        data_ptr_->socket_.close(error_code);
//                        Z_LOG_MESSAGE(
//                            L"Client disconnected! address: %ls port: %d",
//                            string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                            data_ptr_->client_endpoint_.port()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//                    }
//                    else {
//                        Z_LOG_ERROR(
//                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
//                            L"Unknown error! error info: %ls",
//                            string::String2WString(_error_code.message().c_str()).String()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//                    }
//                }
//
//                //handle write message
//                _handle_func(_data_buffer, _write_length);
//        }
//    );
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::AsyncWrite(
//    Void* _data_buffer,
//    Int32 _buffer_size,
//    const TSimpleFunction<Void(Void*, SizeType)>& _handle_func
//) noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    data_ptr_->socket_.async_write_some(
//        boost::asio::buffer(_data_buffer, _buffer_size),
//        [this, _data_buffer, &_handle_func](
//            const std::error_code& _error_code,
//            SizeType _write_length
//            ) {
//                //handle error
//                if (_error_code) {
//                    if (
//                        _error_code.value() == boost::asio::error::connection_reset ||
//                        _error_code.value() == ERROR_FILE_NOT_FOUND
//                        ) {
//                        boost::system::error_code error_code;
//                        data_ptr_->socket_.close(error_code);
//                        Z_LOG_MESSAGE(
//                            L"Client disconnected! address: %ls port: %d",
//                            string::String2WString(data_ptr_->client_endpoint_.address().to_string().c_str()).String(),
//                            data_ptr_->client_endpoint_.port()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Listen;
//                    }
//                    else {
//                        Z_LOG_ERROR(
//                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
//                            L"Unknown error! error info: %ls",
//                            string::String2WString(_error_code.message().c_str()).String()
//                        );
//                        data_ptr_->state_ = ZTCPMultiSessionServerState_Error;
//                    }
//                }
//
//                //handle write message
//                _handle_func(_data_buffer, _write_length);
//        }
//    );
//
//    return ret_val;
//}
//
//NODISCARD ReturnType ZTCPMultiSessionServer::AsyncRun() noexcept {
//    ReturnType ret_val = kOK;
//    ReturnType link_code = kOK;
//    Z_CHECK(
//        data_ptr_->state_ != ZTCPMultiSessionServerState_Connect,
//        error_code::kZSocketErrorCode_StateError,
//        L"Server state error! state: %d expect state: %d",
//        data_ptr_->state_, ZTCPMultiSessionServerState_Connect
//    );
//
//    //start dealing with async operation.
//    data_ptr_->aysnc_thread_ = ZThread(
//        [this]() {
//            data_ptr_->io_context_.run();
//        }
//    );
//
//    return ret_val;
//}
//
//}//socket
//}//zengine