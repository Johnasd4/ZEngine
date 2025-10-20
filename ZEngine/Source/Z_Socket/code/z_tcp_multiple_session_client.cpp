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

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/t_atom.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

#include "data/z_tcp_client_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPMultipleSessionClient::ZTCPMultipleSessionClient() noexcept 
    : data_ptr_(MakeUnique<internal::ZTCPMultipleSessionClientData>())
    , socket_pool_list_(this)
    , state_(ZTCPMultipleSessionClientState_Idle)
{}

ZTCPMultipleSessionClient::~ZTCPMultipleSessionClient() noexcept {
    ReturnType link_code = kOK;
    link_code = Reset();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPMultipleSessionClient::Reset() link error!"
        );
        return;
    }
    if (data_ptr_->aysnc_thread_.Joinable()) {
        data_ptr_->aysnc_thread_.Join();
    }
}

NODISCARD ReturnType ZTCPMultipleSessionClient::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server not open!"
    );

    //close socket
    socket_pool_list_.Lock();
    auto socket_ptr = socket_pool_list_.Begin();
    while (socket_ptr != socket_pool_list_.End()) {
        link_code = socket_ptr->Close();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZSocketErrorCode_LinkError, link_code,
                L"ZTCPSocket::Close() link error!"
            );
            //reset and release socket
            link_code = socket_ptr->Reset();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::Reset() link error!"
                );
            }
        }
        socket_ptr = socket_pool_list_.Release(socket_ptr);
    }
    socket_pool_list_.Unlock();

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::Reset() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPMultipleSessionClientState_Idle) {
        return ret_val;
    }

    //close socket
    socket_pool_list_.Lock();
    auto socket_ptr = socket_pool_list_.Begin();
    while (socket_ptr != socket_pool_list_.End()) {
        link_code = socket_ptr->Close();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZSocketErrorCode_LinkError, link_code,
                L"ZTCPSocket::Close() link error!"
            );
            //reset and release socket
            link_code = socket_ptr->Reset();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::Reset() link error!"
                );
            }
        }
        socket_ptr = socket_pool_list_.Release(socket_ptr);
    }
    socket_pool_list_.Unlock();

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncConnect(
    const Char* _domain_str,
    const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionClientState_Idle
    );

    //get address and port
    auto result_str_list = ZString(_domain_str).Split(':');
    if (result_str_list.Size() != 2) {
        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Domain not valid! domain: %ls",
            string::String2WString(_domain_str).String()
        );
    }
    ZString address_string = std::move(result_str_list.Front());
    result_str_list.PopFront();
    ZString port_string = std::move(result_str_list.Front());

    //resolve endpoints
    boost::asio::ip::tcp::resolver::results_type endpoints;
    endpoints = std::move(data_ptr_->resolver_.resolve(address_string.String(), port_string.String(), error_code));
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls address: %ls port: ls",
            string::String2WString(error_code.message().c_str()).String(),
            string::String2WString(address_string.String()).String(),
            string::String2WString(port_string.String()).String()
        );
        return ret_val;
    }

    //connect
    link_code = AsyncConnect(address_string.String(), port_string.String(), _handle_func, _repeat_times);
    if (link_code != kOK) {
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPMultipleSessionClient::AsyncConnect() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncConnect(
    const Char* _address_str,
    const Char* _port_str,
    const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionClientState_Idle
    );

    //resolve endpoints
    boost::asio::ip::tcp::resolver::results_type endpoints;
    endpoints = std::move(data_ptr_->resolver_.resolve(_address_str, _port_str, error_code));
    if (error_code) {
        ret_val = error_code::kZSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls address: %ls port: ls",
            string::String2WString(error_code.message().c_str()).String(),
            string::String2WString(_address_str).String(),
            string::String2WString(_port_str).String()
        );
        return ret_val;
    }

    //connect
    Z_LOG_START(
        L"Try to connect server... server_address: %ls server_port: %ls",
        string::String2WString(_address_str).String(),
        string::String2WString(_port_str).String()
    );
    link_code = AsyncConnectExecuteP(_address_str, _port_str, &endpoints, _handle_func, _repeat_times, 0);
    if (link_code != kOK) {
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPMultipleSessionClient::AsyncConnectExecuteP() link error!"
        );
        return ret_val;
    }

}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncConnectExecuteP(
    ZString&& _address_str,
    ZString&& _port_str,
    Void* _endpoints_ptr,
    const TFunction<Void(ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times,
    Int32 _reconnect_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    boost::asio::ip::tcp::resolver::results_type* endpoints_ptr =
        reinterpret_cast<boost::asio::ip::tcp::resolver::results_type*>(_endpoints_ptr);

    ZTCPSocket* socket_ptr = socket_pool_list_.Apply();

    //start connect
    boost::asio::async_connect(
        socket_ptr->data_ptr_->socket_, *endpoints_ptr,
        [
            this, socket_ptr, address_str = std::move(_address_str), port_str = std::move(_port_str),
            endpoints = *endpoints_ptr, _handle_func, _repeat_times, _reconnect_times
        ](
            const boost::system::error_code& _error_code,
            const boost::asio::ip::tcp::endpoint& _endpoint
            ) mutable {
                ReturnType link_code = kOK;
                //handle error
                if (_error_code) {
                    //reset and release socket
                    link_code = socket_ptr->Reset();
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kZSocketErrorCode_LinkError, link_code,
                            L"ZTCPSocket::Reset() link error!"
                        );
                    }
                    socket_pool_list_.Release(socket_ptr);

                    //handle error
                    if (_error_code == boost::asio::error::connection_refused) {
                        //retry
                        if (_reconnect_times < _repeat_times) {
                            _reconnect_times += 1;
                            Z_LOG_PROCESS(
                                L"Retry to connect server... repeat_times: %d server_address: %ls server_port: %ls",
                                _reconnect_times,
                                string::String2WString(address_str.String()).String(),
                                string::String2WString(port_str.String()).String()
                            );
                            link_code = AsyncConnectExecuteP(
                                std::move(address_str),
                                std::move(port_str),
                                &endpoints,
                                _handle_func, 
                                _repeat_times,
                                _reconnect_times
                            );
                            if (link_code != kOK) {
                                Z_LOG_ERROR(
                                    error_code::kZSocketErrorCode_LinkError, link_code,
                                    L"ZTCPMultipleSessionClient::AsyncConnectExecuteP() link error!"
                                );
                            }
                            return;
                        }

                        Z_LOG_FAILURE(
                            L"Connect server failed! server_address: %ls server_port: %ls",
                            string::String2WString(address_str.String()).String(),
                            string::String2WString(port_str.String()).String()
                        );
                    }
                    else if (_error_code == boost::asio::error::operation_aborted) {
                        Z_LOG_FAILURE(
                            L"Connect cancelled! server_address: %ls server_port: %ls",
                            string::String2WString(address_str.String()).String(),
                            string::String2WString(port_str.String()).String()
                        );
                    }
                    else {
                        Z_LOG_ERROR(
                            error_code::kZSocketErrorCode_SystemError, _error_code.value(),
                            L"System error! error info: %ls",
                            string::String2WString(_error_code.message().c_str()).String()
                        );
                    }
                    return;
                }

                socket_pool_list_.Push(socket_ptr);
                socket_ptr->OnConnectP();
                Z_LOG_SUCCESS(L"Server connected!");

                if (_handle_func) {
                    _handle_func(this, socket_ptr);
                }
        }
    );

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncBroadcast(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionClientState_Idle
    );
    
    socket_pool_list_.Lock();
    auto socket_ptr = socket_pool_list_.Begin();
    while (socket_ptr != socket_pool_list_.End()) {
        link_code = socket_ptr->AsyncWrite(_data_buffer, _buffer_size, _handle_func);
        if (link_code != kOK) {
            //disconnect
            if (socket_ptr->State() == ZTCPSocket::ZTCPSocketState_Idle) {
                Z_LOG_FINISH(L"Client disconnected!");
            }
            else {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::AsyncWrite() link error!"
                );
            }
            //reset and release socket
            link_code = socket_ptr->Reset();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSocketErrorCode_LinkError, link_code,
                    L"ZTCPSocket::Reset() link error!"
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

NODISCARD ReturnType ZTCPMultipleSessionClient::Run() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionClientState_Idle
    );

    data_ptr_->io_context_.run();
    data_ptr_->io_context_.restart();

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncRun() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPMultipleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Server state error! state: %d expect state: %d",
        state_, ZTCPMultipleSessionClientState_Idle
    );

    //start dealing with async operation.
    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            data_ptr_->io_context_.run();
            data_ptr_->io_context_.restart();
        }
    );

    return ret_val;
}

}//socket
}//zengine