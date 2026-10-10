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

#include "z_object.h"

namespace zengine {

/**
 * @brief A template class that may or may not contain a value.
 *
 * Implementation Principle:
 * This class implements an optional type similar to std::optional. It uses a union (Data_) to handle raw memory
 * storage, ensuring proper alignment while allowing the contained object to be uninitialized (empty state).
 * The lifetime of the object (construction and destruction) is manually managed:
 * - Placement new is used for construction.
 * - Destructors are explicitly called if the object is valid and not trivially destructible.
 * - The state is tracked via an internal 'error_code_'. If 'error_code_' is kOK, the value is considered valid.
 * It inherits from ZObject to utilize the engine's memory pool system.
 *
 * @tparam _ObjectType The type of the value to store.
 */
template<typename _ObjectType>
class TOptional : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Copy constructor.
     * @param _option The other TOptional object to copy from.
     */
    FORCEINLINE TOptional(const TOptional& _option) noexcept 
        : SuperType_(_option)
        , error_code_(_option.error_code_)
        
    {
        if (_option.error_code_ == kOK) {
            new(&data_) _ObjectType(_option.data_.obj_);
        }
    }

    /**
     * @brief Move constructor.
     * @param _option The other TOptional object to move from.
     */
    FORCEINLINE TOptional(TOptional&& _option) noexcept
        : SuperType_(std::move(_option))
        , error_code_(_option.error_code_)
    {
        if (_option.error_code_ == kOK) {
            new(&data_) _ObjectType(std::move(_option.data_.obj_));
        }
    }

    /**
     * @brief Constructs an empty TOptional with a specific error code.
     * @param _err_code The error code indicating the reason for no value.
     */
    FORCEINLINE TOptional(ReturnType _err_code) noexcept 
        : SuperType_()
        , error_code_(_err_code) 
    {}
    
    /**
     * @brief Constructs a TOptional containing a copy of the given object.
     * @param _obj The object to copy.
     */
    FORCEINLINE TOptional(const _ObjectType& _obj) noexcept
        : SuperType_()
        , error_code_(kOK)
    {
        new(&data_) _ObjectType(_obj);
    }

    /**
     * @brief Constructs a TOptional containing a moved object.
     * @param _obj The object to move.
     */
    FORCEINLINE explicit TOptional(_ObjectType&& _obj) noexcept 
        : SuperType_()
        , error_code_(kOK)
    {
        new(&data_) _ObjectType(std::move(_obj));
    }

    /**
     * @brief In-place construction / Perfect forwarding constructor.
     * 
     * Uses SFINAE to disable this constructor if the argument list matches the Copy/Move constructors,
     * the Value constructor, or the ErrorCode constructor.
     * 
     * @tparam _ArgsType The types of the arguments.
     * @param _args The arguments to forward to the object's constructor.
     */
    template<typename... _ArgsType, typename = std::enable_if_t<
        std::is_constructible_v<_ObjectType, _ArgsType...> &&
        (
            sizeof...(_ArgsType) != 1 ||
            (
                (!std::is_same_v<std::decay_t<_ArgsType>, TOptional> && ...) &&
                (!std::is_same_v<std::decay_t<_ArgsType>, _ObjectType> && ...) &&
                (!std::is_same_v<std::decay_t<_ArgsType>, ReturnType> && ...)
            )
        )
    >>
    FORCEINLINE TOptional(_ArgsType&&... _args) noexcept 
        : SuperType_()
        , error_code_(kOK)
    {
        new(&data_) _ObjectType(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Destructor. Destroys the contained object if it exists and is not trivially destructible.
     */
    FORCEINLINE ~TOptional() noexcept {
        if constexpr (!kIsTriviallyDestructible<_ObjectType>) {
            if (error_code_ == kOK) {
                data_.obj_.~_ObjectType();
            }
        }
    }

    /**
     * @brief Copy assignment operator.
     * @param _option The other TOptional object to copy from.
     * @return A reference to this object.
     */
    TOptional& operator=(const TOptional& _option) noexcept {
        if (this == &_option) {
            return *this;
        }
        if (error_code_ == kOK && _option.error_code_ == kOK) {
            data_.obj_ = _option.data_.obj_;
        }
        else if (error_code_ != kOK && _option.error_code_ == kOK) {
            new(&data_) _ObjectType(_option.data_.obj_);
        }
        else if (error_code_ == kOK && _option.error_code_ != kOK) {
            data_.obj_.~_ObjectType();
        }
        error_code_ = _option.error_code_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _option The other TOptional object to move from.
     * @return A reference to this object.
     */
    TOptional& operator=(TOptional&& _option) noexcept {
        if (this == &_option) {
            return *this;
        }
        if (error_code_ == kOK && _option.error_code_ == kOK) {
            data_.obj_ = std::move(_option.data_.obj_);
        }
        else if (error_code_ != kOK && _option.error_code_ == kOK) {
            new(&data_) _ObjectType(std::move(_option.data_.obj_));
        }
        else if (error_code_ == kOK && _option.error_code_ != kOK) {
            data_.obj_.~_ObjectType();
        }
        error_code_ = _option.error_code_;
        return *this;
    }

    /**
     * @brief Assigns an error code to this optional, making it empty if the code is not kOK.
     * @param _error_code The error code to assign.
     * @return A reference to this object.
     */
    FORCEINLINE TOptional& operator=(ReturnType _error_code) noexcept {
        if constexpr (!kIsTriviallyDestructible<_ObjectType>) {
            if (error_code_ == kOK && _error_code != kOK) {
                data_.obj_.~_ObjectType();
            }
        }
        error_code_ = _error_code;
        return *this;
    }

    /**
     * @brief Assigns a copy of the given object to this optional.
     * @param _obj The object to copy.
     * @return A reference to this object.
     */
    TOptional& operator=(const _ObjectType& _obj) noexcept {
        if (error_code_ == kOK) {
            data_.obj_ = _obj;
        }
        else {
            new(&data_) _ObjectType(_obj);
            error_code_ = kOK;
        }
        return *this;
    }

    /**
     * @brief Assigns a moved object to this optional.
     * @param _obj The object to move.
     * @return A reference to this object.
     */
    TOptional& operator=(_ObjectType&& _obj) noexcept {
        if (error_code_ == kOK) {
            data_.obj_ = std::move(_obj);
        }
        else {
            new(&data_) _ObjectType(std::move(_obj));
            error_code_ = kOK;
        }
        return *this;
    }

    /**
     * @brief Checks if the optional contains a value.
     * @return True if a value is present, false otherwise.
     */
    NODISCARD FORCEINLINE Bool HasValue() const noexcept { return error_code_ == kOK; }

    /**
     * @brief Gets the error code indicating the state of the optional.
     * @return ReturnType will return kOK if success, if failed returns error code.
     */
    NODISCARD FORCEINLINE ReturnType GetErrorCode() noexcept { return error_code_; }

    /**
     * @brief Gets a reference to the contained value.
     * @note Asserts that the value exists.
     * @return A reference to the contained object.
     */
    NODISCARD FORCEINLINE _ObjectType& GetValue() noexcept {
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return data_.obj_;
    }

    /**
     * @brief Gets a const reference to the contained value.
     * @note Asserts that the value exists.
     * @return A const reference to the contained object.
     */
    NODISCARD FORCEINLINE const _ObjectType& GetValue() const noexcept {
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return data_.obj_;
    }

    /**
     * @brief Returns the contained value or a default value if not present.
     * @param _default_value The value to return if the optional is empty.
     * @return The contained value or the default value.
     */
    NODISCARD FORCEINLINE _ObjectType GetValueOr(_ObjectType&& _default_value) const noexcept {
        return error_code_ == kOK ? data_.obj_ : std::move(_default_value);
    }

    /**
     * @brief Checks if the optional contains a value.
     * @return True if a value is present (error code is kOK), false otherwise.
     */
    FORCEINLINE explicit operator Bool() const noexcept { return error_code_ == kOK; }

    /**
     * @brief Dereferences the optional to access the contained value.
     * @note Asserts that the value exists.
     * @return A reference to the contained object.
     */
    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { 
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return data_.obj_; 
    }

    /**
     * @brief Dereferences the optional to access the contained value (const version).
     * @note Asserts that the value exists.
     * @return A const reference to the contained object.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { 
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return data_.obj_; 
    }

    /**
     * @brief Accesses the contained value via pointer.
     * @note Asserts that the value exists.
     * @return A pointer to the contained object.
     */
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { 
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return &data_.obj_; 
    }

    /**
     * @brief Accesses the contained value via pointer (const version).
     * @note Asserts that the value exists.
     * @return A const pointer to the contained object.
     */
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { 
        ASSERT(error_code_ == kOK, "TOptional: Value not exist!");
        return &data_.obj_; 
    }
    
    /**
     * @brief Constructs the contained value in-place.
     * @tparam _ArgsType The types of the arguments.
     * @param _args The arguments to forward to the constructor.
     * @return A reference to the newly constructed object.
     */
    template<typename... _ArgsType>
    NODISCARD _ObjectType& Emplace(_ArgsType&&... _args) noexcept {
        if (error_code_ == kOK) {
            data_.obj_ = _ObjectType(std::forward<_ArgsType>(_args)...);
        }
        else {
            new(&data_) _ObjectType(std::forward<_ArgsType>(_args)...);
            error_code_ = kOK;
        }
        return data_.obj_;
    }

private:
    /**
     * @brief Internal union to store the object or dummy data.
     * Ensures proper alignment for the object type.
     */
    union Data_ {
        /** @brief The actual stored object. Aligned according to _ObjectType requirements. */
        ALIGN(alignof(_ObjectType)) _ObjectType obj_;
        /** @brief Dummy member to ensure union validity when empty. */
        Char dummy_;

        /** @brief Default constructor for the union (does nothing). */
        FORCEINLINE Data_() noexcept {}
        /** @brief Destructor for the union (does nothing, managed by TOptional). */
        FORCEINLINE ~Data_() noexcept {}
    };

private:
    /** @brief The internal storage data. */
    Data_ data_;
    /** @brief The error code indicating the state. ReturnType will return kOK if success, if failed returns error code. */
    ReturnType error_code_;
};

} // zengine