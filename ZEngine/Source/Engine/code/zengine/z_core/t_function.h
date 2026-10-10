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

#include "m_log.h"
#include "t_smart_pointer.h"
#include "z_object.h"

namespace zengine {

namespace error_code {

/** @brief Error codes related to TFunction operations. */
enum TFunctionErrorCodeEnum : ReturnType::ValueType {
    /** @brief Link error occurred. */
    kTFunctionErrorCode_LinkError = kErrorCodeBase_TFunction,
    /** @brief System or library internal error. */
    kTFunctionErrorCode_SystemOrLibraryError,
    /** @brief Parameter is a null pointer. */
    kTFunctionErrorCode_NullptrParam,
    /** @brief Parameter is out of valid range. */
    kTFunctionErrorCode_ParamOutOfRange,
    /** @brief The target function does not exist or is empty. */
    kTFunctionErrorCode_FunctionNotExist,
};

} // error_code

/*
    Function class, same as std::function. Use zengine's memory pool.
    Template: TFunction<_ReturnType(_Arg1, _Arg2, _Arg3...)> = _func.
    [_func] can be basic function or non-capture lambda expression. 
*/
/**
 * @brief A lightweight function wrapper class specifically for raw function pointers.
 *
 * This class encapsulates a raw function pointer. It can hold basic functions or
 * non-capturing lambda expressions that decay to function pointers.
 *
 * @tparam _Signature The function signature.
 */
template<typename _Signature>
class TSimpleFunction {
    STATIC_ASSERT(
        (!kIsSameType<_Signature, _Signature>),
        "TSimpleFunction: Signature not valid!"
    );
};

/**
 * @brief Specialization of TSimpleFunction for a specific function signature.
 *
 * @tparam _ReturnType The return type of the function.
 * @tparam Args The argument types of the function.
 */
template<typename _ReturnType, typename... Args>
class TSimpleFunction<_ReturnType(Args...)> {
    NO_NEW_DELETE
public:
    /** @brief Defines the raw function pointer type. */
    using FunctionType_ = _ReturnType(*)(Args...);

public:
    /** @brief Default constructor. Initializes the internal pointer to nullptr. */
    FORCEINLINE TSimpleFunction() noexcept : func_ptr_(nullptr) {}

    /**
     * @brief Constructs the TSimpleFunction from a raw function pointer.
     *
     * @param _origin_func The raw function pointer to wrap.
     */
    FORCEINLINE TSimpleFunction(FunctionType_ _origin_func) noexcept
        : func_ptr_(_origin_func) {
    }

    /**
     * @brief Copy constructor.
     *
     * @param _func The source TSimpleFunction to copy from.
     */
    TSimpleFunction(const TSimpleFunction& _func) noexcept
        : func_ptr_(_func.func_ptr_) {}

    /**
     * @brief Copy assignment operator.
     *
     * @param _func The source TSimpleFunction to copy from.
     * @return Reference to this object.
     */
    TSimpleFunction& operator=(const TSimpleFunction& _func) noexcept {
        func_ptr_ = _func.func_ptr_;
        return *this;
    }

    /**
     * @brief Equality operator to check if two TSimpleFunctions point to the same function.
     *
     * @param _func The other TSimpleFunction to compare.
     * @return True if both hold the same function pointer, False otherwise.
     */
    friend NODISCARD Bool operator==(const TSimpleFunction& _left_func, const TSimpleFunction& _right_func) noexcept {
        return _left_func.func_ptr_ == _right_func.func_ptr_;
    }

    /**
     * @brief Executes the wrapped function.
     *
     * Checks if the function exists. If not, logs an error and returns a default value or exits
     * based on the return type properties.
     *
     * @param args Arguments to pass to the function.
     * @return The result of the function execution.
     */
    _ReturnType operator()(Args... args) const noexcept {
        if (!func_ptr_) {
            if constexpr (!kIsSameType<_ReturnType, Void>) {
                if constexpr (std::is_default_constructible<_ReturnType>::value) {
                    Z_LOG_ERROR(
                        error_code::kTFunctionErrorCode_FunctionNotExist, 0,
                        "Function not exist, can not execute!"
                    );
                    return _ReturnType();
                }
                else {
                    Z_LOG_ERROR(
                        error_code::kTFunctionErrorCode_FunctionNotExist, 0,
                        "Function not exist, exit program!"
                    );
                    Exit(error_code::kTFunctionErrorCode_FunctionNotExist);
                    return *reinterpret_cast<std::remove_reference_t<_ReturnType>*>(nullptr);
                }
            }
            else {
                Z_LOG_ERROR(
                    error_code::kTFunctionErrorCode_FunctionNotExist, 0,
                    "Function not exist, can not execute!"
                );
                return;
            }
        }
        return func_ptr_(std::forward<Args>(args)...);
    }

    /**
     * @brief Checks if the function pointer is valid (not null).
     *
     * @return True if the function is valid, False otherwise.
     */
    FORCEINLINE explicit operator Bool() const noexcept {
        return func_ptr_ != nullptr;
    }

private:
    /** @brief The underlying raw function pointer. */
    FunctionType_ func_ptr_;
};

/*
    Function class, same as std::function. Use zengine's memory pool.
    Template: TFunction<_ReturnType(_Arg1, _Arg2, _Arg3...)> = _func.
    [_func] can be function, lambda expreesion or functor. 
*/
/**
 * @brief A general-purpose polymorphic function wrapper.
 *
 * Similar to std::function, this class can store, copy, and invoke any Callable target
 * (functions, lambda expressions, bind expressions, or other function objects).
 * It uses Type Erasure to handle different callable types uniformly.
 * Inherits from ZObject to utilize the ZEngine memory management system.
 *
 * @tparam _Signature The function signature.
 */
template<typename _Signature>
class TFunction : public ZObject {
    STATIC_ASSERT(
        (!kIsSameType<_Signature, _Signature>),
        "TFunction: Signature not valid!"
    );
};

/**
 * @brief Specialization of TFunction for a specific function signature.
 *
 * @tparam _ReturnType The return type of the function.
 * @tparam ArgTypes The argument types of the function.
 */
template<typename _ReturnType, typename... ArgTypes>
class TFunction<_ReturnType(ArgTypes...)> : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;

public:
    /** @brief Default constructor. Creates an empty function wrapper. */
    FORCEINLINE TFunction() noexcept : SuperType_(), func_() {}

    /**
     * @brief Constructs a TFunction from any callable object.
     *
     * @tparam _OriginFunc The type of the callable object.
     * @param _origin_func The callable object to wrap.
     */
    template<typename _OriginFunc, typename = std::enable_if<!kIsSameType<TFunction, _OriginFunc>>::type>
    FORCEINLINE TFunction(_OriginFunc&& _origin_func) noexcept
        : SuperType_()
        , func_(MakeUnique<FunctionImplP_<std::decay_t<_OriginFunc>>>(std::forward<_OriginFunc>(_origin_func))) {}

    /**
     * @brief Copy constructor.
     *
     * Performs a deep copy of the stored callable object.
     *
     * @param _func The source TFunction to copy from.
     */
    TFunction(const TFunction& _func) noexcept
        : SuperType_()
        , func_(_func.func_ ? _func.func_->Clone() : nullptr) {}

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the stored callable object.
     *
     * @param _func The source TFunction to move from.
     */
    TFunction(TFunction&& _func) noexcept
        : SuperType_()
        , func_(std::move(_func.func_)) {}

    /**
     * @brief Copy assignment operator.
     *
     * Performs a deep copy of the stored callable object.
     *
     * @param _func The source TFunction to copy from.
     * @return Reference to this object.
     */
    TFunction& operator=(const TFunction& _func) noexcept {
        func_ = _func.func_ ? _func.func_->Clone() : nullptr;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * Transfers ownership of the stored callable object.
     *
     * @param _func The source TFunction to move from.
     * @return Reference to this object.
     */
    FORCEINLINE TFunction& operator=(TFunction&& _func) noexcept {
        func_ = std::move(_func.func_);
        return *this;
    }

    /**
     * @brief Assignment operator for nullptr.
     *
     * Clears the stored callable object.
     *
     * @param _nullptr Nullptr literal.
     * @return Reference to this object.
     */
    TFunction& operator=(NullptrType _nullptr) noexcept {
        func_.Reset();
        return *this;
    }

    /**
     * @brief Assignment operator for any callable object.
     *
     * @tparam _OriginFunc The type of the callable object.
     * @param _origin_func The callable object to assign.
     * @return Reference to this object.
     */
    template<typename _OriginFunc, typename = std::enable_if<!kIsSameType<TFunction, _OriginFunc>>::type>
    TFunction& operator=(_OriginFunc&& _origin_func) noexcept {
        func_ = MakeUnique<FunctionImplP_<std::decay_t<_OriginFunc>>>(std::forward<_OriginFunc>(_origin_func));
        return *this;
    }

    /**
     * @brief Executes the wrapped function.
     *
     * Checks if a callable object is stored. If not, logs an error and returns a default value or exits.
     *
     * @param args Arguments to pass to the function.
     * @return The result of the function execution.
     */
    _ReturnType operator()(ArgTypes... args) const noexcept {
        if (!func_) {
            if constexpr (!kIsSameType<_ReturnType, Void>) {
                if constexpr (std::is_default_constructible<_ReturnType>::value) {
                    Z_LOG_ERROR(
                        error_code::kTFunctionErrorCode_FunctionNotExist, 0, 
                        "Function not exist, can not execute!"
                    );
                    return _ReturnType();
                }
                else {
                    Z_LOG_ERROR(
                        error_code::kTFunctionErrorCode_FunctionNotExist, 0, 
                        "Function not exist, exit program!"
                    );
                    Exit(error_code::kTFunctionErrorCode_FunctionNotExist);
                    return *reinterpret_cast<std::remove_reference_t<_ReturnType>*>(nullptr);
                }
            }
            else {
                Z_LOG_ERROR(
                    error_code::kTFunctionErrorCode_FunctionNotExist, 0,
                    "Function not exist, can not execute!"
                );
                return;
            }
        }
        return func_->Execute(std::forward<ArgTypes>(args)...);
    }

    /**
     * @brief Checks if the TFunction stores a valid callable object.
     *
     * @return True if a callable is stored, False otherwise.
     */
    FORCEINLINE explicit operator Bool() const noexcept {
        return func_.operator Bool();
    }

private:
    /** @brief Abstract base class for the type-erased function wrapper. */
    class FunctionBaseP_ : public ZObject {
    public:
        /** @brief Alias for the base class ZObject. */
        using SuperType_ = ZObject;

    public:
        /** @brief Default constructor. */
        FORCEINLINE FunctionBaseP_() noexcept : SuperType_() {}

        /** @brief Virtual destructor. */
        virtual ~FunctionBaseP_() noexcept = default;

        /**
         * @brief Executes the stored function.
         *
         * @param args The arguments to pass to the function.
         * @return The result of the execution.
         */
        virtual _ReturnType Execute(ArgTypes... args) const noexcept = 0;

        /**
         * @brief Clones the stored function object.
         *
         * @return A unique pointer to the cloned function wrapper.
         */
        virtual TUniquePointer<FunctionBaseP_> Clone() const noexcept = 0;
    };

    /**
     * @brief Concrete implementation of the type-erased function wrapper.
     *
     * @tparam _DecayFunction The decay type of the original callable object.
     */
    template<typename _DecayFunction>
    class FunctionImplP_ : public FunctionBaseP_ {
    public:
        /** @brief Type alias for the base class. */
        using SuperType_ = FunctionBaseP_;

    public:
        /**
         * @brief Constructs the implementation wrapper.
         *
         * @tparam _OriginFunc The original type of the callable object.
         * @param _func The callable object to store.
         */
        template<typename _OriginFunc>
        FORCEINLINE FunctionImplP_(_OriginFunc&& _func) noexcept 
            : SuperType_(), decay_func_(std::forward<_OriginFunc>(_func)) {}

        /**
         * @brief Executes the stored callable object.
         *
         * @param args The arguments to pass.
         * @return The result of the execution.
         */
        virtual _ReturnType Execute(ArgTypes... args) const noexcept {
            return decay_func_(std::forward<ArgTypes>(args)...);
        }

        /**
         * @brief Clones this implementation wrapper.
         *
         * @return A unique pointer to the cloned wrapper.
         */
        virtual TUniquePointer<FunctionBaseP_> Clone() const noexcept {
            return MakeUnique<FunctionImplP_>(decay_func_);
        }

    private:
        /** @brief The stored callable object (function, lambda, functor). */
        mutable _DecayFunction decay_func_;
    };

    /** @brief Unique pointer to the type-erased function implementation. */
    TUniquePointer<FunctionBaseP_> func_;
};

} // zengine