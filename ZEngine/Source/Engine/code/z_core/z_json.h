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

#include "internal/z_drive.h"

#include "z_string.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZJsonErrorCode : ReturnType {
    kZJsonErrorCode_LinkError = kErrorCodeBase_ZJson,
    kZJsonErrorCode_JsonParseError,
    kZJsonErrorCode_NullptrParams
};
}//error_code
}//zengine

namespace rapidjson {

template<typename _EncodingType, typename _AllocatorType>
class GenericValue;
template<typename _AllocatorType>
class MemoryPoolAllocator;
template<typename _CharType = zengine::Char>
struct UTF8;

}

namespace zengine {

namespace internal {
    
class JsonAllocatorP;
class JsonDocument;
using JsonAllocator = rapidjson::MemoryPoolAllocator<internal::JsonAllocatorP>;
using JsonValue = rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>;

}

/*
    Json value class.
*/
class CORE_DLLAPI ZJsonValue : public ZObject {
public:
    ZJsonValue() noexcept;
    ZJsonValue(ZJsonValue&& _value) noexcept;

    ZJsonValue(const Char* _key, Int32 _value) noexcept;
    ZJsonValue(const Char* _key, Int64 _value) noexcept;
    ZJsonValue(const Char* _key, UInt32 _value) noexcept;
    ZJsonValue(const Char* _key, UInt64 _value) noexcept;
    ZJsonValue(const Char* _key, Float32 _value) noexcept;
    ZJsonValue(const Char* _key, Float64 _value) noexcept;
    ZJsonValue(const Char* _key, const Char* _value) noexcept;
    ZJsonValue(const Char* _key, const ZJsonValue& _value) noexcept;
    ZJsonValue(const Char* _key, ZJsonValue&& _value) noexcept;

    ~ZJsonValue() noexcept;
    
    ZJsonValue& operator=(Int32 _value) noexcept;
    ZJsonValue& operator=(Int64 _value) noexcept;
    ZJsonValue& operator=(UInt32 _value) noexcept;
    ZJsonValue& operator=(UInt64 _value) noexcept;
    ZJsonValue& operator=(Float32 _value) noexcept;
    ZJsonValue& operator=(Float64 _value) noexcept;
    ZJsonValue& operator=(const Char* _value) noexcept;
    ZJsonValue& operator=(const ZJsonValue& _value) noexcept;
    ZJsonValue& operator=(ZJsonValue&& _value) noexcept;

    NODISCARD ZJsonValue operator[](const Char* _key) noexcept;

    Void AddKey(const Char* _key, Int32 _value) noexcept;
    Void AddKey(const Char* _key, Int64 _value) noexcept;
    Void AddKey(const Char* _key, UInt32 _value) noexcept;
    Void AddKey(const Char* _key, UInt64 _value) noexcept;
    Void AddKey(const Char* _key, Float32 _value) noexcept;
    Void AddKey(const Char* _key, Float64 _value) noexcept;
    Void AddKey(const Char* _key, const Char* _value) noexcept;
    Void AddKey(const Char* _key, const ZJsonValue& _value) noexcept;
    Void AddKey(const Char* _key, ZJsonValue&& _value) noexcept;

    /*
        Returns true if the key exist.
    */
    NODISCARD Bool RemoveKey(const Char* _key) noexcept;

    /*
        Returns true if the key exist.
    */
    NODISCARD Bool HasKey(const Char* _key) noexcept;

    Void Clear() noexcept;

    /*
        Generates the string by the json object.
    */
    NODISCARD ZString GenerateJsonString() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZJsonDocument;

private:
    ZJsonValue(const ZJsonValue&) = delete;

    Void CheckPointerP() noexcept;

    internal::JsonValue* json_value_ptr_;
    Bool need_delete_;
};

/*
    Json document class.
*/
class CORE_DLLAPI ZJsonDocument : public ZObject {
public:
    ZJsonDocument() noexcept;
    ZJsonDocument(ZJsonDocument&& _value) noexcept;

    ZJsonDocument(const Char* _key, Int32 _value) noexcept;
    ZJsonDocument(const Char* _key, Int64 _value) noexcept;
    ZJsonDocument(const Char* _key, UInt32 _value) noexcept;
    ZJsonDocument(const Char* _key, UInt64 _value) noexcept;
    ZJsonDocument(const Char* _key, Float32 _value) noexcept;
    ZJsonDocument(const Char* _key, Float64 _value) noexcept;
    ZJsonDocument(const Char* _key, const Char* _value) noexcept;
    ZJsonDocument(const Char* _key, const ZJsonDocument& _value) noexcept;
    ZJsonDocument(const Char* _key, ZJsonDocument&& _value) noexcept;

    ~ZJsonDocument() noexcept;

    ZJsonDocument& operator=(ZJsonDocument&& _value) noexcept;

    NODISCARD ZJsonValue operator[](const Char* _key) noexcept;

    Void AddKey(const Char* _key, Int32 _value) noexcept;
    Void AddKey(const Char* _key, Int64 _value) noexcept;
    Void AddKey(const Char* _key, UInt32 _value) noexcept;
    Void AddKey(const Char* _key, UInt64 _value) noexcept;
    Void AddKey(const Char* _key, Float32 _value) noexcept;
    Void AddKey(const Char* _key, Float64 _value) noexcept;
    Void AddKey(const Char* _key, const Char* _value) noexcept;
    Void AddKey(const Char* _key, const ZJsonDocument& _value) noexcept;
    Void AddKey(const Char* _key, ZJsonDocument&& _value) noexcept;

    /*
        Returns true if the key exist.
    */
    NODISCARD Bool RemoveKey(const Char* _key) noexcept;

    /*
        Returns true if the key exist.
    */
    NODISCARD Bool HasKey(const Char* _key) noexcept;

    Void Clear() noexcept;

    /*
        Paese the string to json object.
    */
    NODISCARD ReturnType Parse(const Char* _str) noexcept;

    /*
        Generates the string by the json object.
    */
    NODISCARD ZString GenerateJsonString() noexcept;
    /*
        Read a json file.
    */
    NODISCARD ReturnType ReadFile(const WChar* _path_dir) noexcept;
    /*
        Write a json file.
    */
    NODISCARD ReturnType WriteFile(const WChar* _path_dir) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZJsonDocument(const ZJsonDocument&) = delete;
    ZJsonDocument& operator=(const ZJsonDocument&) = delete;

    Void CheckPointerP() noexcept;

    internal::JsonDocument* json_doc_ptr_;
    Bool need_delete_;
};

}//zengine