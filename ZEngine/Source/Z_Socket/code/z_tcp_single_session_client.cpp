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

#include "z_tcp_client.h"

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/t_list.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

#include "data/z_tcp_client_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPSingleSessionClient::ZTCPSingleSessionClient() noexcept 
    : data_ptr_(MakeUnique<internal::ZTCPSingleSessionClientData>())
    , socket_(this)
    , state_(ZTCPSingleSessionClientState_Idle)
{
    socket_.SetAsyncErrorHandleFunctionP(
        [this]() {
            //disconnect
            if (socket_.State() == ZTCPSocket::ZTCPSocketState_Idle) {
                //connect->listen
                if (state_ == ZTCPSingleSessionClientState_Connect) {
                    state_ = ZTCPSingleSessionClientState_Idle;
                    Z_LOG_FINISH(L"Server disconnected!");
                }
            }
        }
    );
}

ZTCPSingleSessionClient::~ZTCPSingleSessionClient() noexcept {
    ReturnType link_code = kOK;
    link_code = Reset();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPClient::Reset() link error!"
        );
        return;
    }
    if (data_ptr_->aysnc_thread_.Joinable()) {
        data_ptr_->aysnc_thread_.Join();
    }
}

NODISCARD ReturnType ZTCPSingleSessionClient::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::SetOSWriteBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::SetOSReadBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::SetOSReadBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
    );

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    state_ = ZTCPSingleSessionClientState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Reset() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPSingleSessionClientState_Idle) {
        return ret_val;
    }

    link_code = socket_.Reset();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    state_ = ZTCPSingleSessionClientState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Connect(
    const Char* _domain_str,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
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
    Z_LOG_START(
        L"Try to connect server... server_address: %ls server_port: %ls",
        string::String2WString(address_string.String()).String(),
        string::String2WString(port_string.String()).String()
    );

    Int32 reconnect_times = 0;
    do {
        boost::asio::connect(*socket_.data_ptr_->socket_ptr_, endpoints, error_code);
        if (error_code) {
            if (error_code == boost::asio::error::connection_refused) {
                reconnect_times += 1;
                Z_LOG_PROCESS(
                    L"Retry to connect server... repeat_times: %d server_address: %ls server_port: %ls",
                    reconnect_times,
                    string::String2WString(address_string.String()).String(),
                    string::String2WString(port_string.String()).String()
                );
            }
            else {
                ret_val = error_code::kZSocketErrorCode_SystemError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                state_ = ZTCPSingleSessionClientState_Error;
                return ret_val;
            }
        }
        else {
            socket_.OnConnectP();
            state_ = ZTCPSingleSessionClientState_Connect;
            Z_LOG_SUCCESS(L"Server connected!");
            break;
        }
    } while (_repeat_times > reconnect_times);

    if (state_ != ZTCPSingleSessionClientState_Connect) {
        ret_val = error_code::kZSocketErrorCode_ConnectFailed;
        Z_LOG_FAILURE(
            L"Connect server failed! server_address: %ls server_port: %ls",
            string::String2WString(address_string.String()).String(),
            string::String2WString(port_string.String()).String()
        );
        return ret_val;    
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Connect(
    const Char* _address_str,
    const Char* _port_str,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
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

    Int32 reconnect_times = 0;
    do {
        boost::asio::connect(*socket_.data_ptr_->socket_ptr_, endpoints, error_code);
        if (error_code) {
            if (error_code == boost::asio::error::connection_refused) {
                reconnect_times += 1;
                Z_LOG_PROCESS(
                    L"Retry to connect server... repeat_times: %d server_address: %ls server_port: %ls",
                    reconnect_times,
                    string::String2WString(_address_str).String(),
                    string::String2WString(_port_str).String()
                );
            }
            else {
                ret_val = error_code::kZSocketErrorCode_SystemError;
                Z_LOG_ERROR(
                    ret_val, error_code.value(),
                    L"System error! error info: %ls",
                    string::String2WString(error_code.message().c_str()).String()
                );
                state_ = ZTCPSingleSessionClientState_Error;
                return ret_val;
            }
        }
        else {
            socket_.OnConnectP();
            state_ = ZTCPSingleSessionClientState_Connect;
            Z_LOG_SUCCESS(L"Server connected!");
            break;
        }
    } while (_repeat_times > reconnect_times);

    if (state_ != ZTCPSingleSessionClientState_Connect) {
        ret_val = error_code::kZSocketErrorCode_ConnectFailed;
        Z_LOG_FAILURE(
            L"Connect server failed! server_address: %ls server_port: %ls",
            string::String2WString(_address_str).String(),
            string::String2WString(_port_str).String()
        );
        return ret_val;    
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Read(
    Void* _data_buffer, 
    Int32 _buffer_size, 
    SizeType* _message_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.Read(_data_buffer, _buffer_size, _message_size_ptr);
    if (link_code != kOK) {
        if (ret_val == error_code::kZSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_LOG_FINISH(L"Server disconnected!");
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kZSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Read() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::AsyncRead(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.AsyncRead(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Write(
    const Void* _data_buffer, 
    SizeType _date_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.Write(_data_buffer, _date_size);
    if (link_code != kOK) {
        if (ret_val == error_code::kZSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_LOG_FINISH(L"Server disconnected!");
            ret_val = error_code::kZSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kZSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Write() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::AsyncWrite(
    Void* _data_buffer,
    Int32 _buffer_size,
    const TFunction<Void(ZTCPSocket*, Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.AsyncWrite(_data_buffer, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Run() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    //start dealing with async operation.
    data_ptr_->io_context_.run();

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::AsyncRun() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kZSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    //start dealing with async operation.
    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            data_ptr_->io_context_.run();
        }
    );

    return ret_val;
}

}//socket
}//zengine