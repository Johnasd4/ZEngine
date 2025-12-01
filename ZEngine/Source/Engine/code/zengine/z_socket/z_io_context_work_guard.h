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

#include "../z_core/t_array.h"
#include "../z_core/t_atom.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZIOContextWorkGuardData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    IO context work guard. When this object is alive, the io_context will not stop running.
*/
class SOCKET_DLLAPI ZIOContextWorkGuard : public ZObject {
public:
    ZIOContextWorkGuard(ZIOContext* _io_context_ptr) noexcept;
    ~ZIOContextWorkGuard() noexcept;

    Void Reset() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZIOContextWorkGuard(const ZIOContextWorkGuard&) = delete;
    ZIOContextWorkGuard(ZIOContextWorkGuard&&) = delete;
    ZIOContextWorkGuard& operator=(const ZIOContextWorkGuard&) = delete;
    ZIOContextWorkGuard& operator=(ZIOContextWorkGuard&&) = delete;

private:
    TUniquePointer<internal::ZIOContextWorkGuardData> data_ptr_;
};

}//socket
}//zengine