/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "internal/l_rapidjson.h"
#include "internal/t_order_data_iterator.h"

#include "m_log.h" 
#include "t_smart_pointer.h"
#include "z_file.h"
#include "z_string.h"
#include "z_string_view.h"
#include "z_static_string_view.h"
#include "z_object.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration of error codes related to JSON operations.
 */
enum ZJsonErrorCodeEnum : ReturnType::ValueType {
    /** @brief Indicates an error related to linking or file access, based on the ZJson base code. */
    kZJsonErrorCode_LinkError = kErrorCodeBase_ZJson,
    /** @brief Indicates an error originating from the system or an external library. */
    kZJsonErrorCode_SystemOrLibraryError,
    /** @brief Indicates that a required parameter was nullptr. */
    kZJsonErrorCode_NullptrParam,
    /** @brief Indicates that a parameter was outside the valid range. */
    kZJsonErrorCode_ParamOutOfRange,
    /** @brief Indicates a failure when parsing the JSON string. */
    kZJsonErrorCode_JsonParseError,
};

} // error_code
} // zengine

namespace zengine {
namespace internal {

template<Bool _IsGlobal> 
class TJsonValue;
template<Bool _IsGlobal> 
class TJsonValueIterator;
template<Bool _IsGlobal> 
class TJsonValueConstIterator;

/**
 * @brief A wrapper class representing a JSON value node.
 *
 * This class encapsulates the underlying RapidJSON value and acts as a view or reference to a node
 * within a JSON structure. It simplifies interactions with JSON objects, arrays, and primitive types.
 * It manages the allocator context but generally does not own the root memory unless it is a TJsonDocument.
 *
 * @tparam _IsGlobal If true, the class uses the global memory pool. If false, it uses a local allocator.
 */
template<Bool _IsGlobal>
class TJsonValue : public ZObject {
public:
    /** @brief Alias for the base object type. */
    using SuperType_ = ZObject;
    /** @brief Type definition for the non-const iterator. */
    using Iterator_ = TJsonValueIterator<_IsGlobal>;
    /** @brief Type definition for the const iterator. */
    using ConstIterator_ = TJsonValueConstIterator<_IsGlobal>;

    /**
     * @brief Enumeration of JSON value types.
     */
    enum class Type_ {
        /** @brief Represents a null value. */
        kNull,
        /** @brief Represents a boolean value. */
        kBool,
        /** @brief Represents a JSON object. */
        kObject,
        /** @brief Represents a JSON array. */
        kArray,
        /** @brief Represents a string value. */
        kString,
        /** @brief Represents a number value. */
        kNumber
    };

private:
    /** @brief Type alias for the RapidJSON string buffer using the specified allocator. */
    using JsonStringBuffer_ = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, TJsonAllocatorP<_IsGlobal>>;
    /** @brief Type alias for the RapidJSON allocator using the specified allocator. */
    using RapidJsonAllocator_ = RapidJsonAllocator<_IsGlobal>;
    /** @brief Type alias for the RapidJSON value using the specified allocator. */
    using RapidJsonValue_ = RapidJsonValue<_IsGlobal>;

    /**
     * @brief returns a reference to a static empty JSON value.
     * @return A reference to the static empty TJsonValue.
     */
    static TJsonValue& EmptyJsonValueP() noexcept {
        static TJsonValue empty_json_value{ RapidJsonValue_(rapidjson::kNullType), nullptr };
        return empty_json_value;
    }

public:
    /**
     * @brief Copy constructor.
     * @param _value The source TJsonValue to copy from.
     */
    FORCEINLINE TJsonValue(const TJsonValue& _value) noexcept
        : json_value_ptr_(_value.json_value_ptr_)
        , json_allocator_ptr_(_value.json_allocator_ptr_)
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TJsonValue() noexcept = default;
   
    /**
     * @brief Copy assignment operator.
     * @param _value The source TJsonValue to copy from.
     * @return A reference to this object.
     */
    FORCEINLINE TJsonValue& operator=(const TJsonValue& _value) noexcept {
        json_value_ptr_ = _value.json_value_ptr_;
        json_allocator_ptr_ = _value.json_allocator_ptr_;
        return *this;
    }

    /**
     * @brief Assignment operator for compatible value types.
     * @tparam _ValueType The type of the value being assigned.
     * @param _value The value to assign.
     * @return A reference to this object.
     */
    template<typename _ValueType>
    FORCEINLINE TJsonValue& operator=(_ValueType&& _value) noexcept {
        *json_value_ptr_ = PrepareRapidJsonValueP(std::forward<_ValueType>(_value));
        return *this;
    }

    /**
     * @brief Checks if the value is a boolean.
     * @return True if it is a boolean, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsBool() const noexcept { return json_value_ptr_->IsBool(); }

    /**
     * @brief Checks if the value is a 32-bit integer.
     * @return True if it is an Int32, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsInt32() const noexcept { return json_value_ptr_->IsInt(); }

    /**
     * @brief Checks if the value is a 64-bit integer.
     * @return True if it is an Int64, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsInt64() const noexcept { return json_value_ptr_->IsInt64(); }

    /**
     * @brief Checks if the value is an unsigned 32-bit integer.
     * @return True if it is a UInt32, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsUInt32() const noexcept { return json_value_ptr_->IsUint(); }

    /**
     * @brief Checks if the value is an unsigned 64-bit integer.
     * @return True if it is a UInt64, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsUInt64() const noexcept { return json_value_ptr_->IsUint64(); }

    /**
     * @brief Checks if the value is a 32-bit floating point number.
     * @return True if it is a Float32, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsFloat32() const noexcept { return json_value_ptr_->IsFloat(); }

    /**
     * @brief Checks if the value is a 64-bit floating point number.
     * @return True if it is a Float64, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsFloat64() const noexcept { return json_value_ptr_->IsDouble(); }

    /**
     * @brief Checks if the value is a string.
     * @return True if it is a string, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsString() const noexcept { return json_value_ptr_->IsString(); }

    /**
     * @brief Checks if the value is an array.
     * @return True if it is an array, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsArray() const noexcept { return json_value_ptr_->IsArray(); }

    /**
     * @brief Checks if the value is null.
     * @return True if it is null, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsNull() const noexcept { return json_value_ptr_->IsNull(); }

    /**
     * @brief Checks if the value is an object.
     * @return True if it is an object, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsObject() const noexcept { return json_value_ptr_->IsObject(); }

    /**
     * @brief Gets the type of the JSON value.
     * @return The internal Type_ enum representing the value type.
     */
    NODISCARD Type_ GetType() const noexcept {
        switch (json_value_ptr_->GetType()) {
        case rapidjson::kNullType:
            return Type_::kNull;
        case rapidjson::kFalseType:
            return Type_::kBool;
        case rapidjson::kTrueType:
            return Type_::kBool;
        case rapidjson::kObjectType:
            return Type_::kObject;
        case rapidjson::kArrayType:
            return Type_::kArray;
        case rapidjson::kStringType:
            return Type_::kString;
        case rapidjson::kNumberType:
            return Type_::kNumber;
        default:
            return Type_::kNull;
        }
    }

    /**
     * @brief Gets the value as a boolean.
     * @return The boolean value.
     */
    NODISCARD FORCEINLINE Bool GetBool() const noexcept { return json_value_ptr_->GetBool(); }

    /**
     * @brief Gets the value as a 32-bit integer.
     * @return The Int32 value.
     */
    NODISCARD FORCEINLINE Int32 GetInt32() const noexcept { return json_value_ptr_->GetInt(); }

    /**
     * @brief Gets the value as a 64-bit integer.
     * @return The Int64 value.
     */
    NODISCARD FORCEINLINE Int64 GetInt64() const noexcept { return json_value_ptr_->GetInt64(); }

    /**
     * @brief Gets the value as an unsigned 32-bit integer.
     * @return The UInt32 value.
     */
    NODISCARD FORCEINLINE UInt32 GetUInt32() const noexcept { return json_value_ptr_->GetUint(); }

    /**
     * @brief Gets the value as an unsigned 64-bit integer.
     * @return The UInt64 value.
     */
    NODISCARD FORCEINLINE UInt64 GetUInt64() const noexcept { return json_value_ptr_->GetUint64(); }

    /**
     * @brief Gets the value as a 32-bit floating point number.
     * @return The Float32 value.
     */
    NODISCARD FORCEINLINE Float32 GetFloat32() const noexcept { return json_value_ptr_->GetFloat(); }

    /**
     * @brief Gets the value as a 64-bit floating point number.
     * @return The Float64 value.
     */
    NODISCARD FORCEINLINE Float64 GetFloat64() const noexcept { return json_value_ptr_->GetDouble(); }

    /**
     * @brief Gets the value as a string view.
     * @return A ZStringView pointing to the string data.
     */
    NODISCARD FORCEINLINE ZStringView GetString() const noexcept {
        return ZStringView(json_value_ptr_->GetString(), json_value_ptr_->GetStringLength());
    }

    /**
     * @brief Sets the value to a boolean.
     * @param _value The boolean value.
     */
    FORCEINLINE Void SetBool(Bool _value) noexcept { json_value_ptr_->SetBool(_value); }

    /**
     * @brief Sets the value to a 32-bit integer.
     * @param _value The Int32 value.
     */
    FORCEINLINE Void SetInt32(Int32 _value) noexcept { json_value_ptr_->SetInt(_value); }

    /**
     * @brief Sets the value to a 64-bit integer.
     * @param _value The Int64 value.
     */
    FORCEINLINE Void SetInt64(Int64 _value) noexcept { json_value_ptr_->SetInt64(_value); }

    /**
     * @brief Sets the value to an unsigned 32-bit integer.
     * @param _value The UInt32 value.
     */
    FORCEINLINE Void SetUInt32(UInt32 _value) noexcept { json_value_ptr_->SetUint(_value); }

    /**
     * @brief Sets the value to an unsigned 64-bit integer.
     * @param _value The UInt64 value.
     */
    FORCEINLINE Void SetUInt64(UInt64 _value) noexcept { json_value_ptr_->SetUint64(_value); }

    /**
     * @brief Sets the value to a 32-bit floating point number.
     * @param _value The Float32 value.
     */
    FORCEINLINE Void SetFloat32(Float32 _value) noexcept { json_value_ptr_->SetFloat(_value); }

    /**
     * @brief Sets the value to a 64-bit floating point number.
     * @param _value The Float64 value.
     */
    FORCEINLINE Void SetFloat64(Float64 _value) noexcept { json_value_ptr_->SetDouble(_value); }

    /**
     * @brief Sets the value to a string using a static string view.
     * @param _value The static string view.
     */
    FORCEINLINE Void SetString(ZStaticStringView _value) noexcept { 
        json_value_ptr_->Set(rapidjson::StringRef(_value.GetDataPtr(), _value.GetSize()));
    }

    /**
     * @brief Sets the value to a string using a string view.
     * @param _value The string view.
     */
    FORCEINLINE Void SetString(ZStringView _value) noexcept {
        json_value_ptr_->SetString(_value.GetDataPtr(), _value.GetSize(), *json_allocator_ptr_);
    }

    /**
     * @brief Sets the value to an empty array.
     */
    FORCEINLINE Void SetArray() noexcept { json_value_ptr_->SetArray(); }

    /**
     * @brief Sets the value to null.
     */
    FORCEINLINE Void SetNull() noexcept { json_value_ptr_->SetNull(); }

    /**
     * @brief Sets the value to an empty object.
     */
    FORCEINLINE Void SetObject() noexcept { json_value_ptr_->SetObject(); }

    /**
     * @brief Checks if the JSON value is valid (i.e., has a valid allocator).
     * @return True if valid, false otherwise.
     */
    explicit FORCEINLINE operator Bool() const noexcept {
        return json_allocator_ptr_ != nullptr;
    }

    /**
     * @brief Accesses or creates a member by a static string key.
     *
     * If the current value is not an object, it is converted to one. If the key does not exist,
     * a new member is added.
     *
     * @param _key The static string key.
     * @return A TJsonValue wrapper for the accessed member.
     */
    NODISCARD TJsonValue operator[](ZStaticStringView _key) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetObject();
        }

        if (!json_value_ptr_->IsObject()) {
            Z_DEBUG_LOG_MESSAGE("Trying to change a non-object JSON value to an object type! _key: {}", _key);
            json_value_ptr_->SetObject();
        }

        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        auto member_iter = json_value_ptr_->FindMember(key_ref);

        if (member_iter != json_value_ptr_->MemberEnd()) {
            return TJsonValue{ &member_iter->value, json_allocator_ptr_ };
        }

        json_value_ptr_->AddMember(
            key_ref,
            RapidJsonValue_(rapidjson::kObjectType),
            *json_allocator_ptr_
        );
        auto last_iter = json_value_ptr_->MemberEnd() - 1;
        return TJsonValue{ &last_iter->value, json_allocator_ptr_ };
    }

    /**
     * @brief Accesses a member by a static string key (const version).
     *
     * This function asserts if the value is not an object or if the key does not exist.
     *
     * @param _key The static string key.
     * @return A const TJsonValue wrapper for the accessed member.
     */
    NODISCARD const TJsonValue operator[](ZStaticStringView _key) const noexcept {
        if (!json_value_ptr_->IsObject()) {
            ASSERT(false, "TJsonValue: operator[] called on non-object type!");
            return EmptyJsonValueP();
        }

        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        auto member_iter = json_value_ptr_->FindMember(key_ref);

        if (member_iter != json_value_ptr_->MemberEnd()) {
            return TJsonValue{ const_cast<RapidJsonValue_*>(&member_iter->value), json_allocator_ptr_ };
        }

        ASSERT(false, "TJsonValue: Key not found in const operator[]!");
        return EmptyJsonValueP();
    }

    /**
     * @brief Accesses or creates a member by a string view key.
     *
     * If the current value is not an object, it is converted to one. If the key does not exist,
     * a new member is added.
     *
     * @param _key The string view key.
     * @return A TJsonValue wrapper for the accessed member.
     */
    NODISCARD TJsonValue operator[](ZStringView _key) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetObject();
        }

        if (!json_value_ptr_->IsObject()) {
            Z_DEBUG_LOG_MESSAGE("Trying to change a non-object JSON value to an object type! _key: {}", _key);
            json_value_ptr_->SetObject();
        }

        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        auto member_iter = json_value_ptr_->FindMember(key_ref);

        if (member_iter != json_value_ptr_->MemberEnd()) {
            return TJsonValue{ &member_iter->value, json_allocator_ptr_ };
        }

        json_value_ptr_->AddMember(
            RapidJsonValue_(
                _key.GetDataPtr(),
                static_cast<rapidjson::SizeType>(_key.GetSize()),
                *json_allocator_ptr_
            ),
            RapidJsonValue_(rapidjson::kObjectType),
            *json_allocator_ptr_
        );
        auto last_iter = json_value_ptr_->MemberEnd() - 1;
        return TJsonValue{ &last_iter->value, json_allocator_ptr_ };
    }

    /**
     * @brief Accesses a member by a string view key (const version).
     *
     * This function asserts if the value is not an object or if the key does not exist.
     *
     * @param _key The string view key.
     * @return A const TJsonValue wrapper for the accessed member.
     */
    NODISCARD const TJsonValue operator[](ZStringView _key) const noexcept {
        if (!json_value_ptr_->IsObject()) {
            ASSERT(false, "TJsonValue: operator[] called on non-object type!");
            return EmptyJsonValueP();
        }

        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        auto member_iter = json_value_ptr_->FindMember(key_ref);

        if (member_iter != json_value_ptr_->MemberEnd()) {
            return TJsonValue{ const_cast<RapidJsonValue_*>(&member_iter->value), json_allocator_ptr_ };
        }

        ASSERT(false, "TJsonValue: Key not found in const operator[]!");
        return EmptyJsonValueP();
    }

    /**
     * @brief Adds a member to the object.
     * @tparam _KeyType The type of the key.
     * @tparam _ValueType The type of the value.
     * @param _key The key to add.
     * @param _value The value to add.
     * @return True if the member was added, false if the key already exists or if not an object.
     */
    template<typename _KeyType, typename _ValueType>
    FORCEINLINE Bool AddMember(_KeyType&& _key, _ValueType&& _value) {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetObject();
        }

        if (!json_value_ptr_->IsObject()) {
            ASSERT(false, "TJsonValue: AddMemberNoSameKeyCheckP() called on non-object type!");
            return false;
        }

        auto key = PrepareRapidJsonKeyP(std::forward<_KeyType>(_key));

        if (json_value_ptr_->HasMember(key)) {
            return false;
        }

        json_value_ptr_->AddMember(
            std::move(key),
            PrepareRapidJsonValueP(std::forward<_ValueType>(_value)),
            *json_allocator_ptr_
        );

        return true;
    }

    /**
     * @brief Adds a member to the object without checking for existing keys.
     * @tparam _KeyType The type of the key.
     * @tparam _ValueType The type of the value.
     * @param _key The key to add.
     * @param _value The value to add.
     * @return True if successful, false if not an object.
     */
    template<typename _KeyType, typename _ValueType>
    FORCEINLINE Bool AddMemberNoSameKeyCheck(_KeyType&& _key, _ValueType&& _value) {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetObject();
        }

        if (!json_value_ptr_->IsObject()) {
            ASSERT(false, "TJsonValue: AddMemberNoSameKeyCheckP() called on non-object type!");
            return false;
        }

        json_value_ptr_->AddMember(
            PrepareRapidJsonKeyP(std::forward<_KeyType>(_key)),
            PrepareRapidJsonValueP(std::forward<_ValueType>(_value)),
            *json_allocator_ptr_
        );

        return true;
    }

    /**
     * @brief Changes an existing member's value or adds it if it doesn't exist.
     * @tparam _KeyType The type of the key.
     * @tparam _ValueType The type of the value.
     * @param _key The key to change or add.
     * @param _value The new value.
     * @return True if successful, false if not an object.
     */
    template<typename _KeyType, typename _ValueType>
    FORCEINLINE Bool ChangeOrAddMember(_KeyType&& _key, _ValueType&& _value) {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetObject();
        }

        if (!json_value_ptr_->IsObject()) {
            ASSERT(false, "TJsonValue: ChangeOrAddMember() called on non-object type!");
            return false;
        }

        auto key = PrepareRapidJsonKeyP(std::forward<_KeyType>(_key));
        auto value = PrepareRapidJsonValueP(std::forward<_ValueType>(_value));
        auto member_iter = json_value_ptr_->FindMember(key);
        if (member_iter != json_value_ptr_->MemberEnd()) {
            member_iter->value = std::move(value);
        }
        else {
            json_value_ptr_->AddMember(
                std::move(key),
                std::move(value),
                *json_allocator_ptr_
            );
        }

        return true;
    }

    /**
     * @brief Removes a member by a static string key.
     * @param _key The static string key.
     * @return True if the member was found and removed, false otherwise.
     */
    FORCEINLINE Bool RemoveMember(ZStaticStringView _key) noexcept {
        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        return json_value_ptr_->RemoveMember(key_ref);
    }

    /**
     * @brief Removes a member by a string view key.
     * @param _key The string view key.
     * @return True if the member was found and removed, false otherwise.
     */
    FORCEINLINE Bool RemoveMember(ZStringView _key) noexcept {
        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        return json_value_ptr_->RemoveMember(key_ref);
    }

    /**
     * @brief Checks if a member exists by a static string key.
     * @param _key The static string key to check.
     * @return True if the member exists, false otherwise.
     */
    NODISCARD FORCEINLINE Bool HasMember(ZStaticStringView _key) const noexcept {
        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        return json_value_ptr_->HasMember(key_ref);
    }

    /**
     * @brief Checks if a member exists by a string view key.
     * @param _key The string view key to check.
     * @return True if the member exists, false otherwise.
     */
    NODISCARD FORCEINLINE Bool HasMember(ZStringView _key) const noexcept { 
        auto key_ref = rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize());
        return json_value_ptr_->HasMember(key_ref);
    }

    /**
     * @brief Resets the value to null.
     */
    FORCEINLINE Void Reset() noexcept { json_value_ptr_->SetNull(); }

    /**
     * @brief Clears the value, emptying arrays or objects.
     */
    FORCEINLINE Void Clear() noexcept { json_value_ptr_->Clear(); }

    /**
     * @brief Accesses or creates an array element by index.
     * 
     * If the value is not an array, it is converted to one. If the index is out of bounds,
     * the array is resized (filled with nulls) to accommodate the index.
     * 
     * @param _index The array index.
     * @return A TJsonValue wrapper for the element.
     */
    NODISCARD TJsonValue operator[](UInt32 _index) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            Z_DEBUG_LOG_MESSAGE("Trying to change a non-array JSON value to an array type!");
            json_value_ptr_->SetArray();
        }

        if (_index >= json_value_ptr_->Size()) {
            Z_DEBUG_LOG_MESSAGE(
                "operator[] index out of range, resizing array to fit! _index: {}, size: {}", 
                _index, json_value_ptr_->Size()
            );
            ASSERT(false, "TJsonValue: operator[] index out of range!");
            Resize(_index + 1U);
        }

        return TJsonValue{ &((*json_value_ptr_)[_index]), json_allocator_ptr_ };
    }

    /**
     * @brief Accesses an array element by index (const version).
     * 
     * Asserts if the value is not an array or if the index is out of bounds.
     * 
     * @param _index The array index.
     * @return A const TJsonValue wrapper for the element.
     */
    NODISCARD const TJsonValue operator[](UInt32 _index) const noexcept {
        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "TJsonValue: operator[] called on non-array type!");
            return EmptyJsonValueP();
        }

        if (_index >= json_value_ptr_->Size()) {
            ASSERT(false, "TJsonValue: operator[] index out of range!");
            return EmptyJsonValueP();
        }

        return TJsonValue{ const_cast<RapidJsonValue_*>(&(*json_value_ptr_)[_index]), json_allocator_ptr_ };
    }

    /**
     * @brief Gets the number of elements in the array.
     * @return The size of the array, or 0 if not an array.
     */
    NODISCARD FORCEINLINE UInt32 Size() const noexcept {
        return json_value_ptr_->IsArray() ? json_value_ptr_->Size() : 0ULL;
    }

    /**
     * @brief Checks if the array is empty.
     * @return True if empty or if not an array.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept {
        return json_value_ptr_->IsArray() ? json_value_ptr_->Empty() : true;
    }

    /**
     * @brief Gets the capacity of the array.
     * @return The capacity of the array, or 0 if not an array.
     */
    NODISCARD FORCEINLINE UInt32 Capacity() const noexcept {
        return json_value_ptr_->IsArray() ? json_value_ptr_->Capacity() : 0U;
    }

    /**
     * @brief Returns an iterator to the beginning of the array.
     * @return An iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept;

    /**
     * @brief Returns a const iterator to the beginning of the array.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept;

    /**
     * @brief Returns an iterator to the end of the array.
     * @return An iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept;

    /**
     * @brief Returns a const iterator to the end of the array.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept;

    /**
     * @brief Appends a value to the end of the array.
     * @tparam _ValueType The type of the value to append.
     * @param _value The value to append.
     * @return True if successful, false if not an array.
     */
    template<typename _ValueType>
    FORCEINLINE Bool PushBack(_ValueType&& _value) noexcept {
        using DecayType = std::decay_t<_ValueType>;

        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "Reserve called on non-array type!");
            return false;
        }

        json_value_ptr_->PushBack(PrepareRapidJsonValueP(std::forward<_ValueType>(_value)), *json_allocator_ptr_);
        return true;
    }

    /**
     * @brief Removes the last element of the array.
     * @return True if successful, false if not an array.
     */
    Bool PopBack() noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "Reserve called on non-array type!");
            return false;
        }

        if (!json_value_ptr_->Empty()) {
            json_value_ptr_->PopBack();
        }

        return true;
    }

    /**
     * @brief Removes an element at a specific index.
     * @param _index The index of the element to remove.
     * @return True if successful, false if not an array or if index is invalid.
     */
    Bool Erase(UInt32 _index) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "Reserve called on non-array type!");
            return false;
        }

        if (_index >= json_value_ptr_->Size()) {
            ASSERT(false, "TJsonValue: Remove index out of range!");
            return false;
        }

        json_value_ptr_->Erase(json_value_ptr_->Begin() + _index);

        return true;
    }

    /**
     * @brief Reserves memory for the array.
     * @param _capacity The capacity to reserve.
     * @return True if successful, false if not an array.
     */
    Bool Reserve(UInt32 _capacity) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "Reserve called on non-array type!");
            return false;
        }

        json_value_ptr_->Reserve(_capacity, *json_allocator_ptr_);
        return true;
    }

    /**
     * @brief Resizes the array.
     * @param _size The new size.
     * @return True if successful, false if not an array.
     */
    Bool Resize(UInt32 _size) noexcept {
        if (json_value_ptr_->IsNull()) {
            json_value_ptr_->SetArray();
        }

        if (!json_value_ptr_->IsArray()) {
            ASSERT(false, "Reserve called on non-array type!");
            return false;
        }

        SizeType current_size = json_value_ptr_->Size();

        if (_size == current_size) {
            return true;
        }

        if (_size < current_size) {
            SizeType remove_num = current_size - _size;
            for (SizeType count = 0; count < remove_num; ++count) {
                json_value_ptr_->PopBack();
            }
        }
        else {
            json_value_ptr_->Reserve(_size, *json_allocator_ptr_);
            SizeType add_num = _size - current_size;
            for (SizeType count = 0; count < add_num; ++count) {
                json_value_ptr_->PushBack(RapidJsonValue_(rapidjson::kNullType), *json_allocator_ptr_);
            }
        }
        return true;
    }

    /**
     * @brief Generates a JSON string representation without indentation.
     * @return A ZString containing the JSON string.
     */
    NODISCARD ZString GenerateJsonStringNoIndent() const noexcept {
        if (json_value_ptr_->IsNull()) {
            return ZString("");
        }

        // json to string
        JsonStringBuffer_ buffer;
        rapidjson::Writer<JsonStringBuffer_> writer(buffer);
        json_value_ptr_->Accept(writer);
        return ZString{ buffer.GetString(), buffer.GetSize() };
    }

    /**
     * @brief Generates a JSON string representation with indentation.
     * @return A ZString containing the formatted JSON string.
     */
    NODISCARD ZString GenerateJsonStringWithIndent() const noexcept {
        if (json_value_ptr_->IsNull()) {
            return ZString("");
        }

        // json to string
        JsonStringBuffer_ buffer;
        rapidjson::PrettyWriter<JsonStringBuffer_> writer(buffer);
        writer.SetIndent(' ', 4);
        json_value_ptr_->Accept(writer);
        return ZString{ buffer.GetString(), buffer.GetSize()};
    }

private:
    /**
     * @brief Default constructor.
     */
    FORCEINLINE TJsonValue() noexcept = default;

    /**
     * @brief Constructor using existing RapidJSON value and allocator.
     * @param _json_value Pointer to the RapidJSON value.
     * @param _json_allocator Pointer to the RapidJSON allocator.
     */
    FORCEINLINE TJsonValue(
        RapidJsonValue_* _json_value, 
        RapidJsonAllocator_* _json_allocator
    ) noexcept
        : json_value_ptr_(_json_value)
        , json_allocator_ptr_(_json_allocator)
    {}

    /**
     * @brief Move constructor.
     * @param _value The value to move.
     */
    FORCEINLINE TJsonValue(TJsonValue&& _value) noexcept
        : json_value_ptr_(_value.json_value_ptr_)
        , json_allocator_ptr_(_value.json_allocator_ptr_)
    {}

    /**
     * @brief Helper to prepare a RapidJSON key from various input types.
     * @tparam _KeyType The input key type.
     * @param _key The input key.
     * @return A RapidJsonValue_ representing the key.
     */
    template<typename _KeyType>
    NODISCARD FORCEINLINE RapidJsonValue_ PrepareRapidJsonKeyP(_KeyType&& _key) {
        using DecayType = std::decay_t<_KeyType>;

        if constexpr (kIsSameType<DecayType, ZStaticStringView>) {
            return RapidJsonValue_(rapidjson::StringRef(_key.GetDataPtr(), _key.GetSize()));
        }
        else if constexpr (kIsConvertibleType<DecayType, ZStringView>) {
            ZStringView key_string_view{ _key };
            return RapidJsonValue_(
                key_string_view.GetDataPtr(),
                static_cast<rapidjson::SizeType>(key_string_view.GetSize()),
                *json_allocator_ptr_
            );
        }
        else {
            STATIC_ASSERT((!kIsSameType<DecayType, DecayType>), "TJsonValue: Unsupported key type!");
            return RapidJsonValue_(rapidjson::kNullType);
        }
    }

    /**
     * @brief Helper to prepare a RapidJSON value from various input types.
     * @tparam _ValueType The input value type.
     * @param _value The input value.
     * @return A RapidJsonValue_ representing the value.
     */
    template<typename _ValueType>
    NODISCARD FORCEINLINE RapidJsonValue_ PrepareRapidJsonValueP(_ValueType&& _value) {
        using DecayType = std::decay_t<_ValueType>;

        if constexpr (kIsSameType<DecayType, ZStaticStringView>) {
            return RapidJsonValue_(rapidjson::StringRef(_value.GetDataPtr(), _value.GetSize()));
        }
        else if constexpr (kIsConvertibleType<DecayType, ZStringView>) {
            ZStringView value_string_view{ _value };
            return RapidJsonValue_(
                value_string_view.GetDataPtr(),
                static_cast<rapidjson::SizeType>(value_string_view.GetSize()),
                *json_allocator_ptr_
            );
        }
        else if constexpr (kIsSameType<DecayType, TJsonValue>) {
            if constexpr (kIsRightValue<_ValueType&&>) {
                return RapidJsonValue_(std::move(*_value.json_value_ptr_));
            }
            else {
                return RapidJsonValue_(*_value.json_value_ptr_, *json_allocator_ptr_);
            }
        }
        else {
            STATIC_ASSERT(
                (kIsSameType<DecayType, Bool>) || kIsNumber<DecayType>,
                "TJsonValue: Unsupported value type!"
            );
            return RapidJsonValue_(std::forward<_ValueType>(_value));
        }
    }

private:
    /** @brief Friend declaration for TJsonDocument to access protected members. */
    template<Bool _IsAlsoGlobal>
    friend class TJsonDocument;

    /** @brief Friend declaration for TJsonValueIterator. */
    template<Bool _IsAlsoGlobal>
    friend class TJsonValueIterator;

    /** @brief Friend declaration for TJsonValueConstIterator. */
    template<Bool _IsAlsoGlobal>
    friend class TJsonValueConstIterator;

private:
    /** @brief Pointer to the internal RapidJSON value. */
    RapidJsonValue_* json_value_ptr_;
    /** @brief Pointer to the allocator used by this value. */
    RapidJsonAllocator_* json_allocator_ptr_;
};

/**
 * @brief Iterator class for traversing JSON arrays.
 * 
 * This class wraps the RapidJSON iterator and provides an interface compatible with standard iterators,
 * returning TJsonValue wrappers upon dereferencing.
 * 
 * @tparam _IsGlobal If true, uses the global memory pool.
 */
template<Bool _IsGlobal>
class TJsonValueIterator {
    NO_NEW_DELETE
public:
    /** @brief Type alias for the RapidJSON allocator using the specified allocator. */
    using RapidJsonAllocator_ = RapidJsonAllocator<_IsGlobal>;
    /** @brief Type alias for the RapidJSON value using the specified allocator. */
    using RapidJsonValue_ = RapidJsonValue<_IsGlobal>;
    /** @brief Type alias for TJsonValue using the specified allocator. */
    using JsonValue_ = TJsonValue<_IsGlobal>;

public:
    /**
     * @brief Default constructor.
     */
    FORCEINLINE TJsonValueIterator() = default;

    /**
     * @brief Constructor using RapidJSON value and allocator pointers.
     * @param _json_value Pointer to the RapidJSON value.
     * @param _json_allocator Pointer to the RapidJSON allocator.
     */
    FORCEINLINE explicit TJsonValueIterator(
        RapidJsonValue_* _json_value,
        RapidJsonAllocator_* _json_allocator
    ) noexcept
        : json_value_(_json_value, _json_allocator)
    {}

    /**
     * @brief Dereference operator.
     * @return A reference to the current TJsonValue wrapper.
     */
    NODISCARD FORCEINLINE JsonValue_& operator*() const noexcept { return json_value_; }

    /**
     * @brief Member access operator.
     * @return A pointer to the current TJsonValue wrapper.
     */
    NODISCARD FORCEINLINE JsonValue_* operator->() const noexcept { return &json_value_; }

    /**
     * @brief Gets the pointer to the current TJsonValue wrapper.
     * @return A pointer to the TJsonValue.
     */
    NODISCARD FORCEINLINE JsonValue_* GetPtr() const noexcept { return &json_value_; }

    /**
     * @brief Pre-increment operator.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueIterator& operator++() noexcept {
        ++json_value_.json_value_ptr_;
        return *this;
    }

    /**
     * @brief Post-increment operator.
     * @return The iterator state before incrementing.
     */
    FORCEINLINE TJsonValueIterator operator++(int) noexcept {
        auto tmp = *this;
        ++json_value_.json_value_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueIterator& operator--() noexcept {
        --json_value_.json_value_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator.
     * @return The iterator state before decrementing.
     */
    FORCEINLINE TJsonValueIterator operator--(int) noexcept {
        auto tmp = *this;
        --json_value_.json_value_ptr_;
        return tmp;
    }

    /**
     * @brief Advance iterator by a given number of steps.
     * @param _num The number of steps to advance.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueIterator& operator+=(SizeType _num) noexcept {
        json_value_.json_value_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Retreat iterator by a given number of steps.
     * @param _num The number of steps to retreat.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueIterator& operator-=(SizeType _num) noexcept {
        json_value_.json_value_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Addition operator.
     * @param _num The number of steps to add.
     * @return A new iterator at the advanced position.
     */
    NODISCARD FORCEINLINE TJsonValueIterator operator+(SizeType _num) const noexcept {
        return TJsonValueIterator(json_value_.json_value_ptr_ + _num);
    }

    /**
     * @brief Subtraction operator.
     * @param _num The number of steps to subtract.
     * @return A new iterator at the retreated position.
     */
    NODISCARD FORCEINLINE TJsonValueIterator operator-(SizeType _num) const noexcept {
        return TJsonValueIterator(json_value_.json_value_ptr_ - _num);
    }

    /**
     * @brief Three-way comparison operator.
     * @param _other The iterator to compare with.
     * @return The result of the comparison.
     */
    NODISCARD FORCEINLINE auto operator<=>(const TJsonValueIterator&) const = default;

protected:
    /** @brief Friend declaration for TJsonValueConstIterator. */
    template<Bool _IsAlsoGlobal>
    friend class TJsonValueConstIterator;

private:
    /** @brief Mutable proxy value used for dereferencing. */
    mutable JsonValue_ json_value_;
};

/**
 * @brief Constant iterator class for traversing JSON arrays.
 * 
 * This class wraps the RapidJSON iterator for read-only access, returning const TJsonValue wrappers.
 * 
 * @tparam _IsGlobal If true, uses the global memory pool.
 */
template<Bool _IsGlobal>
class TJsonValueConstIterator {
    NO_NEW_DELETE
public:
    /** @brief Type alias for the RapidJSON allocator using the specified allocator. */
    using RapidJsonAllocator_ = RapidJsonAllocator<_IsGlobal>;
    /** @brief Type alias for the RapidJSON value using the specified allocator. */
    using RapidJsonValue_ = RapidJsonValue<_IsGlobal>;
    /** @brief Type alias for TJsonValue using the specified allocator. */
    using JsonValue_ = TJsonValue<_IsGlobal>;

public:
    /**
     * @brief Default constructor.
     */
    FORCEINLINE TJsonValueConstIterator() = default;

    /**
     * @brief Constructor using RapidJSON value and allocator pointers.
     * @param _json_value Pointer to the RapidJSON value.
     * @param _json_allocator Pointer to the RapidJSON allocator.
     */
    FORCEINLINE explicit TJsonValueConstIterator(
        RapidJsonValue_* _json_value,
        RapidJsonAllocator_* _json_allocator
    ) noexcept
        : json_value_(_json_value, _json_allocator)
    {}

    /**
     * @brief Constructor from a non-const iterator.
     * @param _it The non-const iterator.
     */
    FORCEINLINE TJsonValueConstIterator(const TJsonValueIterator<_IsGlobal>& _it) noexcept
        : json_value_(_it.json_value_)
    {}

    /**
     * @brief Dereference operator.
     * @return A const reference to the current TJsonValue wrapper.
     */
    NODISCARD FORCEINLINE const JsonValue_& operator*() const noexcept { return json_value_; }

    /**
     * @brief Member access operator.
     * @return A const pointer to the current TJsonValue wrapper.
     */
    NODISCARD FORCEINLINE const JsonValue_* operator->() const noexcept { return &json_value_; }

    /**
     * @brief Gets the const pointer to the current TJsonValue wrapper.
     * @return A const pointer to the TJsonValue.
     */
    NODISCARD FORCEINLINE const JsonValue_* GetPtr() const noexcept { return &json_value_; }

    /**
     * @brief Pre-increment operator.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueConstIterator& operator++() noexcept {
        ++json_value_.json_value_ptr_;
        return *this;
    }

    /**
     * @brief Post-increment operator.
     * @return The iterator state before incrementing.
     */
    FORCEINLINE TJsonValueConstIterator operator++(int) noexcept {
        auto tmp = *this;
        ++json_value_.json_value_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueConstIterator& operator--() noexcept {
        --json_value_.json_value_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator.
     * @return The iterator state before decrementing.
     */
    FORCEINLINE TJsonValueConstIterator operator--(int) noexcept {
        auto tmp = *this;
        --json_value_.json_value_ptr_;
        return tmp;
    }

    /**
     * @brief Advance iterator by a given number of steps.
     * @param _num The number of steps to advance.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueConstIterator& operator+=(SizeType _num) noexcept {
        json_value_.json_value_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Retreat iterator by a given number of steps.
     * @param _num The number of steps to retreat.
     * @return A reference to the updated iterator.
     */
    FORCEINLINE TJsonValueConstIterator& operator-=(SizeType _num) noexcept {
        json_value_.json_value_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Addition operator.
     * @param _num The number of steps to add.
     * @return A new iterator at the advanced position.
     */
    NODISCARD FORCEINLINE TJsonValueConstIterator operator+(SizeType _num) const noexcept {
        return TJsonValueConstIterator(json_value_.json_value_ptr_ + _num);
    }

    /**
     * @brief Subtraction operator.
     * @param _num The number of steps to subtract.
     * @return A new iterator at the retreated position.
     */
    NODISCARD FORCEINLINE TJsonValueConstIterator operator-(SizeType _num) const noexcept {
        return TJsonValueConstIterator(json_value_.json_value_ptr_ - _num);
    }

    /**
     * @brief Three-way comparison operator.
     * @param _other The iterator to compare with.
     * @return The result of the comparison.
     */
    NODISCARD FORCEINLINE auto operator<=>(const TJsonValueConstIterator&) const = default;

private:
    /** @brief Proxy value used for dereferencing. */
    JsonValue_ json_value_;
};

/**
 * @brief Gets an iterator to the beginning of the array.
 * @return An iterator to the first element.
 */
template<Bool _IsGlobal>
FORCEINLINE typename TJsonValue<_IsGlobal>::Iterator_ TJsonValue<_IsGlobal>::Begin() noexcept {
    ASSERT(IsArray(), "Begin() called on non-array type!");
    return Iterator_(json_value_ptr_->Begin(), json_allocator_ptr_);
}

/**
 * @brief Gets a const iterator to the beginning of the array.
 * @return A const iterator to the first element.
 */
template<Bool _IsGlobal>
FORCEINLINE typename TJsonValue<_IsGlobal>::ConstIterator_ TJsonValue<_IsGlobal>::Begin() const noexcept {
    ASSERT(IsArray(), "Begin() called on non-array type!");
    return ConstIterator_(json_value_ptr_->Begin(), json_allocator_ptr_);
}

/**
 * @brief Gets an iterator to the end of the array.
 * @return An iterator to the element past the end of the array.
 */
template<Bool _IsGlobal>
FORCEINLINE typename TJsonValue<_IsGlobal>::Iterator_ TJsonValue<_IsGlobal>::End() noexcept {
    ASSERT(IsArray(), "End() called on non-array type!");
    return Iterator_(json_value_ptr_->End(), json_allocator_ptr_);
}

/**
 * @brief Gets a const iterator to the end of the array.
 * @return A const iterator to the element past the end of the array.
 */
template<Bool _IsGlobal>
FORCEINLINE typename TJsonValue<_IsGlobal>::ConstIterator_ TJsonValue<_IsGlobal>::End() const noexcept {
    ASSERT(IsArray(), "End() called on non-array type!");
    return ConstIterator_(json_value_ptr_->End(), json_allocator_ptr_);
}

/**
 * @brief Represents a JSON document.
 * 
 * This class inherits from TJsonValue and represents the root of a JSON DOM tree.
 * It owns the memory allocator and the root value. It provides functionality to parse JSON strings
 * and read/write JSON files.
 * 
 * @tparam _IsGlobal If true, uses the global memory pool.
 */
template<Bool _IsGlobal>
class TJsonDocument final : public TJsonValue<_IsGlobal> {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = TJsonValue<_IsGlobal>;

    /** @brief Indicates that this class has specific override behaviors. */
    static inline constexpr Bool kDefaultOverrideExist = true;

private:
    /** @brief Type alias for the RapidJSON string buffer using the specified allocator. */
    using JsonStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, TJsonAllocatorP<_IsGlobal>>;
    /** @brief Type alias for the RapidJSON document using the specified allocator. */
    using RapidJsonDocument_ = RapidJsonDocument<_IsGlobal>;
    /** @brief Type alias for the RapidJSON value using the specified allocator. */
    using RapidJsonValue_ = RapidJsonValue<_IsGlobal>;

public:
    /**
     * @brief Default constructor. Initializes an empty JSON object.
     */
    FORCEINLINE TJsonDocument() noexcept
        : json_doc_()
    {
        json_doc_.SetObject();
        SuperType_::json_value_ptr_ = &json_doc_;
        SuperType_::json_allocator_ptr_ = &json_doc_.GetAllocator();
    }

    /** @brief Deleted copy constructor. */
    TJsonDocument(const TJsonDocument&) = delete;

    /**
     * @brief Move constructor.
     * @param _doc The source document to move.
     */
    FORCEINLINE TJsonDocument(TJsonDocument&& _doc) noexcept
        : SuperType_(std::move(_doc))
        , json_doc_(std::move(_doc.json_doc_))
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TJsonDocument() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    TJsonDocument& operator=(const TJsonDocument&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _doc The source document to move.
     * @return A reference to this document.
     */
    FORCEINLINE TJsonDocument& operator=(TJsonDocument&& _doc) noexcept {
        json_doc_ = std::move(_doc.json_doc_);
        SuperType_::json_value_ptr_ = &json_doc_;
        SuperType_::json_allocator_ptr_ = &json_doc_.GetAllocator();
        return *this;
    }

    /**
     * @brief Parses a JSON string.
     * @param _parse_str The string containing JSON data.
     * @return kOK on success, or kZJsonErrorCode_JsonParseError on failure.
     */
    NODISCARD ReturnType Parse(ZStringView _parse_str) noexcept {
        ReturnType ret_val = kOK;

        json_doc_.Parse(_parse_str.GetDataPtr(), _parse_str.GetSize());

        if (json_doc_.HasParseError()) {
            ret_val = error_code::kZJsonErrorCode_JsonParseError;
            Z_LOG_ERROR(ret_val, 0, "Json parse error! pos: %d", json_doc_.GetErrorOffset());
            return ret_val;
        }

        return ret_val;
    }

    /**
     * @brief Reads and parses JSON data from a file.
     * @param _file_path The file path to read from.
     * @return kOK on success, or an error code (e.g., LinkError, JsonParseError) on failure.
     */
    NODISCARD ReturnType ReadFile(const Char* _file_path) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;

        // open file
        link_code = file.Open(_file_path, ZFile::kReadOnly);
        if (link_code != kOK) {
            ret_val = error_code::kZJsonErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
            return ret_val;
        }

        // read json string from the file
        Int32 json_str_size = static_cast<Int32>(file.ComputeSize());
        ZMemory json_raw_str(json_str_size + 1);
        link_code = file.Read(ZBuffer(json_raw_str.GetDataPtr<Void>(), json_str_size), json_str_size);
        if (link_code != kOK) {
            ret_val = error_code::kZJsonErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::Read() link error!");
            return ret_val;
        }

        // close file
        file.Close();

        json_raw_str.GetDataPtr<Char>()[json_str_size] = '\0';

        // parse the string
        link_code = Parse(json_raw_str.GetDataPtr<Char>());
        if (link_code != kOK) {
            ret_val = error_code::kZJsonErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZJsonDocument::Parse() link error!");
            return ret_val;
        }

        return ret_val;
    }

    /**
     * @brief Writes the JSON document to a file.
     * @param _file_path The file path to write to.
     * @param _with_indent If true, writes formatted (pretty) JSON. If false, writes compact JSON.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType WriteFile(const Char* _file_path, Bool _with_indent = true) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;

        // open file
        link_code = file.OpenSafe(_file_path, ZFile::kWriteOnly | ZFile::kCreate | ZFile::kTruncate);
        if (link_code != kOK) {
            ret_val = error_code::kZJsonErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
            return ret_val;
        }

        // json to string        
        ZString json_str = 
            _with_indent ? SuperType_::GenerateJsonStringWithIndent() : SuperType_::GenerateJsonStringNoIndent();

        // write string to file
        link_code = file.Write(ZConstBuffer(json_str.GetDataPtr(), json_str.GetSize()));
        if (link_code != kOK) {
            ret_val = error_code::kZJsonErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::Write() link error!");
            return ret_val;
        }

        // close file
        file.Close();

        return ret_val;
    }

private:
    /** @brief Friend declaration for ZJsonValue. */
    friend class ZJsonValue;

private:
    /** @brief The internal RapidJSON document instance. */
    mutable RapidJsonDocument_ json_doc_;
};

} // internal
} // zengine

namespace zengine {

using ZJsonDocument = internal::TJsonDocument<true>;
using ZJsonDocumentLocal = internal::TJsonDocument<false>;
using ZJsonValue = internal::TJsonValue<true>;
using ZJsonValueLocal = internal::TJsonValue<false>;

} // zengine