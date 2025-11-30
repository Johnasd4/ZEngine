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

#include "z_udp_endpoint.h"

namespace zengine {
namespace socket {

ZUDPEndpoint::ZUDPEndpoint() noexcept 
    : SuperType_()
    , endpoint_data_()
{}

ZUDPEndpoint::ZUDPEndpoint(const ZUDPEndpoint& _endpoint) noexcept
    : SuperType_()
    , endpoint_data_(_endpoint.endpoint_data_)
{}

ZUDPEndpoint::ZUDPEndpoint(ZUDPEndpoint&& _endpoint) noexcept
    : SuperType_()
    , endpoint_data_(_endpoint.endpoint_data_)
{}

ZUDPEndpoint::ZUDPEndpoint(const Char* _ip_str, UInt16 _port) noexcept {
    boost::system::error_code error_code;
    boost::asio::ip::address address = boost::asio::ip::make_address(_ip_str, error_code);
    if (error_code) {
        Z_LOG_ERROR(
            error_code::kPSocketErrorCode_IPNotVaild, 0,
            L"IP not vaild! _ip_str: %ls",
            string::String2WString(_ip_str).String()
        );
        return;
    }
    *endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint>() = boost::asio::ip::udp::endpoint(
        boost::asio::ip::make_address(_ip_str), _port
    );
}

ZUDPEndpoint::ZUDPEndpoint(UInt32 _ip, UInt16 _port) noexcept {
    *endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint>() = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4(_ip), _port
    );
}

NODISCARD ReturnType ZUDPEndpoint::SetEndpoint(const Char* _ip_str, UInt16 _port) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;
    boost::asio::ip::address address = boost::asio::ip::make_address(_ip_str, error_code);
    if (error_code) {
        ret_val = error_code::kPSocketErrorCode_IPNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"IP not vaild! _ip_str: %ls",
            string::String2WString(_ip_str).String()
        );
        return ret_val;
    }
    *endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint>() = boost::asio::ip::udp::endpoint(
        boost::asio::ip::make_address(_ip_str), _port
    );
    return ret_val;
}

Void ZUDPEndpoint::SetEndpoint(UInt32 _ip, UInt16 _port) noexcept {
    *endpoint_data_.DataPtr<boost::asio::ip::udp::endpoint>() = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4(_ip), _port
    );
}

NODISCARD IPTypeEnum ZUDPEndpoint::IPType() const noexcept {
    Bool is_ip6 = 
        endpoint_data_.DataPtr<const boost::asio::ip::udp::endpoint>()->protocol() == boost::asio::ip::udp::v6();
    return is_ip6 ? IPTypeEnum::IP6 : IPTypeEnum::IP4;
}
NODISCARD ZString ZUDPEndpoint::IPString() const noexcept {
    return endpoint_data_.DataPtr<const boost::asio::ip::udp::endpoint>()->address().to_string().c_str();
}
NODISCARD UInt32 ZUDPEndpoint::IP4() const noexcept {
    return endpoint_data_.DataPtr<const boost::asio::ip::udp::endpoint>()->address().to_v4().to_uint();
}
NODISCARD TFixedMemory<ZUDPEndpoint::KIP6Size> ZUDPEndpoint::IP6() const noexcept {
    TFixedMemory<KIP6Size> data;
    Copy(
        data.DataPtr<Void>(),
        endpoint_data_.DataPtr<const boost::asio::ip::udp::endpoint>()->address().to_v6().to_bytes().data(),
        KIP6Size
    );
    return data;
}
NODISCARD UInt16 ZUDPEndpoint::Port() const noexcept {
    return endpoint_data_.DataPtr<const boost::asio::ip::udp::endpoint>()->port();
}

ZUDPEndpoint::~ZUDPEndpoint() noexcept {}

}//socket
}//zengine