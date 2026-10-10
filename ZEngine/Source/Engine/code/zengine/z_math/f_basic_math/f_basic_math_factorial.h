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

#include "../../z_core/t_fixed_array.h"

#include "f_basic_math_valid.h"

namespace zengine {
namespace math {

namespace internal {

/** @brief The size of the floating-point factorial look-up table (0! to 170!). */
constexpr inline SizeType kFloatFactorialTableSize = 171;
/** @brief The size of the integer factorial look-up table (0! to 20!). */
constexpr inline SizeType kIntFactorialTableSize = 21;

/**
 * @brief Pre-computed factorial values for floating-point numbers (0 to 170).
 * @details Implementation Principle: Generated at compile-time using a lambda and std::invoke.
 * Stores values as Float64 to maintain precision for larger factorials up to the double precision limit.
 * This look-up table (LUT) allows for O(1) complexity factorial retrieval.
 */
inline constexpr TFixedArray<Float64, kFloatFactorialTableSize> kFloatFactorialTable = std::invoke([](){
    TFixedArray<Float64, kFloatFactorialTableSize> table;
    table[0] = 1.0;
    for (SizeType index = 1ULL; index < table.GetSize(); ++index) {
        table[index] = table[index - 1ULL] * static_cast<Float64>(index);
    }
    return table;
});

/**
 * @brief Pre-computed factorial values for 64-bit unsigned integers (0 to 20).
 * @details Implementation Principle: Generated at compile-time to ensure zero runtime overhead.
 * Uses UInt64 arithmetic to ensure bit-perfect precision for integer factorials, avoiding floating-point errors.
 */
inline constexpr TFixedArray<UInt64, kIntFactorialTableSize> kIntFactorialTable = std::invoke([](){
    TFixedArray<UInt64, kIntFactorialTableSize> table;
    table[0ULL] = 1ULL;
    for (SizeType index = 1ULL; index < table.GetSize(); ++index) {
        table[index] = table[index - 1ULL] * static_cast<UInt64>(index);
    }
    return table;
});

/**
 * @brief Pre-computed reciprocal factorial values (1/n!) for floating-point numbers.
 * @details Implementation Principle: Derived from kFloatFactorialTable at compile-time.
 * Used to accelerate calculations requiring division by factorials (e.g., Taylor series expansion).
 */
inline constexpr TFixedArray<Float64, kFloatFactorialTableSize> kFloatFactorialReciprocalTable = std::invoke([]() {
    TFixedArray<Float64, kFloatFactorialTableSize> table;
    for (SizeType index = 0ULL; index < table.GetSize(); ++index) {
        table[index] = 1.0 / kFloatFactorialTable[index];
    }
    return table;
});

/** @brief Maximum factorial order representable by Float64 before Infinity. */
inline constexpr SizeType kFactorialFloat64MaxOrder = 170;
/** @brief Maximum factorial order representable by Float32 before Infinity. */
inline constexpr SizeType kFactorialFloat32MaxOrder = 34;
/** @brief Maximum factorial order representable by UInt8 before overflow. */
inline constexpr SizeType kFactorialUInt8MaxOrder = 5;
/** @brief Maximum factorial order representable by UInt16 before overflow. */
inline constexpr SizeType kFactorialUInt16MaxOrder = 8;
/** @brief Maximum factorial order representable by UInt32 before overflow. */
inline constexpr SizeType kFactorialUInt32MaxOrder = 12;
/** @brief Maximum factorial order representable by UInt64 before overflow. */
inline constexpr SizeType kFactorialUInt64MaxOrder = 20;
/** @brief Maximum factorial order representable by Int8 before overflow. */
inline constexpr SizeType kFactorialInt8MaxOrder = 5;
/** @brief Maximum factorial order representable by Int16 before overflow. */
inline constexpr SizeType kFactorialInt16MaxOrder = 7;
/** @brief Maximum factorial order representable by Int32 before overflow. */
inline constexpr SizeType kFactorialInt32MaxOrder = 12;
/** @brief Maximum factorial order representable by Int64 before overflow. */
inline constexpr SizeType kFactorialInt64MaxOrder = 20;

} // internal

/**
 * @brief Calculates the factorial of the given order.
 * @details Implementation Principle:
 * This function utilizes a hybrid approach with compile-time Look-Up Tables (LUT).
 * - Performance: Achieves O(1) time complexity by avoiding iterative multiplication.
 * - Precision: Dispatches to separate tables for integers (UInt64) and floating-points (Float64) to guarantee
 *   exact results for integers and maximum precision for floats.
 * - Safety: Includes rigorous compile-time and runtime checks for signedness, negative inputs, and type-specific
 *   overflows, returning standard limits (Inf/Max) when bounds are exceeded.
 * @tparam _NumberType The numeric type of the input (integer or floating-point).
 * @param _order The non-negative number to calculate the factorial for.
 * @return The calculated factorial. Returns 0 for negative inputs, and TypeMax/Infinity for overflows.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Factorial(_NumberType _order) {
    if constexpr (!kIsUnsignedInt<_NumberType>) {
        if (_order < static_cast<_NumberType>(0)) {
            ASSERT(false, "Factorial order is out of range!");
            return static_cast<_NumberType>(0);
        }
    }

    if constexpr (kIsSameType<_NumberType, Float64>) {
        if (_order > internal::kFactorialFloat64MaxOrder) {
            return InfP<Float64>();
        }
    }
    else if constexpr (kIsSameType<_NumberType, Float32>) {
        if (_order > internal::kFactorialFloat32MaxOrder) {
            return InfP<Float64>();
        }
    }
    else if constexpr (kIsSameType<_NumberType, UInt8>) {
        if (_order > internal::kFactorialUInt8MaxOrder) {
            return kUInt8Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, UInt16>) {
        if (_order > internal::kFactorialUInt16MaxOrder) {
            return kUInt16Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, UInt32>) {
        if (_order > internal::kFactorialUInt32MaxOrder) {
            return kUInt32Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, UInt64>) {
        if (_order > internal::kFactorialUInt64MaxOrder) {
            return kUInt64Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, Int8>) {
        if (_order > internal::kFactorialInt8MaxOrder) {
            return kInt8Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, Int16>) {
        if (_order > internal::kFactorialInt16MaxOrder) {
            return kInt16Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, Int32>) {
        if (_order > internal::kFactorialInt32MaxOrder) {
            return kInt32Max;
        }
    }
    else if constexpr (kIsSameType<_NumberType, Int64>) {
        if (_order > internal::kFactorialInt64MaxOrder) {
            return kInt64Max;
        }
    }

    if constexpr (kIsFloat<_NumberType>) {
        return static_cast<_NumberType>(internal::kFloatFactorialTable[static_cast<SizeType>(_order)]);
    }
    else {
        return static_cast<_NumberType>(internal::kIntFactorialTable[static_cast<SizeType>(_order)]);
    }
}

/**
 * @brief Calculates the reciprocal of the factorial of the given order (1 / n!).
 * @details Implementation Principle:
 * Uses a pre-computed reciprocal LUT for floating-point types to provide fast O(1) access.
 * For integer types, since 1/n! is 0 for n >= 2, the logic simplifies to branch checks,
 * returning 1 for 0! and 1!, and 0 for others.
 * @tparam _NumberType The numeric type of the input.
 * @param _order The non-negative number to calculate the reciprocal factorial for.
 * @return The reciprocal factorial. Returns 0 for negative inputs or if the result is effectively zero.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType FactorialReciprocal(_NumberType _order) {
    if constexpr (!kIsUnsignedInt<_NumberType>) {
        if (_order < static_cast<_NumberType>(0)) {
            ASSERT(false, "Factorial order is out of range!");
            return static_cast<_NumberType>(0);
        }
    }

    if constexpr (kIsSameType<_NumberType, Float64>) {
        if (_order > internal::kFactorialFloat64MaxOrder) { 
            return static_cast<_NumberType>(0);
        }
    }
    else if constexpr (kIsSameType<_NumberType, Float32>) {
        if (_order > internal::kFactorialFloat32MaxOrder) {
            return static_cast<_NumberType>(0);
        }
    }
    else {
        if (_order <= 1) {
            return static_cast<_NumberType>(1);
        }
        else {
            return static_cast<_NumberType>(0);
        }
    }

    return static_cast<_NumberType>(internal::kFloatFactorialReciprocalTable[static_cast<SizeType>(_order)]);
}

} // math
} // zengine