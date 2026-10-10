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
 * @brief A wrapper class for Lua coroutines, inheriting from ZObject.
 *
 * Implementation Principle:
 * This class encapsulates `sol::coroutine` to provide a safe mechanism for invoking Lua coroutines
 * from C++. Unlike raw Lua calls or standard `sol::coroutine`, this wrapper handles Lua runtime errors
 * gracefully by returning a `ZLuaResult` object instead of throwing C++ exceptions. It ensures type safety
 * and validity checks during construction from generic `ZLuaObject`s, acting as a specialized interface
 * for callable Lua objects.
 */
class LUA_DLLAPI ZLuaCoroutine : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Enumeration representing the status of a coroutine execution.
     */
    enum class StatusEnum_ : Int32 {
        /** @brief The call was successful. */
        kOK = sol::call_status::ok,
        /** @brief The call yielded (coroutine suspended). */
        kYielded = sol::call_status::yielded,
        /** @brief A runtime error occurred during the call. */
        kError_Runtime = sol::call_status::runtime,
        /** @brief A memory allocation error occurred during the call. */
        kError_Memory = sol::call_status::memory,
        /** @brief An error occurred in the error handler during the call. */
        kError_Handler = sol::call_status::handler,
        /** @brief A garbage collection error occurred during the call. */
        kError_GC = sol::call_status::gc,
        /** @brief A syntax error occurred during the call. */
        kError_Syntax = sol::call_status::syntax,
        /** @brief A file error occurred during the call. */
        kError_File = sol::call_status::file,
    };

public:
    /**
     * @brief Default constructor.
     * Initializes an empty, invalid Lua coroutine object.
     */
    FORCEINLINE ZLuaCoroutine() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source ZLuaCoroutine object to copy from.
     */
    FORCEINLINE ZLuaCoroutine(const ZLuaCoroutine&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param other The source ZLuaCoroutine object to move from.
     */
    FORCEINLINE ZLuaCoroutine(ZLuaCoroutine&&) noexcept = default;

    /**
     * @brief Constructs a ZLuaCoroutine from a raw sol::coroutine.
     * @param _coroutine The sol protected coroutine object to wrap.
     */
    FORCEINLINE ZLuaCoroutine(const sol::coroutine& _coroutine) noexcept
        : SuperType_()
        , coroutine_(_coroutine)
    {}

    /**
     * @brief Move-constructs a ZLuaCoroutine from a raw sol::coroutine.
     * @param _coroutine The sol protected coroutine object to move from.
     */
    FORCEINLINE ZLuaCoroutine(sol::coroutine&& _coroutine) noexcept
        : SuperType_()
        , coroutine_(std::move(_coroutine))
    {}

    /**
     * @brief Constructs a ZLuaCoroutine from a generic ZLuaObject.
     * @param _obj The source ZLuaObject to convert from.
     * @note Asserts that the object is valid and is a coroutine.
     */
    FORCEINLINE explicit ZLuaCoroutine(const ZLuaObject& _obj) noexcept
        : SuperType_()
        , coroutine_(_obj.obj_)
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::coroutine>(), "ZLuaObject is not a coroutine!");
    }

    /**
     * @brief Move-constructs a ZLuaCoroutine from a generic ZLuaObject.
     * @param _obj The source ZLuaObject to move from.
     * @note Asserts that the object is valid and is a coroutine.
     */
    FORCEINLINE explicit ZLuaCoroutine(ZLuaObject&& _obj) noexcept
        : SuperType_()
        , coroutine_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::coroutine>(), "ZLuaObject is not a coroutine!");
        coroutine_ = std::move(_obj.obj_);
    }

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~ZLuaCoroutine() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaCoroutine& operator=(const ZLuaCoroutine&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaCoroutine& operator=(ZLuaCoroutine&&) noexcept = default;

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the underlying Lua coroutine is valid, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return coroutine_.valid();
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return coroutine_.lua_state();
    }

    /**
     * @brief Gets the current status of the thread.
     * @return The StatusEnum_ representing the thread's state (OK, Yielded, Dead, etc.).
     */
    NODISCARD FORCEINLINE StatusEnum_ GetStatus() const noexcept {
        return static_cast<StatusEnum_>(coroutine_.status());
    }

    /**
     * @brief Invokes the Lua coroutine using the coroutine call operator.
     * @tparam ArgsType The types of the arguments passed to the coroutine.
     * @param args The arguments to pass to the Lua coroutine.
     * @return A ZLuaResult containing the result of the coroutine call or error information.
     */
    template <typename... ArgsType>
    NODISCARD FORCEINLINE ZLuaResult operator()(ArgsType&&... args) const {
        return ZLuaResult(std::move(coroutine_(std::forward<ArgsType>(args)...)));
    }

    /**
     * @brief Invokes the Lua coroutine.
     * @tparam ArgsType The types of the arguments passed to the coroutine.
     * @param args The arguments to pass to the Lua coroutine.
     * @return A ZLuaResult containing the result of the coroutine call or error information.
     */
    template <typename... ArgsType>
    NODISCARD ZLuaResult Call(ArgsType&&... args) const noexcept {
        return ZLuaResult(std::move(coroutine_(std::forward<ArgsType>(args)...)));
    }

    /**
     * @brief Converts the Lua object to a string representation.
     * @return A ZString containing the string representation (e.g., via __tostring metamethod).
     */
    NODISCARD ZString ToString() const noexcept;

private:
    /** @brief The underlying sol protected coroutine object used to execute Lua code safely. */
    sol::coroutine coroutine_;
};

} // lua
} // zengine

// ZLuaCoroutine to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaCoroutine> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaCoroutine& _coroutine, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_coroutine.ToString().STDString(), _context);
    }
};