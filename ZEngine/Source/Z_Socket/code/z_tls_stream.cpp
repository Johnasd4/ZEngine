///*
//    Copyright (c) YuLin Zhu
//
//    This code file is licensed under the Creative Commons
//    Attribution-NonCommercial 4.0 International License.
//
//    You may obtain a copy of the License at
//    https://creativecommons.org/licenses/by-nc/4.0/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    Author: YuLin Zhu
//    Contact: 1152325286@qq.com
//*/
//#define SOCKET_DLLFILE
//
//#include "z_tls_stream.h"
//
//#include <boost/asio.hpp>
//
//#include "z_tls_context.h"
//#include "z_socket.h"
//
//#include "data/z_tcp_socket_data.h"
//#include "data/z_tls_context_data.h"
//#include "data/z_tls_stream_data.h"
//
//namespace zengine {
//namespace socket {
//
//ZTLSStream::ZTLSStream(ZTCPSocket* _socket_ptr, ZTLSContext* _tls_context_ptr) noexcept
//    : SuperType_()
//    , data_ptr_()
//    , tls_type_(_tls_context_ptr->tls_type_)
//    , state_(ZTLSStreamTCPClientState_Uninitialized)
//{
//    if (_socket_ptr->State() != ZTCPSocket::ZTCPSocketState_Connect) {
//        Z_LOG_ERROR(
//            error_code::kPSocketErrorCode_StateError, 0,
//            L"Socket state error! state: %d expect state: %d",
//            _socket_ptr->State(), ZTCPSocket::ZTCPSocketState_Connect
//        );
//    }
//
//    data_ptr_ = MakeUnique<internal::ZTLSStreamData>(
//        _socket_ptr->data_ptr_->socket_, _tls_context_ptr->data_ptr_->tls_context_
//    );
//    state_ = ZTLSStreamTCPClientState_Idle;
//}
//
//ZTLSStream::ZTLSStream(ZTLSStream&& _stream) noexcept
//    : SuperType_()
//    , data_ptr_()
//    , tls_type_(_tls_context_ptr->tls_type_)
//    , state_(ZTLSStreamTCPClientState_Uninitialized)
//{
//    if (_socket_ptr->State() != ZTCPSocket::ZTCPSocketState_Connect) {
//        Z_LOG_ERROR(
//            error_code::kPSocketErrorCode_StateError, 0,
//            L"Socket state error! state: %d expect state: %d",
//            _socket_ptr->State(), ZTCPSocket::ZTCPSocketState_Connect
//        );
//    }
//
//    data_ptr_ = MakeUnique<internal::ZTLSStreamData>(
//        _socket_ptr->data_ptr_->socket_, _tls_context_ptr->data_ptr_->tls_context_
//    );
//    state_ = ZTLSStreamTCPClientState_Idle;
//}
//
//ZTLSStream::~ZTLSStream() noexcept {}
//
//NODISCARD ReturnType ZTLSStream::Initialize(ZTCPSocket* _socket_ptr, ZTLSContext* _tls_context_ptr) noexcept {
//    ReturnType ret_val = kOK;
//
//    Z_CHECK(
//        state_ != ZTLSStreamTCPClientState_Uninitialized,
//        error_code::kPSocketErrorCode_StateError,
//        L"TLS stream state error! state: %d expect state: %d",
//        state_, ZTLSStreamTCPClientState_Uninitialized
//    );
//    Z_CHECK(
//        _socket_ptr->State() != ZTCPSocket::ZTCPSocketState_Connect,
//        error_code::kPSocketErrorCode_StateError,
//        L"Socket state error! state: %d expect state: %d",
//        _socket_ptr->State(), ZTCPSocket::ZTCPSocketState_Connect
//    );
//
//    data_ptr_ = MakeUnique<internal::ZTLSStreamData>(
//        _socket_ptr->data_ptr_->socket_, _tls_context_ptr->data_ptr_->tls_context_
//    );
//    tls_type_ = _tls_context_ptr->tls_type_;
//    state_ = ZTLSStreamTCPClientState_Idle;
//
//    return ret_val;
//}
//
//}//socket
//}//zengine