#ifndef Z_MATH_BASIC_F_TRIGANOMETRIC_H_
#define Z_MATH_BASIC_F_TRIGANOMETRIC_H_

#include "internal\drive.h"

#include "z_interpolation_table.h"

#include "f_numeral.h"

#pragma warning(disable : 26451)

namespace zengine {
namespace math {

namespace internal {

constexpr Int32 kSin64TaylorSeriesFactor = 12;
constexpr Int32 kSin32TaylorSeriesFactor = 7;
constexpr Int32 kCos64TaylorSeriesFactor = 12;
constexpr Int32 kCos32TaylorSeriesFactor = 7;

}//internal

template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType SinA(NumberType radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    radian -= 
        static_cast<NumberType>(
            static_cast<Int32>((radian + Sgn2State(radian) * static_cast<NumberType>(kPI64)) / 
                static_cast<NumberType>(k2PI64))) * 
        static_cast<NumberType>(k2PI64);
    NumberType ans = radian;
    NumberType term_value = radian;
    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        for (IndexType term = 1; term <= internal::kSin64TaylorSeriesFactor; ++term) {
            term_value *= -radian * radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        for (IndexType term = 1; term <= internal::kSin32TaylorSeriesFactor; ++term) {
            term_value *= -radian * radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    return ans;
}


template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType CosA(NumberType radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    radian -=
        static_cast<NumberType>(
            static_cast<Int32>((radian + Sgn2State(radian) * static_cast<NumberType>(kPI64)) /
                static_cast<NumberType>(k2PI64))) *
        static_cast<NumberType>(k2PI64);
    NumberType ans = 1;
    NumberType term_value = 1;
    //Calculate the taylor expansion.
    if constexpr (std::is_same_v<NumberType, Float64>) {
        for (IndexType term = 1; term <= internal::kCos64TaylorSeriesFactor; ++term) {
            term_value *= -radian * radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
            ans += term_value;
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        for (IndexType term = 1; term <= internal::kCos32TaylorSeriesFactor; ++term) {
            term_value *= -radian * radian / static_cast<NumberType>((term * 2 + 1) * term * 2);
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
__forceinline constexpr const NumberType Sin(NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kSinFastSinOffset)));
}

template<typename NumberType>
requires std::is_floating_point_v<NumberType>
__forceinline constexpr const NumberType Cos(NumberType radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(radian + static_cast<NumberType>(internal::kCosFastCosOffset)));
}

}//math
}//zengine

#pragma warning(default : 26451)

#endif //!Z_MATH_BASIC_F_TRIGANOMETRIC_H_