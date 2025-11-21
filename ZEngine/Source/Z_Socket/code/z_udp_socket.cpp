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

#include "z_udp_socket.h"

#include "z_core/m_log.h"
#include "z_core/z_string.h"

#include "z_io_context.h"

#include "data/z_io_context_data.h"
#include "data/z_udp_socket_data.h"
#include "z_socket_allocator.h"

namespace zengine {
namespace socket {

ZUDPSocket::ZUDPSocket() noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZUDPSocketState_Uninitialized)
    , io_context_ptr_(nullptr)
{}

ZUDPSocket::ZUDPSocket(const ZUDPSocket& _socket) noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZUDPSocketState_Uninitialized)
    , io_context_ptr_(_socket.io_context_ptr_)
{
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZUDPSocketData(&io_context_ptr_->data_ptr_->io_context_));
        state_ = ZUDPSocketState_Idle;
    }
}

ZUDPSocket::ZUDPSocket(ZUDPSocket&& _socket) noexcept
    : SuperType_(std::forward<ZUDPSocket>(_socket))
{
    MoveP(std::forward<ZUDPSocket>(_socket));
}

ZUDPSocket::ZUDPSocket(ZIOContext* _context_ptr) noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZUDPSocketState_Uninitialized)
    , io_context_ptr_(_context_ptr)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZUDPSocketData>(&_context_ptr->data_ptr_->io_context_);
    state_ = ZUDPSocketState_Idle;
}

ZUDPSocket::~ZUDPSocket() noexcept {
    ReturnType link_code = kOK;
    link_code = Close();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZUDPSocket::Close() link error!"
        );
        return;
    }
}

ZUDPSocket& ZUDPSocket::operator=(const ZUDPSocket& _socket) noexcept {
    if (_socket.io_context_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZUDPSocketData(&io_context_ptr_->data_ptr_->io_context_));
        state_ = ZUDPSocketState_Idle;
    }
    else {
        data_ptr_.Reset();
        state_ = ZUDPSocketState_Uninitialized;
    }
    return *this;
}

ZUDPSocket& ZUDPSocket::operator=(ZUDPSocket&& _socket) noexcept {
    SuperType_::operator=(std::forward<ZUDPSocket>(_socket));
    MoveP(std::forward<ZUDPSocket>(_socket));
    return *this;
}

NODISCARD ReturnType ZUDPSocket::Initialize(ZIOContext* _io_context_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != ZUDPSocketState_Uninitialized,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Uninitialized
    );

    Z_CHECK(
        _io_context_ptr == nullptr,
        error_code::kPSocketErrorCode_NullptrParam,
        L"_io_context_ptr is nullptr!"
    );

    data_ptr_ = MakeUnique<internal::ZUDPSocketData>(&_io_context_ptr->data_ptr_->io_context_);
    io_context_ptr_ = _io_context_ptr;
    state_ = ZUDPSocketState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::BindEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        _port < 0 || _port > 65535,
        error_code::kPSocketErrorCode_PortNotVaild,
        L"Expect port 0 ~ 65535! port: %d",
        _port
    );

    boost::system::error_code error_code;
    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Address not vaild! address: %ls",
            string::String2WString(_address_str).String()
        );
        return ret_val;
    }

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }
    data_ptr_->socket_.bind(boost::asio::ip::udp::endpoint(address, _port), error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    data_ptr_->if_endpoint_bind_ = true;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );

    boost::system::error_code error_code;
    data_ptr_->socket_.set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );

    boost::system::error_code error_code;
    data_ptr_->socket_.set_option(boost::asio::socket_base::receive_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD Void ZUDPSocket::SetAsyncErrorHandleFunction(TFunction<Void()>&& _handle_func) noexcept {
    data_ptr_->async_error_handle_func_ = std::forward<TFunction<Void()>>(_handle_func);
}

NODISCARD ReturnType ZUDPSocket::Cancel() noexcept {
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
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    if (state_ == ZUDPSocketState_Uninitialized) {
        return ret_val;
    }

    try {
        data_ptr_->if_endpoint_bind_ = false;
        data_ptr_->if_connected_ = false;
        data_ptr_->async_error_handle_func_ = nullptr;
        if (data_ptr_->socket_.is_open()) {
            data_ptr_->socket_.cancel();
            data_ptr_->socket_.close();
        }
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Connect(
    const Char* _address_str,
    Int32 _port,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        _port < 0 || _port > 65535,
        error_code::kPSocketErrorCode_PortNotVaild,
        L"Expect port 0 ~ 65535! port: %d",
        _port
    );

    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Address not vaild! address: %ls",
            string::String2WString(_address_str).String()
        );
        return ret_val;
    }

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }
    data_ptr_->socket_.connect(boost::asio::ip::udp::endpoint(address, _port), error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    data_ptr_->if_connected_ = true;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::ReceiveFrom(
    ZBuffer _buffer,
    SizeType* _message_size_ptr,
    ZString* _address_str_ptr,
    Int32* _port_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_endpoint_bind_ == false,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Socket endpoint not bind!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    boost::asio::ip::udp::endpoint endpoint;

    SizeType data_size = data_ptr_->socket_.receive_from(
        boost::asio::buffer(_buffer.BufferPtr<Void*>(), _buffer.Size()),
        endpoint,
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    if (_address_str_ptr != nullptr) {
        *_address_str_ptr = endpoint.address().to_string().c_str();
    }
    if (_port_ptr != nullptr) {
        *_port_ptr = static_cast<Int32>(endpoint.port());
    }
    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncReceiveFrom(
    ZBuffer _buffer,
    const TFunction<Void(ZUDPSocket*, const ZConstBuffer, const Char*, Int32)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_endpoint_bind_ == false,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Socket endpoint not bind!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    data_ptr_->socket_.async_receive_from(
        boost::asio::buffer(_buffer.BufferPtr<Void*>(), _buffer.Size()), data_ptr_->async_receive_endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.BufferPtr<const Void*>(), _handle_func](
            const boost::system::error_code& _error_code, 
            SizeType _message_size
        ) {
            //handle error
            if (_error_code) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(_error_code.message().c_str()).String()
                );
                state_ = ZUDPSocketState_Error;

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle write message
            if (_handle_func) {
                _handle_func(
                    this, ZConstBuffer(buffer_ptr, _message_size),
                    data_ptr_->async_receive_endpoint_.address().to_string().c_str(),
                    static_cast<Int32>(data_ptr_->async_receive_endpoint_.port())
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
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_endpoint_bind_ == false,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Socket endpoint not bind!"
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kPSocketErrorCode_UDPSocketNotConnected,
        L"Socket not connected!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    SizeType data_size = data_ptr_->socket_.receive(
        boost::asio::buffer(_buffer.BufferPtr<Void*>(), _buffer.Size()),
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_size;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncReceive(
    ZBuffer _buffer,
    const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_endpoint_bind_ == false,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Socket endpoint not bind!"
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kPSocketErrorCode_UDPSocketNotConnected,
        L"Socket not connected!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    data_ptr_->socket_.async_receive_from(
        boost::asio::buffer(_buffer.BufferPtr<Void*>(), _buffer.Size()), data_ptr_->async_receive_endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.BufferPtr<const Void*>(), _handle_func](
            const boost::system::error_code& _error_code,
            SizeType _message_size
        ) {
            //handle error
            if (_error_code) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(_error_code.message().c_str()).String()
                );
                state_ = ZUDPSocketState_Error;

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle write message
            if (_handle_func) {
                _handle_func(this, ZConstBuffer(buffer_ptr, _message_size));
            }
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SendTo(
    const Char* _address_str,
    Int32 _port,
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Address not vaild! address: %ls",
            string::String2WString(_address_str).String()
        );
        return ret_val;
    }

    boost::asio::ip::udp::endpoint endpoint(address, _port);

    data_ptr_->socket_.send_to(
        boost::asio::buffer(_buffer.BufferPtr<const Void*>(), _buffer.Size()),
        endpoint,
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSendTo(
    const Char* _address_str,
    Int32 _port,
    ZConstBuffer _buffer,
    const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    boost::asio::ip::address address = boost::asio::ip::make_address(_address_str, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Address not vaild! address: %ls",
            string::String2WString(_address_str).String()
        );
        return ret_val;
    }

    data_ptr_->socket_.async_send_to(
        boost::asio::buffer(_buffer.BufferPtr<const Void*>(), _buffer.Size()),
        boost::asio::ip::udp::endpoint(address, _port), 
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.BufferPtr<const Void*>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(_error_code.message().c_str()).String()
                );
                state_ = ZUDPSocketState_Error;

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle write message
            if (_handle_func) {
                _handle_func(this, ZConstBuffer(buffer_ptr, _data_size));
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
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kPSocketErrorCode_UDPSocketNotConnected,
        L"Socket not connected!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    data_ptr_->socket_.send(
        boost::asio::buffer(_buffer.BufferPtr<const Void*>(), _buffer.Size()),
        0,
        error_code
    );
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls",
            string::String2WString(error_code.message().c_str()).String()
        );
        state_ = ZUDPSocketState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSend(
    ZConstBuffer _buffer,
    const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZUDPSocketState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZUDPSocketState_Idle
    );
    Z_CHECK(
        data_ptr_->if_connected_ == false,
        error_code::kPSocketErrorCode_UDPSocketNotConnected,
        L"Socket not connected!"
    );

    if (!data_ptr_->socket_.is_open()) {
        data_ptr_->socket_.open(boost::asio::ip::udp::v4(), error_code);
        if (error_code) {
            ret_val = error_code::kPSocketErrorCode_SystemError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                L"System error! error info: %ls",
                string::String2WString(error_code.message().c_str()).String()
            );
            state_ = ZUDPSocketState_Error;
            return ret_val;
        }
    }

    data_ptr_->socket_.async_send(
        boost::asio::buffer(_buffer.BufferPtr<const Void*>(), _buffer.Size()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.BufferPtr<const Void*>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            //handle error
            if (_error_code) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(_error_code.message().c_str()).String()
                );
                state_ = ZUDPSocketState_Error;

                //call hook error handle func
                if (data_ptr_->async_error_handle_func_) {
                    data_ptr_->async_error_handle_func_();
                }

                return;
            }

            //handle write message
            if (_handle_func) {
                _handle_func(this, ZConstBuffer(buffer_ptr, _data_size));
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
    _socket.state_ = ZUDPSocketState_Uninitialized;
    _socket.io_context_ptr_ = nullptr;
    _socket.link_object_ptr_ = nullptr;
}

}//socket
}//zengine