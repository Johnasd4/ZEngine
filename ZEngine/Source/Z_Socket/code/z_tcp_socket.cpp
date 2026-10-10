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
    Contact: 1152325286@qq.com85
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_tcp_socket.h"

#include "z_socket_allocator.h"

#include "internal/z_socket_delete_manager.h"

namespace zengine {
namespace socket {

Void ZTCPSocket::operator delete(ZTCPSocket* _socket_ptr, std::destroying_delete_t) noexcept {
    if (_socket_ptr->state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Socket not closed before destructing! state: {}",
            static_cast<Int32>(_socket_ptr->state_.Load())
        );
    }
    _socket_ptr->Close();

    internal::ZTCPSocketDeleteManager::GetInstance().DeleteSocket(_socket_ptr);
}

ZTCPSocket::ZTCPSocket(ZIOContext& _io_context) noexcept
    : SuperType_()
    , state_(StateEnum_::kClosed)
    , async_operation_count_(0)
    , if_endpoint_bind_(false)
    , if_reuse_address_(false)
    , socket_(_io_context.io_context_)
    , bind_endpoint_()
    , remote_endpoint_()
    , async_error_handle_func_()
    , io_context_(_io_context)
    , link_object_ptr_(nullptr)
{}

ZTCPSocket::~ZTCPSocket() noexcept {
    if (state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Socket not closed before destructing! state: {}",
            static_cast<Int32>(state_.Load())
        );
    }
    Close();
}

NODISCARD ReturnType ZTCPSocket::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    socket_.set_option(asio::socket_base::send_buffer_size(_size), error_code);
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

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    socket_.set_option(asio::socket_base::receive_buffer_size(_size), error_code);
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

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::SetIfReuseAddress(Bool _if_reuse) noexcept {
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

NODISCARD ReturnType ZTCPSocket::Open(IPTypeEnum _ip_type) noexcept {
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
        socket_.open(asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        socket_.open(asio::ip::tcp::v6(), error_code);
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
        asio::ip::tcp::socket::reuse_address(if_reuse_address_), 
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

NODISCARD ReturnType ZTCPSocket::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    bind_endpoint_ = _tcp_endpoint;
    if_endpoint_bind_ = true;

    return ret_val;
}

Void ZTCPSocket::Cancel() noexcept {
    std::error_code error_code;
    socket_.cancel(error_code);
}

Void ZTCPSocket::Close() noexcept {
    std::error_code error_code;

    if (state_ == StateEnum_::kClosed) {
        return;
    }

    if_endpoint_bind_ = false;
    async_error_handle_func_ = nullptr;
    socket_.cancel(error_code);
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both, error_code);
    socket_.close(error_code);

    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZTCPSocket::Connect(
    const ZTCPEndpoint& _tcp_endpoint,
    Int32 _repeat_times
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

#if USE_DEBUG_LOG
    ZString ip_str = _tcp_endpoint.GenerateIPString();
    UInt16 port = _tcp_endpoint.GetPort();
#endif

    state_ = StateEnum_::kConnecting;

    //connect
    Z_DEBUG_LOG_START(
        "Try to connect... ip: {} port: {}",
        ip_str, port
    );

    Int32 reconnect_times = 0;
    do {
        if (if_endpoint_bind_) {
            socket_.close();
            switch (bind_endpoint_.GetIPType()) {
            case IPTypeEnum::IP4:
                socket_.open(asio::ip::tcp::v4(), error_code);
                break;
            case IPTypeEnum::IP6:
                socket_.open(asio::ip::tcp::v6(), error_code);
                break;
            }
            if (error_code) {
                ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    "System error! error info: {}",
                    error_code.message()
                );
                return ret_val;
            }

            socket_.set_option(
                asio::ip::tcp::socket::reuse_address(if_reuse_address_),
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

            //bind endpoint
            socket_.bind(bind_endpoint_.endpoint_, error_code);
        }

        //connect
        socket_.connect(_tcp_endpoint.endpoint_, error_code);

        //connect failed
        if (error_code) {
            if (state_ != StateEnum_::kConnecting) {
                Z_DEBUG_LOG_PROCESS("Connect stopped!");
				break;
            }
            else if (error_code == asio::error::connection_refused) {
                if (_repeat_times <= reconnect_times) {
                    break;
                }
                reconnect_times += 1;
                Z_DEBUG_LOG_PROCESS(
                    "Retry to connect... repeat_times: {} ip: {} port: {}",
                    reconnect_times,
                    ip_str,
                    port
                );
            }
            else {
                state_ = StateEnum_::kConnected;
                ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    "System error! error info: {}",
                    error_code.message()
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
        ret_val = error_code::kSocketErrorCode_ConnectFailed;
        Z_DEBUG_LOG_FAILURE(
            "Socket connect failed! ip: {} port: {}",
            ip_str,
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
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Socket state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    //connect
    Z_DEBUG_LOG_START(
        "Try to connect... ip: {} port: {}",
        _tcp_endpoint.GenerateIPString(),
        _tcp_endpoint.GetPort()
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
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncConnectExecuteP() link error!"
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
    std::error_code error_code;

    if (if_endpoint_bind_) {
        //open
        socket_.close();
        switch (bind_endpoint_.GetIPType()) {
        case IPTypeEnum::IP4:
            socket_.open(asio::ip::tcp::v4(), error_code);
            break;
        case IPTypeEnum::IP6:
            socket_.open(asio::ip::tcp::v6(), error_code);
            break;
        }
        if (error_code) {
            state_ = StateEnum_::kConnected;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
            );
            return ret_val;
        }

        socket_.set_option(
            asio::ip::tcp::socket::reuse_address(if_reuse_address_),
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

        //bind endpoint
        socket_.bind(bind_endpoint_.endpoint_, error_code);
        if (error_code) {
            state_ = StateEnum_::kConnected;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
            );
            return ret_val;
        }
    }

    Z_DEBUG_LOG_PROCESS(
        "Connecting... ip: {} port: {}",
        _tcp_endpoint.GenerateIPString(),
        _tcp_endpoint.GetPort()
    );

    async_operation_count_ += 1;

    //start connect
    socket_.async_connect(
        bind_endpoint_.endpoint_,
        MakeSocketHandlerAllocator([
            this, _tcp_endpoint, _handle_func, _repeat_times, _reconnect_times
        ](
            const std::error_code& _error_code
        ) mutable {
            ReturnType ret_val = kOK;
            ReturnType link_code = kOK;

            do {
                //handle error
                if (_error_code) {
                    //handle error
                    if (_error_code == asio::error::connection_refused) {
                        Bool if_retry = false;
                        //retry
                        if (state_ != StateEnum_::kConnecting) {
                            Z_DEBUG_LOG_PROCESS("Connect stopped!");
                        }
                        else if (_reconnect_times < _repeat_times) {
                            _reconnect_times += 1;
                            if_retry = true;
                            Z_DEBUG_LOG_PROCESS(
                                "Retry to connect... repeat_times: {} ip: {} port: {}",
                                _reconnect_times,
                                _tcp_endpoint.GenerateIPString(),
                                _tcp_endpoint.GetPort()
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
                                ret_val = error_code::kSocketErrorCode_LinkError;
                                Z_LOG_ERROR(
                                    ret_val, link_code,
                                    "ZTCPSocket::AsyncConnectExecuteP() link error!"
                                );
                                _handle_func(ret_val, this);
                                break;
                            }
                            break;
                        }

                        ret_val = error_code::kSocketErrorCode_ConnectRefused;
                        //timeout
                        Z_DEBUG_LOG_FAILURE(
                            "Connect failed! ip: {} port: {}",
                            _tcp_endpoint.GenerateIPString(),
                            _tcp_endpoint.GetPort()
                        );
                    }
                    else if (_error_code == asio::error::operation_aborted) {
                        ret_val = error_code::kSocketErrorCode_OperationCanceled;
                        Z_DEBUG_LOG_FAILURE(
                            "Connect canceled! ip: {} port: {}",
                            _tcp_endpoint.GenerateIPString(),
                            _tcp_endpoint.GetPort()
                        );
                    }
                    else {
                        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                        Z_LOG_ERROR(
                            ret_val, _error_code.value(),
                            "System error! error info: {}",
                            _error_code.message()
                        );
                    }

                    if (state_ == StateEnum_::kConnecting) {
                        state_ = StateEnum_::kOpened;
                    }
                }
                else {
                    //connect success
                    OnConnectP();
                }
                if (_handle_func) {
                    _handle_func(ret_val, this);
                }

            } while (false);

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD Void ZTCPSocket::StopConnect() noexcept {
    if (state_ == StateEnum_::kConnecting) {
        state_ = StateEnum_::kOpened;
        Z_DEBUG_LOG_PROCESS("Stop connecting...");
    }
}

NODISCARD ReturnType ZTCPSocket::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = socket_.read_some(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()), error_code
    );
    
    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                "Socket disconnected! ip: {} port: {}",
                remote_endpoint_.GenerateIPString(),
                remote_endpoint_.GetPort()
            );
            return ret_val;
        } 
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
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
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    async_operation_count_ += 1;

    socket_.async_read_some(
        asio::buffer(_buffer.GetDataPtr<Void>(), _buffer.GetSize()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<Void>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;            

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        "Socket disconnected! ip: {} port: {}",
                        remote_endpoint_.GenerateIPString(),
                        remote_endpoint_.GetPort()
                    );
                }
                else if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message()
                    );
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle read message
            if (_handle_func) {
                _handle_func(ret_val, this, ZConstBuffer(buffer_ptr, _data_size));
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::ReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = asio::read_until(
        socket_, _buffer_ptr->buffer_, _match_char, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                "Socket disconnected! ip: {} port: {}",
                remote_endpoint_.GenerateIPString(),
                remote_endpoint_.GetPort()
            );
            return ret_val;
        }
        else if (error_code == asio::error::not_found) {
            ret_val = error_code::kSocketErrorCode_ReadUntilSymbolNotFound;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "Match char not found!"
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
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
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = asio::read_until(
        socket_, _buffer_ptr->buffer_, _match_str, error_code
    );

    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                "Socket disconnected! ip: {} port: {}",
                remote_endpoint_.GenerateIPString(),
                remote_endpoint_.GetPort()
            );
            return ret_val;
        } 
        else if (error_code == asio::error::not_found) {
            ret_val = error_code::kSocketErrorCode_ReadUntilSymbolNotFound;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "Match string not found!"
            );
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
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
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    async_operation_count_ += 1;

    asio::async_read_until(
        socket_, _buffer_ptr->buffer_, _match_char,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        "Socket disconnected! ip: {} port: {}",
                        remote_endpoint_.GenerateIPString(),
                        remote_endpoint_.GetPort()
                    );
                }
                else if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (_error_code == asio::error::not_found) {
                    ret_val = error_code::kSocketErrorCode_ReadUntilSymbolNotFound;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "Match char not found!"
                    );
                }
                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message()
                    );
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle read message
            if (_handle_func) {
                _handle_func(ret_val, this, _buffer_ptr);
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    async_operation_count_ += 1;

    asio::async_read_until(
        socket_, _buffer_ptr->buffer_, _match_str,
        MakeSocketHandlerAllocator([this, _buffer_ptr, _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        "Socket disconnected! ip: {} port: {}",
                        remote_endpoint_.GenerateIPString(),
                        remote_endpoint_.GetPort()
                    );
                }
                else if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }
                else if (_error_code == asio::error::not_found) {
                    ret_val = error_code::kSocketErrorCode_ReadUntilSymbolNotFound;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "Match string not found!"
                    );
                }
                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message()
                    );
                }

                //call hook error handle func
                if (async_error_handle_func_) {
                    async_error_handle_func_(ret_val);
                }
            }

            //handle read message
            if (_handle_func) {
                _handle_func(ret_val, this, _buffer_ptr);
            }

            async_operation_count_ -= 1;
        })
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPSocket::ReadUntilClose(
    ZBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    SizeType data_size = 0ULL;
    do {
        data_size += asio::read(
            socket_, 
            _buffer_ptr->buffer_, 
            asio::transfer_at_least(1), 
            error_code
        );
    } while (!error_code);

    if (IS_DICONNECT_ERROR(error_code)) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_Disconnected;
        Z_DEBUG_LOG_FINISH(
            "Socket disconnected! ip: {} port: {}",
            remote_endpoint_.GenerateIPString(),
            remote_endpoint_.GetPort()
        );
    }
    else {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message()
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
    std::error_code error_code;

    if (state_ != StateEnum_::kConnected) {
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }
    
    SizeType length = socket_.write_some(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        error_code
    );
    if (error_code) {
        if (IS_DICONNECT_ERROR(error_code)) {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_Disconnected;
            Z_DEBUG_LOG_FINISH(
                "Socket disconnected! ip: {} port: {}",
                remote_endpoint_.GenerateIPString(),
                remote_endpoint_.GetPort()
            );
            return ret_val;
        }
        else if (IS_RETRY_ERROR(error_code)) {
            ret_val = error_code::kSocketErrorCode_TryResend;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, error_code.value(),
                "System error! error info: {}",
                error_code.message()
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
        ret_val = error_code::kSocketErrorCode_StateError;
        return ret_val;
    }

    async_operation_count_ += 1;

    socket_.async_write_some(
        asio::buffer(_buffer.GetDataPtr<const Void>(), _buffer.GetSize()),
        MakeSocketHandlerAllocator([this, buffer_ptr = _buffer.GetDataPtr<const Void>(), _handle_func](
            const std::error_code& _error_code,
            SizeType _data_size
        ) {
            ReturnType ret_val = kOK;

            //handle error
            if (_error_code) {
                if (IS_DICONNECT_ERROR(_error_code)) {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_Disconnected;
                    Z_DEBUG_LOG_FINISH(
                        "Socket disconnected! ip: {} port: {}",
                        remote_endpoint_.GenerateIPString(),
                        remote_endpoint_.GetPort()
                    );
                }
                else if (IS_RETRY_ERROR(_error_code)) {
                    ret_val = error_code::kSocketErrorCode_TryResend;
                    return ret_val;
                }
                else if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                }

                else {
                    state_ = StateEnum_::kError;
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message()
                    );
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

Void ZTCPSocket::OnConnectP() noexcept {
    std::error_code error_code;

    state_ = StateEnum_::kConnected;
    remote_endpoint_ = socket_.remote_endpoint(error_code);
    if(error_code) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_SystemOrLibraryError, error_code.value(),
            "System error! error info: {}",
            error_code.message()
        );
    }

    Z_DEBUG_LOG_SUCCESS(
        "Socket connected! ip: {} port: {}",
        remote_endpoint_.GenerateIPString(),
        remote_endpoint_.GetPort()
    );
}

} // socket
} // zengine