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

#include "../z_core/t_function.h"
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTLSContextData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    TLS client context. 
*/
class SOCKET_DLLAPI ZTLSContext : public ZObject {
public:
    ZTLSContext(TLSTypeEnum _tls_type) noexcept;
    ~ZTLSContext() noexcept;

    NODISCARD FORCEINLINE TLSTypeEnum TLSType() const noexcept { return tls_type_; }

protected:
    using SuperType_ = ZObject;
    friend class ZTLSStream;

private:
    ZTLSContext(const ZTLSContext&) = delete;
    ZTLSContext(ZTLSContext&&) = delete;
    ZTLSContext& operator=(const ZTLSContext&) = delete;
    ZTLSContext& operator=(ZTLSContext&&) = delete;

private:
    TUniquePointer<internal::ZTLSContextData> data_ptr_;
    TLSTypeEnum tls_type_;
};

}//socket
}//zengine