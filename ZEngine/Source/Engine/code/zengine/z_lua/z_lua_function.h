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
#include "z_lua_result.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for Lua functions, inheriting from ZObject.
 *
 * Implementation Principle:
 * This class encapsulates `sol::protected_function` to provide a safe mechanism for invoking Lua functions
 * from C++. Unlike raw Lua calls or standard `sol::function`, this wrapper handles Lua runtime errors
 * gracefully by returning a `ZLuaResult` object instead of throwing C++ exceptions. It ensures type safety
 * and validity checks during construction from generic `ZLuaObject`s, acting as a specialized interface
 * for callable Lua objects.
 */
class LUA_DLLAPI ZLuaFunction : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor.
     * Initializes an empty, invalid Lua function object.
     */
    FORCEINLINE ZLuaFunction() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source ZLuaFunction object to copy from.
     */
    FORCEINLINE ZLuaFunction(const ZLuaFunction&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param other The source ZLuaFunction object to move from.
     */
    FORCEINLINE ZLuaFunction(ZLuaFunction&&) noexcept = default;

    /**
     * @brief Constructs a ZLuaFunction from a raw sol::protected_function.
     * @param _func The sol protected function object to wrap.
     */
    FORCEINLINE ZLuaFunction(const sol::protected_function& _func) noexcept
        : SuperType_()
        , func_(_func)
    {}

    /**
     * @brief Move-constructs a ZLuaFunction from a raw sol::protected_function.
     * @param _func The sol protected function object to move from.
     */
    FORCEINLINE ZLuaFunction(sol::protected_function&& _func) noexcept
        : SuperType_()
        , func_(std::move(_func))
    {}

    /**
     * @brief Constructs a ZLuaFunction from a generic ZLuaObject.
     * @param _obj The source ZLuaObject to copy from.
     * @note This constructor asserts that the source object is valid and is indeed a function type.
     */
    FORCEINLINE explicit ZLuaFunction(const ZLuaObject& _obj) noexcept
        : SuperType_()
        , func_(_obj.obj_)
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::function>(), "ZLuaObject is not a function!");
    }

    /**
     * @brief Move-constructs a ZLuaFunction from a generic ZLuaObject.
     * @param _obj The source ZLuaObject to move from.
     * @note This constructor asserts that the source object is valid and is indeed a function type.
     */
    FORCEINLINE explicit ZLuaFunction(ZLuaObject&& _obj) noexcept
        : SuperType_()
        , func_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::function>(), "ZLuaObject is not a function!");
        func_ = std::move(_obj.obj_);
    }

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~ZLuaFunction() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaFunction& operator=(const ZLuaFunction&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaFunction& operator=(ZLuaFunction&&) noexcept = default;

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the underlying Lua function is valid, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return func_.valid();
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return func_.lua_state();
    }

    /**
     * @brief Invokes the Lua function using the function call operator.
     * @tparam ArgsType The types of the arguments passed to the function.
     * @param args The arguments to pass to the Lua function.
     * @return A ZLuaResult containing the result of the function call or error information.
     */
    template <typename... ArgsType>
    NODISCARD FORCEINLINE ZLuaResult operator()(ArgsType&&... args) const {
        return ZLuaResult(std::move(func_(std::forward<ArgsType>(args)...)));
    }

    /**
     * @brief Invokes the Lua function.
     * @tparam ArgsType The types of the arguments passed to the function.
     * @param args The arguments to pass to the Lua function.
     * @return A ZLuaResult containing the result of the function call or error information.
     */
    template <typename... ArgsType>
    NODISCARD ZLuaResult Call(ArgsType&&... args) const noexcept {
        return ZLuaResult(std::move(func_(std::forward<ArgsType>(args)...)));
    }

    /**
     * @brief Converts the Lua object to a string representation.
     * @return A ZString containing the string representation (e.g., via __tostring metamethod).
     */
    NODISCARD ZString ToString() const noexcept;

private:
    /** @brief The underlying sol protected function object used to execute Lua code safely. */
    sol::protected_function func_;
};

} // lua
} // zengine

// ZLuaFunction to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaFunction> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaFunction& _func, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_func.ToString().STDString(), _context);
    }
};