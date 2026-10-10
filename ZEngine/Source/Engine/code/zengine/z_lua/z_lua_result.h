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
 * @brief A wrapper class for the result of a protected Lua function call.
 *
 * Implementation Principle:
 * This class encapsulates `sol::protected_function_result` to manage the lifecycle of Lua return values.
 * It is designed to be a stack-only object (heap allocation is explicitly disabled via deleted `operator new`)
 * to ensure proper Lua stack balancing when the scope ends.
 *
 * Data Access:
 * Accessors like `operator[]` or `At()` convert the underlying stack proxy into a `ZLuaObject` (which holds a
 * `sol::object`). This creates a strong reference in the Lua registry, ensuring the data remains valid even after
 * this result object is destroyed and the Lua stack is popped.
 */
class LUA_DLLAPI ZLuaResult {
    NO_NEW_DELETE
public:
    /**
     * @brief Enumeration representing the status of a Lua function call.
     * Returns kOK if the call was successful, otherwise returns a specific error code.
     */
    enum class StatusEnum_ : Int32 {
        /** @brief The call was successful. */
        kOK = sol::call_status::ok,
        /** @brief The call yielded (coroutine). */
        kYielded = sol::call_status::yielded,
        /** @brief A runtime error occurred during execution. */
        kError_Runtime = sol::call_status::runtime,
        /** @brief A memory allocation error occurred. */
        kError_Memory = sol::call_status::memory,
        /** @brief An error occurred in the error handler. */
        kError_Handler = sol::call_status::handler,
        /** @brief A garbage collection error occurred. */
        kError_GC = sol::call_status::gc,
        /** @brief A syntax error occurred during parsing. */
        kError_Syntax = sol::call_status::syntax,
        /** @brief An error occurred while loading a file. */
        kError_File = sol::call_status::file,
    };

public:
    /**
     * @brief Default constructor.
     * Initializes an empty/invalid result object.
     */
    FORCEINLINE ZLuaResult() noexcept = default;

    /** @brief Deleted copy constructor. */
    ZLuaResult(const ZLuaResult&) = delete;

    /**
     * @brief Move constructor.
     * Transfers ownership of the Lua result from another object.
     * @param other The source object to move from.
     */
    FORCEINLINE ZLuaResult(ZLuaResult&&) noexcept = default;

    /**
     * @brief Explicit constructor from a sol::protected_function_result.
     * @param _result The raw sol2 result object to wrap.
     */
    FORCEINLINE ZLuaResult(sol::protected_function_result&& _result) noexcept
        : result_(std::move(_result))
    {}

    /**
     * @brief Destructor.
     * Cleans up the result, effectively popping the return values from the Lua stack.
     */
    FORCEINLINE ~ZLuaResult() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZLuaResult& operator=(const ZLuaResult&) = delete;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return A reference to this object.
     */
    FORCEINLINE ZLuaResult& operator=(ZLuaResult&&) noexcept = default;

    /**
     * @brief Accesses the return value at the specified index.
     * Requires the result to be valid. The returned object holds a strong reference.
     * @param _index The zero-based index of the return value.
     * @return A ZLuaObject containing the value at the specified index.
     */
    NODISCARD FORCEINLINE ZLuaObject operator[](Int32 _index) noexcept {
        ASSERT(result_.valid(), "ZLuaResult: Invalid Lua result!");
        return ZLuaObject(sol::object(result_[_index]));
    }

    /**
     * @brief Checks if the Lua function call was successful.
     * @return True if the call status is kOK, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return result_.valid();
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return result_.lua_state();
    }

    /**
     * @brief Retrieves the status code of the function call.
     * @return The StatusEnum_ enum value corresponding to the call result.
     */
    NODISCARD FORCEINLINE StatusEnum_ GetStatus() const noexcept {
        return static_cast<StatusEnum_>(result_.status());
    }

    /**
     * @brief Retrieves the number of values returned by the Lua function.
     * @return The count of return values.
     */
    NODISCARD FORCEINLINE Int32 GetReturnCount() const noexcept {
        return result_.return_count();
    }

    /**
     * @brief Retrieves the error message if the call failed.
     * @return A ZString containing the error description, or an empty string if valid.
     */
    NODISCARD ZString GetErrorMessage() const noexcept;

    /**
     * @brief Accesses the return value at the specified index (const version).
     * Requires the result to be valid. The returned object holds a strong reference.
     * @param _index The zero-based index of the return value. Default is 0.
     * @return A ZLuaObject containing the value at the specified index.
     */
    NODISCARD FORCEINLINE ZLuaObject At(Int32 _index = 0) const noexcept {
        ASSERT(result_.valid(), "ZLuaResult: Invalid Lua result!");
        return ZLuaObject(sol::object(result_[_index]));
    }

private:
    /** @brief The underlying sol2 result object managing the stack entries. */
    sol::protected_function_result result_;
};

} // lua
} // zengine