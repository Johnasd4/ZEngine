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

#include "f_basic_math_digit.h"

namespace zengine {
namespace math {

/**
 * @brief Calculates the modulo (remainder) of two numbers.
 *
 * For integer types, this function uses the standard modulo operator (%).
 * For floating-point types, it calculates the remainder using the formula:
 * result = dividend - (trunc(dividend / modulus) * modulus).
 * This implementation relies on casting the quotient to Int64 to perform truncation.
 *
 * @tparam _NumberType The numeric type of the operands. Must satisfy the kIsNumber concept.
 * @param _dividend The number to be divided.
 * @param _modulus The number to divide by.
 * @return The remainder of the division.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Mod(_NumberType _dividend, _NumberType _modulus) noexcept {
    if constexpr (kIsInt<_NumberType>) {
        return _dividend % _modulus;
    }
    else {
        if (IsCompileTimeConstant()) {
            return _dividend - static_cast<_NumberType>(static_cast<Int64>(_dividend / _modulus)) * _modulus;
        }
        else {
            return std::fmod(_dividend, _modulus);
        }
    }
}

} // math
} // zengine