/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "../drive.h"

#include "../z_interpolation_table.h"

namespace zengine {
namespace math {

namespace internal {

inline constexpr Int32 kSin64TaylorSeriesFactor = 13;
inline constexpr Int32 kSin32TaylorSeriesFactor = 5;
inline constexpr Int32 kCos64TaylorSeriesFactor = 13;
inline constexpr Int32 kCos32TaylorSeriesFactor = 5;

inline constexpr Float64 kSinCosFactorTaylorSeries1 = 1;
inline constexpr Float64 kSinCosFactorTaylorSeries2 = 0.16666666666666665741;
inline constexpr Float64 kSinCosFactorTaylorSeries3 = 0.0083333333333333332177;
inline constexpr Float64 kSinCosFactorTaylorSeries4 = 0.00019841269841269838542;
inline constexpr Float64 kSinCosFactorTaylorSeries5 = 2.7557319223985888276E-06;
inline constexpr Float64 kSinCosFactorTaylorSeries6 = 2.5052108385441720224E-08;
inline constexpr Float64 kSinCosFactorTaylorSeries7 = 1.6059043836821618511E-10;
inline constexpr Float64 kSinCosFactorTaylorSeries8 = 7.6471637318198153958E-13;
inline constexpr Float64 kSinCosFactorTaylorSeries9 = 2.8114572543455198092E-15;
inline constexpr Float64 kSinCosFactorTaylorSeries10 = 8.2206352466243294955E-18;
inline constexpr Float64 kSinCosFactorTaylorSeries11 = 1.9572941063391259586E-20;

}//internal

/*
    Calculate the accurate Sin.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD constexpr const NumberType SinA(const NumberType _radian) noexcept {
    //Makes sure the value is between [-PI,PI] 
    NumberType calculate_radian = _radian - 
        static_cast<NumberType>(
            static_cast<Int32>((_radian + Sgn2(_radian) * static_cast<NumberType>(kPI64)) / 
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
NODISCARD FORCEINLINE constexpr const NumberType CosA(const NumberType _radian) noexcept {
    return SinA(_radian + static_cast<NumberType>(kHalfPI64));
}


namespace internal {

inline constexpr SizeType kSinCosTableSize = 1024;
inline constexpr Float64 kSinCosTableRadianStepDistance = k2PI64 / static_cast<Float64>(kSinCosTableSize);
inline constexpr Float64 kSinCosTableOffset = 0.0;

/*
    Contains the value of sin.
*/
inline constexpr ZInterpolationTable<Float64, kSinCosTableSize> kSinCosTable = ZInterpolationTable<Float64, kSinCosTableSize>(
    kSinCosTableOffset, kSinCosTableRadianStepDistance,
    [](ZInterpolationTable<Float64, kSinCosTableSize>* _table_ptr) {
        for (SizeType index = 0; index < _table_ptr->Capacity(); ++index) {
            (*_table_ptr)[index] =
                SinA<Float64>(static_cast<Float64>(index * kSinCosTableRadianStepDistance));
        }
    });

inline constexpr Float64 kSinSearchOffset = 0.0;
inline constexpr Float64 kCosSearchOffset = kHalfPI64;

}//internal

/*
    Calculate Sin, use calculate SinA() for more accurate answer.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType Sin(const NumberType _radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(_radian + static_cast<NumberType>(internal::kSinSearchOffset)));
}

/*
    Calculate Cos, use calculate CosA() for more accurate answer.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType Cos(const NumberType _radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopLinearSearchTable(_radian + static_cast<NumberType>(internal::kCosSearchOffset)));
}

/*
    The fastest Sin.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType SinF(const NumberType _radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopSearchTable(_radian + static_cast<NumberType>(internal::kSinSearchOffset)));
}

/*
    The fastest Cos.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType CosF(const NumberType _radian) {
    return static_cast<NumberType>(
        internal::kSinCosTable.LoopSearchTable(_radian + static_cast<NumberType>(internal::kCosSearchOffset)));
}

}//math
}//zengine