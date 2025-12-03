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

#include "t_smart_pointer.h"
#include "z_object.h"
#include "z_string.h"

namespace zengine {
namespace error_code {
enum ZConfigErrorCodeEnum : ReturnType {
    kZConfigErrorCode_LinkError = kErrorCodeBase_ZConfig,
    kZConfigErrorCode_SystemError,
    kZConfigErrorCode_NullptrParam,
    kZConfigErrorCode_ParamOutOfRange,
    kZConfigErrorCode_MemberNotExist
};
}//error_code
}//zengine

namespace zengine {
class ZJsonDocument;
}

namespace zengine {

/*
    Config class. Used for storing constants.
    Aoto saves and reads.
    WARNING: Large config file will impact io performance.
*/
class CORE_DLLAPI ZConfig : public ZObject {
public:
    ZConfig(const WChar* _file_dir) noexcept;
    ~ZConfig() noexcept;

    NODISCARD Bool GetMemberValueBool(const Char* _key) const noexcept;
    NODISCARD Int32 GetMemberValueInt32(const Char* _key) const noexcept;
    NODISCARD Int64 GetMemberValueInt64(const Char* _key) const noexcept;
    NODISCARD UInt32 GetMemberValueUInt32(const Char* _key) const noexcept;
    NODISCARD UInt64 GetMemberValueUInt64(const Char* _key) const noexcept;
    NODISCARD Float32 GetMemberValueFloat32(const Char* _key) const noexcept;
    NODISCARD Float64 GetMemberValueFloat64(const Char* _key) const noexcept;
    NODISCARD const Char* GetMemberValueString(const Char* _key) const noexcept;

    NODISCARD ReturnType SetMemberValue(const Char* _key, Bool _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, Int32 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, Int64 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, UInt32 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, UInt64 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, Float32 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, Float64 _value) noexcept;
    NODISCARD ReturnType SetMemberValue(const Char* _key, const Char* _value) noexcept;

    /*
        Default not override exist member.
    */
    NODISCARD ReturnType AddMember(const Char* _key, Bool _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist = false) noexcept;
    /*
        Overwrites the existing member if already exists.
    */
    NODISCARD ReturnType AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist = false) noexcept;

    /*
        Returns true if the member exist.
    */
    NODISCARD ReturnType RemoveMember(const Char* _key) noexcept;

    NODISCARD ReturnType Clear() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZConfig(const ZConfig&) = delete;
    ZConfig(ZConfig&&) = delete;

    /*
        Save file.
    */
    NODISCARD ReturnType SaveConfigP() noexcept;

    /*
        Update version.
    */
    Bool UpdateVersionP() noexcept;

    /*
        Update save time.
    */
    Void UpdateSaveTimeP() noexcept;

private:
    static inline constexpr SizeType kUpdateTimeStringLength = 32;
    static inline constexpr Char kVersionKey[] = "VERSION";
    static inline constexpr Char kSaveTimeKey[] = "SAVE_TIME";
    static inline constexpr WChar kBackUpExtension[] = L".bak";

    TUniquePointer<ZJsonDocument> config_data_ptr_;
    ZWString file_dir_;
};

}//zengine