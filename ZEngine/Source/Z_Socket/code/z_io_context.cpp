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

#include "z_io_context.h"
#include "z_tcp_endpoint.h"
#include "z_udp_endpoint.h"

#include "data/z_io_context_data.h"

namespace zengine {
namespace socket {

NODISCARD ZIOContext& ZIOContext::Instance() noexcept {
    static ZIOContext io_context;
    return io_context;
}

ZIOContext::ZIOContext() noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZIOContextData>())
    , running_thread_count_(0ULL)
{}

ZIOContext::~ZIOContext() noexcept {}

NODISCARD ReturnType ZIOContext::ResolveTCPAddress(
    ZStringView _address_str,
    ZStringView _port_str,
    TArray<ZTCPEndpoint>* _endpoint_array_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    //resolve endpoints
    boost::asio::ip::tcp::resolver resolver(data_ptr_->io_context_);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(
        boost::asio::string_view(_address_str.DataPtr(), _address_str.Size()),
        boost::asio::string_view(_port_str.DataPtr(), _port_str.Size()),
        error_code
    );

    if (error_code) {
        ret_val = error_code::kSocketErrorCode_AddressNotVaild;
        ZString address_str(_address_str);
        ZString port_str(_port_str);
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls address: %ls port: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr(),
            string::StringToWString(address_str.DataPtr()).DataPtr(),
            string::StringToWString(port_str.DataPtr()).DataPtr()
        );
        return ret_val;
    }

    _endpoint_array_ptr->Resize(endpoints.size());
    SizeType array_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin(); 
        endpoint_iterator != endpoints.end(); 
        ++endpoint_iterator, ++array_index
    ) {
        *(*_endpoint_array_ptr)[array_index].endpoint_data_.DataPtr<boost::asio::ip::tcp::endpoint>() = 
            *endpoint_iterator;
    }
    return ret_val;
}

NODISCARD ReturnType ZIOContext::ResolveUDPAddress(
    ZStringView _address_str,
    ZStringView _port_str,
    TArray<ZUDPEndpoint>* _endpoint_array_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    boost::system::error_code error_code;

    //resolve endpoints
    boost::asio::ip::udp::resolver resolver(data_ptr_->io_context_);
    boost::asio::ip::udp::resolver::results_type endpoints = resolver.resolve(
        boost::asio::string_view(_address_str.DataPtr(), _address_str.Size()),
        boost::asio::string_view(_port_str.DataPtr(), _port_str.Size()),
        error_code
    );

    if (error_code) {
        ret_val = error_code::kSocketErrorCode_AddressNotVaild;
        ZString address_str(_address_str);
        ZString port_str(_port_str);
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls address(%d): %ls port(%d): %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr(),
            _address_str.Size(), string::StringToWString(address_str.DataPtr()).DataPtr(),
            _port_str.Size(), string::StringToWString(port_str.DataPtr()).DataPtr()
        );
        return ret_val;
    }

    _endpoint_array_ptr->Resize(endpoints.size());
    SizeType array_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin();
        endpoint_iterator != endpoints.end();
        ++endpoint_iterator, ++array_index
        ) {
        *(*_endpoint_array_ptr)[array_index].endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint>() =
            *endpoint_iterator;
    }
    return ret_val;
}

Void ZIOContext::Stop() noexcept {
    data_ptr_->io_context_.stop();
}

Void ZIOContext::Run() noexcept {
    if (data_ptr_->io_context_.stopped()) {
        data_ptr_->io_context_.restart();
    }
    ++running_thread_count_;
    data_ptr_->io_context_.run();
    --running_thread_count_;
}

Void ZIOContext::AsyncRun() noexcept {
    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            Run();
        }
    );
}

Void ZIOContext::Join() noexcept {
    if (data_ptr_->aysnc_thread_.Joinable()) {
        data_ptr_->aysnc_thread_.Join();
    }
}

}//socket
}//zengine