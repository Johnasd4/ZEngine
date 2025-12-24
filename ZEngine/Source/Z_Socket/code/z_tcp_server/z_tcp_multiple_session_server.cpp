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
            error_code::kSocketErrorCode_NullptrParam, 0,
            "_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZTCPMultipleSessionServerData>(&_io_context_ptr->data_ptr_->io_context_);
    socket_pool_list_.SetModel(_io_context_ptr);

    state_ = StateEnum_::kClosed;
}

ZTCPMultipleSessionServer::~ZTCPMultipleSessionServer() noexcept {
    Close();
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Open(IPTypeEnum _ip_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kClosed,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
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
        ret_val = error_code::kSocketErrorCode_ParamOutOfRange;
        Z_LOG_ERROR(
            ret_val, 0,
            "Enum out of range! _ip_type: %d",
            _ip_type
        );
        return ret_val;
    }

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

NODISCARD ReturnType ZTCPMultipleSessionServer::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kOpened,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kOpened
    );

    data_ptr_->acceptor_.bind(
        *_tcp_endpoint.endpoint_data_.GetDataPtr<const boost::asio::ip::tcp::endpoint>(),
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

    state_ = StateEnum_::kEndpointBind;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Listen(Int32 _max_wait_connect_client_num) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kEndpointBind,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kEndpointBind
    );

    data_ptr_->acceptor_.listen(_max_wait_connect_client_num, error_code);
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

    state_ = StateEnum_::kListen;

    return ret_val;
}

Void ZTCPMultipleSessionServer::Close() noexcept {
    boost::system::error_code error_code;

    if (state_ == StateEnum_::kUninitialized || state_ == StateEnum_::kClosed) {
        return;
    }

    //close socket
    socket_pool_list_.Lock();
    auto socket_iter = socket_pool_list_.Begin();
    while (socket_iter != socket_pool_list_.End()) {
        socket_iter->Close();
        socket_iter = socket_pool_list_.ReleaseIterator(socket_iter);
    }
    socket_pool_list_.Unlock();

    //close acceptor
    data_ptr_->acceptor_.close(error_code);
    state_ = StateEnum_::kClosed;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncAccept(
    const TFunction<Void(ReturnType, ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != StateEnum_::kListen,
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
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
                socket_ptr->Close();
                socket_pool_list_.Release(socket_ptr);

                //handle error
                if (_error_code == boost::asio::error::operation_aborted) {
                    ret_val = error_code::kSocketErrorCode_OperationCanceled;
                    Z_DEBUG_LOG_FAILURE("Server accept cancelled!");
                }
                else {
                    ret_val = error_code::kSocketErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: %ls",
                        string::StringToWString(_error_code.message().c_str()).GetDataPtr()
                    );
                }
            }
            else {
                socket_ptr->OnConnectP();
                socket_ptr->SetAsyncErrorHandleFunction(
                    [this, socket_ptr](ReturnType _error_code) {
                        //disconnect
                        if (socket_ptr->State() == ZTCPSocket::StateEnum_::kError) {
                            socket_ptr->Close();
                            Z_DEBUG_LOG_FINISH("Client disconnected!");

                            //release socket
                            socket_pool_list_.Erase(socket_ptr);
                            socket_pool_list_.Release(socket_ptr);
                        }
                    }
                );
                socket_pool_list_.PushBack(socket_ptr);
                Z_DEBUG_LOG_SUCCESS("Client connected!");
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
        error_code::kSocketErrorCode_StateError,
        "Server state error! state: %d expect state: %d",
        state_, StateEnum_::kListen
    );
    
    socket_pool_list_.Lock();
    auto socket_iter = socket_pool_list_.Begin();
    while (socket_iter != socket_pool_list_.End()) {
        link_code = socket_iter->AsyncWrite(_buffer, _handle_func);
        if (link_code != kOK) {
            //disconnect
            if (socket_iter->State() == ZTCPSocket::StateEnum_::kClosed) {
                Z_DEBUG_LOG_FINISH("Client disconnected!");
            }
            //close and release socket
            socket_iter->Close();
            socket_iter = socket_pool_list_.ReleaseIterator(socket_iter);
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