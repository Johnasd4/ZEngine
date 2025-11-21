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

#include "z_buffer.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZHTTP11ResponseResolverData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    HTTP response resolver, based on boost::beast.
*/
class SOCKET_DLLAPI ZHTTP11ResponseResolver : public ZObject {
public:
    ZHTTP11ResponseResolver() noexcept;
    ~ZHTTP11ResponseResolver() noexcept;

    NODISCARD ReturnType Resolve(ZConstBuffer _buffer) noexcept;

    NODISCARD UInt32 GetVersion() noexcept;
    NODISCARD UInt32 GetResult() noexcept;
    NODISCARD const Char* GetReason() noexcept;

    NODISCARD ReturnType GetInt32(Int32* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetInt64(Int64* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetUInt32(UInt32* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetUInt64(UInt64* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetFloat32(Float32* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetFloat64(Float64* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetStringView(ZStringView* _value_ptr, const Char* _name) noexcept;
    NODISCARD ReturnType GetString(ZString* _value_ptr, const Char* _name) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZHTTP11ResponseResolver(const ZHTTP11ResponseResolver&) = delete;
    ZHTTP11ResponseResolver(ZHTTP11ResponseResolver&&) = delete;
    ZHTTP11ResponseResolver& operator=(const ZHTTP11ResponseResolver&) = delete;
    ZHTTP11ResponseResolver& operator=(ZHTTP11ResponseResolver&&) = delete;

    TUniquePointer<internal::ZHTTP11ResponseResolverData> data_ptr_;
};

}//socket
}//zengine