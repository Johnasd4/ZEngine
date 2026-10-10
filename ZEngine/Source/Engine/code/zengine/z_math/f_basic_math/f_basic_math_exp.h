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

#include "f_basic_math_valid.h"

namespace zengine {
namespace math {

namespace internal {

/**
 * @brief Calculates the natural exponential function (e^x) at compile-time.
 *
 * Implementation Principle:
 * This function uses a combination of range reduction and polynomial approximation.
 * 1. Range Reduction (Cody-Waite): The input x is reduced to a small interval using ln(2), such that
 *    x = k * ln(2) + r, where k is an integer and r is within [-0.5*ln(2), 0.5*ln(2)].
 * 2. Polynomial Approximation (Remez Algorithm): The exponential of the reduced argument r is approximated
 *    using a high-degree polynomial (Degree 6 for Float64, Degree 4/5 for Float32).
 * 3. Reconstruction: The final result is computed as result_p * 2^k by directly manipulating the floating-point
 *    exponent bits for efficiency.
 * Use strict boundary checks to return 0 or Infinity to avoid overflow/underflow artifacts.
 *
 * @tparam _FloatType The floating-point type (Float32 or Float64).
 * @param _exponent The exponent value to calculate e raised to.
 * @return The calculated exponential value.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD constexpr _FloatType ExpCalculate(_FloatType _exponent) noexcept {
    // Constants for Range Reduction and Polynomial Approximation
    constexpr Float64 kExpInvLn2 = 1.4426950408889634074; // 1/ln(2)

    // ln(2) split into high and low parts for higher precision reduction (Cody-Waite reduction)
    constexpr Float64 kExpLn2Hi = 6.93147180369123816490e-01;
    constexpr Float64 kExpLn2Lo = 1.90821492927058770002e-10;

    // Remez algorithm coefficients for exp(x) in [-0.5*ln2, 0.5*ln2]
    // Degree 6 is sufficient for Float64 precision in this reduced range.
    constexpr Float64 kExpCoeff0 = 1.0;
    constexpr Float64 kExpCoeff1 = 1.0;
    constexpr Float64 kExpCoeff2 = 0.5;
    constexpr Float64 kExpCoeff3 = 0.16666666666666666667;
    constexpr Float64 kExpCoeff4 = 0.04166666666666666667;
    constexpr Float64 kExpCoeff5 = 0.00833333333333333333;
    constexpr Float64 kExpCoeff6 = 0.00138888888888888889;

    constexpr _FloatType kMaxInput = kIsSameType<_FloatType, Float64> ? 709.78271289338399 : 88.72283905206835f;
    constexpr _FloatType kMinInput = kIsSameType<_FloatType, Float64> ? -708.3964185322641 : -87.3365447504484f;

    if (_exponent > kMaxInput) {
        // Return Infinity (Construction depends on your library)
        return InfP<_FloatType>();
    }
    if (_exponent < kMinInput) {
        return static_cast<_FloatType>(0.0);
    }

    // Range Reduction: k = round(x / ln2)
    // We assume the rounding mode is consistent. Adding 0.5/-0.5 handles rounding to nearest.
    const _FloatType k_float = static_cast<_FloatType>(
        static_cast<Int64>(_exponent * static_cast<_FloatType>(kExpInvLn2) + (_exponent >= 0 ? 0.5 : -0.5))
    );
    const Int32 k = static_cast<Int32>(k_float);

    // Calculate r = x - k * ln2 (using High/Low split to preserve precision)
    // r will be in range approx [-0.35, 0.35]
    _FloatType r = _exponent - k_float * static_cast<_FloatType>(kExpLn2Hi);
    r = r - k_float * static_cast<_FloatType>(kExpLn2Lo);

    // 4. Polynomial Approximation using Horner's Method
    _FloatType result_p;

    if constexpr (kIsSameType<_FloatType, Float64>) {
        // Degree 6 polynomial for Double
        // 1 + r*(1 + r*(c2 + r*(c3 + r*(c4 + r*(c5 + r*c6)))))
        result_p = kExpCoeff2 + r * (kExpCoeff3 + r * (kExpCoeff4 + r * (kExpCoeff5 + r * kExpCoeff6)));
        result_p = 1.0 + r * (1.0 + r * result_p);
    }
    else {
        // Degree 4 or 5 is enough for Float32
        // Using simplified coefficients for float speed
        const _FloatType c2 = 0.5f;
        const _FloatType c3 = 0.166666667f;
        const _FloatType c4 = 0.041666668f;
        const _FloatType c5 = 0.008333334f;

        result_p = 1.0f + r * (1.0f + r * (c2 + r * (c3 + r * (c4 + r * c5))));
    }

    // Reconstruction: result = result_p * 2^k
    // Directly manipulating exponent bits for performance
    if constexpr (kIsSameType<_FloatType, Float64>) {
        UInt64 bits = std::bit_cast<UInt64>(result_p);
        bits += static_cast<UInt64>(k) << 52; // Add k to exponent field
        return std::bit_cast<Float64>(bits);
    }
    else if constexpr (kIsSameType<_FloatType, Float32>) {
        UInt32 bits = std::bit_cast<UInt32>(result_p);
        bits += static_cast<UInt32>(k) << 23; // Add k to exponent field
        return std::bit_cast<Float32>(bits);
    }
    else {
        STATIC_ASSERT(false, "ExpCalculateA: Unsupported floating point type.");
        return 0.0;
    }
}

} // internal

/**
 * @brief Computes the exponential function (e^x) with compile-time support.
 *
 * Implementation Principle:
 * Checks if the function is invoked in a constant-evaluated context (compile-time).
 * - If yes, validates the input for NaN and delegates to the internal pure C++ `ExpCalculate` implementation.
 * - If no (runtime), delegates to `std::exp` to leverage potential SIMD instructions or hardware acceleration.
 *
 * @tparam _FloatType The floating-point type.
 * @param _exponent The exponent value.
 * @return The exponential value.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType Exp(_FloatType _exponent) noexcept {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_exponent)) {
            return NaN<_FloatType>();
        }
        return internal::ExpCalculate(_exponent);
    }
    else {
        return std::exp(_exponent);
    }
}

/**
 * @brief Computes a fast, approximate exponential function.
 *
 * Implementation Principle:
 * Utilizes Schraudolph's method (or similar floating-point hacks) which exploits the IEEE 754 memory layout.
 * Since the bit representation of a float is approximately linear with respect to its log2 value,
 * e^x can be approximated by scaling x (base change from e to 2) and adding a bias to the integer representation.
 * result_int = x * (2^MantissaBits / ln(2)) + Bias.
 * This effectively performs 2^x in hardware integer arithmetic followed by a bit cast.
 *
 * @tparam _FloatType The floating-point type.
 * @param _exponent The exponent value.
 * @return The approximate exponential value.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType ExpFast(_FloatType _exponent) {
    if constexpr (IsCompileTimeConstant()) {
        return Exp(_exponent);
    }
    else if constexpr (kIsSameType<_FloatType, Float32>) {
        constexpr Float32 kExpScaleA = 12102203.16165404f;
        constexpr Int32   kExpOffsetB = 1064986824;

        // Core calculation: 1 multiplication, 1 addition, 1 cast
        Int32 ans_raw = static_cast<Int32>(_exponent * kExpScaleA) + kExpOffsetB;

        // Use std::bit_cast to avoid store-to-load forwarding stalls, 
        // allowing data to move directly between integer and float registers.
        return std::bit_cast<Float32>(ans_raw);
    }
    else if constexpr (kIsSameType<_FloatType, Float64>) {
        constexpr Float64 kExpScaleA = 6497320848556798.0;
        constexpr Int64   kExpOffsetB = 4606853616395542528LL;

        // Core calculation: 1 multiplication, 1 addition, 1 cast
        Int64 ans_raw = static_cast<Int64>(_exponent * kExpScaleA) + kExpOffsetB;

        // Use std::bit_cast to avoid store-to-load forwarding stalls, 
        // allowing data to move directly between integer and float registers.
        return std::bit_cast<Float64>(ans_raw);
    }
    else {
        STATIC_ASSERT(false, "ExpFast: Unsupported floating point type.");
        return static_cast<_FloatType>(0);
    }
}

} // math
} // zengine