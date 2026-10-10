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

namespace zengine {
namespace math {
namespace internal {

/** @brief High part of the natural logarithm of 2, used for Cody-Waite reduction. */
inline constexpr Float64 kLogFactorLn2High = 6.93147180369123816490E-01;
/** @brief Low part of the natural logarithm of 2, used for Cody-Waite reduction. */
inline constexpr Float64 kLogFactorLn2Low = 1.90821492927058770002E-10;
/** @brief Square root of 2, used as a threshold for exponent adjustment. */
inline constexpr Float64 kLogFactorSqrt2 = 1.4142135623730951F;
/** @brief Coefficient 1 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg1 = 6.666666666666735130E-01;
/** @brief Coefficient 2 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg2 = 3.999999999940941908E-01;
/** @brief Coefficient 3 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg3 = 2.857142874366239149E-01;
/** @brief Coefficient 4 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg4 = 2.222219843214978396E-01;
/** @brief Coefficient 5 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg5 = 1.818357216161805012E-01;
/** @brief Coefficient 6 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg6 = 1.531383769920937332E-01;
/** @brief Coefficient 7 for the polynomial approximation. */
inline constexpr Float64 kLogFactorLg7 = 1.479819860511658591E-01;

/** @brief Reciprocal of the natural logarithm of 2 (1 / ln(2)). */
inline constexpr Float64 kLogFactorLn2Reciprocal = 1.4426950408889634074;
/** @brief Reciprocal of the natural logarithm of 10 (1 / ln(10)). */
inline constexpr Float64 kLogFactorLn10Reciprocal = 4.3429448190325182765E-01;

/**
 * @brief Calculates the natural logarithm of a number using Taylor expansion and argument reduction.
 *
 * Implementation Principle:
 * The algorithm relies on the identity Ln(x) = Ln(2^k * (1 + f)) = k * Ln(2) + Ln(1 + f).
 * 1. Argument Reduction: The input number is decomposed into an exponent k and a mantissa f.
 *    The mantissa is normalized to the range [1/sqrt(2), sqrt(2)] to minimize the approximation error.
 * 2. Variable Transformation: A variable s = f / (2 + f) is introduced to accelerate convergence.
 *    Ln(1 + f) is approximated as 2s + R(s), where R(s) is an even polynomial series.
 * 3. Polynomial Approximation: A high-degree polynomial (Taylor series or Remez approximation)
 *    is used to calculate R(s) = Lg1*s^2 + Lg2*s^4 + ... + Lg7*s^14.
 * 4. Reconstruction: The final result is reconstructed using Cody-Waite reduction to preserve
 *    numerical precision, specifically handling the k * Ln2 term as (k * Ln2High + k * Ln2Low).
 *
 * @tparam _NumberType The floating-point type (e.g., Float32, Float64).
 * @param _number The input value (must be positive and valid).
 * @return The calculated natural logarithm of the input.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
constexpr _NumberType LnCalculate(_NumberType _number) noexcept {
    Int32 k;
    _NumberType f, ff_d_2;
    _NumberType s, s_2, s_4;
    _NumberType r, r_1, r_2;
    //calculate s f k
    k = FloatExponent(_number);
    f = FloatSignificand(_number);
    if (f >= static_cast<_NumberType>(kLogFactorSqrt2)) {
        f *= static_cast<_NumberType>(0.5);
        ++k;
    }
    f -= 1;
    s = f / (2.0 + f);
    ff_d_2 = f * f * 0.5;
    s_2 = s * s;
    s_4 = s_2 * s_2;

    //Calculate the taylor expansion.
    if constexpr (kIsSameType<_NumberType, Float64>) {
        //calculate r
        r_1 = 
            s_2 * (static_cast<_NumberType>(kLogFactorLg1) + 
                s_4 * (static_cast<_NumberType>(kLogFactorLg3) +
                    s_4 * (static_cast<_NumberType>(kLogFactorLg5) + 
                        s_4 * static_cast<_NumberType>(kLogFactorLg7))));
        r_2 =
            s_4 * (static_cast<_NumberType>(kLogFactorLg2) +
                s_4 * (static_cast<_NumberType>(kLogFactorLg4) +
                    s_4 * static_cast<_NumberType>(kLogFactorLg6)));
        r = r_1 + r_2;
    }
    else if constexpr (kIsSameType<_NumberType, Float32>) {
        //calculate r
        r_1 = s_2 * (static_cast<_NumberType>(kLogFactorLg1) + s_4 * static_cast<_NumberType>(kLogFactorLg3));
        r_2 = s_4 * (static_cast<_NumberType>(kLogFactorLg2) + s_4 * static_cast<_NumberType>(kLogFactorLg4));
        r = r_1 + r_2;
    }
    else {
        STATIC_ASSERT(false, "LnCalculate: Unsupported floating point type.");
    }

    const _NumberType k_val = static_cast<_NumberType>(k);

    //calculate Ln(x)
    return k_val * static_cast<_NumberType>(kLogFactorLn2High) -
        ((ff_d_2 - (s * (ff_d_2 + r) + k_val * static_cast<_NumberType>(kLogFactorLn2Low))) - f);
}

/** @brief Scale factor for Float32 fast log (1 / 2^23). */
inline constexpr Float32 kFastLogScaleF32 = 1.1920928955078125e-7F;
/** @brief Bias adjustment for Float32 fast log (127 - 0.05747). */
inline constexpr Float32 kFastLogBiasF32 = 126.942529F;

/** @brief Scale factor for Float64 fast log (1 / 2^52). */
inline constexpr Float64 kFastLogScaleF64 = 2.2204460492503131E-16;
/** @brief Bias adjustment for Float64 fast log (1023 - 0.05747). */
inline constexpr Float64 kFastLogBiasF64 = 1022.942529;

/** @brief Natural logarithm of 2 (Ln(2)). */
inline constexpr Float64 kFastLogFactorLn2 = 0.69314718055994530941;
/** @brief Reciprocal of natural logarithm of 10 (1 / Ln(10)). */
inline constexpr Float64 kFastLogFactorLn10Reciprocal = 0.43429448190325182765;

/**
 * @brief Calculates an approximation of Log2(x) using bit manipulation.
 *
 * Implementation Principle:
 * Utilizes the property of IEEE 754 floating point representation where the
 * bit-integer interpretation is piecewise linear to the logarithm.
 * Log2(x) ~ I_x * 2^(-L) - (B - sigma)
 * Where I_x is the integer representation, L is mantissa length, B is bias.
 *
 * Note: This function prioritizes speed over precision.
 * Error margin is approximately +/- 0.00xxx.
 *
 * @tparam _NumberType The floating-point type (Float32 or Float64).
 * @param _number The input value (must be positive).
 * @return The approximate binary logarithm.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
constexpr _NumberType Log2CalculateFast(_NumberType _number) noexcept {
    // Note: No safety checks (IsInvalid/<=0) are performed here to maximize performance.
    // Caller ensures _number > 0.
    if constexpr (kIsSameType<_NumberType, Float32>) {
        const UInt32 ix = std::bit_cast<UInt32>(_number);
        _NumberType val = static_cast<_NumberType>(ix);
        val *= kFastLogScaleF32;
        val -= kFastLogBiasF32;
        return val;
    }
    else if constexpr (kIsSameType<_NumberType, Float64>) {
        const UInt64 ix = std::bit_cast<UInt64>(_number);
        _NumberType val = static_cast<_NumberType>(ix);
        val *= static_cast<_NumberType>(kFastLogScaleF64);
        val -= static_cast<_NumberType>(kFastLogBiasF64);
        return val;
    }
    else {
        STATIC_ASSERT(false, "Log2CalculateFast: Unsupported floating point type.");
    }
}

} // internal

/**
 * @brief Calculates the natural logarithm (base e) of a number.
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value.
 * @return The natural logarithm of _number, or NaN if the input is invalid or non-positive.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Ln(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_number) || _number <= static_cast<_NumberType>(0)) {
            return NaN<_NumberType>();
        }
        else if (_number == static_cast<_NumberType>(1)) {
            return static_cast<_NumberType>(0);
        }
        return internal::LnCalculate(_number);
    }
    else {
        return std::log(_number);
    }
}

/**
 * @brief Calculates a fast approximation of the natural logarithm (base e).
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value (should be > 0).
 * @return The approximate Ln of _number.
 */
template<typename _NumberType>
    requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType LnFast(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        return Ln(_number);
    }
    else {
        return internal::Log2CalculateFast(_number) * static_cast<_NumberType>(internal::kFastLogFactorLn2);
    }
}

/**
 * @brief Calculates the binary logarithm (base 2) of a number.
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value.
 * @return The binary logarithm of _number, or NaN if the input is invalid or non-positive.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Log2(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_number) || _number <= static_cast<_NumberType>(0)) {
            return NaN<_NumberType>();
        }
        else if (_number == static_cast<_NumberType>(1)) {
            return static_cast<_NumberType>(0);
        }
        return internal::LnCalculate(_number) * static_cast<_NumberType>(internal::kLogFactorLn2Reciprocal);
    }
    else {
        return std::log2(_number);
    }
}

/**
 * @brief Calculates a fast approximation of the binary logarithm (base 2).
 *
 * High performance, lower precision (approx 3-5% error).
 * Ideal for graphics, audio, or heuristics.
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value (should be > 0).
 * @return The approximate Log2 of _number.
 */
template<typename _NumberType>
    requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Log2Fast(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        return Log2(_number);
    }
    else {
        return internal::Log2CalculateFast(_number);
    }
}

/**
 * @brief Calculates the common logarithm (base 10) of a number.
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value.
 * @return The common logarithm of _number, or NaN if the input is invalid or non-positive.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Lg(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_number) || _number <= static_cast<_NumberType>(0)) {
            return NaN<_NumberType>();
        }
        else if (_number == static_cast<_NumberType>(1)) {
            return static_cast<_NumberType>(0);
        }
        return internal::LnCalculate(_number) * static_cast<_NumberType>(internal::kLogFactorLn10Reciprocal);
    }
    else {
        return std::log10(_number);
    }
}

/**
 * @brief Calculates a fast approximation of the common logarithm (base 10).
 *
 * @tparam _NumberType The floating-point type.
 * @param _number The input value (should be > 0).
 * @return The approximate Lg of _number.
 */
template<typename _NumberType>
    requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType LgFast(_NumberType _number) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        return Lg(_number);
    }
    else {
        return internal::Log2CalculateFast(_number) * static_cast<_NumberType>(internal::kFastLogFactorLn10Reciprocal);
    }
}

/**
 * @brief Calculates the logarithm of a number with a specified base.
 *
 * @tparam _NumberType The floating-point type.
 * @param _logarithm_number The value to calculate the logarithm of.
 * @param _base_number The base of the logarithm.
 * @return The logarithm of _logarithm_number base _base_number, or NaN if inputs are invalid, non-positive,
 *         or if the base is 1.
 */
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Log(
    _NumberType _logarithm_number, 
    _NumberType _base_number
) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (
            IsInvalid(_logarithm_number) ||
            IsInvalid(_base_number) ||
            _logarithm_number <= static_cast<_NumberType>(0) ||
            _base_number <= static_cast<_NumberType>(0) ||
            _base_number == static_cast<_NumberType>(1)
            ) {
            return NaN<_NumberType>();
        }
        return internal::LnCalculate(_logarithm_number) / internal::LnCalculate(_base_number);
    }
    else {
        return std::log(_logarithm_number) / std::log(_base_number);
    }
}

/**
 * @brief Calculates a fast approximation of the logarithm with a specified base.
 *
 * Implementation Principle:
 * Uses the change of base formula: Log_b(x) = Log2(x) / Log2(b).
 * This relies on the Schraudolph's algorithm implemented in internal::Log2CalculateFast.
 *
 * Performance Note:
 * This involves one division, which is more expensive than multiplication.
 * If _base_number is constant, prefer pre-calculating the inverse of its Log2
 * and using multiplication.
 *
 * @tparam _NumberType The floating-point type.
 * @param _logarithm_number The value to calculate the logarithm of (should be > 0).
 * @param _base_number The base of the logarithm (should be > 0 and != 1).
 * @return The approximate logarithm of _logarithm_number base _base_number.
 */
template<typename _NumberType>
    requires kIsFloat<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType LogFast(
    _NumberType _logarithm_number, 
    _NumberType _base_number
) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        return Log(_logarithm_number, _base_number);
    }
    else {
        // Note: Like other Fast functions, this omits checks for IsInvalid, <=0, or base==1 
        // to maintain maximum throughput.
        return internal::Log2CalculateFast(_logarithm_number) / internal::Log2CalculateFast(_base_number);
    }
}

} // math
} // zengine 