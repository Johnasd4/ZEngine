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

#include "z_http_request_generator.h"

#include "../z_core/t_allocator.h"
#include "../z_core/z_string.h"

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

Void ZHTTP11RequestGenerator::SetRequestType(RequestType_ _request_type) noexcept {
    switch (_request_type) {
    case RequestType_Get:
        data_ptr_->request_.method(boost::beast::http::verb::get);
        break;
    default:
        break;
    }
}
Void ZHTTP11RequestGenerator::SetTarget(const Char* _target_str) noexcept {
    data_ptr_->request_.target(_target_str);
}

Void ZHTTP11RequestGenerator::SetHost(const Char* _host_str) noexcept {
    data_ptr_->request_.set(boost::beast::http::field::host, _host_str);
}
Void ZHTTP11RequestGenerator::SetAccept(const Char* _accept_str) noexcept {
    data_ptr_->request_.set(boost::beast::http::field::accept, _accept_str);
}
Void ZHTTP11RequestGenerator::SetConnection(const Char* _connection_str) noexcept {
    data_ptr_->request_.set(boost::beast::http::field::connection, _connection_str);
}
Void ZHTTP11RequestGenerator::SetUserAgent(const Char* _user_agent_str) noexcept {
    data_ptr_->request_.set(boost::beast::http::field::user_agent, _user_agent_str);
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