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

#include "../drive.h"

#include "../../z_core/m_log.h"

#ifndef SOL_NO_EXCEPTIONS
#define SOL_NO_EXCEPTIONS 1
#endif

#include "sol/sol.hpp"

namespace zengine {
namespace lua {

/**
 * @brief Enumeration representing Lua data types.
 */
enum class LuaTypeEnum : Int32 {
    /** @brief No value. */
    kNone = sol::type::none,
    /** @brief Lua nil value. */
    kNil = sol::type::lua_nil,
    /** @brief Boolean value. */
    kBoolean = sol::type::boolean,
    /** @brief Light userdata (pointer). */
    kLightUserdata = sol::type::lightuserdata,
    /** @brief Number (integer or float). */
    kNumber = sol::type::number,
    /** @brief String value. */
    kString = sol::type::string,
    /** @brief Lua table. */
    kTable = sol::type::table,
    /** @brief Lua function. */
    kFunction = sol::type::function,
    /** @brief Userdata (C++ object). */
    kUserdata = sol::type::userdata,
    /** @brief Lua thread (coroutine). */
    kThread = sol::type::thread
};

/**
 * @brief Enumeration representing Lua metamethods used for operator overloading and behavior customization.
 */
enum class MetaFunctionEnum : Int32 {
    /** @brief Corresponds to object construction. */
    kConstruct = sol::meta_function::construct,
    /** @brief Corresponds to the __index metamethod (table access). */
    kIndex = sol::meta_function::index,
    /** @brief Corresponds to the __newindex metamethod (table assignment). */
    kNewIndex = sol::meta_function::new_index,
    /** @brief Corresponds to the __mode metamethod (weak tables). */
    kMode = sol::meta_function::mode,
    /** @brief Corresponds to the __call metamethod (function call). */
    kCall = sol::meta_function::call,
    /** @brief Corresponds to the __metatable metamethod (metatable protection). */
    kMetatable = sol::meta_function::metatable,
    /** @brief Corresponds to the __tostring metamethod (string conversion). */
    kToString = sol::meta_function::to_string,
    /** @brief Corresponds to the __len metamethod (length operator #). */
    kLength = sol::meta_function::length,
    /** @brief Corresponds to the __unm metamethod (unary minus). */
    kUnaryMinus = sol::meta_function::unary_minus,
    /** @brief Corresponds to the __add metamethod (addition +). */
    kAddition = sol::meta_function::addition,
    /** @brief Corresponds to the __sub metamethod (subtraction -). */
    kSubtraction = sol::meta_function::subtraction,
    /** @brief Corresponds to the __mul metamethod (multiplication *). */
    kMultiplication = sol::meta_function::multiplication,
    /** @brief Corresponds to the __div metamethod (division /). */
    kDivision = sol::meta_function::division,
    /** @brief Corresponds to the __mod metamethod (modulo %). */
    kModulus = sol::meta_function::modulus,
    /** @brief Corresponds to the __pow metamethod (exponentiation ^). */
    kPowerOf = sol::meta_function::power_of,
    /** @brief Corresponds to the __concat metamethod (concatenation ..). */
    kConcatenation = sol::meta_function::concatenation,
    /** @brief Corresponds to the __eq metamethod (equality ==). */
    kEqualTo = sol::meta_function::equal_to,
    /** @brief Corresponds to the __lt metamethod (less than <). */
    kLessThan = sol::meta_function::less_than,
    /** @brief Corresponds to the __le metamethod (less than or equal to <=). */
    kLessThanOrEqualTo = sol::meta_function::less_than_or_equal_to,
    /** @brief Corresponds to the __gc metamethod (garbage collection). */
    kGarbageCollect = sol::meta_function::garbage_collect,
};

/**
 * @brief Type alias for the raw Lua state structure.
 */
using RawLuaState = lua_State;

/**
 * @brief Type alias for sol::usertype, binding a C++ class to Lua.
 * @details This implementation relies on the Sol2 library's template metaprogramming capabilities to create
 *          bindings between C++ classes and Lua userdata. It automatically generates metatables containing
 *          member functions, variables, and properties defined by the user, handling type safety and conversion.
 * @tparam _ClassType The C++ class type being exposed to the Lua environment.
 */
template<typename _ClassType>
using LuaUserType = sol::usertype<_ClassType>;

} // lua
} // zengine