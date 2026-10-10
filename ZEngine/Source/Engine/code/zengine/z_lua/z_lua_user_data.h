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

#include "../z_core/z_string_view.h"

#include "internal/l_sol.h"
#include "z_lua_object.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for Lua userdatas, providing safe type access and manipulation.
 *
 * Implementation Principle:
 * This class inherits from ZObject and acts as a specialized view for Lua userdatas. It internally holds a sol::userdata
 * object. To ensure safety in a no-exception environment, constructors asserting validity when converting from generic
 * ZLuaObjects. The Get() methods utilize sol::optional to detect type mismatches or missing keys without triggering
 * Lua errors or C++ exceptions, returning default values instead. This abstraction unifies the interface and isolates
 * direct dependency on the sol2 library's userdata implementation.
 */
class LUA_DLLAPI ZLuaUserData : public ZObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;

public:
    /** @brief Default constructor. */
    FORCEINLINE ZLuaUserData() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source object to copy from.
     */
    FORCEINLINE ZLuaUserData(const ZLuaUserData&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param other The source object to move from.
     */
    FORCEINLINE ZLuaUserData(ZLuaUserData&&) noexcept = default;

    /**
     * @brief Constructs a ZLuaUserData from a raw sol::userdata reference.
     * @param _user_data The sol::userdata to wrap.
     */
    FORCEINLINE ZLuaUserData(const sol::userdata& _user_data) noexcept
        : SuperType_()
        , user_data_(_user_data)
    {}

    /**
     * @brief Move-constructs a ZLuaUserData from a raw sol::userdata.
     * @param _user_data The sol::userdata to move.
     */
    FORCEINLINE ZLuaUserData(sol::userdata&& _user_data) noexcept
        : SuperType_()
        , user_data_(std::move(_user_data))
    {}

    /**
     * @brief Constructs a ZLuaUserData from a generic ZLuaObject.
     * @param _obj The source ZLuaObject.
     * @note This constructor asserts that the source object is valid and is indeed a userdata.
     */
    FORCEINLINE explicit ZLuaUserData(const ZLuaObject& _obj) noexcept 
        : SuperType_() 
        , user_data_(_obj.obj_)
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::userdata>(), "ZLuaObject is not a userdata!");
    }

    /**
     * @brief Move-constructs a ZLuaUserData from a generic ZLuaObject.
     * @param _obj The source ZLuaObject.
     * @note This constructor asserts that the source object is valid and is indeed a userdata.
     */
    FORCEINLINE explicit ZLuaUserData(ZLuaObject&& _obj) noexcept 
        : SuperType_() 
        , user_data_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::userdata>(), "ZLuaObject is not a userdata!");
        user_data_ = std::move(_obj.obj_);
    }

    /** @brief Destructor. */
    FORCEINLINE ~ZLuaUserData() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaUserData& operator=(const ZLuaUserData&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaUserData& operator=(ZLuaUserData&&) noexcept = default;

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the object is valid, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return user_data_.valid();
    }

    /**
     * @brief Checks if the userdata is of a specific C++ type.
     * @tparam _ObjectType The C++ type to check against.
     * @return True if the userdata holds the specified type, false otherwise.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool Is() const noexcept {
        return user_data_.is<_ObjectType>();
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return user_data_.lua_state();
    }

    /**
     * @brief Retrieves a pointer to the underlying C++ object data.
     * @tparam _ObjectType The C++ type of the underlying data.
     * @return A pointer to the data if the type matches, otherwise nullptr or undefined depending on implementation.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() const noexcept {
        return user_data_.as<_ObjectType*>();
    }

    /**
     * @brief Converts the userdata content to a string representation.
     * @return A string describing the userdata.
     */
    NODISCARD ZString ToString() const noexcept;

private:
    /** @brief The underlying sol::userdata object. */
    sol::userdata user_data_;
};

} // lua
} // zengine

// ZLuaUserData to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaUserData> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaUserData& _user_data, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_user_data.ToString().STDString(), _context);
    }
};