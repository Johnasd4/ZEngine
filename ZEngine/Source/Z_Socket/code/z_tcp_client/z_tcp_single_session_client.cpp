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

#include "z_core/f_string.h"
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
    link_code = Close();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_LinkError, link_code,
            L"ZTCPSingleSessionClient::Close() link error!"
        );
        return;
    }
}

NODISCARD ReturnType ZTCPSingleSessionClient::BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Idle
    );

    link_code = socket_.BindEndpoint(_tcp_endpoint);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
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
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.SetOSWriteBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
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
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.SetOSReadBufferSize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::SetOSReadBufferSize() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Close() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (state_ == ZTCPSingleSessionClientState_Uninitialized || state_ == ZTCPSingleSessionClientState_Idle) {
        return ret_val;
    }

    link_code = socket_.Close();
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    state_ = ZTCPSingleSessionClientState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Connect(
    const ZTCPEndpoint& _tcp_endpoint,
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
    link_code = socket_.Connect(_tcp_endpoint, _repeat_times);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_ConnectFailed) {
            Z_DEBUG_LOG_FAILURE(L"Server connect failed!");
            return link_code;
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

    state_ = ZTCPSingleSessionClientState_Connected;

    socket_.SetAsyncErrorHandleFunction(
        [this]() {
            //disconnect
            Z_DEBUG_LOG_FINISH(
                L"Server disconnected! server_ip: %ls server_port: %d",
                string::String2WString(socket_.RemoteEndpoint().IPString().String()).String(),
                socket_.RemoteEndpoint().Port()
            );
        }
    );

    return ret_val;
}

NODISCARD Void ZTCPSingleSessionClient::StopConnect() noexcept {
    socket_.StopConnect();
}

NODISCARD ReturnType ZTCPSingleSessionClient::Read(
    ZBuffer _buffer,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.Read(_buffer, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_DEBUG_LOG_FINISH(L"Server disconnected!");
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
    ZBuffer _buffer,
    const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.AsyncRead(_buffer, _handle_func);
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

NODISCARD ReturnType ZTCPSingleSessionClient::ReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_char, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_DEBUG_LOG_FINISH(L"Server disconnected!");
            ret_val = error_code::kPSocketErrorCode_Disconnected;
            return ret_val;
        }
        else if (link_code == error_code::kPSocketErrorCode_ReadUntilSymbolNotFound) {
            return link_code;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::ReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.ReadUntil(_buffer_ptr, _match_str, _data_size_ptr);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_DEBUG_LOG_FINISH(L"Server disconnected!");
            return link_code;
        }
        else if (link_code == error_code::kPSocketErrorCode_ReadUntilSymbolNotFound) {
            return link_code;
        }
        else {
            state_ = ZTCPSingleSessionClientState_Error;
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    Char _match_char,
    const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_char, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::AsyncReadUntil(
    ZBufferStream* _buffer_ptr,
    const Char* _match_str,
    const TFunction<Void(ZTCPSocket*, ZBufferStream*)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.AsyncReadUntil(_buffer_ptr, _match_str, _handle_func);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::AsyncReadUntil() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::ReadUntilClose(
    ZBufferStream* _buffer_ptr,
    SizeType* _data_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.ReadUntilClose(_buffer_ptr, _data_size_ptr);
    if (link_code != kOK) {
        state_ = ZTCPSingleSessionClientState_Error;
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::ReadUntilClose() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPSingleSessionClient::Write(
    ZConstBuffer _buffer
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.Write(_buffer);
    if (link_code != kOK) {
        if (link_code == error_code::kPSocketErrorCode_Disconnected) {
            state_ = ZTCPSingleSessionClientState_Idle;
            Z_DEBUG_LOG_FINISH(L"Server disconnected!");
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
    ZConstBuffer _buffer,
    const TFunction<Void(ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZTCPSingleSessionClientState_Connected,
        error_code::kPSocketErrorCode_StateError,
        L"Client state error! state: %d expect state: %d",
        state_, ZTCPSingleSessionClientState_Connected
    );

    link_code = socket_.AsyncWrite(_buffer, _handle_func);
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