#ifndef Z_MATH_BASIC_F_NUMERAL_H_
#define Z_MATH_BASIC_F_NUMERAL_H_

#include "internal\drive.h"

#include "z_interpolation_table.h"

#pragma warning(disable : 26451)

namespace zengine {
namespace math {

namespace internal {

constexpr Int32 kFloat64MantissaOffset = 1023;
constexpr UInt64 kFloat64MantissaMask = 0X7ff0000000000000ULL;
constexpr UInt64 kFloat64ExponentWithoutSymbolMask = 0X000fffffffffffffULL;
constexpr UInt64 kFloat64ExponentWithSymbolMask = 0X800fffffffffffffULL;
constexpr UInt64 kFloat64MantissaInvalidValueBinary = 0X7ff0000000000000ULL;
constexpr UInt64 kFloat64ExponentInvalidValueBinary = 0X0000000000000000ULL;
constexpr Int64 kFloat64ExponentSize = 52LL;
constexpr UInt64 kFloat64MantissaZeroValue = 0X3ff0000000000000ULL;
constexpr UInt64 kFloat64NaNBinary = 0XffffffffffffffffULL;
constexpr UInt64 kFloat64InfPositiveBinary = 0X7ff0000000000000ULL;
constexpr UInt64 kFloat64InfNegativeBinary = 0Xfff0000000000000ULL;

constexpr Int32 kFloat32MantissaOffset = 127;
constexpr UInt32 kFloat32MantissaMask = 0X7f800000U;
constexpr UInt32 kFloat32ExponentWithoutSymbolMask = 0X007fffffU;
constexpr UInt32 kFloat32ExponentWithSymbolMask = 0X807fffffU;
constexpr UInt32 kFloat32MantissaInvalidValueBinary = 0X7f800000U;
constexpr UInt32 kFloat32ExponentInvalidValueBinary = 0X00000000U;
constexpr Int32 kFloat32ExponentSize = 23;
constexpr UInt32 kFloat32MantissaZeroValue = 0X3f800000U;
constexpr UInt32 kFloat32NaNBinary = 0XffffffffU;
constexpr UInt32 kFloat32InfPositiveBinary = 0X7f800000U;
constexpr UInt32 kFloat32InfNegativeBinary = 0Xff800000U;

}//internal

/*
    Returns the mantissa of the Float number.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Int32 FloatMantissa(const NumberType number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(number);
        return 
            static_cast<Int32>(
                (temp_number.u_int_64 & internal::kFloat64MantissaMask) >> internal::kFloat64ExponentSize)  -
            internal::kFloat64MantissaOffset;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(number);
        return 
            static_cast<Int32>(
                (temp_number.u_int_32 & internal::kFloat32MantissaMask) >> internal::kFloat32ExponentSize) -
            internal::kFloat32MantissaOffset;
    }
}

/*
    Returns the exponent of the Float number, the value's range is [1,2) U {0}.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType FloatExponent(const NumberType number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(number);
        if (temp_number.int_64 != 0) {
            temp_number.u_int_64 = (temp_number.u_int_64 & internal::kFloat64ExponentWithoutSymbolMask) |
                internal::kFloat64MantissaZeroValue;
        }
        return temp_number.float_64;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(number);
        if (temp_number.int_32 != 0) {
            temp_number.u_int_32 = (temp_number.u_int_32 & internal::kFloat32ExponentWithoutSymbolMask) |
                internal::kFloat32MantissaZeroValue;
        }
        return temp_number.float_32;
    }
}

/*
    Returns a NaN. Can't be used to compare with another NaN, 
    Use IsNaN() to judge if a number is NaN.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType NaN() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(internal::kFloat64NaNBinary);
        return temp_number.float_64;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(internal::kFloat32NaNBinary);
        return temp_number.float_32;
    }
}

/*
    Returns inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType InfP() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(internal::kFloat64InfPositiveBinary);
        return temp_number.float_64;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(internal::kFloat32InfPositiveBinary);
        return temp_number.float_32;
    }
}

/*
    Returns -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType InfN() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(internal::kFloat64InfNegativeBinary);
        return temp_number.float_64;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(internal::kFloat32InfNegativeBinary);
        return temp_number.float_32;
    }
}

/*
    If number is NaN or inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsInvalid(const NumberType number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(number);
        return (temp_number.u_int_64 & internal::kFloat64MantissaMask) == internal::kFloat64MantissaInvalidValueBinary;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(number);
        return (temp_number.u_int_32 & internal::kFloat32MantissaMask) == internal::kFloat32MantissaInvalidValueBinary;
    }
}

/*
    If number is NaN.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsNaN(const NumberType number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(number);
        return
            ((temp_number.u_int_64 & internal::kFloat64MantissaMask) == internal::kFloat64MantissaInvalidValueBinary) &&
            ((temp_number.u_int_64 & internal::kFloat64ExponentWithoutSymbolMask) != 
            internal::kFloat64ExponentInvalidValueBinary);
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(number);
        return
            ((temp_number.u_int_32 & internal::kFloat32MantissaMask) == internal::kFloat32MantissaInvalidValueBinary) &&
            ((temp_number.u_int_32 & internal::kFloat32ExponentWithoutSymbolMask) != 
            internal::kFloat32ExponentInvalidValueBinary);
    }
}

/*
    If number is inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsInf(const NumberType number) {
    return number == InfP<NumberType>() || number == InfN<NumberType>();
}

/*
    If number is inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsInfP(const NumberType number) {
    return number == InfP<NumberType>();
}

/*
    If number is -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsInfN(const NumberType number) {
    return number == InfN<NumberType>();
}

/*
    Returns  1 when number >= 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr NumberType Sgn2State(const NumberType number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return static_cast<NumberType>(1);
    }
    else {
        return static_cast<NumberType>((number >= static_cast<NumberType>(0)) - (number < static_cast<NumberType>(0)));
    }
}

/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr NumberType Sgn3State(const NumberType number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return number == static_cast<NumberType>(0) ? static_cast<NumberType>(0) : static_cast<NumberType>(1);
    }
    else {
        return static_cast<NumberType>((number > static_cast<NumberType>(0)) - (number < static_cast<NumberType>(0)));
    }
}

/*
    Returns the absolute value of the number
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr NumberType Abs(const NumberType number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return number;
    }
    else {
        return number < static_cast<NumberType>(0) ? -number : number;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr const NumberType Max(const NumberType number_1, const NumberType number_2) {
    return number_1 > number_2 ? number_1 : number_2;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr const NumberType Min(const NumberType number_1, const NumberType number_2) {
    return number_1 < number_2 ? number_1 : number_2;
}

}//math
}//zengine

#pragma warning(default : 26451)

#endif //!Z_MATH_BASIC_F_NUMERAL_H_