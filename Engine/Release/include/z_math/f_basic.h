#ifndef Z_MATH_F_BASIC_H_
#define Z_MATH_F_BASIC_H_

#include "internal/drive.h"

#include "z_interpolation_table.h"

namespace zengine {
namespace math {

#pragma region numeral

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

#pragma endregion //numeral
#pragma region triganometric

namespace internal {

constexpr Int32 kSin64TaylorSeriesFactor = 12;
constexpr Int32 kSin32TaylorSeriesFactor = 7;
constexpr Int32 kCos64TaylorSeriesFactor = 12;
constexpr Int32 kCos32TaylorSeriesFactor = 7;

}//internal

template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType SinA(const NumberType radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    NumberType calculate_radian = radian - 
        static_cast<NumberType>(
            static_cast<Int32>((radian + Sgn2State(radian) * static_cast<NumberType>(kPI64)) / 
                static_cast<NumberType>(k2PI64))) * 
        static_cast<NumberType>(k2PI64);
    NumberType ans = calculate_radian;
    NumberType term_value = calculate_radian;
    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        for (IndexType term = 1; term <= internal::kSin64TaylorSeriesFactor; ++term) {
            term_value *= -calculate_radian * calculate_radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        for (IndexType term = 1; term <= internal::kSin32TaylorSeriesFactor; ++term) {
            term_value *= -calculate_radian * calculate_radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    return ans;
}


template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType CosA(const NumberType radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    NumberType calculate_radian = radian -
        static_cast<NumberType>(
            static_cast<Int32>((radian + Sgn2State(radian) * static_cast<NumberType>(kPI64)) /
                static_cast<NumberType>(k2PI64))) *
        static_cast<NumberType>(k2PI64);
    NumberType ans = 1;
    NumberType term_value = 1;
    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        for (IndexType term = 1; term <= internal::kCos64TaylorSeriesFactor; ++term) {
            term_value *= -calculate_radian * calculate_radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        for (IndexType term = 1; term <= internal::kCos32TaylorSeriesFactor; ++term) {
            term_value *= -calculate_radian * calculate_radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    return ans;
}

namespace internal {

constexpr IndexType kTriganometricTableSize = 1024;
constexpr Float64 kTriganometricTableRadianStepDistance = k2PI64 / static_cast<Float64>(kTriganometricTableSize);

constexpr Float64 kInterpolationTableOffset = 0.0;

/*
    Contains the value of sin.
*/
constexpr ZInterpolationTable<Float64, kTriganometricTableSize> kSinCosTable =
    ZInterpolationTable<Float64, kTriganometricTableSize>(
        kInterpolationTableOffset, kTriganometricTableRadianStepDistance,
        [](ZInterpolationTable<Float64, kTriganometricTableSize>* table_ptr) {
            for (IndexType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = 
                    SinA<Float64>(static_cast<Float64>(index * kTriganometricTableRadianStepDistance));
            }
        });

constexpr Float64 kSinFastSinOffset = 0.0;
constexpr Float64 kCosFastCosOffset = kHalfPI64;

}//internal

template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType Sin(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kSinFastSinOffset)));
}

template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType Cos(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kCosFastCosOffset)));
}

#pragma endregion //triganometric
#pragma region log

namespace internal {

constexpr Float64 kLogParameterLn2High = 6.93147180369123816490E-01;
constexpr Float64 kLogParameterLn2Low = 1.90821492927058770002E-10;
constexpr Float64 kLogParameterSqrt2 = 1.4142135623730951F;
constexpr Float64 kLogParameterLg1 = 6.666666666666735130E-01;
constexpr Float64 kLogParameterLg2 = 3.999999999940941908E-01;
constexpr Float64 kLogParameterLg3 = 2.857142874366239149E-01;
constexpr Float64 kLogParameterLg4 = 2.222219843214978396E-01;
constexpr Float64 kLogParameterLg5 = 1.818357216161805012E-01;
constexpr Float64 kLogParameterLg6 = 1.531383769920937332E-01;
constexpr Float64 kLogParameterLg7 = 1.479819860511658591E-01;

constexpr Float64 kLogParameterLog2Reciprocal = 1.4426950408889634074;
constexpr Float64 kLogParameterLog10Reciprocal = 4.3429448190325182765E-01;

/*
    Ln(x) = Ln2(2^k * (1 + f))
    Ln(x) = k * Ln2 + Ln(1 + f)
    Ln(1 + f) = 2s + R(z)
    R(z) = Lg1 * s^2 + Lg2 * s^4 + Lg3 * s^6 + Lg4 * s^8 + Lg5 * s^10 + Lg6 * s^12 + Lg7 * s^14
    s = f / (2 + f)
    2s = f - s * f = f - f * f / 2 + s * f * f / 2
    ff_d_2 = f * f * 0.5
    Ln(x) = k * Ln2High - ((ff_d_2 - (s * (ff_d_2 + R(z)) + k * Ln2Low)) - f)
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const Float64 LnCalculateA(const NumberType number) noexcept {
    Int32 k;
    NumberType f, ff_d_2;
    NumberType s, s_2, s_4;
    NumberType r;
    //calculate s f k
    k = FloatMantissa(number);
    f = FloatExponent(number);
    if (f >= static_cast<NumberType>(kLogParameterSqrt2)) {
        f *= 0.5;
        ++k;
    }
    f -= 1;
    s = f / (2.0 + f);
    ff_d_2 = f * f * 0.5;
    s_2 = s * s;
    s_4 = s_2 * s_2;

    //calculate r
    r = s_2 * (static_cast<NumberType>(kLogParameterLg1) + s_4 * (static_cast<NumberType>(kLogParameterLg3) + 
            s_4 * (static_cast<NumberType>(kLogParameterLg5) + s_4 * static_cast<NumberType>(kLogParameterLg7)))) +
        s_4 * (static_cast<NumberType>(kLogParameterLg2) + s_4 * (static_cast<NumberType>(kLogParameterLg4) + 
            s_4 * static_cast<NumberType>(kLogParameterLg6)));

    //calculate Ln(x)
    return  k * static_cast<NumberType>(kLogParameterLn2High) -
        ((ff_d_2 - (s * (ff_d_2 + r) + k * static_cast<NumberType>(kLogParameterLn2Low))) - f);
}

}//internal

/*
    Calculate log(number) with the base of e.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType LnA(const NumberType number) noexcept {
    if (IsInvalid(number) || number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(number);
}

/*
    Calculate log(number) with the base of 2.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType Log2A(const NumberType number) noexcept {
    if (IsInvalid(number) || number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(number) * static_cast<NumberType>(internal::kLogParameterLog2Reciprocal);
}

/*
    Calculate log(number) with the base of 10.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType LgA(const NumberType number) noexcept {
    if (IsInvalid(number) || number <= static_cast<NumberType>(0) ) {
        return NaN<NumberType>();
    }
    else if (number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(number) * static_cast<NumberType>(internal::kLogParameterLog10Reciprocal);
}

/*
    Calculate log(logarithm_number) with the base of base_number.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType LogA(const NumberType logarithm_number, const NumberType base_number) noexcept {
    if (IsInvalid(logarithm_number) || IsInvalid(base_number) || 
        logarithm_number <= static_cast<NumberType>(0) || base_number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (base_number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(logarithm_number) / internal::LnCalculateA(base_number);
}

#pragma endregion //log
#pragma region pow

//namespace internal {
//
//constexpr Float64 kPowParameterLn2High = 6.93147180369123816490E-01;
//constexpr Float64 kLogParameterLn2Low = 1.90821492927058770002E-10;
//constexpr Float64 kLogParameterSqrt2 = 1.4142135623730951F;
//constexpr Float64 kLogParameterLg1 = 6.666666666666735130E-01;
//constexpr Float64 kLogParameterLg2 = 3.999999999940941908E-01;
//constexpr Float64 kLogParameterLg3 = 2.857142874366239149E-01;
//constexpr Float64 kLogParameterLg4 = 2.222219843214978396E-01;
//constexpr Float64 kLogParameterLg5 = 1.818357216161805012E-01;
//constexpr Float64 kLogParameterLg6 = 1.531383769920937332E-01;
//constexpr Float64 kLogParameterLg7 = 1.479819860511658591E-01;
//
//constexpr Float64 kLogParameterLog2Reciprocal = 1.4426950408889634074;
//constexpr Float64 kLogParameterLog10Reciprocal = 4.3429448190325182765E-01;
//
///*
//    Ln(x) = Ln2(2^k * (1 + f))
//    Ln(x) = k * Ln2 + Ln(1 + f)
//    Ln(1 + f) = 2s + R(z)
//    R(z) = Lg1 * s^2 + Lg2 * s^4 + Lg3 * s^6 + Lg4 * s^8 + Lg5 * s^10 + Lg6 * s^12 + Lg7 * s^14
//    s = f / (2 + f)
//    2s = f - s * f = f - f * f / 2 + s * f * f / 2
//    ff_d_2 = f * f * 0.5
//    Ln(x) = k * Ln2High - ((ff_d_2 - (s * (ff_d_2 + R(z)) + k * Ln2Low)) - f)
//*/
//template<typename NumberType>
//requires std::is_floating_point_v<NumberType>
//constexpr const Float64 ExpCalculateA(const NumberType exponent_number) noexcept {
//    Int32 k;
//    NumberType f, ff_d_2;
//    NumberType s, s_2, s_4;
//    NumberType r;
//    //calculate s f k
//    k = FloatMantissa(exponent_number);
//    f = FloatExponent(exponent_number);
//    if (f >= static_cast<NumberType>(kLogParameterSqrt2)) {
//        f *= 0.5;
//        ++k;
//    }
//    f -= 1;
//    s = f / (2.0 + f);
//    ff_d_2 = f * f * 0.5;
//    s_2 = s * s;
//    s_4 = s_2 * s_2;
//
//    //calculate r
//    r = s_2 * (static_cast<NumberType>(kLogParameterLg1) + s_4 * (static_cast<NumberType>(kLogParameterLg3) + 
//            s_4 * (static_cast<NumberType>(kLogParameterLg5) + s_4 * static_cast<NumberType>(kLogParameterLg7)))) +
//        s_4 * (static_cast<NumberType>(kLogParameterLg2) + s_4 * (static_cast<NumberType>(kLogParameterLg4) + 
//            s_4 * static_cast<NumberType>(kLogParameterLg6)));
//
//    //calculate Ln(x)
//    return  k * static_cast<NumberType>(kLogParameterLn2High) -
//        ((ff_d_2 - (s * (ff_d_2 + r) + k * static_cast<NumberType>(kLogParameterLn2Low))) - f);
//}
//
//}//internal
//
//
///*
//    Calculate log(logarithm_number) with the base of base_number.
//*/
//template<typename NumberType>
//requires std::is_floating_point_v<NumberType>
//constexpr const NumberType Exp(const NumberType exponent_number) noexcept {
//    if (IsInvalid(exponent_number)) {
//        return NaN<NumberType>();
//    }
//    return internal::ExpCalculate(exponent_number);
//}

#pragma endregion //pow

}//math
}//zengine

#endif //!Z_MATH_F_BASIC_H_
