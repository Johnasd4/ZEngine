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

#include "z_tcp_server.h"

#include "z_io_context.h"

#include "data/z_io_context_data.h"
#include "data/z_tcp_server_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPMultipleSessionServer::ZTCPMultipleSessionServer(ZIOContext* _io_context_ptr) noexcept
    : data_ptr_()
    , socket_pool_list_()
    , io_context_ptr_(_io_context_ptr)
    , state_(StateEnum_::kUninitialized)
{
    if (_io_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZTCPMultipleSessionServerData>(&_io_context_ptr->data_ptr_->io_context_);
    socket_pool_list_.SetModel(_io_context_ptr);

    state_ = StateEnum_::kClosed;
}

ZTCPMultipleSessionServer::~ZTCPMultipleSessionServer() noexcept {
    ReturnType link_code = kOK;
    link_code = Close();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPMultipleSessionServer::Close() link error!"
        );
        return;
    }
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, StateEnum_::kClosed
    );

    switch (_ip_type) {
    case IPTypeEnum::IP4:
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4(), error_code);
        break;
    case IPTypeEnum::IP6:
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v6(), error_code);
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

NODISCARD ReturnType ZTCPMultipleSessionServer::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->acceptor_.bind(
        *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint>(),
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

    state_ = StateEnum_::kEndpointBind;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Listen(Int32 _max_wait_connect_client_num) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kEndpointBind,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, StateEnum_::kEndpointBind
    );

    data_ptr_->acceptor_.listen(_max_wait_connect_client_num, error_code);
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

    state_ = StateEnum_::kListen;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kClosed) {
        return ret_val;
    }

    //close socket
    socket_pool_list_.Lock();
    auto socket_ptr = socket_pool_list_.Begin();
    while (socket_ptr != socket_pool_list_.End()) {
        link_code = socket_ptr->Close();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kPSocketErrorCode_LinkError, link_code,
                L"ZTCPSocket::Close() link error!"
            );
        }
        socket_ptr = socket_pool_list_.Release(socket_ptr);
    }
    socket_pool_list_.Unlock();

    //close acceptor
    try {
        data_ptr_->acceptor_.close();
        state_ = StateEnum_::kClosed;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = StateEnum_::kError;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncAccept(
    const TFunction<Void(ReturnType, ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kListen,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, StateEnum_::kListen
    );

    ZTCPSocket* socket_ptr = socket_pool_list_.Apply();
    data_ptr_->acceptor_.async_accept(
        socket_ptr->data_ptr_->socket_, 
        [this, socket_ptr, _handle_func](
            const boost::system::error_code& _error_code
        ) {
            ReturnType ret_val = kOK;

            if (_error_code) {
                //close and release socket
                ReturnType link_code = socket_ptr->Close();
                if (link_code != kOK) {
                    Z_LOG_ERROR(
                        error_code::kPSocketErrorCode_LinkError, link_code,
                        L"ZTCPSocket::Close() link error!"
                    );
                }
                socket_pool_list_.Release(socket_ptr);

                //handle error
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kPSocketErrorCode_OperationCanceled;
                    Z_DEBUG_LOG_FAILURE(L"Server accept cancelled!");
                }
                else {
                    ret_val = error_code::kPSocketErrorCode_SystemError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }
            }
            else {
                socket_ptr->OnConnectP();
                socket_ptr->SetAsyncErrorHandleFunction(
                    [this, socket_ptr](ReturnType _error_code) {
                        //disconnect
                        if (socket_ptr->State() == ZTCPSocket::StateEnum_::kError) {
                            ReturnType link_code = kOK;
                            link_code = socket_ptr->Close();
                            if (link_code != kOK) {
                                Z_LOG_ERROR(
                                    error_code::kPSocketErrorCode_LinkError, link_code,
                                    L"ZTCPSocket::Close() link error!"
                                );
                                return;
                            }

                            Z_DEBUG_LOG_FINISH(L"Client disconnected!");

                            //release socket
                            socket_pool_list_.Release(socket_ptr);
                        }
                    }
                );
                socket_pool_list_.PushBack(socket_ptr);
                Z_DEBUG_LOG_SUCCESS(L"Client connected!");
            }

            if (_handle_func) {
                _handle_func(ret_val, this, socket_ptr);
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
        state_ != StateEnum_::kListen,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, StateEnum_::kListen
    );
    
    socket_pool_list_.Lock();
    auto socket_iter = socket_pool_list_.Begin();
    while (socket_iter != socket_pool_list_.End()) {
        link_code = socket_iter->AsyncWrite(_buffer, _handle_func);
        if (link_code != kOK) {
            //disconnect
            Z_DEBUG_LOG_FINISH(L"Client disconnected!");

            //close and release socket
            link_code = socket_iter->Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::Close() link error!"
                );
            }
            socket_iter = socket_pool_list_.Release(socket_iter);
        }
        else {
            ++socket_iter;
        }
    }
    socket_pool_list_.Unlock();

    return ret_val;
}

}//socket
}//zengine