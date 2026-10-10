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
#include "../z_core/z_system_time.h"

#include "internal/l_sol.h"
#include "t_lua_user_type.h"
#include "z_lua_function.h"
#include "z_lua_result.h"
#include "z_lua_state_view.h"
#include "z_lua_table.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for the Lua state, encapsulating the `sol2` library functionality.
 *
 * This class manages the lifecycle of a Lua virtual machine. It provides a high-level interface for:
 * - Executing Lua scripts and files.
 * - Binding C++ functions and classes (UserTypes) to Lua.
 * - Data exchange between C++ and Lua environments.
 * - Managed garbage collection strategies (step-based or time-based).
 *
 * The implementation relies on `sol::state` to handle the underlying Lua C API interaction,
 * providing type safety and modern C++ features like variadic templates for bindings.
 * It inherits from ZObject to integrate with the engine's memory and object management system.
 */
class LUA_DLLAPI ZLuaState : public ZLuaStateView {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZLuaStateView;

public:
    /**
     * @brief constructor.
     */
    ZLuaState(Bool _use_global_memory_pool = DEFAULT_USE_GLOBAL_MEMORY_POOL) noexcept;

    /** @brief Deleted copy constructor. */
    ZLuaState(const ZLuaState&) = delete;

    /**
     * @brief Move constructor.
     * @param _other The other ZLuaState to move from.
     */
    FORCEINLINE ZLuaState(ZLuaState&&) noexcept = default;

    /**
     * @brief Destructor.
     */
    ~ZLuaState() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZLuaState& operator=(const ZLuaState&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _other The other ZLuaState to move from.
     * @return A reference to this ZLuaState.
     */
    FORCEINLINE ZLuaState& operator=(ZLuaState&&) noexcept = default;

    /**
     * @brief Initializes the Lua state and opens standard Lua libraries.
     * @return kOK if initialization is successful, otherwise an error code.
     */
    NODISCARD ReturnType Initialize() noexcept;

private:
    /**
     * @brief Initializes standard Lua libraries in the state.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType InitializeStandardLibrariesP() noexcept;
    /**
     * @brief Initializes the panic handler for the Lua state.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType InitializePanicHandlerP() noexcept;
    /**
     * @brief Initializes the error handler for the Lua state.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType InitializeErrorHandlerP() noexcept;
    /**
     * @brief Initializes the logging function for the Lua state.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType InitializeLogFunctionP() noexcept;
};

} // lua
} // zengine