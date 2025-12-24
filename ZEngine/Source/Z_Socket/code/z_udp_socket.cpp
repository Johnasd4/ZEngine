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

#include "z_udp_socket.h"

#include "z_io_context.h"
#include "z_socket_allocator.h"

#include "data/z_io_context_data.h"
#include "data/z_udp_socket_data.h"


namespace zengine {
namespace socket {

ZUDPSocket::ZUDPSocket() noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(nullptr)
{}

ZUDPSocket::ZUDPSocket(const ZUDPSocket& _socket) noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(_socket.io_context_ptr_)
{
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_= MakeUnique<internal::ZUDPSocketData>(&io_context_ptr_->data_ptr_->io_context_);
        state_ = StateEnum_::kClosed;
    }
}

ZUDPSocket::ZUDPSocket(ZUDPSocket&& _socket) noexcept
    : SuperType_(std::forward<ZUDPSocket>(_socket))
{
    MoveP(std::forward<ZUDPSocket>(_socket));
}

ZUDPSocket::ZUDPSocket(ZIOContext* _context_ptr) noexcept
    : SuperType_()
    , data_ptr_()
    , state_(StateEnum_::kUninitialized)
    , io_context_ptr_(_context_ptr)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_NullptrParam, 0,
            "_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZUDPSocketData>(&_context_ptr->data_ptr_->io_context_);
    state_ = StateEnum_::kClosed;
}

ZUDPSocket::~ZUDPSocket() noexcept {
    Close();
}

ZUDPSocket& ZUDPSocket::operator=(const ZUDPSocket& _socket) noexcept {
    if (this == &_socket) {
        return *this;
    }
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_ = MakeUnique<internal::ZUDPSocketData>(&io_context_ptr_->data_ptr_->io_context_);
        state_ = StateEnum_::kClosed;
    }
    else {
        data_ptr_.Reset();
        state_ = StateEnum_::kUninitialized;
    }
    return *this;
}

ZUDPSocket& ZUDPSocket::operator=(ZUDPSocket&& _socket) noexcept {
    if (this == &_socket) {
        return *this;
    }
    SuperType_::operator=(std::forward<ZUDPSocket>(_socket));
    MoveP(std::forward<ZUDPSocket>(_socket));
    return *this;
}

NODISCARD ZUDPEndpoint ZUDPSocket::LocalEndpoint() const noexcept {
    ZUDPEndpoint endpoint;
    *endpoint.endpoint_data_.GetDataPtr<boost::asio::ip::udp::endpoint>() = data_ptr_->socket_.local_endpoint();
    return endpoint;
}

NODISCARD ReturnType ZUDPSocket::Initialize(ZIOContext* _io_context_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != StateEnum_::kUninitialized,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kUninitialized
    );

    Z_CHECK(
        _io_context_ptr == nullptr,
        error_code::kSocketErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );

    data_ptr_ = MakeUnique<internal::ZUDPSocketData>(&_io_context_ptr->data_ptr_->io_context_);
    io_context_ptr_ = _io_context_ptr;
    state_ = StateEnum_::kClosed;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        data_ptr_->socket_.open(boost::asio::ip::udp::v6(), error_code);
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

    data_ptr_->socket_.set_option(
        boost::asio::ip::udp::socket::reuse_address(data_ptr_->if_reuse_address_),
        error_code
    );

    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::BindEndpoint(const ZUDPEndpoint& _udp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.bind(
        *_udp_endpoint.endpoint_data_.GetDataPtr<const boost::asio::ip::udp::endpoint>(), error_code
    );
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.set_option(boost::asio::socket_base::receive_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    data_ptr_->if_reuse_address_ = _if_reuse;

    return ret_val;
}

NODISCARD Void ZUDPSocket::SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
    data_ptr_->async_error_handle_func_ = std::forward<TFunction<Void(ReturnType)>>(_handle_func);
}

Void ZUDPSocket::Cancel() noexcept {
    boost::system::error_code error_code;

    data_ptr_->socket_.cancel(error_code);
}

Void ZUDPSocket::Close() noexcept {
    boost::system::error_code error_code;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kClosed) {
        return;
    }

    data_ptr_->if_connected_ = false;
    data_ptr_->async_error_handle_func_ = nullptr;
    data_ptr_->socket_.cancel(error_code);
    data_ptr_->socket_.close(error_code);

    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZUDPSocket::Connect(const ZUDPEndpoint& _udp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.connect(
        *_udp_endpoint.endpoint_data_.GetDataPtr<const boost::asio::ip::udp::endpoint>(), error_code
    );

    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::ReceiveFrom(
    ZBuffer _buffer,
    SizeType* _message_size_ptr,
    ZUDPEndpoint* _udp_endpoint_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    boost::asio::ip::udp::endpoint endpoint;

    SizeType data_size = data_ptr_->socket_.receive_from(
        boost::asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        endpoint,
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_size;
    }

    if (_udp_endpoint_ptr != nullptr) {
        *_udp_endpoint_ptr->endpoint_data_.GetDataPtr<boost::asio::ip::udp::endpoint>() =
            endpoint;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncReceiveFrom(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer, const ZUDPEndpoint&)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.async_receive_from(
        boost::asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()), 
        data_ptr_->async_receive_endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const boost::system::error_code& _error_code, 
            SizeType _message_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: %ls",
                        string::StringToWString(_error_code.message().c_str()).GetDataPtr()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(
                    ret_val, this, ZConstBuffer(buffer_ptr, _message_size),
                    *reinterpret_cast<const ZUDPEndpoint*>(&data_ptr_->async_receive_endpoint_)
                );
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Receive(
    ZBuffer _buffer,
    SizeType* _message_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    SizeType data_size = data_ptr_->socket_.receive(
        boost::asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncReceive(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    data_ptr_->socket_.async_receive_from(
        boost::asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()), data_ptr_->async_receive_endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _message_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: %ls",
                        string::StringToWString(_error_code.message().c_str()).GetDataPtr()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _message_size));
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SendTo(
    const ZUDPEndpoint& _udp_endpoint,
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.send_to(
        boost::asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        *_udp_endpoint.endpoint_data_.GetDataPtr<const boost::asio::ip::udp::endpoint>(),
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSendTo(
    const ZUDPEndpoint& _udp_endpoint,
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->socket_.async_send_to(
        boost::asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        *_udp_endpoint.endpoint_data_.GetDataPtr<const boost::asio::ip::udp::endpoint>(),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: %ls",
                        string::StringToWString(_error_code.message().c_str()).GetDataPtr()
                    );
                    state_ = StateEnum_::kError;
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

NODISCARD ReturnType ZUDPSocket::Send(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    data_ptr_->socket_.send(
        boost::asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).GetDataPtr()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSend(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    data_ptr_->socket_.async_send(
        boost::asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: %ls",
                        string::StringToWString(_error_code.message().c_str()).GetDataPtr()
                    );
                    state_ = StateEnum_::kError;
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

NODISCARD SizeType ZUDPSocket::ReadableDataSize() noexcept {
    return data_ptr_->socket_.available();
}

Void ZUDPSocket::MoveP(ZUDPSocket&& _socket) noexcept {
    data_ptr_ = std::move(_socket.data_ptr_);
    state_ = _socket.state_;
    io_context_ptr_ = _socket.io_context_ptr_;
    link_object_ptr_ = _socket.link_object_ptr_;
    _socket.state_ = StateEnum_::kUninitialized;
    _socket.io_context_ptr_ = nullptr;
    _socket.link_object_ptr_ = nullptr;
}

}//socket
}//zengine