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

#include "z_tcp_socket.h"

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/z_string.h"

#include "z_io_context.h"

#include "data/z_context_data.h"
#include "data/z_tcp_socket_data.h"
#include "z_socket_allocator.h"

namespace zengine {
namespace socket {

ZTCPSocket::ZTCPSocket() noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZTCPSocketState_Uninitialized)
    , io_context_ptr_(nullptr)
{}

ZTCPSocket::ZTCPSocket(const ZTCPSocket& _socket) noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZTCPSocketState_Uninitialized)
    , io_context_ptr_(_socket.io_context_ptr_)
{
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZTCPSocketData(&io_context_ptr_->data_ptr_->io_context_));
        state_ = ZTCPSocketState_Idle;
    }
}

ZTCPSocket::ZTCPSocket(ZTCPSocket&& _socket) noexcept
    : SuperType_(std::forward<ZTCPSocket>(_socket))
{
    MoveP(std::forward<ZTCPSocket>(_socket));
}

ZTCPSocket::ZTCPSocket(ZIOContext* _context_ptr) noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZTCPSocketState_Uninitialized)
    , io_context_ptr_(_context_ptr)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_context_ptr->data_ptr_->io_context_);
    state_ = ZTCPSocketState_Idle;
}

ZTCPSocket::~ZTCPSocket() noexcept {
    ReturnType link_code = kOK;
    link_code = Reset();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Reset() link error!"
        );
        return;
    }
}

ZTCPSocket& ZTCPSocket::operator=(const ZTCPSocket& _socket) noexcept {
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZTCPSocketData(&io_context_ptr_->data_ptr_->io_context_));
        state_ = ZTCPSocketState_Idle;
    }
    else {
        data_ptr_.Reset();
        state_ = ZTCPSocketState_Uninitialized;
    }
    return *this;
}

ZTCPSocket& ZTCPSocket::operator=(ZTCPSocket&& _socket) noexcept {
    SuperType_::operator=(std::forward<ZTCPSocket>(_socket));
    MoveP(std::forward<ZTCPSocket>(_socket));
    return *this;
}

NODISCARD const ZString& ZTCPSocket::RemoteAddress() noexcept {
    return data_ptr_->address_;
}
NODISCARD const Int32 ZTCPSocket::RemotePort() noexcept {
    return data_ptr_->port_;
}

NODISCARD ReturnType ZTCPSocket::Initialize(ZIOContext* _io_context_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != ZTCPSocketState_Uninitialized,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Uninitialized
    );

    Z_CHECK(
        _io_context_ptr == nullptr,
        error_code::kZSocketErrorCode_NullptrParam,
        L"_io_context_ptr is nullptr!"
    );

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_io_context_ptr->data_ptr_->io_context_);
    io_context_ptr_ = _io_context_ptr;
    state_ = ZTCPSocketState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::BindEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != ZTCPSocketState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Idle
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

    data_ptr_->bind_endpoint_.address(address);
    data_ptr_->bind_endpoint_.port(_port);
    data_ptr_->if_endpoint_bind_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSocketState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Connect
    );

    boost::system::error_code error_code;
    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSocketState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Connect
    );

    boost::system::error_code error_code;
    data_ptr_->socket_.set_option(boost::asio::socket_base::receive_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZTCPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Cancel() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        data_ptr_->socket_.is_open() == false,
        error_code::kZSocketErrorCode_SocketNotOpen,
        L"Socket not open!"
    );

    try {
        data_ptr_->socket_.cancel();
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZTCPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        data_ptr_->socket_.is_open() == false,
        error_code::kZSocketErrorCode_SocketNotOpen,
        L"Socket not open!"
    );

    try {
        data_ptr_->if_endpoint_bind_ = false;
        data_ptr_->async_error_handle_func_ = nullptr;
        data_ptr_->socket_.cancel();
        if (state_ == ZTCPSocketState_Connect) {
            data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        }
        data_ptr_->socket_.close();
        state_ = ZTCPSocketState_Idle;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZTCPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Reset() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    if (state_ == ZTCPSocketState_Uninitialized) {
        return ret_val;
    }

    try {
        data_ptr_->if_endpoint_bind_ = false;
        data_ptr_->async_error_handle_func_ = nullptr;
        if (data_ptr_->socket_.is_open()) {
            data_ptr_->socket_.cancel();
        }
        if (state_ == ZTCPSocketState_Connect) {
            data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        }
        data_ptr_->socket_.close();
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kZSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZTCPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Connect(
    const Char* _address_str,
    const Char* _port_str,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPSocketState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Idle
    );

    //resolve endpoints
    boost::asio::ip::tcp::resolver::results_type endpoints;
    boost::asio::ip::tcp::resolver resolver(io_context_ptr_->data_ptr_->io_context_);
    endpoints = std::move(resolver.resolve(_address_str, _port_str, error_code));

    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls address: %ls port: ls",
            string::String2WString(error_code.message().c_str()).String(),
            string::String2WString(_address_str).String(),
            string::String2WString(_port_str).String()
        );
        return ret_val;
    }

    //connect
    Z_LOG_START(
        L"Try to connect server... server_address: %ls server_port: %ls",
        string::String2WString(_address_str).String(),
        string::String2WString(_port_str).String()
    );

    Int32 reconnect_times = 0;
    do {
        for (auto endpoint_iterator = endpoints.begin(); endpoint_iterator != endpoints.end(); ++endpoint_iterator) {
            if (data_ptr_->if_endpoint_bind_) {
                //open
                data_ptr_->socket_.close();
                data_ptr_->socket_.open(boost::asio::ip::tcp::v4(), error_code);
                if (error_code) {
                    break;
                }

                //bind endpoint
                data_ptr_->socket_.bind(data_ptr_->bind_endpoint_ , error_code);
            }

            //connect
            data_ptr_->socket_.connect(*endpoint_iterator, error_code);

            //connect success
            if (!error_code) {
                break;
            }
        }

        //connect failed
        if (error_code) {
            if (error_code == boost::asio::error::connection_refused) {
                reconnect_times += 1;
                Z_LOG_PROCESS(
                    L"Retry to connect server... repeat_times: %d server_address: %ls server_port: %ls",
                    reconnect_times,
                    string::String2WString(_address_str).String(),
                    string::String2WString(_port_str).String()
                );
            }
            else {
                ret_val = error_code::kZSocketErrorCode_SystemError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                state_ = ZTCPSocketState_Error;
                break;
            }
            continue;
        }

        //connect success
        OnConnectP();
        break;
    } while (_repeat_times > reconnect_times);

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_ConnectFailed;
        Z_LOG_FAILURE(
            L"Socket connect failed! server_address: %ls server_port: %ls",
            string::String2WString(_address_str).String(),
            string::String2WString(_port_str).String()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncConnect(
    const Char* _address_str,
    const Char* _port_str,
    const TFunction<Void(Bool)>& _handle_func,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSocketState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Idle
    );

    //resolve endpoints
    ZString address_str(_address_str);
    ZString port_str(_port_str);
    SocketAllocatorType alloc;

    io_context_ptr_->data_ptr_->async_resolver_.async_resolve(
        _address_str, _port_str, 
        MakeSocketHandlerAllocator([
            this, address_str = std::move(address_str), port_str = std::move(port_str), 
            _handle_func, _repeat_times
        ](
            const boost::system::error_code& _error_code, 
            boost::asio::ip::tcp::resolver::results_type _endpoints
        ) mutable {
            if (_error_code) {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_AddressNotVaild, _error_code.value(),
                    L"System error! error info: %ls address: %ls port: ls",
                    string::String2WString(_error_code.message().c_str()).String(),
                    string::String2WString(address_str.String()).String(),
                    string::String2WString(port_str.String()).String()
                );
                return;
            }

            //connect
            Z_LOG_START(
                L"Try to connect... address: %ls port: %ls",
                string::String2WString(address_str.String()).String(),
                string::String2WString(port_str.String()).String()
            );

            ReturnType link_code = kOK;
            auto endpoint_iterator = _endpoints.begin();
            link_code = AsyncConnectExecuteP(
                std::move(address_str), 
                std::move(port_str), 
                &_endpoints,
                &endpoint_iterator,
                _handle_func, 
                _repeat_times, 
                0
            );
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::AsyncConnectExecuteP() link error!"
                );
                return;
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncConnectExecuteP(
    ZString&& _address_str,
    ZString&& _port_str,
    Void* _endpoints_ptr,
    Void* _endpoint_iterator,
    const TFunction<Void(Bool)>& _handle_func,
    Int32 _repeat_times,
    Int32 _reconnect_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    boost::asio::ip::tcp::resolver::results_type* endpoints_ptr =
        reinterpret_cast<boost::asio::ip::tcp::resolver::results_type*>(_endpoints_ptr);
    boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp>* _endpoint_iterator_ptr =
        reinterpret_cast<boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp>*>(_endpoints_ptr);


    if (data_ptr_->if_endpoint_bind_) {
        //open
        data_ptr_->socket_.close();
        data_ptr_->socket_.open(boost::asio::ip::tcp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kZSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPSocketState_Error;
            return ret_val;
        }

        //bind endpoint
        data_ptr_->socket_.bind(data_ptr_->bind_endpoint_, error_code);
        if (error_code) {
            ret_val = error_code::kZSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPSocketState_Error;
            return ret_val;
        }
    }

    auto endpoint = (*_endpoint_iterator_ptr)->endpoint();

    Z_LOG_PROCESS(
        L"Connecting... address: %ls port: %d",
        string::String2WString(endpoint.address().to_string().c_str()).String(),
        endpoint.port()
    );

    //start connect
    data_ptr_->socket_.async_connect(endpoint, 
        MakeSocketHandlerAllocator([
            this, address_str = std::move(_address_str), port_str = std::move(_port_str),
            endpoints = std::move(*endpoints_ptr), endpoint_iterator = std::move(*_endpoint_iterator_ptr), 
            _handle_func, _repeat_times, _reconnect_times
        ](
            const boost::system::error_code& _error_code
        ) mutable {
                ReturnType link_code = kOK;
                //handle error
                if (_error_code) {
                    //handle error
                    if (_error_code == boost::asio::error::connection_refused) {
                        Bool if_retry = false;
                        //retry
                        if (endpoint_iterator != endpoints.end()) {
                            ++endpoint_iterator;
                            if_retry = true;
                        }
                        else if (_reconnect_times < _repeat_times) {
                            _reconnect_times += 1;
                            endpoint_iterator = endpoints.begin();
                            if_retry = true;
                            Z_LOG_PROCESS(
                                L"Retry to connect... repeat_times: %d address: %ls port: %ls",
                                _reconnect_times,
                                string::String2WString(address_str.String()).String(),
                                string::String2WString(port_str.String()).String()
                            );
                        }
                        if (if_retry) {
                            link_code = AsyncConnectExecuteP(
                                std::move(address_str),
                                std::move(port_str),
                                &endpoints,
                                &endpoint_iterator,
                                _handle_func,
                                _repeat_times,
                                _reconnect_times
                            );
                            if (link_code != kOK) {
                                Z_LOG_ERROR(
                                    error_code::kZSocketErrorCode_LinkError, link_code,
                                    L"ZTCPSocket::AsyncConnectExecuteP() link error!"
                                );
                                return;
                            }
                            return;
                        }

                        //timeout
                        Z_LOG_FAILURE(
                            L"Connect server failed! server_address: %ls server_port: %ls",
                            string::String2WString(address_str.String()).String(),
                            string::String2WString(port_str.String()).String()
                        );
                    }
                    else if (_error_code == boost::asio::error::operation_aborted) {
                        Z_LOG_FAILURE(
                            L"Connect cancelled! server_address: %ls server_port: %ls",
                            string::String2WString(address_str.String()).String(),
                            string::String2WString(port_str.String()).String()
                        );
                    }
                    else {
                        Z_LOG_ERROR(
                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
                            L"System error! error info: %ls",
                            string::String2WString(_error_code.message().c_str()).String()
                        );
                    }

                    link_code = Reset();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kZSocketErrorCode_LinkError, link_code,
                            L"ZTCPSocket::Reset() link error!"
                        );
                        return;
                    }

                    _handle_func(false);
                    return;
                }

                //connect success
                OnConnectP();

                _handle_func(true);
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Read(
    Void* _buffer_ptr,
    SizeType _buffer_size,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = data_ptr_->socket_.read_some(boost::asio::buffer(_buffer_ptr, _buffer_size), error_code);

    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_.close(error_code);
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %d",
                string::String2WString(data_ptr_->address_.String()).String(),
                data_ptr_->port_
            );
            state_ = ZTCPSocketState_Idle;
            return ret_val;
        } 
        else {
            ret_val = error_code::kZSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPSocketState_Error;
            return ret_val;
        }
    }

    if (_data_size_ptr != nullptr) {
        *_data_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncRead(
    Void* _buffer_ptr,
    SizeType _buffer_size,
    const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_.async_read_some(
        boost::asio::buffer(_buffer_ptr, _buffer_size), 
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                if (
                    _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_.close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %d",
                        string::String2WString(data_ptr_->address_.String()).String(),
                        data_ptr_->port_
                    );
                    state_ = ZTCPSocketState_Idle;
                }
                else {
                    Z_LOG_ERROR(
                        error_code::kZSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                    state_ = ZTCPSocketState_Error;
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
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

NODISCARD ReturnType ZTCPSocket::Write(
    const Void* _data_ptr, 
    SizeType _data_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType length = data_ptr_->socket_.write_some(boost::asio::buffer(_data_ptr, _data_size), error_code);
    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_.close(error_code);
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %d",
                string::String2WString(data_ptr_->address_.String()).String(),
                data_ptr_->port_
            );
            state_ = ZTCPSocketState_Idle;
            return ret_val;
        }
        else {
            ret_val = error_code::kZSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZTCPSocketState_Error;
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncWrite(
    const Void* _data_ptr,
    SizeType _data_size,
    const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_.async_write_some(
        boost::asio::buffer(_data_ptr, _data_size), 
        MakeSocketHandlerAllocator([this, _data_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                if (
                     _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                    ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_.close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %d",
                        string::String2WString(data_ptr_->address_.String()).String(),
                        data_ptr_->port_
                    );
                    state_ = ZTCPSocketState_Idle;
                }
                else {
                    Z_LOG_ERROR(
                        error_code::kZSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                    state_ = ZTCPSocketState_Error;
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle write message
            if (_handle_func) {
                _handle_func(this, _data_ptr, _data_size);
            }
        })
    );

    return ret_val;
}

NODISCARD SizeType ZTCPSocket::ReadableDataSize() noexcept {
    if (data_ptr_ == nullptr) {
        return 0;
    }
    return data_ptr_->socket_.available();
}

Void ZTCPSocket::MoveP(ZTCPSocket&& _socket) noexcept {
    data_ptr_ = std::move(_socket.data_ptr_);
    state_ = _socket.state_;
    io_context_ptr_ = _socket.io_context_ptr_;
    link_object_ptr_ = _socket.link_object_ptr_;
    _socket.state_ = ZTCPSocketState_Uninitialized;
    _socket.io_context_ptr_ = nullptr;
    _socket.link_object_ptr_ = nullptr;
}

Void ZTCPSocket::OnConnectP() noexcept {
    auto remote_endpoint = data_ptr_->socket_.remote_endpoint();
    data_ptr_->address_ = remote_endpoint.address().to_string().c_str();
    data_ptr_->port_ = static_cast<Int32>(remote_endpoint.port());

    state_ = ZTCPSocket::ZTCPSocketState_Connect;

    Z_LOG_SUCCESS(
        L"Socket connected! address: %ls port: %d",
        string::String2WString(data_ptr_->address_.String()).String(),
        data_ptr_->port_
    );
}

NODISCARD Void ZTCPSocket::SetAsyncErrorHandleFunction(TFunction<Void()>&& _handle_func) noexcept {
    data_ptr_->async_error_handle_func_ = std::forward<TFunction<Void()>>(_handle_func);
}

}//socket
}//zengine