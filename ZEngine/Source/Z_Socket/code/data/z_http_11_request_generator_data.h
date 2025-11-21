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

#include <boost/beast.hpp>

#include "z_core/t_allocator.h"
#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

using HTTP11Field = boost::beast::http::basic_fields<TAllocator<Char>>;
using HTTP11Request = boost::beast::http::request<boost::beast::http::string_body, HTTP11Field>;
using HTTP11StringStream = std::basic_ostringstream<Char, std::char_traits<Char>, TAllocator<Char>>;

struct ZHTTP11GeneratorData : public ZObject {
public:
    ZHTTP11GeneratorData() noexcept;

protected:
    using SuperType_ = ZObject;

public:
    HTTP11Request request_;
    HTTP11StringStream string_stream_;
};

}//internal
}//socket
}//zengine

