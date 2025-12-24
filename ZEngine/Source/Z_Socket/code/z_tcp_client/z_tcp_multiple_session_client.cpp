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

#include "z_tcp_client.h"

#include "z_io_context.h"

#include "data/z_io_context_data.h"
#include "data/z_tcp_socket_data.h"

namespace zengine {
namespace socket {

ZTCPMultipleSessionClient::ZTCPMultipleSessionClient(ZIOContext* _context_ptr) noexcept
    : SuperType_()
    , socket_pool_list_()
    , io_context_ptr_(_context_ptr)
{
    if (_context_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_NullptrParam, 0,
            "_io_context_ptr is nullptr!"
        );
        return;
    }

    socket_pool_list_.SetModel(_context_ptr);
}

ZTCPMultipleSessionClient::~ZTCPMultipleSessionClient() noexcept {
    Close();
}

Void ZTCPMultipleSessionClient::Close() noexcept {
    //close socket
    socket_pool_list_.Lock();
    auto socket_iter = socket_pool_list_.Begin();
    while (socket_iter != socket_pool_list_.End()) {
        socket_iter->Close();
        socket_iter = socket_pool_list_.ReleaseIterator(socket_iter);
    }
    socket_pool_list_.Unlock();

    return;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncConnect(
    const ZTCPEndpoint& _tcp_endpoint,
    const TFunction<Void(ReturnType, ZTCPMultipleSessionClient*, ZTCPSocket*)>& _handle_func,
    Int32 _repeat_times,
    ZTCPSocket** _tcp_socket_ptr_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    ZTCPSocket* socket_ptr = socket_pool_list_.Apply();
    if (_tcp_socket_ptr_ptr != nullptr) {
        *_tcp_socket_ptr_ptr = socket_ptr;
    }

    link_code = socket_ptr->Open(_tcp_endpoint.IPType());
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_LinkError, link_code,
            "ZTCPSocket::Close() link error!"
        );
    }

    link_code = socket_ptr->AsyncConnect(
        _tcp_endpoint,
        [this, _handle_func](
            ReturnType _error_code,
            ZTCPSocket* _socket_ptr
        ) {
            //failed
            if (_error_code != kOK) {
                socket_pool_list_.Release(_socket_ptr);
            }
            else {
                //success
                socket_pool_list_.PushBack(_socket_ptr);
                _socket_ptr->SetAsyncErrorHandleFunction(
                    [this, _socket_ptr](ReturnType _error_code) {
                        //close and release socket
                        if (_socket_ptr->State() == ZTCPSocket::StateEnum_::kError) {
                            _socket_ptr->Close();
                            socket_pool_list_.Erase(_socket_ptr);
                            socket_pool_list_.Release(_socket_ptr);

                            //disconnect
                            Z_DEBUG_LOG_FINISH(
                                "Server disconnected! server_address: %ls server_port: %ls",
                                string::StringToWString(_socket_ptr->RemoteEndpoint().IPString().GetDataPtr()).GetDataPtr(),
                                _socket_ptr->RemoteEndpoint().Port()
                            );
                        }
                    }
                );

                Z_DEBUG_LOG_SUCCESS("Server connected!");
            }

            if (_handle_func) {
                _handle_func(_error_code, this, _socket_ptr);
            }
        },
        _repeat_times
    );

    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZTCPSocket::Connect() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTCPMultipleSessionClient::AsyncBroadcast(
    ZConstBuffer _buffer,
    const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    
    socket_pool_list_.Lock();
    auto socket_iter = socket_pool_list_.Begin();
    while (socket_iter != socket_pool_list_.End()) {
        link_code = socket_iter->AsyncWrite(_buffer, _handle_func);
        if (link_code != kOK) {
            //disconnect
            if (socket_iter->State() == ZTCPSocket::StateEnum_::kClosed) {
                Z_DEBUG_LOG_FINISH("Server disconnected!");
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