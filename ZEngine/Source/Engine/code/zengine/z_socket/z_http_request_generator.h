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

struct ZHTTP11GeneratorData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    HTTP Request Generator, based on boost::beast.
*/
class SOCKET_DLLAPI ZHTTP11RequestGenerator : public ZObject {
public:
    enum class RequestTypeEnum_ : Int32 {
        kGet
    };

    ZHTTP11RequestGenerator() noexcept;
    ~ZHTTP11RequestGenerator() noexcept;

    Void SetRequestType(RequestTypeEnum_ _request_type) noexcept;
    Void SetTarget(ZStringView _target_str) noexcept;

    Void SetHost(ZStringView _host_str) noexcept;
    Void SetAccept(ZStringView _accept_str) noexcept;
    Void SetConnection(ZStringView _connection_str) noexcept;
    Void SetUserAgent(ZStringView _user_agent_str) noexcept;

    Void Clear() noexcept;

    /*
        Request type and target must be set before request string generated.
    */
    ZString GenerateString() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZHTTP11RequestGenerator(const ZHTTP11RequestGenerator&) = delete;
    ZHTTP11RequestGenerator(ZHTTP11RequestGenerator&&) = delete;
    ZHTTP11RequestGenerator& operator=(const ZHTTP11RequestGenerator&) = delete;
    ZHTTP11RequestGenerator& operator=(ZHTTP11RequestGenerator&&) = delete;

    TUniquePointer<internal::ZHTTP11GeneratorData> data_ptr_;
};

}//socket
}//zengine