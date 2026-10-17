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

#include "z_tcp_server.h"

#include "z_io_context.h"

#include "internal/z_socket_delete_manager.h"

//ZTCPSingleSessionServer
namespace zengine {
namespace socket {

ZTCPSingleSessionServer::ZTCPSingleSessionServer(ZIOContext& _io_context) noexcept
    : state_(StateEnum_::kClosed)
    , socket_ptr_(MakeUnique<ZTCPSocket>(_io_context))
    , acceptor_(_io_context.io_context_)
    , io_context_(_io_context)
{}

ZTCPSingleSessionServer::~ZTCPSingleSessionServer() noexcept {
    if (state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Server not closed before destructing! state: {}",
            static_cast<Int32>(state_.Load())
        );
    }
    Close();
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::SetOSWriteBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::SetOSReadBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(_if_reuse), error_code);
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

NODISCARD ReturnType ZTCPSingleSessionServer::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kClosed)
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        acceptor_.open(asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        acceptor_.open(asio::ip::tcp::v6(), error_code);
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
            error_code.message()
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::BindEndpoint(const ZTCPEndpoint& _endpoint) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    acceptor_.bind(_endpoint.endpoint_, error_code);
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message()
        );
        return ret_val;
    }

    state_ = StateEnum_::kEndpointBind;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Listen(Int32 _max_wait_connect_client_num) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kEndpointBind,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kEndpointBind)
    );

    acceptor_.listen(_max_wait_connect_client_num, error_code);
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

    state_ = StateEnum_::kListen;

    return ret_val;
}

Void ZTCPSingleSessionServer::Close() noexcept {
    std::error_code error_code;

    if (state_ == StateEnum_::kClosed) {
        return;
    }

    acceptor_.close(error_code);
    socket_ptr_->Close();

    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Accept() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kListen,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kListen)
    );

    Z_DEBUG_LOG_START("Wait for client connect...");

    acceptor_.accept(socket_ptr_->socket_, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message().c_str()
        );
        return ret_val;
    }

    socket_ptr_->SetAsyncErrorHandleFunction(
        [this](ReturnType error_code) {
            //disconnect
            if (socket_ptr_->state_ == ZTCPSocket::StateEnum_::kError) {
                socket_ptr_->Close();

                //connect->listen
                if (state_ == StateEnum_::kConnected) {
                    state_ = StateEnum_::kListen;
                    Z_DEBUG_LOG_FINISH("Client disconnected!");
                }
            }
        }
    );

    socket_ptr_->OnConnectP();
    state_ = StateEnum_::kConnected;
    Z_DEBUG_LOG_SUCCESS("Client connected!");

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->Read(_buffer, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_ptr_->Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::Read() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncRead(
    ZBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->AsyncRead(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->ReadUntil(_buffer_ptr, _match_char, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_ptr_->Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->ReadUntil(_buffer_ptr, _match_str, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_ptr_->Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->AsyncReadUntil(_buffer_ptr, _match_char, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->AsyncReadUntil(_buffer_ptr, _match_str, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::ReadUntilClose(
    ZBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->ReadUntilClose(_buffer_ptr, _data_size_ptr);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::ReadUntilClose() link error!"
        );
    }

    socket_ptr_->Close();

    if(state_ == StateEnum_::kConnected) {
        state_ = StateEnum_::kListen;
        Z_DEBUG_LOG_FINISH("Client disconnected!");
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->Write(_buffer);
    if (link_code != kOK) {
        if (link_code == error_code::kSocketErrorCode_Disconnected) {
            state_ = StateEnum_::kListen;
            Z_DEBUG_LOG_FINISH("Client disconnected!");
            socket_ptr_->Close();
            ret_val = error_code::kSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = StateEnum_::kError;
            ret_val = error_code::kSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZTCPSocket::Write() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionServer::AsyncWrite(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kConnected,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kConnected)
    );

    link_code = socket_ptr_->AsyncWrite(_buffer, _handle_func);
    if (link_code != kOK) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

} // socket
} // zengine

//ZTCPMultipleSessionServer
namespace zengine {
namespace socket {

ZTCPMultipleSessionServer::ZTCPMultipleSessionServer(ZIOContext& _io_context) noexcept
    : state_(StateEnum_::kClosed)
    , acceptor_(_io_context.io_context_)
    , socket_list_(&socket_pool_)
    , socket_pool_()
    , io_context_(_io_context)
{
    socket_pool_.SetExtendObjectModel(_io_context);
}

ZTCPMultipleSessionServer::~ZTCPMultipleSessionServer() noexcept {
    if (state_ != StateEnum_::kClosed) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_DestructedBeforeClose, 0,
            "Server not closed before destructing! state: {}",
            static_cast<Int32>(state_.Load())
        );
    }
    Close();
}

NODISCARD ReturnType ZTCPMultipleSessionServer::SetIfReuseAddress(Bool _if_reuse) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Client state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(_if_reuse), error_code);
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

NODISCARD ReturnType ZTCPMultipleSessionServer::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kClosed)
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        acceptor_.open(asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        acceptor_.open(asio::ip::tcp::v6(), error_code);
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
            error_code.message()
        );
        return ret_val;
    }

    state_ = StateEnum_::kOpened;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::BindEndpoint(const ZTCPEndpoint& _endpoint) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kOpened)
    );

    acceptor_.bind(
        _endpoint.endpoint_,
        error_code
    );
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message()
        );
        return ret_val;
    }

    state_ = StateEnum_::kEndpointBind;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Listen(Int32 _max_wait_connect_client_num) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    Z_CHECK(
        state_ == StateEnum_::kEndpointBind,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kEndpointBind)
    );

    acceptor_.listen(_max_wait_connect_client_num, error_code);
    if (error_code) {
        state_ = StateEnum_::kError;
        ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {}",
            error_code.message()
        );
        return ret_val;
    }

    state_ = StateEnum_::kListen;

    return ret_val;
}

Void ZTCPMultipleSessionServer::Close() noexcept {
    std::error_code error_code;

    if (state_ == StateEnum_::kClosed) {
        return;
    }
    
    state_ = StateEnum_::kClosed;

    //close acceptor
    acceptor_.close(error_code);

    //get operating socket list
    socket_list_mutex_.Lock();
    TArray<ZTCPSocket*> socket_array;
    socket_array.Reserve(socket_list_.GetSize());
    for (auto& socket_iter : socket_list_) {
        socket_array.PushBack(&socket_iter);
    }
    socket_list_mutex_.Unlock();


    //close socket
    for (auto& socket_ptr_iter : socket_array) {
        socket_ptr_iter->Close();
    }

    socket_list_mutex_.Lock();
    socket_list_.Clear();
    socket_list_mutex_.Unlock();
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncAccept(
    const TFunction<Void(ReturnType, ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ == StateEnum_::kListen,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kListen)
    );

    ZTCPSocket* socket_ptr = socket_pool_.Apply();
    acceptor_.async_accept(
        socket_ptr->socket_, 
        [this, socket_ptr, _handle_func](
            const std::error_code& _error_code
        ) mutable {
            ReturnType ret_val = kOK;
            ReturnType link_code = kOK;

            if (_error_code) {
                //close and release socket
                socket_ptr->Close();
                internal::ZTCPSocketDeleteManager::GetInstance().DeleteSocket(
                    socket_ptr, 
                    [this, socket_ptr]() {socket_pool_.Release(socket_ptr); }
                );              
                socket_ptr = nullptr;

                //handle error
                if (_error_code == asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                    Z_DEBUG_LOG_FAILURE("Server accept cancelled!");
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {}",
                        _error_code.message()
                    );
                }
            }
            else {
                socket_ptr->OnConnectP();
                socket_ptr->SetAsyncErrorHandleFunction(
                    [this, socket_ptr](ReturnType _error_code) {
                        //disconnect
                        if (socket_ptr->state_ == ZTCPSocket::StateEnum_::kError) {
                            Z_DEBUG_LOG_FINISH("Client disconnected!");
                            socket_ptr->Close();

                            //release socket
                            socket_list_mutex_.Lock();
                            socket_list_.Erase(socket_ptr);
                            socket_list_mutex_.Unlock();

                            //destroy socket
                            internal::ZTCPSocketDeleteManager::GetInstance().DeleteSocket(
                                socket_ptr,
                                [this, socket_ptr]() {socket_pool_.Release(socket_ptr); }
                            );
                        }
                    }
                );
                socket_list_mutex_.Lock();
                socket_list_.PushBack(socket_ptr);
                socket_list_mutex_.Unlock();
                Z_DEBUG_LOG_SUCCESS("Client connected!");
            }

            if (_handle_func) {
                _handle_func(ret_val, this, socket_ptr);
            }

            if (ret_val == kOK && state_.Load() == StateEnum_::kListen) {
                link_code = AsyncAccept(_handle_func);
                if (link_code != kOK) {
                    ret_val = error_code::kSocketErrorCode_LinkError;
                    Z_LOG_ERROR(
                        ret_val, link_code,
                        "ZTPCMultipleSessionServer::AsyncAccept() link error!"
                    );
                }
            }
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncBroadcast(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kListen,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: {} expect state: {}",
        static_cast<Int32>(state_.Load()), static_cast<Int32>(StateEnum_::kListen)
    );

    //get operating socket list
    socket_list_mutex_.Lock();
    TArray<ZTCPSocket*> socket_array;
    socket_array.Reserve(socket_list_.GetSize());
    for (auto& socket_iter : socket_list_) {
        socket_array.PushBack(&socket_iter);
    }
    socket_list_mutex_.Unlock();

    //broadcast
    for (auto& socket_ptr_iter : socket_array) {
        link_code = socket_ptr_iter->AsyncWrite(_buffer, _handle_func);
        if (link_code != kOK) {
            //disconnect
            if (socket_ptr_iter->state_ == ZTCPSocket::StateEnum_::kClosed) {
                Z_DEBUG_LOG_FINISH("Client disconnected!");
            }
            else {
                Z_LOG_ERROR(
                    error_code::kSocketErrorCode_LinkError, link_code,
                    "ZTCPSocket::AsyncWrite() link error!"
                );
            }
        }
    }

    return ret_val;
}

} // socket
} // zengine