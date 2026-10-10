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

#include "../z_core/z_string.h"
#include "../z_core/z_string_view.h"

#include "internal/l_sol.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for Lua objects, encapsulating sol::object implementation.
 *
 * Implementation Principle:
 * This class serves as a bridge between the C++ object system (ZObject) and Lua's dynamic typing.
 * It holds a `sol::object` internally to manage the Lua reference count and provide type-safe access.
 * The implementation leverages `sol2` for low-level stack operations while exposing a unified ZEngine interface.
 * It provides specialized template handling to map ZEngine string types (ZString, ZStringView) to
 * Lua strings (via std::string_view) transparently.
 */
class LUA_DLLAPI ZLuaObject : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor. Creates an invalid (nil) Lua object.
     */
    FORCEINLINE ZLuaObject() noexcept = default;

    /**
     * @brief Copy constructor. Copies the underlying Lua reference.
     * @param other The source object to copy from.
     */
    FORCEINLINE ZLuaObject(const ZLuaObject&) noexcept = default;

    /**
     * @brief Move constructor. Moves the underlying Lua reference.
     * @param other The source object to move from.
     */
    FORCEINLINE ZLuaObject(ZLuaObject&&) noexcept = default;

    /**
     * @brief Constructor from a sol::object (copy).
     * @param _obj The source sol::object to copy.
     */
    FORCEINLINE ZLuaObject(const sol::object& _obj) noexcept
        : SuperType_()
        , obj_(_obj)
    {}

    /**
     * @brief Constructor from a sol::object (move).
     * @param _obj The source sol::object to move.
     */
    FORCEINLINE ZLuaObject(sol::object&& _obj) noexcept
        : SuperType_()
        , obj_(std::move(_obj))
    {}

    /**
     * @brief Destructor. Releases the Lua reference.
     */
    FORCEINLINE ~ZLuaObject() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaObject& operator=(const ZLuaObject&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaObject& operator=(ZLuaObject&&) noexcept = default;

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the object is not null, false otherwise.
     */
    NODISCARD FORCEINLINE explicit operator Bool() const noexcept {
        return !IsNull();
    }

    /**
     * @brief Equality comparison operator.
     * @param _obj The other ZLuaObject to compare with.
     * @return True if both objects refer to the same Lua object, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const ZLuaObject& _obj) const noexcept {
        return obj_ == _obj.obj_;
    }

    /**
     * @brief Inequality comparison operator.
     * @param _obj The other ZLuaObject to compare with.
     * @return True if objects refer to different Lua objects, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const ZLuaObject& _obj) const noexcept {
        return obj_ != _obj.obj_;
    }

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the object is valid, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return obj_.valid();
    }

    /**
     * @brief Checks if the object is null or nil.
     * @return True if the object is invalid or is explicitly Lua nil.
     */
    NODISCARD FORCEINLINE Bool IsNull() const noexcept {
        return !obj_.valid() || obj_.get_type() == sol::type::lua_nil;
    }

    /**
     * @brief Checks if the object is of a specific type.
     * @tparam _ObjectType The C++ type to check against.
     * @return True if the object can be cast to _ObjectType, false otherwise.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool Is() const noexcept {
        if constexpr (kIsSameType<_ObjectType, ZString>) {
            return obj_.is<std::string_view>();
        }
        else if constexpr (kIsSameType<_ObjectType, ZStringView>) {
            return obj_.is<std::string_view>();
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaTable>) {
            return obj_.is<sol::table>();
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaFunction>) {
            return obj_.is<sol::protected_function>();
        }
        else {
            return obj_.is<_ObjectType>();
        }
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return obj_.lua_state();
    }

    /**
     * @brief Gets the Lua type of the stored object.
     * @return The LuaTypeEnum enumeration value corresponding to the object's type.
     */
    NODISCARD FORCEINLINE LuaTypeEnum GetType() const noexcept {
        return static_cast<LuaTypeEnum>(obj_.get_type());
    }

    /**
     * @brief Casts the object to a specific type without safety checks.
     * @tparam _ObjectType The target C++ type.
     * @return The object converted to _ObjectType.
     * @warning If the type does not match and exceptions are disabled, this may cause a crash.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Cast() const noexcept {
        if constexpr (kIsSameType<_ObjectType, ZString>) {
            std::string_view std_string_view = obj_.as<std::string_view>();
            return ZString(std_string_view.data(), std_string_view.length());
        }
        else if constexpr (kIsSameType<_ObjectType, ZStringView>) {
            return ZStringView(obj_.as<std::string_view>());
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaTable>) {
            return ZLuaTable(obj_.as<sol::table>());
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaFunction>) {
            return ZLuaFunction(obj_.as<sol::protected_function>());
        }
        else {
            return obj_.as<_ObjectType>();
        }
    }

    /**
     * @brief Attempts to safely cast the object to a pointer of a specific type.
     * @tparam _ObjectType The target C++ type.
     * @param _obj Ref to store the result if the cast is successful.
     * @return True if the cast was successful, false if the type mismatched or pointer was null.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool TryCast(_ObjectType& _obj) const noexcept {
        if ((!Is<_ObjectType>())) {
            return false;
        }

        if constexpr (kIsSameType<_ObjectType, ZString>) {
            std::string_view std_string_view = obj_.as<std::string_view>();
            _obj.Assign(std_string_view.data(), std_string_view.length());
        }
        else if constexpr (kIsSameType<_ObjectType, ZStringView>) {
            _obj = obj_.as<std::string_view>();
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaTable>) {
            _obj = ZLuaTable(obj_.as<sol::table>());
        }
        else if constexpr (kIsSameType<_ObjectType, ZLuaFunction>) {
            _obj = ZLuaFunction(obj_.as<sol::protected_function>());
        }
        else {
            _obj = obj_.as<_ObjectType>();
        }
        return true;
    }

    /**
     * @brief Converts the Lua object to a string representation.
     * @return A ZString containing the string representation (e.g., via __tostring).
     */
    NODISCARD ZString ToString() const noexcept;

private:
    friend class ZLuaTable;
    friend class ZLuaFunction;
    friend class ZLuaThread;
    friend class ZLuaUserData;
    friend class ZLuaCoroutine;

private:
    /** @brief The internal sol::object wrapper. */
    sol::object obj_;
};

} // lua
} // zengine

// ZLuaObject to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaObject> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaObject& _obj, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_obj.ToString().STDString(), _context);
    }
};