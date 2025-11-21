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

#include "z_url_resolver.h"

#include "data/z_url_resolver_data.h"

namespace zengine {
namespace socket {

ZURLResolver::ZURLResolver() noexcept 
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZURLResolverData>())
{}

ZURLResolver::~ZURLResolver() noexcept {}

NODISCARD ReturnType ZURLResolver::Resolve(ZStringView _url_str) noexcept {
    ReturnType ret_val = kOK;
    data_ptr_->url_result_ = boost::urls::parse_uri(boost::core::string_view(_url_str.DataPtr(), _url_str.Size()));
    //url not valid
    if (!data_ptr_->url_result_.has_value()) {
        ZString url_str(_url_str.DataPtr(), _url_str.Size());
        ret_val = error_code::kPURLErrorCode_URLResolveFailed;
        Z_LOG_ERROR(
            ret_val, 0,
            L"URL resolved failed! _url_str: %ls",
            string::String2WString(url_str.String()).String()
        );
        return ret_val;
    }
    data_ptr_->url_ptr_ = &data_ptr_->url_result_.value();
    return ret_val;
}

NODISCARD ZString ZURLResolver::GetScheme() noexcept {
    ZString str;
    if (data_ptr_->url_ptr_) {
        auto str_view = data_ptr_->url_ptr_->scheme();
        str.Assign(str_view.data(), str_view.size());
    }
    return str;
}
NODISCARD ZString ZURLResolver::GetHost() noexcept {
    ZString str;
    if (data_ptr_->url_ptr_) {
        auto str_result = data_ptr_->url_ptr_->host();
        str.Assign(str_result.c_str(), str_result.size());
    }
    return str;
}
NODISCARD ZString ZURLResolver::GetPort() noexcept {
    ZString str;
    if (data_ptr_->url_ptr_) {
        auto str_view = data_ptr_->url_ptr_->port();
        if (str_view != "") {
            str.Assign(str_view.data(), str_view.size());
        }
        else {
            auto scheme_str_view = data_ptr_->url_ptr_->scheme();
            if (scheme_str_view == "http") {
                str.Assign(kHTTPDefaultPortString, sizeof(kHTTPDefaultPortString));
            }
            else if (scheme_str_view == "https") {
                str.Assign(kHTTPSDefaultPortString, sizeof(kHTTPSDefaultPortString));
            }
        }
    }
    return str;
}
NODISCARD ZString ZURLResolver::GetPath() noexcept {
    ZString str;
    if (data_ptr_->url_ptr_) {
        auto str_result = data_ptr_->url_ptr_->path();
        str.Assign(str_result.c_str(), str_result.size());
    }
    return str;
}
NODISCARD ZString ZURLResolver::GetQuery() noexcept {
    ZString str;
    if (data_ptr_->url_ptr_) {
        auto str_result = data_ptr_->url_ptr_->query();
        str.Assign(str_result.c_str(), str_result.size());
    }
    return str;
}

}//socket
}//zengine