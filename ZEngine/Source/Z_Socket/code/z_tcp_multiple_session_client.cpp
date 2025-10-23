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
#include "z_core/t_atom.h"
#include "z_core/z_string.h"
#include "z_core/z_thread.h"

#include "z_io_context.h"

#include "data/z_context_data.h"
#include "data/z_tcp_client_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPMultipleSessionClient::ZTCPMultipleSessionClient(ZIOContext* _context_ptr) noexcept
    : data_ptr_()
    , socket_pool_list_()
    , io_context_ptr_(_context_ptr)
    , state_(ZTCPMultipleSessionClientState_Uninitialized)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kZSocketErrorCode_NullptrParam, 0,
            L"_io_context_ptr is nullptr!"
        );
        return;
    }

    data_ptr_ = MakeUnique<internal::ZTCPMultipleSessionClientData>(&_context_ptr->data_ptr_->io_context_);
    socket_pool_list_.SetModel(_context_ptr);

    state_ = ZTCPMultipleSessionClientState_Idle;
}

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

    ZTCPSocket* socket_ptr = socket_pool_list_.Apply();
    ZString address_str = _address_str;
    ZString port_str = _port_str;

    link_code = socket_ptr->AsyncConnect(_address_str, _port_str,
        [this, socket_ptr, address_str = std::move(address_str), port_str = std::move(port_str), _handle_func](
            Bool _connect_success
        ) {
            //failed
            if (!_connect_success) {
                socket_pool_list_.Release(socket_ptr);
            }

            //success
            socket_pool_list_.Push(socket_ptr);
            socket_ptr->SetAsyncErrorHandleFunction(
                [address_str = std::move(address_str), port_str = std::move(port_str)]() {
                    //disconnect
                    Z_LOG_FINISH(
                        L"Server disconnected! server_address: %ls server_port: %ls",
                        string::String2WString(address_str.String()).String(),
                        string::String2WString(port_str.String()).String()
                    );
                }
            );

            Z_LOG_SUCCESS(L"Server connected!");

            if (_handle_func) {
                _handle_func(this, socket_ptr);
            }
        },
        _repeat_times
    );

    if (link_code != kOK) {
        ret_val = error_code::kZSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Connect() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncBroadcast(
    const Void* _data_ptr,
    SizeType _data_size,
    const TFunction<Void(ZTCPSocket*, const Void*, SizeType)>& _handle_func
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
        link_code = socket_ptr->AsyncWrite(_data_ptr, _data_size, _handle_func);
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

}//socket
}//zengine