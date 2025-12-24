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

#include "z_http_request_generator.h"

#include "data/z_http_11_request_generator_data.h"

namespace zengine {
namespace socket {

ZHTTP11RequestGenerator::ZHTTP11RequestGenerator() noexcept 
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZHTTP11GeneratorData>())
{
    data_ptr_->request_.version(11);
}

ZHTTP11RequestGenerator::~ZHTTP11RequestGenerator() noexcept {}

Void ZHTTP11RequestGenerator::SetRequestType(RequestTypeEnum_ _request_type) noexcept {
    switch (_request_type) {
    case ZHTTP11RequestGenerator::RequestTypeEnum_::kGet:
        data_ptr_->request_.method(boost::beast::http::verb::get);
        break;
    default:
        break;
    }
}
Void ZHTTP11RequestGenerator::SetTarget(ZStringView _target_str) noexcept {
    data_ptr_->request_.target(
        boost::core::string_view(_target_str.GetDataPtr(), _target_str.GetSize())
    );
}

Void ZHTTP11RequestGenerator::SetHost(ZStringView _host_str) noexcept {
    data_ptr_->request_.set(
        boost::beast::http::field::host, 
        boost::core::string_view(_host_str.GetDataPtr(), _host_str.GetSize())
    );
}
Void ZHTTP11RequestGenerator::SetAccept(ZStringView _accept_str) noexcept {
    data_ptr_->request_.set(
        boost::beast::http::field::accept, 
        boost::core::string_view(_accept_str.GetDataPtr(), _accept_str.GetSize())      
    );
}
Void ZHTTP11RequestGenerator::SetConnection(ZStringView _connection_str) noexcept {
    data_ptr_->request_.set(
        boost::beast::http::field::connection,
        boost::core::string_view(_connection_str.GetDataPtr(), _connection_str.GetSize())
    );
}
Void ZHTTP11RequestGenerator::SetUserAgent(ZStringView _user_agent_str) noexcept {
    data_ptr_->request_.set(
        boost::beast::http::field::user_agent,
        boost::core::string_view(_user_agent_str.GetDataPtr(), _user_agent_str.GetSize())
    );
}

Void ZHTTP11RequestGenerator::Clear() noexcept {
    data_ptr_->request_.clear();
    data_ptr_->request_.version(11);
}

ZString ZHTTP11RequestGenerator::GenerateString() noexcept {
    data_ptr_->string_stream_ << data_ptr_->request_;
    ZString request_str(std::move(data_ptr_->string_stream_.str()));
    data_ptr_->string_stream_.str("");
    return request_str;
}

}//socket
}//zengine