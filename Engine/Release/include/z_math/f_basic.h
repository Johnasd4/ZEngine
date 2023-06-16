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
    If number is not NaN or inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const Bool IsValid(const NumberType number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Type temp_number(number);
        return (temp_number.u_int_64 & internal::kFloat64MantissaMask) != internal::kFloat64MantissaInvalidValueBinary;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Type temp_number(number);
        return (temp_number.u_int_32 & internal::kFloat32MantissaMask) != internal::kFloat32MantissaInvalidValueBinary;
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
__forceinline constexpr NumberType Sgn2(const NumberType number) {
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
__forceinline constexpr NumberType Sgn3(const NumberType number) {
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
#pragma region factorial

namespace internal {

constexpr IndexType kFactorialTableSize = 171;
constexpr Float64 kFactorialTableRadianStepDistance = 1.0;
constexpr Float64 kFactorialTableOffset = 0.0;

/*
    Contains the value of factorial 1 - 170.
*/
constexpr ZInterpolationTable<Float64, kFactorialTableSize,true> kFactorialTable = 
    ZInterpolationTable<Float64, kFactorialTableSize, true>(
        [](ZInterpolationTable<Float64, kFactorialTableSize, true>* table_ptr) {
            (*table_ptr)(0) = 1.0;
            for (IndexType index = 1; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = (*table_ptr)(index - 1) * static_cast<Float64>(index);
            }
        });

/*
    Contains the value of the reciprocal of factorial 1 - 170.
*/
constexpr ZInterpolationTable<Float64, kFactorialTableSize, true> kFactorialReciprocalTable =
ZInterpolationTable<Float64, kFactorialTableSize, true>(
    [](ZInterpolationTable<Float64, kFactorialTableSize, true>* table_ptr) {
        for (IndexType index = 0; index < table_ptr->size(); ++index) {
            (*table_ptr)(index) = 1.0 / kFactorialTable(index);
        }
    });

constexpr IndexType kFactorialFloat64MaxIndex = 170;
constexpr IndexType kFactorialFloat32MaxIndex = 34;

}//internal

/*
    Returns  1 when number >= 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
__forceinline constexpr NumberType Factorial(const IndexType order) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        if (order > internal::kFactorialFloat64MaxIndex) {
            return InfP<Float64>();
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        if (order > internal::kFactorialFloat32MaxIndex) {
            return InfP<Float64>();
        }
    }
    else{
        if (order > internal::kFactorialFloat64MaxIndex) {
            return static_cast<NumberType>(0);
        }
    }
    return static_cast<NumberType>(internal::kFactorialTable.At(order));
}

/*
    Returns  1 when number >= 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr NumberType FactorialReciprocal(const IndexType order) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        if (order > internal::kFactorialFloat64MaxIndex) {
            return static_cast<NumberType>(0);
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        if (order > internal::kFactorialFloat32MaxIndex) {
            return static_cast<NumberType>(0);
        }
    }
    return static_cast<NumberType>(internal::kFactorialReciprocalTable.At(order));
}

#pragma endregion //factorial
#pragma region triganometric

namespace internal {

constexpr Int32 kSin64TaylorSeriesFactor = 13;
constexpr Int32 kSin32TaylorSeriesFactor = 5;
constexpr Int32 kCos64TaylorSeriesFactor = 13;
constexpr Int32 kCos32TaylorSeriesFactor = 5;

constexpr Float64 kSinCosFactorTaylorSeries1 = 1;
constexpr Float64 kSinCosFactorTaylorSeries2 = 0.16666666666666665741;
constexpr Float64 kSinCosFactorTaylorSeries3 = 0.0083333333333333332177;
constexpr Float64 kSinCosFactorTaylorSeries4 = 0.00019841269841269838542;
constexpr Float64 kSinCosFactorTaylorSeries5 = 2.7557319223985888276E-06;
constexpr Float64 kSinCosFactorTaylorSeries6 = 2.5052108385441720224E-08;
constexpr Float64 kSinCosFactorTaylorSeries7 = 1.6059043836821618511E-10;
constexpr Float64 kSinCosFactorTaylorSeries8 = 7.6471637318198153958E-13;
constexpr Float64 kSinCosFactorTaylorSeries9 = 2.8114572543455198092E-15;
constexpr Float64 kSinCosFactorTaylorSeries10 = 8.2206352466243294955E-18;
constexpr Float64 kSinCosFactorTaylorSeries11 = 1.9572941063391259586E-20;

}//internal

/*
    Calculate the accurate Sin.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType SinA(const NumberType radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    NumberType calculate_radian = radian - 
        static_cast<NumberType>(
            static_cast<Int32>((radian + Sgn2(radian) * static_cast<NumberType>(kPI64)) / 
                static_cast<NumberType>(k2PI64))) * 
        static_cast<NumberType>(k2PI64);
    NumberType ans;
    if (calculate_radian > static_cast<NumberType>(kPI64 * 0.5)) {
        ans = static_cast<NumberType>(-1);
        calculate_radian -= static_cast<NumberType>(kPI64);
    }
    else if (calculate_radian < static_cast<NumberType>(-kPI64 * 0.5)) {
        ans = static_cast<NumberType>(-1);
        calculate_radian += static_cast<NumberType>(kPI64);
    }
    else {
        ans = static_cast<NumberType>(1);
    }
    NumberType calculate_radian_2 = calculate_radian * calculate_radian;
    NumberType calculate_radian_4 = calculate_radian_2 * calculate_radian_2;
    NumberType ans_1, ans_2;
    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        ans_1 = static_cast<NumberType>(internal::kSinCosFactorTaylorSeries1) +
            calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries3) +
                calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries5) +
                    calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries7) +
                        calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries9) +
                            calculate_radian_4 * static_cast<NumberType>(internal::kSinCosFactorTaylorSeries11)))));
        ans_2 =
            calculate_radian_2 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries2) +
                calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries4) +
                    calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries6) +
                        calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries8) +
                            calculate_radian_4 * static_cast<NumberType>(internal::kSinCosFactorTaylorSeries10)))));

    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        ans_1 = static_cast<NumberType>(internal::kSinCosFactorTaylorSeries1) +
            calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries3) +
                calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries5)));
        ans_2 =
            calculate_radian_2 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries2) +
                calculate_radian_4 * (static_cast<NumberType>(internal::kSinCosFactorTaylorSeries4) +
                    calculate_radian_4 * static_cast<NumberType>(internal::kSinCosFactorTaylorSeries6)));
    }
    ans *= (ans_1 - ans_2) * calculate_radian;
    return ans;
}

/*
    Calculate the accurate Cos.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType CosA(const NumberType radian) noexcept {
    return SinA(radian + static_cast<NumberType>(kHalfPI64));
}


namespace internal {

constexpr IndexType kSinCosTableSize = 1024;
constexpr Float64 kSinCosTableRadianStepDistance = k2PI64 / static_cast<Float64>(kSinCosTableSize);
constexpr Float64 kSinCosTableOffset = 0.0;

/*
    Contains the value of sin.
*/
constexpr ZInterpolationTable<Float64, kSinCosTableSize> kSinCosTable = ZInterpolationTable<Float64, kSinCosTableSize>(
    kSinCosTableOffset, kSinCosTableRadianStepDistance,
    [](ZInterpolationTable<Float64, kSinCosTableSize>* table_ptr) {
        for (IndexType index = 0; index < table_ptr->size(); ++index) {
            (*table_ptr)(index) =
                SinA<Float64>(static_cast<Float64>(index * kSinCosTableRadianStepDistance));
        }
    });

constexpr Float64 kSinSearchOffset = 0.0;
constexpr Float64 kCosSearchOffset = kHalfPI64;
constexpr Float64 kSinFastSearchOffset = 0.5;
constexpr Float64 kCosFastSearchOffset = kHalfPI64 + 0.5;

}//internal

/*
    Calculate Sin, use calculate SinA() for more accurate answer.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType Sin(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kSinSearchOffset)));
}

/*
    Calculate Cos, use calculate CosA() for more accurate answer.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType Cos(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kCosSearchOffset)));
}

/*
    The fastest Sin.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType SinF(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopSearchTable(radian + static_cast<NumberType>(internal::kSinSearchOffset)));
}

/*
    The fastest Cos.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType CosF(const NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopSearchTable(radian + static_cast<NumberType>(internal::kCosSearchOffset)));
}


#pragma endregion //triganometric
#pragma region log

namespace internal {

constexpr Float64 kLogFactorLn2High = 6.93147180369123816490E-01;
constexpr Float64 kLogFactorLn2Low = 1.90821492927058770002E-10;
constexpr Float64 kLogFactorSqrt2 = 1.4142135623730951F;
constexpr Float64 kLogFactorLg1 = 6.666666666666735130E-01;
constexpr Float64 kLogFactorLg2 = 3.999999999940941908E-01;
constexpr Float64 kLogFactorLg3 = 2.857142874366239149E-01;
constexpr Float64 kLogFactorLg4 = 2.222219843214978396E-01;
constexpr Float64 kLogFactorLg5 = 1.818357216161805012E-01;
constexpr Float64 kLogFactorLg6 = 1.531383769920937332E-01;
constexpr Float64 kLogFactorLg7 = 1.479819860511658591E-01;

constexpr Float64 kLogFactorLn2Reciprocal = 1.4426950408889634074;
constexpr Float64 kLogFactorLn10Reciprocal = 4.3429448190325182765E-01;

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
    NumberType r, r_1, r_2;
    //calculate s f k
    k = FloatMantissa(number);
    f = FloatExponent(number);
    if (f >= static_cast<NumberType>(kLogFactorSqrt2)) {
        f *= static_cast<NumberType>(0.5);
        ++k;
    }
    f -= 1;
    s = f / (2.0 + f);
    ff_d_2 = f * f * 0.5;
    s_2 = s * s;
    s_4 = s_2 * s_2;

    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        //calculate r
        r_1 = s_2 * (static_cast<NumberType>(kLogFactorLg1) + s_4 * (static_cast<NumberType>(kLogFactorLg3) +
            s_4 * (static_cast<NumberType>(kLogFactorLg5) + s_4 * static_cast<NumberType>(kLogFactorLg7))));
        r_2 = s_4 * (static_cast<NumberType>(kLogFactorLg2) + s_4 * (static_cast<NumberType>(kLogFactorLg4) +
            s_4 * static_cast<NumberType>(kLogFactorLg6)));
        r = r_1 + r_2;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        //calculate r
        r_1 = s_2 * (static_cast<NumberType>(kLogFactorLg1) + s_4 * static_cast<NumberType>(kLogFactorLg3));
        r_2 = s_4 * (static_cast<NumberType>(kLogFactorLg2) + s_4 * static_cast<NumberType>(kLogFactorLg4));
        r = r_1 + r_2;
    }
    //calculate Ln(x)
    return  k * static_cast<NumberType>(kLogFactorLn2High) -
        ((ff_d_2 - (s * (ff_d_2 + r) + k * static_cast<NumberType>(kLogFactorLn2Low))) - f);
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
    return internal::LnCalculateA(number) * static_cast<NumberType>(internal::kLogFactorLn2Reciprocal);
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
    return internal::LnCalculateA(number) * static_cast<NumberType>(internal::kLogFactorLn10Reciprocal);
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

namespace internal {

constexpr Float64 kExpFactorLn2Reciprocal = 1.4426950408889634074;
constexpr Float64 kExpFactorTaylorSeries0 = 1;
constexpr Float64 kExpFactorTaylorSeries1 = 0.69314718055994528623;
constexpr Float64 kExpFactorTaylorSeries2 = 0.69314718055994528623;
constexpr Float64 kExpFactorTaylorSeries3 = 0.3465735902799725876;
constexpr Float64 kExpFactorTaylorSeries4 = 0.009618129107628475144;
constexpr Float64 kExpFactorTaylorSeries5 = 0.0013333558146428441116;
constexpr Float64 kExpFactorTaylorSeries6 = 0.00015403530393381608776;
constexpr Float64 kExpFactorTaylorSeries7 = 1.5252733804059837695E-05;
constexpr Float64 kExpFactorTaylorSeries8 = 1.3215486790144305273E-06;
constexpr Float64 kExpFactorTaylorSeries9 = 1.0178086009239697275E-07;
constexpr Float64 kExpFactorTaylorSeries10 = 7.0549116208011200503E-09;
constexpr Float64 kExpFactorTaylorSeries11 = 4.4455382718708100739E-10;
constexpr Float64 kExpFactorTaylorSeries12 = 2.5678435993488195818E-11;
constexpr Float64 kExpFactorTaylorSeries13 = 1.3691488853904122045E-12;
constexpr Float64 kExpFactorTaylorSeries14 = 6.77872635482254254e-14;
constexpr Float64 kExpFactorTaylorSeries15 = 3.13243670708842708e-15;
constexpr Float64 kExpFactorTaylorSeries16 = 1.35702479487551409e-16;
constexpr Float64 kExpFactorTaylorSeries17 = 5.53304653245823832e-18;
constexpr Float64 kExpFactorTaylorSeries18 = 2.13067533548911659e-19;
constexpr Float64 kExpFactorTaylorSeries19 = 7.77300842885735062e-21;
constexpr Float64 kExpFactorTaylorSeries20 = 1.3691488853904122045E-12;
constexpr Float64 kExpFactorTaylorSeries21 = 1.3691488853904122045E-12;

//13 and 5
/*
    Calculate exp(x).
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const Float64 ExpCalculateA(const NumberType exponent) noexcept {
    NumberType pow_2_exponent = exponent * static_cast<NumberType>(kExpFactorLn2Reciprocal);

    if constexpr (std::is_same_v<NumberType, Float64>) {
        Int64 int_part = static_cast<Int64>(pow_2_exponent) + kFloat64MantissaOffset;
        NumberType decimal_part = pow_2_exponent - int_part;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Int32 int_part = static_cast<Int32>(pow_2_exponent) + kFloat32MantissaOffset;
        NumberType decimal_part = pow_2_exponent - int_part;
    }

    return  0.0;
}

}//internal

/*
    Calculate Exp(logarithm_number).
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType ExpA(const NumberType exponent_number) noexcept {
    if (IsInvalid(exponent_number)) {
        return NaN<NumberType>();
    }
    return internal::ExpCalculateA(exponent_number);
}

#pragma endregion //pow

}//math
}//zengine

#endif //!Z_MATH_F_BASIC_H_
