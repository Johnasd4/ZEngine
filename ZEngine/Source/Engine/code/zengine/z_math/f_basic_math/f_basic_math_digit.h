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
 * @brief Returns the sign of the number, treating zero as positive (2-state sign).
 * @param _number The input number to evaluate.
 * @return Returns 1 if the number is greater than or equal to 0, returns -1 if the number is less than 0.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Sgn2(_NumberType _number) {
    if constexpr (kIsUnsignedInt<_NumberType>) {
        return static_cast<_NumberType>(1);
    }
    else {
        return static_cast<_NumberType>(
            (_number >= static_cast<_NumberType>(0)) - (_number < static_cast<_NumberType>(0))
        );
    }
}

/**
 * @brief Returns the sign of the number (3-state sign).
 * @param _number The input number to evaluate.
 * @return Returns 1 if the number is positive, 0 if the number is zero, and -1 if the number is negative.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Sgn3(_NumberType _number) {
    if constexpr (kIsUnsignedInt<_NumberType>) {
        return _number == static_cast<_NumberType>(0) ? static_cast<_NumberType>(0) : static_cast<_NumberType>(1);
    }
    else {
        return static_cast<_NumberType>(
            (_number > static_cast<_NumberType>(0)) - (_number < static_cast<_NumberType>(0))
        );
    }
}

/**
 * @brief Calculates the absolute value of the provided number.
 * @param _number The input number.
 * @return The absolute value of the number.
 */
template<typename _NumberType>
requires kIsNumber<_NumberType>
NODISCARD FORCEINLINE constexpr _NumberType Abs(_NumberType _number) {
    if constexpr (IsCompileTimeConstant()) {
        if constexpr (kIsUnsignedInt<_NumberType>) {
            return _number;
        }
        else {
            return _number < static_cast<_NumberType>(0) ? -_number : _number;
        }
    }
    else {
        return std::abs(_number);
    }
}

/*
    Extracts the significand (mantissa) (f) from a floating-point number,
    normalized to the range [1, 2).

    This is achieved by preserving the mantissa bits of the original number
    but overwriting the exponent bits to represent the value 1.0 (2^0).

    @param _number The input floating-point number.
    @return The significand in range [1, 2).
*/
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD constexpr _NumberType FloatSignificand(_NumberType _number) noexcept {
    using Traits = internal::FloatTraits<_NumberType>;
    using UIntType = typename Traits::UIntType;

    const UIntType bits = std::bit_cast<UIntType>(_number);
    constexpr UIntType kExponentOne = std::bit_cast<UIntType>(static_cast<_NumberType>(1));
    const UIntType new_bits = (bits & Traits::kMantissaMask) | kExponentOne;

    return std::bit_cast<_NumberType>(new_bits);
}

/*
    Extracts the unbiased binary exponent (k) from a floating-point number.
    Formula: number = significand * 2^k

    @param _number The input floating-point number.
    @return The integer exponent value.
*/
template<typename _NumberType>
requires kIsFloat<_NumberType>
NODISCARD constexpr Int32 FloatExponent(_NumberType _number) noexcept {
    using Traits = internal::FloatTraits<_NumberType>;
    using UIntType = typename Traits::UIntType;

    constexpr Int32 kMantissaBits = std::numeric_limits<_NumberType>::digits - 1;
    constexpr Int32 kExponentBias = std::numeric_limits<_NumberType>::max_exponent - 1;

    const UIntType bits = std::bit_cast<UIntType>(_number);

    return static_cast<Int32>((bits & Traits::kExpMask) >> kMantissaBits) - kExponentBias;
}

} // math
} // zengine