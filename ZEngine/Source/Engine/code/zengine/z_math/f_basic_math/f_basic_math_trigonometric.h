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

#include "../t_interpolation_table.h"

#include "f_basic_math_mod.h"

namespace zengine {
namespace math {

namespace internal {

/**
 * @brief Calculates the sine of a given radian angle with high precision using Taylor Series.
 *
 * This function implements the Cody-Waite range reduction algorithm to accurately handle large inputs by
 * splitting PI into high and low precision parts. It then uses Horner's Method to evaluate the Taylor Series
 * expansion efficiently. It supports compile-time evaluation and handles symmetry reduction.
 *
 * @tparam _FloatType The floating-point type (float or double).
 * @param _radian The input angle in radians.
 * @param _radian_offset An optional offset added to the radian after reduction (e.g., PI/2 for Cosine).
 * @return The calculated sine value of the angle (plus offset).
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD constexpr _FloatType SinCalculate(
    _FloatType _radian, 
    _FloatType _radian_offset = static_cast<_FloatType>(0)
) noexcept {
    constexpr SizeType kSin32TaylorSeriesFactor = 7;
    constexpr SizeType kSin64TaylorSeriesFactor = 12;
    // Taylor series coefficients for Sin calculation.
    constexpr Float64 kSinCoeffs[] = {
        1.0,
        -1.0 / 6.0,
        1.0 / 120.0,
        -1.0 / 5040.0,
        1.0 / 362880.0,
        -1.0 / 39916800.0,
        1.0 / 6227020800.0,
        -1.0 / 1307674368000.0,
        1.0 / 355687428096000.0,
        -1.0 / 121645100408832000.0,
        1.0 / 51090942171709440000.0,
        -1.0 / 25852016738884976640000.0
    };

    // Step 1: Range Reduction (Mod 2PI)
    // Calculate quotient k = round(_radian / 2PI)
    _FloatType quotient = _radian * static_cast<_FloatType>(kRecip2PI);
    
    // Round to nearest integer (handling positive and negative)
    if (quotient >= 0) {
        quotient = static_cast<_FloatType>(static_cast<Int64>(quotient + 0.5));
    }
    else {
        quotient = static_cast<_FloatType>(static_cast<Int64>(quotient - 0.5));
    }

    // Step 2: Remainder Calculation r = _radian - k * 2PI
    // Using Cody-Waite reduction / High-precision subtraction
    if constexpr (kIsSameType<_FloatType, Float32>) {
        // For float, double precision is enough to handle the difference
        Float64 radian64 = static_cast<Float64>(_radian);
        Float64 quotient64 = static_cast<Float64>(quotient);
        // k2PI must be double precision
        radian64 = radian64 - quotient64 * k2PI;
        _radian = static_cast<Float32>(radian64);
    }
    else {
        // For double, we use split constants: 2PI = High + Low
        constexpr Float64 k2PIHigh = 6.2831853071795862; // 0x401921FB54442D18
        // CORRECTION: Low part must be positive because Real_2PI > Double_2PI(High)
        constexpr Float64 k2PILow  = 2.4492935982947064e-16; 
        
        // r = (x - k*Hi) - k*Lo
        _radian = (_radian - quotient * k2PIHigh) - quotient * k2PILow;
    }

    // Apply any additional radian offset
    _radian += _radian_offset;

    // Step 3: Reduction to [-PI, PI]
    // Theoretically, the rounding above puts us in range, but floating point drift 
    // might put us slightly off at the boundaries, so we keep this safety check.
    if (_radian > static_cast<_FloatType>(kPI)) {
        _radian -= static_cast<_FloatType>(k2PI);
    }        
    else if (_radian < -static_cast<_FloatType>(kPI)) {
        _radian += static_cast<_FloatType>(k2PI);
    }
              
    // Step 4: Reduction to [-PI/2, PI/2] using symmetry sin(PI - x) = sin(x)
    if (_radian > static_cast<_FloatType>(kHalfPI)) {
        _radian = static_cast<_FloatType>(kPI) - _radian;
    }
    else if (_radian < -static_cast<_FloatType>(kHalfPI)) {
        _radian = -static_cast<_FloatType>(kPI) - _radian;
    }

    // Step 5: Taylor Series (Horner's Method)
    _FloatType radian2 = _radian * _radian;

    if constexpr (kIsSameType<_FloatType, Float32>) {
        Float32 res = static_cast<Float32>(kSinCoeffs[kSin32TaylorSeriesFactor - 1ULL]);
        for (int index = kSin32TaylorSeriesFactor - 2; index >= 0; --index) {
            res = res * radian2 + static_cast<Float32>(kSinCoeffs[index]);
        }
        return res * _radian;
    }
    else if constexpr (kIsSameType<_FloatType, Float64>) {
        Float64 res = kSinCoeffs[kSin64TaylorSeriesFactor - 1ULL];
        for (int index = kSin64TaylorSeriesFactor - 2; index >= 0; --index) {
            res = res * radian2 + kSinCoeffs[index];
        }
        return res * _radian;
    }
    else {
        STATIC_ASSERT(false, "SinCalculate: Unsupported floating point type.");
        return static_cast<_FloatType>(0);
    }
}

/** @brief The size of the lookup table for Sin/Cos interpolation. */
inline constexpr SizeType kSinCosTableSize = 1024;
/** @brief The radian step distance between two indices in the lookup table. */
inline constexpr Float64 kSinCosTableStepDistance = k2PI / static_cast<Float64>(kSinCosTableSize);
/** @brief The starting offset for the sine/cosine lookup table. */
inline constexpr Float64 kSinCosTableOffset = 0.0;

/**
 * @brief A compile-time generated interpolation table for Sine/Cosine.
 *
 * This table stores pre-calculated sine values distributed evenly across [0, 2PI].
 * It is used by the Fast variants of Sin/Cos for rapid runtime lookups.
 */
inline constexpr TInterpolationTable<Float64, kSinCosTableSize> kSinCosTable = std::invoke([]() {
    TInterpolationTable<Float64, kSinCosTableSize> table{ kSinCosTableOffset, kSinCosTableStepDistance };
    for (SizeType index = 0; index < table.GetSize(); ++index) {
        table[index] = SinCalculate<Float64>(static_cast<Float64>(index) * kSinCosTableStepDistance);
    }
    return table;
});

/** @brief The offset applied to the radian when searching for Sine in the table. */
inline constexpr Float64 kSinSearchOffset = 0.0;
/** @brief The offset applied to the radian when searching for Cosine in the table (PI/2). */
inline constexpr Float64 kCosSearchOffset = kHalfPI;

} // internal

/**
 * @brief Calculates the sine of an angle in radians with high accuracy.
 *
 * At compile-time, this uses a high-precision custom implementation.
 * At runtime, it delegates to the standard library implementation (std::sin) to leverage hardware acceleration
 * and maximum precision.
 *
 * @tparam _FloatType The floating-point type.
 * @param _radian The angle in radians.
 * @return The sine of the angle, or NaN if the input is invalid.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType Sin(_FloatType _radian) {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_radian)) {
            return NaN<_FloatType>();
        }
        return internal::SinCalculate(_radian);
    }
    else {
        // Runtime: Use hardware instruction or highly optimized std lib
        return std::sin(_radian);
    }
}

/**
 * @brief Calculates an approximate sine of an angle for performance-critical scenarios.
 *
 * At compile-time, this falls back to the accurate implementation.
 * At runtime, it uses a pre-computed lookup table with interpolation. This is faster than std::sin but
 * less accurate, especially for very large inputs due to simplified range reduction.
 *
 * @tparam _FloatType The floating-point type.
 * @param _radian The angle in radians.
 * @return The approximate sine of the angle.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType SinFast(_FloatType _radian) {
    if constexpr (IsCompileTimeConstant()) {
        // No need for "Fast" approximation at compile time, use accurate one
        return Sin(_radian);
    }
    else {
        // Fast manual range reduction (Low precision for large numbers, but fast)
        _radian -= static_cast<_FloatType>(static_cast<Int64>(_radian * kRecip2PI)) * static_cast<_FloatType>(k2PI);
        
        // Add 0 for Sin
        _radian += static_cast<_FloatType>(internal::kSinSearchOffset);

        // Wrap to [0, 2PI] for table lookup
        if (_radian < static_cast<_FloatType>(0)) {
            _radian += static_cast<_FloatType>(k2PI);
        }
        
        return static_cast<_FloatType>(internal::kSinCosTable(_radian));
    }
}

/**
 * @brief Calculates the cosine of an angle in radians with high accuracy.
 *
 * At compile-time, this uses a high-precision custom implementation by calculating Sin(radian + PI/2).
 * At runtime, it delegates to the standard library implementation (std::cos).
 *
 * @tparam _FloatType The floating-point type.
 * @param _radian The angle in radians.
 * @return The cosine of the angle, or NaN if the input is invalid.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType Cos(_FloatType _radian) {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_radian)) {
            return NaN<_FloatType>();
        }
        // Calculate Cos(x) as Sin(x + PI/2)
        return internal::SinCalculate(_radian, static_cast<_FloatType>(kHalfPI));
    }
    else {
        return std::cos(_radian);
    }
}

/**
 * @brief Calculates an approximate cosine of an angle for performance-critical scenarios.
 *
 * At compile-time, this falls back to the accurate implementation.
 * At runtime, it uses a pre-computed lookup table with interpolation.
 *
 * @tparam _FloatType The floating-point type.
 * @param _radian The angle in radians.
 * @return The approximate cosine of the angle.
 */
template<typename _FloatType>
requires kIsFloat<_FloatType>
NODISCARD FORCEINLINE constexpr _FloatType CosFast(_FloatType _radian) {
    if constexpr (IsCompileTimeConstant()) {
        if (IsInvalid(_radian)) {
            return NaN<_FloatType>();
        }
        return internal::SinCalculate(_radian, static_cast<_FloatType>(kHalfPI));
    }
    else {
        // Fast reduction
        _radian -= static_cast<_FloatType>(static_cast<Int64>(_radian * kRecip2PI)) * static_cast<_FloatType>(k2PI);
        
        // Add PI/2 for Cos
        _radian += static_cast<_FloatType>(internal::kCosSearchOffset);
        
        // Handle wrapping [0, 2PI]
        if (_radian < static_cast<_FloatType>(0)) {
            _radian += static_cast<_FloatType>(k2PI);
        }
        else if (_radian >= static_cast<_FloatType>(k2PI)) {
            _radian -= static_cast<_FloatType>(k2PI);
        }
        
        return static_cast<_FloatType>(internal::kSinCosTable(_radian));
    }
}

} // math
} // zengine