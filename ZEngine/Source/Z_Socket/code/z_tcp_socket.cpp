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

#include "data/z_tcp_client_data.h"
#include "data/z_tcp_server_data.h"
#include "data/z_tcp_socket_data.h"

#include "z_tcp_client.h"
#include "z_tcp_server.h"

namespace zengine {
namespace socket {

ZTCPSocket::ZTCPSocket() noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZTCPSocketState_Uninitialized)
{}

ZTCPSocket::ZTCPSocket(const ZTCPSocket& _socket) noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , state_(ZTCPSocketState_Uninitialized)
{
    if (_socket.data_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZTCPSocketData(_socket.data_ptr_->io_context_ptr_));
        state_ = ZTCPSocketState_Idle;
    }
}

ZTCPSocket::ZTCPSocket(ZTCPSocket&& _socket) noexcept
    : SuperType_(std::forward<ZTCPSocket>(_socket))
{
    MoveP(std::forward<ZTCPSocket>(_socket));
}

ZTCPSocket::ZTCPSocket(ZTCPClient* _client_ptr) noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZTCPSocketData>(&_client_ptr->data_ptr_->io_context_))
    , state_(ZTCPSocketState_Idle)
{
}
ZTCPSocket::ZTCPSocket(ZTCPSingleSessionServer* _server_ptr) noexcept
    : data_ptr_(MakeUnique<internal::ZTCPSocketData>(&_server_ptr->data_ptr_->io_context_))
{
}
ZTCPSocket::ZTCPSocket(ZTCPMultipleSessionServer* _server_ptr) noexcept
    : data_ptr_(MakeUnique<internal::ZTCPSocketData>(&_server_ptr->data_ptr_->io_context_))
{
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
    if (_socket.data_ptr_ != nullptr) {
        data_ptr_.Reset(new internal::ZTCPSocketData(_socket.data_ptr_->io_context_ptr_));
        state_ = ZTCPSocketState_Idle;
    }
    else {
        data_ptr_.Reset(nullptr);
        state_ = ZTCPSocketState_Uninitialized;
    }
    return *this;
}

ZTCPSocket& ZTCPSocket::operator=(ZTCPSocket&& _socket) noexcept {
    SuperType_::operator=(std::forward<ZTCPSocket>(_socket));
    MoveP(std::forward<ZTCPSocket>(_socket));
    return *this;
}

NODISCARD const ZString ZTCPSocket::RemoteAddressString() noexcept {
    return data_ptr_->address_string_;
}
NODISCARD const ZString ZTCPSocket::RemotePortString() noexcept {
    return data_ptr_->port_string_;
}

NODISCARD ReturnType ZTCPSocket::Initialize(ZTCPClient* _client_ptr) noexcept {

    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != ZTCPSocketState_Uninitialized,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Uninitialized
    );

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_client_ptr->data_ptr_->io_context_);

    return ret_val;
}
NODISCARD ReturnType ZTCPSocket::Initialize(ZTCPSingleSessionServer* _server_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSocketState_Uninitialized,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Uninitialized
    );

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_server_ptr->data_ptr_->io_context_);

    return ret_val;
}
NODISCARD ReturnType ZTCPSocket::Initialize(ZTCPMultipleSessionServer* _server_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSocketState_Uninitialized,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Uninitialized
    );

    data_ptr_ = MakeUnique<internal::ZTCPSocketData>(&_server_ptr->data_ptr_->io_context_);

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
    data_ptr_->socket_ptr_->set_option(boost::asio::socket_base::send_buffer_size(_size), error_code);
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
    data_ptr_->socket_ptr_->set_option(boost::asio::socket_base::receive_buffer_size(_size), error_code);
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

NODISCARD ReturnType ZTCPSocket::Close() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPSocketState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Socket state error! state: %d expect state: %d",
        state_, ZTCPSocketState_Idle
    );

    try {
        data_ptr_->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        data_ptr_->socket_ptr_->close();
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

    if (state_ == ZTCPSocketState_Idle || state_ == ZTCPSocketState_Uninitialized) {
        return ret_val;
    }

    try {
        data_ptr_->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        data_ptr_->socket_ptr_->close();
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

NODISCARD ReturnType ZTCPSocket::Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_ptr_->socket_ptr_->read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
    }
    else {
        data_ptr_->socket_ptr_->read_some(boost::asio::buffer(_data_buffer, _buffer_size), error_code);
    }

    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_ptr_->close(error_code);
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %ls",
                string::String2WString(data_ptr_->address_string_.String()).String(),
                string::String2WString(data_ptr_->port_string_.String()).String()
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
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncRead(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_ptr_->async_read_some(
        boost::asio::buffer(_data_buffer, _buffer_size), 
        [this, _data_buffer, &_handle_func](
            const std::error_code& _error_code,
            SizeType _read_length
        ) {
            //handle error
            if (_error_code) {
                if (
                    _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_ptr_->close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %ls",
                        string::String2WString(data_ptr_->address_string_.String()).String(),
                        string::String2WString(data_ptr_->port_string_.String()).String()
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
            _handle_func(this, _data_buffer, _read_length);
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncRead(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TSimpleFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_ptr_->async_read_some(
        boost::asio::buffer(_data_buffer, _buffer_size),
        [this, _data_buffer, &_handle_func](
            const std::error_code& _error_code,
            SizeType _read_length
        ) {
            //handle error
            if (_error_code) {
                if (
                    _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_ptr_->close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %ls",
                        string::String2WString(data_ptr_->address_string_.String()).String(),
                        string::String2WString(data_ptr_->port_string_.String()).String()
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
            _handle_func(this, _data_buffer, _read_length);
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::Write(
    const Void* _data_buffer, 
    SizeType _date_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType length = data_ptr_->socket_ptr_->write_some(boost::asio::buffer(_data_buffer, _date_size), error_code);
    if (error_code) {
        if (error_code == boost::asio::error::connection_reset || error_code.value() == ERROR_FILE_NOT_FOUND) {
            data_ptr_->socket_ptr_->close(error_code);
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            Z_LOG_FINISH(
                L"Socket disconnected! address: %ls port: %ls",
                string::String2WString(data_ptr_->address_string_.String()).String(),
                string::String2WString(data_ptr_->port_string_.String()).String()
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
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncWrite(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_ptr_->async_write_some(
        boost::asio::buffer(_data_buffer, _buffer_size),
        [this, _data_buffer, &_handle_func](
            const std::error_code& _error_code,
            SizeType _write_length
        ) {
            //handle error
            if (_error_code) {
                if (
                    _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                    ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_ptr_->close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %ls",
                        string::String2WString(data_ptr_->address_string_.String()).String(),
                        string::String2WString(data_ptr_->port_string_.String()).String()
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
            _handle_func(this, _data_buffer, _write_length);
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncWrite(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TSimpleFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != ZTCPSocketState_Connect) {
        ret_val = error_code::kZSocketErrorCode_StateError;
        return ret_val;
    }

    data_ptr_->socket_ptr_->async_write_some(
        boost::asio::buffer(_data_buffer, _buffer_size),
        [this, _data_buffer, &_handle_func](
            const std::error_code& _error_code,
            SizeType _write_length
        ) {
            //handle error
            if (_error_code) {
                if (
                    _error_code.value() == boost::asio::error::connection_reset ||
                    _error_code.value() == ERROR_FILE_NOT_FOUND
                    ) {
                    boost::system::error_code error_code;
                    data_ptr_->socket_ptr_->close(error_code);
                    Z_LOG_FINISH(
                        L"Socket disconnected! address: %ls port: %ls",
                        string::String2WString(data_ptr_->address_string_.String()).String(),
                        string::String2WString(data_ptr_->port_string_.String()).String()
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
            _handle_func(this, _data_buffer, _write_length);
        }
    );

    return ret_val;
}

Void ZTCPSocket::MoveP(ZTCPSocket&& _socket) noexcept {
    data_ptr_ = std::move(_socket.data_ptr_);
    state_ = _socket.state_;
    link_object_ptr_ = _socket.link_object_ptr_;
    _socket.state_ = ZTCPSocketState_Uninitialized;
    _socket.link_object_ptr_ = nullptr;
}

Void ZTCPSocket::OnConnectP() noexcept {
    auto remote_endpoint = data_ptr_->socket_ptr_->remote_endpoint();
    data_ptr_->address_string_ = remote_endpoint.address().to_string().c_str();
    data_ptr_->port_string_.FromNum(remote_endpoint.port());

    state_ = ZTCPSocket::ZTCPSocketState_Connect;

    Z_LOG_SUCCESS(
        L"Socket connected! address: %ls port: %ls",
        string::String2WString(data_ptr_->address_string_.String()).String(),
        string::String2WString(data_ptr_->port_string_.String()).String()
    );
}

NODISCARD Void ZTCPSocket::SetAsyncErrorHandleFunction(TFunction<Void()>&& _handle_func) noexcept {
    data_ptr_->async_error_handle_func_ = std::forward<TFunction<Void()>>(_handle_func);
}

}//socket
}//zengine