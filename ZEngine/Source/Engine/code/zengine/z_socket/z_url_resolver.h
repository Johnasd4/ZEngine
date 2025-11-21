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

#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string.h"
#include "../z_core/z_string_view.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZURLResolverData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    URL resolver, based on boost::beast.
*/
class SOCKET_DLLAPI ZURLResolver : public ZObject {
public:
    ZURLResolver() noexcept;
    ~ZURLResolver() noexcept;

    NODISCARD ReturnType Resolve(ZStringView _url_str) noexcept;

    NODISCARD ZString GetScheme() noexcept;
    NODISCARD ZString GetHost() noexcept;
    NODISCARD ZString GetPort() noexcept;
    NODISCARD ZString GetPath() noexcept;
    NODISCARD ZString GetQuery() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZURLResolver(const ZURLResolver&) = delete;
    ZURLResolver(ZURLResolver&&) = delete;
    ZURLResolver& operator=(const ZURLResolver&) = delete;
    ZURLResolver& operator=(ZURLResolver&&) = delete;

    TUniquePointer<internal::ZURLResolverData> data_ptr_;
};

}//socket
}//zengine