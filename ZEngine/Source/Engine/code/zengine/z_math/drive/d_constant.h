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

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace math {

/** @brief The Float64 version of PI. */
inline constexpr Float64 kPI = 3.1415926535897932384626433832795;
/** @brief The Float64 reference version of PI (Reciprocal of PI). */
inline constexpr Float64 kRecipPI = 1.0 / kPI;
/** @brief The Float64 version of 2 times PI. */
inline constexpr Float64 k2PI = kPI * 2.0;
/** @brief The Float64 reference version of 2 times PI (Reciprocal of 2PI). */
inline constexpr Float64 kRecip2PI = 1.0 / k2PI;
/** @brief The Float64 version of half PI. */
inline constexpr Float64 kHalfPI = kPI * 0.5;
/** @brief The Float64 reference version of half PI (Reciprocal of HalfPI). */
inline constexpr Float64 kRecipHalfPI = 1.0 / kHalfPI;
/** @brief The Float64 version of square root of 3. */
inline constexpr Float64 kSqrt3 = 1.732050807568877;
/** @brief The Float64 reference version of square root of 3 (Reciprocal of Sqrt3). */
inline constexpr Float64 kRecipSqrt3 = 0.5773502691896257;
/** @brief The Float64 version of square root of 2. */
inline constexpr Float64 kSqrt2 = 1.414213562373095;
/** @brief The Float64 reference version of square root of 2 (Reciprocal of Sqrt2). */
inline constexpr Float64 kRecipSqrt2 = 0.7071067811865476;

} // math
} // zengine

namespace zengine {
namespace math {
namespace internal {

/**
 * @brief Traits class defining bit-level representations and constants for floating-point numbers.
 *
 * Implementation Principle:
 * This structure provides compile-time constants and type definitions to manipulate floating-point
 * numbers at the bit level. By interpreting float/double bits as integers (using std::bit_cast),
 * we can perform fast bitwise operations to detect special states like NaN (Not a Number) or Infinity.
 * This avoids the overhead and potential non-determinism of the FPU status registers.
 *
 * @tparam _FloatType The floating-point type to define traits for (e.g., Float32, Float64).
 */
template<typename _FloatType> 
struct FloatTraits;

/**
 * @brief Specialization of FloatTraits for 64-bit floating-point numbers (double).
 */
template<> 
struct FloatTraits<Float64> {
    /** @brief Unsigned integer type capable of holding the bit representation of Float64. */
    using UIntType = UInt64;
    /** @brief Mask to extract the sign bit (1 bit). */
    static inline constexpr UInt64 kSignMask = 0x8000000000000000ULL;
    /** @brief Mask to extract the exponent bits (11 bits). */
    static inline constexpr UInt64 kExpMask = 0x7FF0000000000000ULL;
    /** @brief Mask to extract the mantissa (fraction) bits (52 bits). */
    static inline constexpr UInt64 kMantissaMask = 0x000FFFFFFFFFFFFFULL;
    /** @brief Bit pattern representing positive infinity. */
    static inline constexpr UInt64 kInfBit = 0x7FF0000000000000ULL;
    /** @brief Bit pattern representing a canonical Quiet NaN. */
    static inline constexpr UInt64 kNanBit = 0x7FF8000000000000ULL;
};

/**
 * @brief Specialization of FloatTraits for 32-bit floating-point numbers (float).
 */
template<> 
struct FloatTraits<Float32> {
    /** @brief Unsigned integer type capable of holding the bit representation of Float32. */
    using UIntType = UInt32;
    /** @brief Mask to extract the sign bit (1 bit). */
    static inline constexpr UInt32 kSignMask = 0x80000000U;
    /** @brief Mask to extract the exponent bits (8 bits). */
    static inline constexpr UInt32 kExpMask = 0x7F800000U;
    /** @brief Mask to extract the mantissa (fraction) bits (23 bits). */
    static inline constexpr UInt32 kMantissaMask = 0x007FFFFFU;
    /** @brief Bit pattern representing positive infinity. */
    static inline constexpr UInt32 kInfBit = 0x7F800000U;
    /** @brief Bit pattern representing a canonical Quiet NaN. */
    static inline constexpr UInt32 kNanBit = 0x7FC00000U;
};

} // internal
} // math
} // zengine