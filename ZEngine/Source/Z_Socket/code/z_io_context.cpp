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

#include "z_io_context.h"

#include "z_core/f_string.h"
#include "z_core/m_log.h"
#include "z_core/z_string.h"

#include "data/z_io_context_data.h"
#include "z_tcp_endpoint.h"
#include "z_udp_endpoint.h"

namespace zengine {
namespace socket {

ZIOContext::ZIOContext() noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZIOContextData>())
    , state_(ZIOContextState_Idle)
{}

ZIOContext::~ZIOContext() noexcept {}

NODISCARD ReturnType ZIOContext::ResolveTCPAddress(
    ZStringView _address_str,
    ZStringView _port_str,
    TVector<ZTCPEndpoint>* _endpoint_vector_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    //resolve endpoints
    boost::asio::ip::tcp::resolver::results_type endpoints;
    boost::asio::ip::tcp::resolver resolver(data_ptr_->io_context_);
    endpoints = std::move(resolver.resolve(
        boost::asio::string_view(_address_str.DataPtr(), _address_str.Size()),
        boost::asio::string_view(_port_str.DataPtr(), _port_str.Size()),
        error_code
    ));

    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        ZString address_str(_address_str);
        ZString port_str(_port_str);
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls address: %ls port: %ls",
            string::String2WString(error_code.message().c_str()).String(),
            string::String2WString(address_str.String()).String(),
            string::String2WString(port_str.String()).String()
        );
        return ret_val;
    }

    _endpoint_vector_ptr->Resize(endpoints.size());
    SizeType vector_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin(); 
        endpoint_iterator != endpoints.end(); 
        ++endpoint_iterator, ++vector_index
    ) {
        *(*_endpoint_vector_ptr)[vector_index].endpoint_data_.DataPtr<boost::asio::ip::tcp::endpoint*>() = 
            *endpoint_iterator;
    }
    return ret_val;
}

NODISCARD ReturnType ZIOContext::ResolveUDPAddress(
    ZStringView _address_str,
    ZStringView _port_str,
    TVector<ZUDPEndpoint>* _endpoint_vector_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    //resolve endpoints
    boost::asio::ip::udp::resolver resolver(data_ptr_->io_context_);
    boost::asio::ip::udp::resolver::results_type endpoints = std::move(resolver.resolve(
        boost::asio::string_view(_address_str.DataPtr(), _address_str.Size()),
        boost::asio::string_view(_port_str.DataPtr(), _port_str.Size()),
        error_code
    ));

    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        ZString address_str(_address_str);
        ZString port_str(_port_str);
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            L"System error! error info: %ls address(%d): %ls port(%d): %ls",
            string::String2WString(error_code.message().c_str()).String(),
            _address_str.Size(), string::String2WString(address_str.String()).String(),
            _port_str.Size(), string::String2WString(port_str.String()).String()
        );
        return ret_val;
    }

    _endpoint_vector_ptr->Resize(endpoints.size());
    SizeType vector_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin();
        endpoint_iterator != endpoints.end();
        ++endpoint_iterator, ++vector_index
        ) {
        *(*_endpoint_vector_ptr)[vector_index].endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint*>() =
            *endpoint_iterator;
    }
    return ret_val;
}

NODISCARD ReturnType ZIOContext::Stop() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZIOContextState_Run,
        error_code::kPSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZIOContextState_Run
    );

    data_ptr_->io_context_.stop();

    return ret_val;
}

NODISCARD ReturnType ZIOContext::Run() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZIOContextState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZIOContextState_Idle
    );

    state_ = ZIOContextState_Run;

    data_ptr_->io_context_.run();
    data_ptr_->io_context_.restart();

    state_ = ZIOContextState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZIOContext::AsyncRun() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZIOContextState_Idle,
        error_code::kPSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZIOContextState_Idle
    );

    state_ = ZIOContextState_Run;

    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            //start dealing with async operation.
            data_ptr_->io_context_.run();
            data_ptr_->io_context_.restart();

            state_ = ZIOContextState_Idle;
        }
    );

    return ret_val;
}

NODISCARD Void ZIOContext::Join() noexcept {
    if (data_ptr_->aysnc_thread_.Joinable()) {
        data_ptr_->aysnc_thread_.Join();
    }
}

}//socket
}//zengine