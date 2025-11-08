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
#pragma once

#include "drive.h"

#include <boost/asio/ssl.hpp>

#include "z_core/m_log.h"
#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTLSContextData : public ZObject {
public:
    ZTLSContextData(TLSTypeEnum _tls_type) noexcept
        : SuperType_()
        , tls_context_(
            _tls_type == kTLSType_Client ? boost::asio::ssl::context::tls_client : boost::asio::ssl::context::tls_server
        )
    {
        switch (_tls_type) {
        case kTLSType_Client:
            tls_context_.load_verify_file("ca.pem");
            tls_context_.set_verify_mode(boost::asio::ssl::verify_peer);
            break;
        case kTLSType_Server:
            tls_context_.use_certificate_file("server.crt", boost::asio::ssl::context::pem);
            tls_context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
            break;
        default:
            Z_LOG_ERROR(
                error_code::kPSocketErrorCode_TLSTypeNotValid, 0, 
                L"_tls_type not valid! _tls_type: %d", _tls_type
            );
        }
    }

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::ssl::context tls_context_;
};

}//internal
}//socket
}//zengine