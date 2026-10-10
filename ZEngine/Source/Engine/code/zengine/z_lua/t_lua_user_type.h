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
 * @brief Wrapper class for binding C++ classes to Lua using Sol2.
 * @details This class wraps `sol::usertype` to provide a unified interface for registering C++ types, methods, 
 *          properties, and metamethods in the Lua environment. It leverages Sol2's template metaprogramming 
 *          implementation to handle type traits, inheritance, and object lifetime management efficiently.
 * @tparam _ClassType The C++ class type to be bound to Lua.
 */
template<typename _ClassType>
class TLuaUserType {
    NO_NEW_DELETE
private:
    /** @brief Type alias for the underlying sol::usertype. */
    using UserType_ = sol::usertype<_ClassType>;

public:
    /**
     * @brief Move-constructs a TLuaUserType from a raw sol::usertype.
     * @param _user_type The sol::usertype to move.
     */
    FORCEINLINE TLuaUserType(UserType_&& _user_type) noexcept
        : user_type_(std::move(_user_type))
    {}

    /** @brief Deleted copy constructor. */
    TLuaUserType(const TLuaUserType&) = delete;

    /**
     * @brief Move constructor.
     * @param other The source object to move from.
     */
    FORCEINLINE TLuaUserType(TLuaUserType&&) noexcept = default;


    /** @brief Destructor. */
    FORCEINLINE ~TLuaUserType() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    TLuaUserType& operator=(const TLuaUserType&) = delete;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE TLuaUserType& operator=(TLuaUserType&&) noexcept = default;

    /**
     * @brief Binds a member function or variable to the Lua type.
     * @tparam _MemberType The type of the member.
     * @param _key The name of the member in Lua.
     * @param _member The member to bind (function pointer or variable).
     * @return Void.
     */
    template<typename _MemberType>
    FORCEINLINE Void Bind(ZStringView _key, _MemberType&& _member) noexcept {
        user_type_[_key.GetSTDStringView()] = std::forward<_MemberType>(_member);
    }

    /**
     * @brief Binds a set of overloaded functions to a single key in Lua.
     * @tparam _OverloadFunctions Variadic template for the overloaded function types.
     * @param _key The name of the function in Lua.
     * @param _funcs The overloaded functions to bind.
     * @return Void.
     */
    template<typename... _OverloadFunctions>
    FORCEINLINE Void BindOverload(ZStringView _key, _OverloadFunctions&&... _funcs) noexcept {
        user_type_[_key.GetSTDStringView()] = sol::overload(std::forward<_OverloadFunctions>(_funcs)...);
    }

    /**
     * @brief Binds a member variable as read-only in Lua.
     * @tparam _MemberType The type of the member variable.
     * @param _key The name of the member in Lua.
     * @param _member The member variable to bind.
     * @return Void.
     */
    template<typename _MemberType>
    FORCEINLINE Void BindReadOnly(ZStringView _key, _MemberType&& _member) noexcept {
        user_type_[_key.GetSTDStringView()] = sol::readonly(std::forward<_MemberType>(_member));
    }

    /**
     * @brief Binds a property with both getter and setter functions.
     * @tparam _GetterFunction The type of the getter function.
     * @tparam _SetterFunction The type of the setter function.
     * @param _key The name of the property in Lua.
     * @param _get_func The function to call when getting the value.
     * @param _set_func The function to call when setting the value.
     * @return Void.
     */
    template<typename _GetterFunction, typename _SetterFunction>
    FORCEINLINE Void BindProperty(ZStringView _key, _GetterFunction&& _get_func, _SetterFunction&& _set_func) noexcept {
        user_type_[_key.GetSTDStringView()] = sol::property(
            std::forward<_GetterFunction>(_get_func),
            std::forward<_SetterFunction>(_set_func)
        );
    }

    /**
     * @brief Binds a read-only property with a getter function.
     * @tparam _GetterFunction The type of the getter function.
     * @param _key The name of the property in Lua.
     * @param _get_func The function to call when getting the value.
     * @return Void.
     */
    template<typename _GetterFunction>
    FORCEINLINE Void BindProperty(ZStringView _key, _GetterFunction&& _get_func) noexcept {
        user_type_[_key.GetSTDStringView()] = sol::property(std::forward<_GetterFunction>(_get_func));
    }

    /**
     * @brief Binds a metamethod (e.g., __add, __call) to the Lua type.
     * @tparam _MetaFunction The type of the metamethod function.
     * @param _meta_type The enum value indicating which metamethod to bind.
     * @param _func The function implementing the metamethod logic.
     * @return Void.
     */
    template<typename _MetaFunction>
    FORCEINLINE Void BindMetaFunction(MetaFunctionEnum _meta_type, _MetaFunction&& _func) noexcept {
        user_type_[_meta_type] = std::forward<_MetaFunction>(_func);
    }

    /**
     * @brief Helper function to bind the __tostring metamethod.
     * @tparam _ToStringFunction The type of the to-string function.
     * @param _func The function that converts the object to a string.
     * @return Void.
     */
    template<typename _ToStringFunction>
    FORCEINLINE Void BindToString(_ToStringFunction&& _func) noexcept {
        user_type_[sol::meta_function::to_string] = std::forward<_ToStringFunction>(_func);
    }

    /**
     * @brief Binds constructors for the C++ class.
     * @tparam _ConstructFunctions Variadic template arguments representing the constructor signatures.
     * @return Void.
     */
    template<typename... _ConstructFunctions>
    FORCEINLINE Void BindConstructors() noexcept {
        user_type_[sol::meta_function::construct] = sol::constructors<_ConstructFunctions...>();
    }

    /**
     * @brief Disables the ability to construct this type from Lua.
     * @return Void.
     */
    FORCEINLINE Void DisableConstructors() noexcept {
        user_type_[sol::meta_function::construct] = sol::no_constructor;
    }

private:
    /** @brief The underlying Sol2 usertype object. */
    UserType_ user_type_;
};

} // lua
} // zengine