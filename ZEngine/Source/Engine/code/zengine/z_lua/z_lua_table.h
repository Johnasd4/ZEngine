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
 * @brief A wrapper class for Lua tables, providing safe type access and manipulation.
 *
 * Implementation Principle:
 * This class inherits from ZObject and acts as a specialized view for Lua tables. It internally holds a sol::table
 * object. To ensure safety in a no-exception environment, constructors assert validity when converting from generic
 * ZLuaObjects. The Get() methods utilize sol::optional to detect type mismatches or missing keys without triggering
 * Lua errors or C++ exceptions, returning default values instead. This abstraction unifies the interface and isolates
 * direct dependency on the sol2 library's table implementation.
 */
class LUA_DLLAPI ZLuaTable : public ZObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;

public:
    /** @brief Default constructor. */
    FORCEINLINE ZLuaTable() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source object to copy from.
     */
    FORCEINLINE ZLuaTable(const ZLuaTable&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param other The source object to move from.
     */
    FORCEINLINE ZLuaTable(ZLuaTable&&) noexcept = default;

    /**
     * @brief Constructs a ZLuaTable from a raw sol::table reference.
     * @param _table The sol::table to wrap.
     */
    FORCEINLINE ZLuaTable(const sol::table& _table) noexcept
        : SuperType_()
        , table_(_table)
    {}

    /**
     * @brief Move-constructs a ZLuaTable from a raw sol::table.
     * @param _table The sol::table to move.
     */
    FORCEINLINE ZLuaTable(sol::table&& _table) noexcept
        : SuperType_()
        , table_(std::move(_table))
    {}

    /**
     * @brief Constructs a ZLuaTable from a generic ZLuaObject.
     * @param _obj The source ZLuaObject.
     * @note This constructor asserts that the source object is valid and is indeed a table.
     */
    FORCEINLINE explicit ZLuaTable(const ZLuaObject& _obj) noexcept 
        : SuperType_() 
        , table_(_obj.obj_)
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::table>(), "ZLuaObject is not a table!");
    }

    /**
     * @brief Move-constructs a ZLuaTable from a generic ZLuaObject.
     * @param _obj The source ZLuaObject.
     * @note This constructor asserts that the source object is valid and is indeed a table.
     */
    FORCEINLINE explicit ZLuaTable(ZLuaObject&& _obj) noexcept 
        : SuperType_() 
        , table_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::table>(), "ZLuaObject is not a table!");
        table_ = std::move(_obj.obj_);
    }

    /** @brief Destructor. */
    FORCEINLINE ~ZLuaTable() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaTable& operator=(const ZLuaTable&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaTable& operator=(ZLuaTable&&) noexcept = default;

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the object is valid, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return table_.valid();
    }

    /**
     * @brief Checks if the table contains a specific key.
     * @param _key The key to search for.
     * @return True if the key exists and is not nil, false otherwise.
     */
    NODISCARD FORCEINLINE Bool Has(ZStringView _key) const noexcept {
        auto option = table_.get<sol::optional<sol::object>>(_key.GetSTDStringView());
        return option.has_value() && option.value() != sol::nil;
    }

    /**
     * @brief Gets the raw Lua state pointer associated with the object.
     * @return The lua_State* pointer.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return table_.lua_state();
    }

    /**
     * @brief Gets the number of elements in the table.
     * @return The size of the table.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept {
        return table_.size();
    }

    /**
     * @brief Retrieves a value associated with a string key.
     * If the key does not exist or the value cannot be converted to the requested type,
     * a default-constructed object of type _ObjectType is returned.
     * @tparam _ObjectType The expected type of the value.
     * @param _key The key to look up.
     * @return The value associated with the key, or a default-constructed value on failure.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        ZStringView _key
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if(option.has_value()) {
            return option.value();
        }
        ASSERT(false, "ZLuaTable::Get: Key not found or type mismatch!");
        return _ObjectType{};
    }

    /**
     * @brief Retrieves a value associated with a string key, returning a fallback value if retrieval fails.
     * @tparam _ObjectType The expected type of the value.
     * @param _key The key to look up.
     * @param _default_value The value to return if the key is missing or type conversion fails.
     * @return The value associated with the key, or the provided default value.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        ZStringView _key,
        _ObjectType&& _default_value
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if (option.has_value()) {
            return option.value();
        }
        return std::forward<_ObjectType>(_default_value);
    }

    /**
     * @brief Retrieves a value associated with an integer index.
     * If the key does not exist or the value cannot be converted to the requested type,
     * a default-constructed object of type _ObjectType is returned.
     * @tparam _ObjectType The expected type of the value.
     * @param _index The index to look up.
     * @return The value associated with the key, or a default-constructed value on failure.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        SizeType _index
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_index);
        if (option.has_value()) {
            return option.value();
        }
        ASSERT(false, "ZLuaTable::Get: Key not found or type mismatch!");
        return _ObjectType{};
    }

    /**
     * @brief Retrieves a value associated with an integer index, returning a fallback value if retrieval fails.
     * @tparam _ObjectType The expected type of the value.
     * @param _index The index to look up.
     * @param _default_value The value to return if the key is missing or type conversion fails.
     * @return The value associated with the key, or the provided default value.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType Get(
        SizeType _index,
        _ObjectType&& _default_value
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_index);
        if (option.has_value()) {
            return option.value();
        }
        return std::forward<_ObjectType>(_default_value);
    }

    /**
     * @brief Attempts to retrieve a value associated with a string key.
     * @tparam _ObjectType The expected type of the value.
     * @param _key The key to look up.
     * @param _value Output parameter to store the retrieved value.
     * @return True if the value was successfully retrieved, false otherwise.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool TryGet(
        ZStringView _key,
        _ObjectType& _value
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_key.GetSTDStringView());
        if (option.has_value()) {
            _value = option.value();
            return true;
        }
        return false;
    }

    /**
     * @brief Attempts to retrieve a value associated with an integer index.
     * @tparam _ObjectType The expected type of the value.
     * @param _index The index to look up.
     * @param _value Output parameter to store the retrieved value.
     * @return True if the value was successfully retrieved, false otherwise.
     */
    template <typename _ObjectType>
    NODISCARD FORCEINLINE Bool TryGet(
        SizeType _index,
        _ObjectType& _value
    ) const noexcept {
        auto option = table_.get<sol::optional<_ObjectType>>(_index);
        if (option.has_value()) {
            _value = option.value();
            return true;
        }
        return false;
    }

    /**
     * @brief Sets a value in the table using a string key.
     * @tparam _ObjectType The type of the value to set.
     * @param _key The key to associate the value with.
     * @param _value The value to store.
     * @return Void.
     */
    template <typename _ObjectType>
    FORCEINLINE Void Set(
        ZStringView _key,
        _ObjectType&& _value
    ) noexcept {
        table_.set(_key.GetSTDStringView(), std::forward<_ObjectType>(_value));
    }

    /**
     * @brief Sets a value in the table using an integer index (array-style).
     * @tparam _ObjectType The type of the value to set.
     * @param _index The index to associate the value with.
     * @param _value The value to store.
     * @return Void.
     */
    template <typename _ObjectType>
    FORCEINLINE Void Set(
        SizeType _index,
        _ObjectType&& _value
    ) noexcept {
        table_.set(_index, std::forward<_ObjectType>(_value));
    }

    /**
     * @brief Converts the table content to a string representation.
     * @return A string describing the table.
     */
    NODISCARD ZString ToString() const noexcept;

private:
    /** @brief The underlying sol::table object. */
    sol::table table_;
};

} // lua
} // zengine

// ZLuaTable to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaTable> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaTable& _table, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_table.ToString().STDString(), _context);
    }
};