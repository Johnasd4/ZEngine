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

#include "f_basic_math_exp.h"
#include "f_basic_math_log.h"

namespace zengine {
namespace math {

namespace internal {

/**
 * @brief Internal implementation for calculating power.
 * @details Handles the logic for power calculation based on the types of the base and exponent.
 *          If the base is an integer and exponent is a float, or if both are floats, it uses the logarithmic identity.
 *          If the exponent is an integer, it uses binary exponentiation (exponentiation by squaring) for efficiency.
 * @tparam _BaseNumberType The type of the base value.
 * @tparam _ExponentNumberType The type of the exponent value.
 * @param _base The base number.
 * @param _exponent The exponent to raise the base to.
 * @return The result of the power calculation.
 */
template<typename _BaseNumberType, typename _ExponentNumberType>
requires kIsNumber<_BaseNumberType> && kIsNumber<_ExponentNumberType>
NODISCARD constexpr auto PowCalculate(
    _BaseNumberType _base,
    _ExponentNumberType _exponent
) noexcept {
    if constexpr (kIsInt<_BaseNumberType> && kIsFloat<_ExponentNumberType>) {
        return Exp(_exponent * Ln(static_cast<_ExponentNumberType>(_base)));
    }
    else if constexpr (kIsInt<_ExponentNumberType>) {
        if (_base == 0) {
            return (_exponent == 0) ? 1 : 0;
        }
        if (_exponent < 0) {
            _base = 1.0 / _base;
            _exponent = -_exponent;
        }
        _BaseNumberType ans = static_cast<_BaseNumberType>(1);
        while (_exponent > static_cast<_ExponentNumberType>(0)) {
            if ((_exponent & static_cast<_ExponentNumberType>(1)) == static_cast<_ExponentNumberType>(1)) {
                ans *= _base;
            }                          
            _base *= _base;
            _exponent >>= static_cast<_ExponentNumberType>(1);
        }
        return ans;
    }
    else {
        return Exp(_exponent * Ln(static_cast<_ExponentNumberType>(_base)));
    }
}

} // internal

/**
 * @brief Computes the value of a base raised to the power of an exponent.
 * @details This function serves as a wrapper that selects the best implementation based on context.
 *          At compile-time, it performs checks and calculation using the internal implementation.
 *          At runtime, if the exponent is an integer, it uses the optimized binary exponentiation.
 *          Otherwise, it falls back to the standard library implementation.
 * @tparam _BaseNumberType The type of the base value.
 * @tparam _ExponentNumberType The type of the exponent value.
 * @param _base The base number.
 * @param _exponent The exponent to raise the base to.
 * @return The result of the power calculation, or NaN if inputs are invalid during compile-time check.
 */
template<typename _BaseNumberType, typename _ExponentNumberType>
requires kIsNumber<_BaseNumberType> && kIsNumber<_ExponentNumberType>
NODISCARD FORCEINLINE constexpr auto Pow(
    _BaseNumberType _base,
    _ExponentNumberType _exponent
) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_base) || IsInvalid(_exponent)) {
            return NaN<_BaseNumberType>();
        }
        return internal::PowCalculate(_base, _exponent);
    }
    else if constexpr (kIsInt<_ExponentNumberType>) {
        return internal::PowCalculate(_base, _exponent);
    }
    else {
        return std::pow(_base, _exponent);
    }
}

/**
 * @brief Computes a fast approximation of the power function.
 * @details This implementation uses bit-manipulation hacks (Schraudolph's approximation) to estimate
 *          the result of the power function. It is significantly faster than standard `Pow` but less precise.
 *          It relies on the IEEE 754 floating-point representation linear approximation property.
 * @tparam _FloatType The floating-point type (Float32 or Float64).
 * @param _base The base number. Must be greater than 0.
 * @param _exponent The exponent to raise the base to.
 * @return The approximate result of the power calculation.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType PowFast(
    _FloatType _base,
    _FloatType _exponent
) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        return Pow(_base, _exponent);
    }
    else if constexpr (kIsSameType<_FloatType, Float32>) {
        ASSERT(_base > static_cast<_FloatType>(0), "PowFast: Requires positive base");

        constexpr Int32 magic_number = 1064866805;
        // Bit Cast: float -> int
        Int32 base_bit = std::bit_cast<Int32>(_base);

        // result_int = exp * (base_int - magic) + magic
        Int32 ans_raw = static_cast<Int32>(_exponent * static_cast<Float32>(base_bit - magic_number)) + magic_number;

        // Bit Cast: int -> float
        return std::bit_cast<Float32>(ans_raw);
    }
    else if constexpr (kIsSameType<_FloatType, Float64>) {
        ASSERT(_base > static_cast<_FloatType>(0), "PowFast: Requires positive base!");
        constexpr Int64 magic_number = 4606921280490663936LL;
        // Bit Cast: float -> int
        Int64 base_bit = std::bit_cast<Int64>(_base);

        // result_int = exp * (base_int - magic) + magic
        Int64 ans_raw = static_cast<Int64>(_exponent * static_cast<Float64>(base_bit - magic_number)) + magic_number;

        // Bit Cast: int -> float
        return std::bit_cast<Float64>(ans_raw);
    }
    else {
        STATIC_ASSERT(false, "ExpFast: Unsupported floating point type.");
        return static_cast<_FloatType>(0);
    }
}

} // math
} // zengine