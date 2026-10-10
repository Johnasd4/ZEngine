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
#include "z_lua_table.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for the Lua state, encapsulating the sol2 library functionality.
 *
 * Implementation Principle:
 * This class wraps an existing raw lua_State pointer using sol::state_view without managing its
 * underlying lifecycle. It leverages template metaprogramming for type-safe data marshing,
 * enabling convenient bindings of C++ types, functions, and state operations in the engine.
 */
class LUA_DLLAPI ZLuaStateView : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Constructs a view from a raw Lua state pointer.
     * @param _state_ptr The raw Lua state pointer to wrap.
     */
    FORCEINLINE ZLuaStateView(RawLuaState* _state_ptr) noexcept : state_(_state_ptr) {}

    /**
     * @brief Copy constructor.
     * @param _other The other ZLuaStateView instance to copy from.
     */
    FORCEINLINE ZLuaStateView(const ZLuaStateView& _other) noexcept = default;

    /**
     * @brief Move constructor.
     * @param _other The other ZLuaStateView instance to move from.
     */
    FORCEINLINE ZLuaStateView(ZLuaStateView&& _other) noexcept = default;

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~ZLuaStateView() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _other The other ZLuaStateView instance to copy from.
     * @return A reference to this instance.
     */
    FORCEINLINE ZLuaStateView& operator=(const ZLuaStateView& _other) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param _other The other ZLuaStateView instance to move from.
     * @return A reference to this instance.
     */
    FORCEINLINE ZLuaStateView& operator=(ZLuaStateView&& _other) noexcept = default;

    /**
     * @brief Checks if a key exists in the Lua state and is not nil.
     * @param _key The name of the variable to check.
     * @return True if the key exists and is not nil, false otherwise.
     */
    NODISCARD FORCEINLINE Bool Has(ZStringView _key) const noexcept {
        auto option = state_.get<sol::optional<sol::object>>(_key.GetSTDStringView());
        return option.has_value() && option.value() != sol::nil;
    }

    /**
     * @brief Retrieves the raw underlying Lua state pointer.
     * @return A pointer to the raw LuaStateRaw (lua_State).
     */
    NODISCARD FORCEINLINE RawLuaState* GetLuaStateRaw() const noexcept {
        return state_.lua_state();
    }

    /**
     * @brief Retrieves a value from the Lua state. Asserts if the key is missing or type mismatches.
     * @tparam _ObjectType The C++ type to cast the Lua value to.
     * @param _key The name of the variable in Lua.
     * @return The retrieved value, or a default constructed object if failed (in Release).
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        ZStringView _key
    ) const noexcept {
        auto option = state_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if (option.has_value()) {
            return option.value();
        }
        ASSERT(false, "ZLuaStateView::Get: Key not found or type mismatch!");
        return _ObjectType{};
    }

    /**
     * @brief Retrieves a value from the Lua state, returning a default value if missing.
     * @tparam _ObjectType The C++ type to cast the Lua value to.
     * @param _key The name of the variable in Lua.
     * @param _default_value The value to return if the key is not found or type mismatches.
     * @return The retrieved value or the default value.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        ZStringView _key,
        _ObjectType&& _default_value
    ) const noexcept {
        auto option = state_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if (option.has_value()) {
            return option.value();
        }
        return std::forward<_ObjectType>(_default_value);
    }

    /**
     * @brief Attempts to retrieve a value from the Lua state without asserting.
     * @tparam _ObjectType The C++ type to cast the Lua value to.
     * @param _key The name of the variable in Lua.
     * @param _value [out] Reference to store the retrieved value if successful.
     * @return True if the value was successfully retrieved, false otherwise.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool TryGet(
        ZStringView _key,
        _ObjectType& _value
    ) const noexcept {
        auto option = state_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if (option.has_value()) {
            _value = option.value();
            return true;
        }
        return false;
    }

    /**
     * @brief Sets a global variable in the Lua state.
     * @tparam _ObjectType The type of the value to set.
     * @param _key The name of the variable in Lua.
     * @param _value The value to assign.
     * @return None.
     */
    template <typename _ObjectType>
    FORCEINLINE Void Set(
        ZStringView _key,
        _ObjectType&& _value
    ) noexcept {
        state_.set(_key.GetSTDStringView(), std::forward<_ObjectType>(_value));
    }

    /**
     * @brief Executes a string of Lua script code.
     * @param _script The string view containing the Lua code.
     * @return A ZLuaResult object containing the execution result or error.
     */
    NODISCARD ZLuaResult ExecuteScript(ZStringView _script) noexcept;

    /**
     * @brief Executes a Lua script from a file.
     * @param _file_path The path to the Lua file.
     * @return A ZLuaResult object containing the execution result or error.
     */
    NODISCARD ZLuaResult ExecuteFile(ZStringView _file_path) noexcept;

    /**
     * @brief Loads a Lua script string without executing it immediately.
     * @param _script The string view containing the Lua code.
     * @return A ZLuaFunction representing the loaded chunk.
     */
    NODISCARD ZLuaFunction LoadScript(ZStringView _script) noexcept;

    /**
     * @brief Loads a Lua script from a file without executing it immediately.
     * @param _file_path The path to the Lua file.
     * @return A ZLuaFunction representing the loaded chunk.
     */
    NODISCARD ZLuaFunction LoadFile(ZStringView _file_path) noexcept;

    /**
     * @brief Registers a C++ user type (class) with the Lua state.
     * @tparam _ClassType The C++ class to register.
     * @tparam _BaseClassesType Variable number of base classes for inheritance.
     * @param _type_name The name of the type as it will appear in Lua.
     * @return A TLuaUserType object used to bind members to this type.
     */
    template<typename _ClassType, typename... _BaseClassesType>
    FORCEINLINE TLuaUserType<_ClassType> RegisterUserType(
        ZStringView _type_name
    ) noexcept {
        if constexpr (sizeof...(_BaseClassesType) == 0ULL) {
            return TLuaUserType<_ClassType>(state_.new_usertype<_ClassType>(
                _type_name.GetSTDStringView())
            );
        }
        else {
            return TLuaUserType<_ClassType>(state_.new_usertype<_ClassType>(
                _type_name.GetSTDStringView(), 
                sol::bases<_BaseClassesType...>())
            );
        }
    }

    /**
     * @brief Registers a C++ function as a global Lua function.
     * @tparam _Function The type of the function or callable.
     * @param _key The name of the function in Lua.
     * @param _func The function implementation.
     * @return None.
     */
    template<typename _Function>
    Void RegisterFunction(ZStringView _key, _Function&& _func) noexcept {
        state_.set_function(_key.GetSTDStringView(), std::forward<_Function>(_func));
    }

    /**
     * @brief Creates a new table in the Lua state.
     * @param _expect_array_size Expected size for the array part of the table (optimization hint).
     * @param _expect_hash_size Expected size for the hash part of the table (optimization hint).
     * @return A ZLuaTable object representing the created table.
     */
    NODISCARD FORCEINLINE ZLuaTable CreateTable(Int32 _expect_array_size, Int32 _expect_hash_size) noexcept {
        return ZLuaTable(state_.create_table(_expect_array_size, _expect_hash_size));
    }

    /**
     * @brief Creates a new named table in the global Lua scope.
     * @tparam ArgsType Variadic arguments for table content initialization.
     * @param _name The global name for the table.
     * @param _args Arguments to initialize the table.
     * @return A ZLuaTable object representing the created table.
     */
    template <typename... ArgsType>
    NODISCARD FORCEINLINE ZLuaTable CreateNamedTable(ZStringView _name, ArgsType&&... _args) noexcept {
        return ZLuaTable(state_.create_named_table(_name.GetSTDStringView(), std::forward<ArgsType>(_args)...));
    }

    /**
     * @brief Performs a full garbage collection cycle.
     * @return None.
     */
    FORCEINLINE Void CollectGarbage() noexcept {
        state_.collect_garbage();
    }

    /**
     * @brief Performs garbage collection incrementally within a specified time budget.
     * @param _time_ms The maximum time in milliseconds to spend on garbage collection.
     * @return None.
     */
    Void CollectGarbageByTimeMs(TimeType _time_ms) noexcept;

    /**
     * @brief Performs garbage collection incrementally by size.
     * @param _size_kb The amount of memory in Kilobytes to step through in the GC cycle.
     * @return None.
     */
    FORCEINLINE Void CollectGarbageBySize(SizeType _size_kb) noexcept {
        lua_gc(state_.lua_state(), LUA_GCSTEP, static_cast<Int32>(_size_kb));
    }

protected:
    /** @brief The underlying sol::state_view instance wrapping the Lua C API. */
    sol::state_view state_;
};

} // namespace lua
} // namespace zengine