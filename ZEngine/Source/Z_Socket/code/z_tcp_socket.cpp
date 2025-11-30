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

#include "z_tcp_socket.h"

#include "z_buffer_stream.h"
#include "z_io_context.h"
#include "z_socket_allocator.h"

#include "data/z_buffer_stream_data.h"
#include "data/z_io_context_data.h"
#include "data/z_tcp_socket_data.h"


namespace zengine {
namespace socket {

ZTCPSocket::ZTCPSocket() noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(nullptr)
{}

ZTCPSocket::ZTCPSocket(const ZTCPSocket& _socket) noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(_socket.io_context_ptr_)
{
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&io_context_ptr_->data_ptr_->io_context_);
        state_ = StateEnum_::kClosed;
    }
}

ZTCPSocket::ZTCPSocket(ZTCPSocket&& _socket) noexcept
    : SuperType_(std::forward<ZTCPSocket>(_socket))
{
    MoveP(std::forward<ZTCPSocket>(_socket));
}

ZTCPSocket::ZTCPSocket(ZIOContext* _context_ptr) noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(_context_ptr)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_context_ptr->data_ptr_->io_context_);
    state_ = StateEnum_::kClosed;
}

ZTCPSocket::~ZTCPSocket() noexcept {
    ReturnType link_code = kOK;
    link_code = Close();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Reset() link error!"
        );
        return;
    }
}

ZTCPSocket& ZTCPSocket::operator=(const ZTCPSocket& _socket) noexcept {
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_= MakeUnique<internal::ZTCPSocketData>(&io_context_ptr_->data_ptr_->io_context_);
        state_ = StateEnum_::kClosed;
    }
    else {
        data_ptr_.Reset();
        state_ = StateEnum_::kUninitialized;
    }
    return *this;
}

ZTCPSocket& ZTCPSocket::operator=(ZTCPSocket&& _socket) noexcept {
    SuperType_::operator=(std::forward<ZTCPSocket>(_socket));
    MoveP(std::forward<ZTCPSocket>(_socket));
    return *this;
}

NODISCARD ZTCPEndpoint ZTCPSocket::LocalEndpoint() const noexcept {
    ZTCPEndpoint endpoint;
    *endpoint.endpoint_data_.DataPtr<boost::asio::ip::tcp::endpoint>() = data_ptr_->socket_.local_endpoint();
    return endpoint;
}

NODISCARD ZTCPEndpoint ZTCPSocket::RemoteEndpoint() const noexcept {
    ZTCPEndpoint endpoint; 
    *endpoint.endpoint_data_.DataPtr<boost::asio::ip::tcp::endpoint>() = data_ptr_->remote_endpoint_;
    return endpoint;
}

NODISCARD ReturnType ZTCPSocket::Initialize(ZIOContext* _io_context_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != StateEnum_::kUninitialized,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kUninitialized
    );

    Z_CHECK(
        _io_context_ptr == nullptr,
        error_code::kPSocketErrorCode_NullptrParam,
        L"_io_context_ptr is nullptr!"
    );

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_io_context_ptr->data_ptr_->io_context_);
    io_context_ptr_ = _io_context_ptr;
    state_ = StateEnum_::kClosed;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        data_ptr_->socket_.open(boost::asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        data_ptr_->socket_.open(boost::asio::ip::tcp::v6(), error_code);
        break;
    default:
        ret_val = error_code::kPSocketErrorCode_ParamOutOfRange;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Enum out of range! _ip_type: %d",
            _ip_type
        );
        return ret_val;
    }

    data_ptr_->socket_.set_option(
        boost::asio::ip::tcp::socket::reuse_address(data_ptr_->if_reuse_address_), 
        error_code
    );

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

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kOpened
    );

    data_ptr_->bind_endpoint_ = *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint>();
    data_ptr_->if_endpoint_bind_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kConnected
    );

    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
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

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kConnected,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kConnected
    );

    data_ptr_->socket_.set_option(boost::asio::socket_base::receive_buffer_size(_size), error_code);
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

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    data_ptr_->if_reuse_address_ = _if_reuse;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Cancel() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        data_ptr_->socket_.is_open() == false,
        error_code::kPSocketErrorCode_SocketNotOpen,
        L"Socket not open!"
    );

    try {
        data_ptr_->socket_.cancel();
    }
    catch (const boost::system::system_error& error) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kClosed) {
        return ret_val;
    }

    data_ptr_->if_endpoint_bind_ = false;
    data_ptr_->async_error_handle_func_ = nullptr;
    if (data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.cancel();
        if (state_ == StateEnum_::kConnected) {
            data_ptr_->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error_code);
            if (error_code) {
                if (!IS_DICONNECT_ERROR(error_code)) {
                    state_ = StateEnum_::kConnected;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(error_code.message().c_str()).String()
                    );
                    return ret_val;
                }
            }
        }
        data_ptr_->socket_.close(error_code);
        if (error_code && error_code != boost::asio::error::not_socket) {
            state_ = StateEnum_::kConnected;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }
    state_ = StateEnum_::kClosed;

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Connect(
    const ZTCPEndpoint& _tcp_endpoint,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kOpened
    );

#if USE_DEBUG_LOG
    ZString ip_str = _tcp_endpoint.IPString();
    UInt16 port = _tcp_endpoint.Port();
#endif

    state_ = StateEnum_::kConnecting;

    //connect
    Z_DEBUG_LOG_START(
        L"Try to connect... ip: %ls port: %d",
        string::String2WString(ip_str.String()).String(),
        port
    );

    Int32 reconnect_times = 0;
    do {
        if (data_ptr_->if_endpoint_bind_) {
            data_ptr_->socket_.close();
            IPTypeEnum ip_type = 
                data_ptr_->bind_endpoint_.protocol() == boost::asio::ip::tcp::v6() ? IPTypeEnum::IP6 : IPTypeEnum::IP4;
            switch (ip_type) {
            case IPTypeEnum::IP4:
                data_ptr_->socket_.open(boost::asio::ip::tcp::v4(), error_code);
                break;
            case IPTypeEnum::IP6:
                data_ptr_->socket_.open(boost::asio::ip::tcp::v6(), error_code);
                break;
            }
            if (error_code) {
                ret_val = error_code::kPSocketErrorCode_SystemError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                break;
            }

            //bind endpoint
            data_ptr_->socket_.bind(data_ptr_->bind_endpoint_, error_code);
        }

        //connect
        data_ptr_->socket_.connect(
            *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint>(), error_code
        );

        //connect failed
        if (error_code) {
            if (state_ != StateEnum_::kConnecting) {
                Z_DEBUG_LOG_PROCESS(L"Connect stopped!");
				break;
            }
            else if (error_code == boost::asio::error::connection_refused) {
                if (_repeat_times <= reconnect_times) {
                    break;
                }
                reconnect_times += 1;
                Z_DEBUG_LOG_PROCESS(
                    L"Retry to connect... repeat_times: %d ip: %ls port: %d",
                    reconnect_times,
                    string::String2WString(ip_str.String()).String(),
                    port
                );
            }
            else {
                state_ = StateEnum_::kConnected;
                ret_val = error_code::kPSocketErrorCode_SystemError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                return ret_val;
            }
            continue;
        }

        //connect success
        OnConnectP();
        break;
    } while (true);

    if (state_ == StateEnum_::kConnecting) {
        state_ = StateEnum_::kOpened;
    }

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_ConnectFailed;
        Z_DEBUG_LOG_FAILURE(
            L"Socket connect failed! ip: %ls port: %d",
            string::String2WString(ip_str.String()).String(),
            port
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncConnect(
    const ZTCPEndpoint& _tcp_endpoint,
    const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_.Value(), StateEnum_::kOpened
    );

    //connect
    Z_DEBUG_LOG_START(
        L"Try to connect... ip: %ls port: %d",
        string::String2WString(_tcp_endpoint.IPString().String()).String(),
        _tcp_endpoint.Port()
    );

    ReturnType link_code = kOK;
    state_ = StateEnum_::kConnecting;
    link_code = AsyncConnectExecuteP(
        _tcp_endpoint,
        _handle_func,
        _repeat_times,
        0
    );
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncConnectExecuteP() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncConnectExecuteP(
    const ZTCPEndpoint& _tcp_endpoint,
    const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times,
    Int32 _reconnect_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (data_ptr_->if_endpoint_bind_) {
        //open
        data_ptr_->socket_.close();
        data_ptr_->socket_.open(boost::asio::ip::tcp::v4(), error_code);
        if (error_code) {
            state_ = StateEnum_::kConnected;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }

        //bind endpoint
        data_ptr_->socket_.bind(data_ptr_->bind_endpoint_, error_code);
        if (error_code) {
            state_ = StateEnum_::kConnected;
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            return ret_val;
        }
    }

    Z_DEBUG_LOG_PROCESS(
        L"Connecting... ip: %ls port: %d",
        string::String2WString(_tcp_endpoint.IPString().String()).String(),
        _tcp_endpoint.Port()
    );

    //start connect
    data_ptr_->socket_.async_connect(
        *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint>(),
        MakeSocketHandlerAllocator([
            this, _tcp_endpoint, _handle_func, _repeat_times, _reconnect_times
        ](
            const boost::system::error_code& _error_code
        ) mutable {
            ReturnType ret_val = kOK;
            ReturnType link_code = kOK;
            //handle error
            if (_error_code) {
                //handle error
                if (_error_code == boost::asio::error::connection_refused) {
                    Bool if_retry = false;
                    //retry
                    if (state_ != StateEnum_::kConnecting) {
                        Z_DEBUG_LOG_PROCESS(L"Connect stopped!");
                    }
                    else if (_reconnect_times < _repeat_times) {
                        _reconnect_times += 1;
                        if_retry = true;
                        Z_DEBUG_LOG_PROCESS(
                            L"Retry to connect... repeat_times: %d ip: %ls port: %d",
                            _reconnect_times,
                            string::String2WString(_tcp_endpoint.IPString().String()).String(),
                            _tcp_endpoint.Port()
                        );
                    }
                    if (if_retry) {
                        link_code = AsyncConnectExecuteP(
                            _tcp_endpoint,
                            _handle_func,
                            _repeat_times,
                            _reconnect_times
                        );
                        if (link_code != kOK) {
                            ret_val = error_code::kPSocketErrorCode_LinkError;
                            Z_LOG_ERROR(
                                ret_val, link_code,
                                L"ZTCPSocket::AsyncConnectExecuteP() link error!"
                            );
                            _handle_func(ret_val, this);
                            return;
                        }
                        return;
                    }

                    ret_val = error_code::kPSocketErrorCode_ConnectFailed;
                    //timeout
                    Z_DEBUG_LOG_FAILURE(
                        L"Connect failed! ip: %ls port: %ls",
                        string::String2WString(_tcp_endpoint.IPString().String()).String(),
                        _tcp_endpoint.Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                    Z_DEBUG_LOG_FAILURE(
                        L"Connect canceled! ip: %ls port: %ls",
                        string::String2WString(_tcp_endpoint.IPString().String()).String(),
                        _tcp_endpoint.Port()
                    );
                }
                else {
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                if (state_ == StateEnum_::kConnecting) {
                    state_ = StateEnum_::kOpened;
                }

                if (_handle_func) {
                    _handle_func(ret_val, this);
                }
                return;
            }

            //connect success
            OnConnectP();
            if (_handle_func) {
                _handle_func(ret_val, this);
            }
        })
    );

    return ret_val;
}

NODISCARD Void ZTCPSocket::StopConnect() noexcept {
    if (state_ == StateEnum_::kConnecting) {
        state_ = StateEnum_::kOpened;
        Z_DEBUG_LOG_PROCESS(L"Stop connecting...");
    }
}

NODISCARD ReturnType ZTCPSocket::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = data_ptr_->socket_.read_some(
        boost::asio::buffer(_buffer.DataPtr<Void>(), _buffer.Size()), error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(RemoteEndpoint().IPString().String()).String(),
                RemoteEndpoint().Port()
            );
            return ret_val;
        } 
        else {
            state_ = StateEnum_::kError;
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

NODISCARD ReturnType ZTCPSocket::AsyncRead(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_.async_read_some(
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
                    ret_val = error_code::kPSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(RemoteEndpoint().IPString().String()).String(),
                        RemoteEndpoint().Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
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

NODISCARD ReturnType ZTCPSocket::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = boost::asio::read_until(
        data_ptr_->socket_, _buffer_ptr->data_ptr_->buffer_, _match_char, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(RemoteEndpoint().IPString().String()).String(),
                RemoteEndpoint().Port()
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

NODISCARD ReturnType ZTCPSocket::ReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = boost::asio::read_until(
        data_ptr_->socket_, _buffer_ptr->data_ptr_->buffer_, _match_str, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(RemoteEndpoint().IPString().String()).String(),
                RemoteEndpoint().Port()
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

NODISCARD ReturnType ZTCPSocket::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    boost::asio::async_read_until(
        data_ptr_->socket_, _buffer_ptr->data_ptr_->buffer_, _match_char,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(RemoteEndpoint().IPString().String()).String(),
                        RemoteEndpoint().Port()
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
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
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

NODISCARD ReturnType ZTCPSocket::AsyncReadUntil(
    ZSocketBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZSocketBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    boost::asio::async_read_until(
        data_ptr_->socket_, _buffer_ptr->data_ptr_->buffer_, _match_str,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(RemoteEndpoint().IPString().String()).String(),
                        RemoteEndpoint().Port()
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
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
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

NODISCARD ReturnType ZTCPSocket::ReadUntilClose(
    ZSocketBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = 0ULL;
    do {
        data_size += boost::asio::read(
            data_ptr_->socket_, 
            _buffer_ptr->data_ptr_->buffer_, 
            boost::asio::transfer_at_least(1), 
            error_code
        );
    } while (!error_code);

    if (IS_DICONNECT_ERROR(error_code)) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kPSocketErrorCode_Disconnected;
        Z_DEBUG_LOG_FINISH(
            L"Socket disconnected! ip: %ls port: %d",
            string::String2WString(RemoteEndpoint().IPString().String()).String(),
            RemoteEndpoint().Port()
        );
        return ret_val;
    }
    else {
        state_ = StateEnum_::kError;
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

NODISCARD ReturnType ZTCPSocket::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }
    
    SizeType length = data_ptr_->socket_.write_some(
        boost::asio::buffer(_buffer.DataPtr<const Void>(), _buffer.Size()),
        error_code
    );
    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                L"Socket disconnected! ip: %ls port: %d",
                string::String2WString(RemoteEndpoint().IPString().String()).String(),
                RemoteEndpoint().Port()
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
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

NODISCARD ReturnType ZTCPSocket::AsyncWrite(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kPSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_.async_write_some(
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
                    ret_val = error_code::kPSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        L"Socket disconnected! ip: %ls port: %d",
                        string::String2WString(RemoteEndpoint().IPString().String()).String(),
                        RemoteEndpoint().Port()
                    );
                }
                else if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }
        })
    );

    return ret_val;
}

NODISCARD SizeType ZTCPSocket::ReadableDataSize() noexcept {
    if (data_ptr_ == nullptr) {
        return 0ULL;
    }
    return data_ptr_->socket_.available();
}

Void ZTCPSocket::MoveP(ZTCPSocket&& _socket) noexcept {
    data_ptr_ = std::move(_socket.data_ptr_);
    state_ = _socket.state_;
    io_context_ptr_ = _socket.io_context_ptr_;
    link_object_ptr_ = _socket.link_object_ptr_;
    _socket.state_ = StateEnum_::kUninitialized;
    _socket.io_context_ptr_ = nullptr;
    _socket.link_object_ptr_ = nullptr;
}

Void ZTCPSocket::OnConnectP() noexcept {
    state_ = StateEnum_::kConnected;
    data_ptr_->remote_endpoint_ = data_ptr_->socket_.remote_endpoint();

    Z_DEBUG_LOG_SUCCESS(
        L"Socket connected! ip: %ls port: %d",
        string::String2WString(RemoteEndpoint().IPString().String()).String(),
        RemoteEndpoint().Port()
    );
}

NODISCARD Void ZTCPSocket::SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
    data_ptr_->async_error_handle_func_ = std::forward<TFunction<Void(ReturnType)>>(_handle_func);
}

}//socket
}//zengine