/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_udp_socket.h"

#include "z_io_context.h"
#include "z_socket_allocator.h"

#include "internal/z_socket_delete_manager.h"

namespace zengine {
namespace socket {

Void ZUDPSocket::operator delete(ZUDPSocket* _socket_ptr, std::destroying_delete_t) noexcept {
    if (_socket_ptr->state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Socket not closed before destructing! state: {}",
            static_cast<Int32>(_socket_ptr->state_.Load())
        );
    }
    _socket_ptr->Close();

    internal::ZUDPSocketDeleteManager::GetInstance().DeleteSocket(_socket_ptr);
}

ZUDPSocket::ZUDPSocket(ZIOContext& _io_context) noexcept
    : SuperType_()
    , state_(StateEnum_::kClosed)
    , async_operation_count_(0)
    , if_connected_(false)
    , if_reuse_address_(false)
    , socket_(_io_context.io_context_)
    , async_receive_endpoint_()
    , async_error_handle_func_()
    , io_context_(_io_context)
    , link_object_ptr_(nullptr)
{
}

ZUDPSocket::~ZUDPSocket() noexcept {
    if (state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Socket not closed before destructing! state: {}",
            static_cast<Int32>(state_.Load())
        );
    }
    Close();
}

NODISCARD ReturnType ZUDPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    socket_.set_option(asio::socket_base::send_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    socket_.set_option(asio::socket_base::receive_buffer_size(_size), error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kClosed)
    );

    if_reuse_address_ = _if_reuse;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kClosed)
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        socket_.open(asio::ip::udp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        socket_.open(asio::ip::udp::v6(), error_code);
        break;
    default:
        ret_val = error_code::kSocketErrorCode_ParamOutOfRange;
        Z_LOG_ERROR(
            ret_val, 0,
            "Enum out of range! _ip_type: {}",
            static_cast<Int32>(_ip_type)
        );
        return ret_val;
    }

    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        return ret_val;
    }

    socket_.set_option(
        asio::ip::udp::socket::reuse_address(if_reuse_address_),
        error_code
    );

    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::BindEndpoint(const ZUDPEndpoint& _udp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    socket_.bind(_udp_endpoint.endpoint_, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

Void ZUDPSocket::Cancel() noexcept {
    std::error_code error_code;
    socket_.cancel(error_code);
}

Void ZUDPSocket::Shutdown() noexcept {
    if (state_ == StateEnum_::kOpened) {
        socket_.shutdown(asio::socket_base::shutdown_both);
        state_ = StateEnum_::kShutdown;
    }
}

Void ZUDPSocket::Close() noexcept {
    std::error_code error_code;

    if (state_ == StateEnum_::kClosed) {
        return;
    }

    if_connected_ = false;
    async_error_handle_func_ = nullptr;
    socket_.cancel(error_code);
    socket_.close(error_code);

    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZUDPSocket::Connect(const ZUDPEndpoint& _udp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    socket_.connect(_udp_endpoint.endpoint_, error_code);

    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    if_connected_ = true;

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::ReceiveFrom(
    ZBuffer _buffer,
    SizeType* _message_size_ptr,
    ZUDPEndpoint* _udp_endpoint_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    asio::ip::udp::endpoint endpoint;

    SizeType data_size = socket_.receive_from(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        endpoint,
        0,
        error_code
    );
    if (error_code) {
        if (IS_SHUTDOWN_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_SocketShutdown;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message().c_str()
            );
            return ret_val;
        }
    }

    if (_message_size_ptr != nullptr) {
        *_message_size_ptr = data_size;
    }

    if (_udp_endpoint_ptr != nullptr) {
        *_udp_endpoint_ptr = std::move(endpoint);
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncReceiveFrom(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer, const ZUDPEndpoint&)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    async_operation_count_ += 1;

    socket_.async_receive_from(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()), 
        async_receive_endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const std::error_code& _error_code, 
            SizeType _message_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (_error_code == asio::error::connection_refused) {
                    ret_val = error_code::kSocketErrorCode_ConnectRefused;
                }
                else if (IS_SHUTDOWN_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_SocketShutdown;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message().c_str()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(
                    ret_val, this, ZConstBuffer(buffer_ptr, _message_size),
                    ZUDPEndpoint(async_receive_endpoint_)
                );
            }

            async_operation_count_ -= 1;
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
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    Z_CHECK(
        if_connected_ == true,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    SizeType data_size = socket_.receive(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        0,
        error_code
    );
    if (error_code) {
        if (IS_SHUTDOWN_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_SocketShutdown;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message().c_str()
            );
            return ret_val;
        }
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
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    Z_CHECK(
        if_connected_ == true,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    async_operation_count_ += 1;

    socket_.async_receive(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _message_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (_error_code == asio::error::connection_refused) {
                    ret_val = error_code::kSocketErrorCode_ConnectRefused;
                }
                else if (IS_SHUTDOWN_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_SocketShutdown;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message().c_str()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _message_size));
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::SendTo(
    const ZUDPEndpoint& _udp_endpoint,
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    socket_.send_to(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        _udp_endpoint.endpoint_,
        0,
        error_code
    );
    if (error_code) {
        if (IS_RETRY_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_TryResend;
        }
        else if (IS_SHUTDOWN_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_SocketShutdown;
        }
        else {
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message().c_str()
            );
            state_ = StateEnum_::kError;
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSendTo(
    const ZUDPEndpoint& _udp_endpoint,
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    async_operation_count_ += 1;

    socket_.async_send_to(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        _udp_endpoint.endpoint_,
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (IS_RETRY_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_TryResend;
                }
                else if (IS_SHUTDOWN_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_SocketShutdown;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message().c_str()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::Send(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    Z_CHECK(
        if_connected_ == true,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    socket_.send(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        0,
        error_code
    );
    if (error_code) {
        if (IS_RETRY_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_TryResend;
        }
        else if (IS_SHUTDOWN_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_SocketShutdown;
        }
        else {
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message().c_str()
            );
            state_ = StateEnum_::kError;
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPSocket::AsyncSend(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    Z_CHECK(
        if_connected_ == true,
        error_code::kSocketErrorCode_UDPSocketNotConnected,
        "Socket not connected!"
    );

    async_operation_count_ += 1;

    socket_.async_send(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (IS_RETRY_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_TryResend;
                }
                else if (IS_SHUTDOWN_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_SocketShutdown;
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message().c_str()
                    );
                    state_ = StateEnum_::kError;
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle write message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

} // socket
} // zengine