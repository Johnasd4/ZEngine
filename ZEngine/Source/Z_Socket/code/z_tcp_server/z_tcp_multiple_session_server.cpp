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

#include "z_tcp_server.h"

#include "z_core/f_string.h"
#include "z_core/m_log.h"
#include "z_core/t_atom.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

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
    , state_(ZTCPMultipleSessionServerState_Uninitialized)
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

    state_ = ZTCPMultipleSessionServerState_Idle;
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

NODISCARD ReturnType ZTCPMultipleSessionServer::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionServerState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionServerState_Idle
    );

    data_ptr_->server_endpoint_ = *_tcp_endpoint.endpoint_data_.DataPtr<const boost::asio::ip::tcp::endpoint*>();
    data_ptr_->if_endpoint_bind_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Listen() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        !data_ptr_->if_endpoint_bind_,
        error_code::kPSocketErrorCode_EndpointNotBind,
        L"Endpoint not bind! Can not open!"
    );
    Z_CHECK(
        state_ != ZTCPMultipleSessionServerState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionServerState_Idle
    );

    try {
        data_ptr_->acceptor_.open(boost::asio::ip::tcp::v4());
        data_ptr_->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        data_ptr_->acceptor_.bind(data_ptr_->server_endpoint_);
        data_ptr_->acceptor_.listen();
        state_ = ZTCPMultipleSessionServerState_Listen;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        state_ = ZTCPMultipleSessionServerState_Error;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPMultipleSessionServerState_Uninitialized || state_ == ZTCPMultipleSessionServerState_Idle) {
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
        state_ = ZTCPMultipleSessionServerState_Idle;
    }
    catch (const boost::system::system_error& error) {
        ret_val = error_code::kPSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error.code().value(),
            L"System error! error info: %ls",
            string::String2WString(error.code().message().c_str()).String()
        );
        state_ = ZTCPMultipleSessionServerState_Error;
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncAccept(
    const TFunction<Void(ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionServerState_Listen,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionServerState_Listen
    );

    ZTCPSocket* socket_ptr = socket_pool_list_.Apply();
    data_ptr_->acceptor_.async_accept(
        socket_ptr->data_ptr_->socket_, 
        [this, socket_ptr, _handle_func](
            const boost::system::error_code& _error_code
        ) {
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
                return;

                //handle error
                if (_error_code == boost::asio::error::operation_aborted) {
                    Z_DEBUG_LOG_FAILURE(L"Server accept cancelled!");
                }
                else {
                    Z_LOG_ERROR(
                        error_code::kPSocketErrorCode_SystemError, _error_code.value(),
                        L"System error! error info: %ls",
                        string::String2WString(_error_code.message().c_str()).String()
                    );
                }
            }

            socket_pool_list_.PushBack(socket_ptr);
            socket_ptr->OnConnectP();
            socket_ptr->SetAsyncErrorHandleFunction(
                []() {
                    //disconnect
                    Z_DEBUG_LOG_FINISH(L"Client disconnected!");
                }
            );
            Z_DEBUG_LOG_SUCCESS(L"Client connected!");

            if (_handle_func) {
                _handle_func(this, socket_ptr);
            }
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionServer::AsyncBroadcast(
    ZConstBuffer _buffer,
    const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionServerState_Listen,
        error_code::kPSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionServerState_Listen
    );
    
    socket_pool_list_.Lock();
    auto socket_ptr = socket_pool_list_.Begin();
    while (socket_ptr != socket_pool_list_.End()) {
        link_code = socket_ptr->AsyncWrite(_buffer, _handle_func);
        if (link_code != kOK) {
            //disconnect
            if (socket_ptr->State() == ZTCPSocket::ZTCPSocketState_Idle) {
                Z_DEBUG_LOG_FINISH(L"Client disconnected!");
            }
            else {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::AsyncWrite() link error!"
                );
            }
            //close and release socket
            link_code = socket_ptr->Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::Close() link error!"
                );
            }
            socket_ptr = socket_pool_list_.Release(socket_ptr);
        }
        else {
            ++socket_ptr;
        }
    }
    socket_pool_list_.Unlock();

    return ret_val;
}

}//socket
}//zengine