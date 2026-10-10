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

/**
 * @brief Returns a canonical Quiet NaN (Not-a-Number).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @return A floating-point value representing a Quiet NaN.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr NumberType NaN() {
    using Traits = internal::FloatTraits<NumberType>;
    return std::bit_cast<NumberType>(Traits::kNanBit);
}

/**
 * @brief Returns positive infinity (+inf).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @return A floating-point value representing positive infinity.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr NumberType InfP() {
    using Traits = internal::FloatTraits<NumberType>;
    return std::bit_cast<NumberType>(Traits::kInfBit);
}

/**
 * @brief Returns negative infinity (-inf).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @return A floating-point value representing negative infinity.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr NumberType InfN() {
    using Traits = internal::FloatTraits<NumberType>;
    return std::bit_cast<NumberType>(Traits::kInfBit | Traits::kSignMask);
}

/**
 * @brief Checks if the number is finite (not NaN and not Infinity).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @param _number The floating-point number to check.
 * @return True if the number is finite, false otherwise.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr Bool IsFinite(NumberType _number) {
    using Traits = internal::FloatTraits<NumberType>;
    const auto bits = std::bit_cast<typename Traits::UIntType>(_number);
    return (bits & Traits::kExpMask) != Traits::kExpMask;
}

/**
 * @brief Checks if the number is NaN (Not-a-Number).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @param _number The floating-point number to check.
 * @return True if the number is NaN, false otherwise.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr Bool IsNaN(NumberType _number) {
    using Traits = internal::FloatTraits<NumberType>;
    const auto bits = std::bit_cast<typename Traits::UIntType>(_number);
    return ((bits & Traits::kExpMask) == Traits::kExpMask) &&
           ((bits & Traits::kMantissaMask) != 0);
}

/**
 * @brief Checks if the number is Infinity (either positive or negative).
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @param _number The floating-point number to check.
 * @return True if the number is Infinity, false otherwise.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr Bool IsInf(NumberType _number) {
    using Traits = internal::FloatTraits<NumberType>;
    const auto bits = std::bit_cast<typename Traits::UIntType>(_number);
    return ((bits & Traits::kExpMask) == Traits::kExpMask) &&
           ((bits & Traits::kMantissaMask) == 0);
}

/**
 * @brief Checks if the number is positive Infinity.
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @param _number The floating-point number to check.
 * @return True if the number is positive Infinity, false otherwise.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr Bool IsInfP(NumberType _number) {
    using Traits = internal::FloatTraits<NumberType>;
    const auto bits = std::bit_cast<typename Traits::UIntType>(_number);
    return bits == Traits::kInfBit;
}

/**
 * @brief Checks if the number is negative Infinity.
 * @tparam NumberType The floating-point type (Float32 or Float64).
 * @param _number The floating-point number to check.
 * @return True if the number is negative Infinity, false otherwise.
 */
template<typename NumberType>
requires kIsFloat<NumberType>
NODISCARD FORCEINLINE constexpr Bool IsInfN(NumberType _number) {
    using Traits = internal::FloatTraits<NumberType>;
    const auto bits = std::bit_cast<typename Traits::UIntType>(_number);
    return bits == (Traits::kInfBit | Traits::kSignMask);
}

} // math
} // zengine