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

#include "library/l_rapidjson.h"

#include "t_smart_pointer.h"
#include "z_string.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZJsonErrorCodeEnum : ReturnType {
    kZJsonErrorCode_LinkError = kErrorCodeBase_ZJson,
    kZJsonErrorCode_SystemError,
    kZJsonErrorCode_NullptrParam,
    kZJsonErrorCode_ParamOutOfRange,
    kZJsonErrorCode_JsonParseError
};
}//error_code
}//zengine

namespace zengine {
/*
    Json value class.
*/
class CORE_DLLAPI ZJsonValue : public ZObject {
public:
    ~ZJsonValue() noexcept;
    
    ZJsonValue& operator=(Bool _value) noexcept;
    ZJsonValue& operator=(Int32 _value) noexcept;
    ZJsonValue& operator=(Int64 _value) noexcept;
    ZJsonValue& operator=(UInt32 _value) noexcept;
    ZJsonValue& operator=(UInt64 _value) noexcept;
    ZJsonValue& operator=(Float32 _value) noexcept;
    ZJsonValue& operator=(Float64 _value) noexcept;
    ZJsonValue& operator=(const Char* _value) noexcept;
    ZJsonValue& operator=(const ZJsonValue& _value) noexcept;
    ZJsonValue& operator=(ZJsonValue&& _value) noexcept;

    operator Bool() const noexcept;
    operator Int32() const noexcept;
    operator Int64() const noexcept;
    operator UInt32() const noexcept;
    operator UInt64() const noexcept;
    operator Float32() const noexcept;
    operator Float64() const noexcept;
    operator const Char*() const noexcept;

    NODISCARD ZJsonValue operator[](const Char* _key) noexcept;
    NODISCARD const ZJsonValue operator[](const Char* _key) const noexcept;

    NODISCARD Bool IsBool() const noexcept;
    NODISCARD Bool IsInt32() const noexcept;
    NODISCARD Bool IsInt64() const noexcept;
    NODISCARD Bool IsUInt32() const noexcept;
    NODISCARD Bool IsUInt64() const noexcept;
    NODISCARD Bool IsFloat32() const noexcept;
    NODISCARD Bool IsFloat64() const noexcept;
    NODISCARD Bool IsString() const noexcept;

    NODISCARD Bool GetBool() const noexcept;
    NODISCARD Int32 GetInt32() const noexcept;
    NODISCARD Int64 GetInt64() const noexcept;
    NODISCARD UInt32 GetUInt32() const noexcept;
    NODISCARD UInt64 GetUInt64() const noexcept;
    NODISCARD Float32 GetFloat32() const noexcept;
    NODISCARD Float64 GetFloat64() const noexcept;
    NODISCARD const Char* GetString() const noexcept;

    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Bool _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, const ZJsonValue& _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, ZJsonValue&& _value, Bool _overwrite_exist = true) noexcept;

    /*
        Returns true if the member exist.
    */
    NODISCARD Bool RemoveMember(const Char* _key) noexcept;

    /*
        Returns true if the member exist.
    */
    NODISCARD Bool HasMember(const Char* _key) const noexcept;

    Void Clear() noexcept;

    /*
        Generates the string by the json object.
    */
    NODISCARD ZString GenerateJsonString() const noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZJsonDocument;

private:
    ZJsonValue(const ZJsonValue&) = delete;
    ZJsonValue& operator=(const ZJsonDocument&) = delete;

    ZJsonValue() noexcept;
    ZJsonValue(ZJsonValue&& _value) noexcept;

    ZJsonValue(const Char* _key, Bool _value) noexcept;
    ZJsonValue(const Char* _key, Int32 _value) noexcept;
    ZJsonValue(const Char* _key, Int64 _value) noexcept;
    ZJsonValue(const Char* _key, UInt32 _value) noexcept;
    ZJsonValue(const Char* _key, UInt64 _value) noexcept;
    ZJsonValue(const Char* _key, Float32 _value) noexcept;
    ZJsonValue(const Char* _key, Float64 _value) noexcept;
    ZJsonValue(const Char* _key, const Char* _value) noexcept;
    ZJsonValue(const Char* _key, const ZJsonValue& _value) noexcept;
    ZJsonValue(const Char* _key, ZJsonValue&& _value) noexcept;

    internal::JsonValue* json_value_ptr_;
    internal::JsonAllocator* json_allocator_ptr_;
};

/*
    Json document class.
*/
class CORE_DLLAPI ZJsonDocument : public ZObject {
public:
    ZJsonDocument() noexcept;
    ZJsonDocument(ZJsonDocument&& _doc) noexcept;

    ZJsonDocument(const Char* _key, Bool _value) noexcept;
    ZJsonDocument(const Char* _key, Int32 _value) noexcept;
    ZJsonDocument(const Char* _key, Int64 _value) noexcept;
    ZJsonDocument(const Char* _key, UInt32 _value) noexcept;
    ZJsonDocument(const Char* _key, UInt64 _value) noexcept;
    ZJsonDocument(const Char* _key, Float32 _value) noexcept;
    ZJsonDocument(const Char* _key, Float64 _value) noexcept;
    ZJsonDocument(const Char* _key, const Char* _value) noexcept;
    ZJsonDocument(const Char* _key, const ZJsonValue& _value) noexcept;
    ZJsonDocument(const Char* _key, ZJsonValue&& _value) noexcept;
    ZJsonDocument(const Char* _key, const ZJsonDocument& _doc) noexcept;
    ZJsonDocument(const Char* _key, ZJsonDocument&& _doc) noexcept;

    ~ZJsonDocument() noexcept;

    ZJsonDocument& operator=(ZJsonDocument&& _doc) noexcept;

    NODISCARD ZJsonValue operator[](const Char* _key) noexcept;
    NODISCARD const ZJsonValue operator[](const Char* _key) const noexcept;

    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Bool _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, const ZJsonValue& _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, ZJsonValue&& _value, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, const ZJsonDocument& _doc, Bool _overwrite_exist = true) noexcept;
    /*
        Default overwrites the existing member if already exists.
    */
    Void AddMember(const Char* _key, ZJsonDocument&& _doc, Bool _overwrite_exist = true) noexcept;

    /*
        Returns true if the member exist.
    */
    NODISCARD FORCEINLINE Bool RemoveMember(const Char* _key) noexcept { return json_doc_.RemoveMember(_key); }

    /*
        Returns true if the member exist.
    */
    NODISCARD FORCEINLINE Bool HasMember(const Char* _key) const noexcept { return json_doc_.HasMember(_key); }

    FORCEINLINE Void Clear() noexcept { json_doc_.Clear(); }

    /*
        Paese the string to json object.
    */
    NODISCARD ReturnType Parse(const Char* _str) noexcept;

    /*
        Generates the string by the json object.
    */
    NODISCARD ZString GenerateJsonString() const noexcept;
    /*
        Read a json file.
    */
    NODISCARD ReturnType ReadFile(const Char* _path_dir) noexcept;
    /*
        Write a json file.
    */
    NODISCARD ReturnType WriteFile(const Char* _path_dir) noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZJsonValue;

private:
    ZJsonDocument(const ZJsonDocument&) = delete;
    ZJsonDocument& operator=(const ZJsonDocument&) = delete;

    internal::JsonDocument json_doc_;
};

}//zengine