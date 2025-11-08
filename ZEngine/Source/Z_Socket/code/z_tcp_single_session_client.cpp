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

#include "z_io_context.h"

#include "data/z_io_context_data.h"
#include "data/z_tcp_client_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPSingleSessionClient::ZTCPSingleSessionClient(ZIOContext* _io_context_ptr) noexcept
    : data_ptr_()
    , socket_()
    , io_context_ptr_(_io_context_ptr)
    , state_(ZTCPSingleSessionClientState_Uninitialized)
{
    if (_io_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    ReturnType link_code = kOK;
    data_ptr_ = MakeUnique<internal::ZTCPSingleSessionClientData>(&_io_context_ptr->data_ptr_->io_context_);
    link_code = socket_.Initialize(_io_context_ptr);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Initialize() link error!"
        );
        return;
    }

    state_ = ZTCPSingleSessionClientState_Idle;
}

ZTCPSingleSessionClient::~ZTCPSingleSessionClient() noexcept {
    ReturnType link_code = kOK;
    link_code = Reset();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPClient::Reset() link error!"
        );
        return;
    }
}

NODISCARD ReturnType ZTCPSingleSessionClient::BindEndpoint(const Char* _address_str, Int32 _port) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
    );

    link_code = socket_.BindEndpoint(_address_str, _port);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::BindEndpoint() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::SetOSWriteBufferSize(Int32 _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
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
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
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
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
    );

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
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
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    state_ = ZTCPSingleSessionClientState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Connect(
    const Char* _address_str,
    const Char* _port_str,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
    );

    //connect
    link_code = socket_.Connect(_address_str, _port_str, _repeat_times);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_ConnectFailed) {
            Z_LOG_FAILURE(L"Server connect failed!");
        }
        else {
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Connect() link error!"
            );
            return ret_val;
        }
    }

    state_ = ZTCPSingleSessionClientState_Connect;

    ZString address_str = _address_str;
    ZString port_str = _port_str;
    socket_.SetAsyncErrorHandleFunction(
        [address_str = std::move(address_str), port_str = std::move(port_str)]() {
            //disconnect
            Z_LOG_FINISH(
                L"Server disconnected! server_address: %ls server_port: %ls",
                string::String2WString(address_str.String()).String(),
                string::String2WString(port_str.String()).String()
            );
        }
    );

    return ret_val;
}


NODISCARD ReturnType ZTCPSingleSessionClient::Read(
    Void* _buffer_ptr, 
    SizeType _buffer_size,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.Read(_buffer_ptr, _buffer_size, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_LOG_FINISH(L"Server disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
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
    Void* _buffer_ptr,
    SizeType _buffer_size,
    const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.AsyncRead(_buffer_ptr, _buffer_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncRead() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Write(
    const Void* _data_ptr, 
    SizeType _data_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.Write(_data_ptr, _data_size);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_LOG_FINISH(L"Server disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
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
    const Void* _data_ptr,
    SizeType _data_size,
    const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connect,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connect
    );

    link_code = socket_.AsyncWrite(_data_ptr, _data_size, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncWrite() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

}//socket
}//zengine