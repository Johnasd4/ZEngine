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

#include "f_basic_math_valid.h"

namespace zengine {
namespace math {

namespace internal {

constexpr Float64 kExpFactorLn2Reciprocal = 1.4426950408889634074;
constexpr Float64 kExpFactorTaylorSeries0 = 1;
constexpr Float64 kExpFactorTaylorSeries1 = 0.69314718055994528623;
constexpr Float64 kExpFactorTaylorSeries2 = 0.24022650695910069407;
constexpr Float64 kExpFactorTaylorSeries3 = 0.055504108664821569241;
constexpr Float64 kExpFactorTaylorSeries4 = 0.009618129107628475144;
constexpr Float64 kExpFactorTaylorSeries5 = 0.0013333558146428441116;
constexpr Float64 kExpFactorTaylorSeries6 = 0.00015403530393381608776;
constexpr Float64 kExpFactorTaylorSeries7 = 1.5252733804059837695e-05;
constexpr Float64 kExpFactorTaylorSeries8 = 1.3215486790144305273e-06;
constexpr Float64 kExpFactorTaylorSeries9 = 1.0178086009239697275e-07;
constexpr Float64 kExpFactorTaylorSeries10 = 7.0549116208011200503e-09;
constexpr Float64 kExpFactorTaylorSeries11 = 4.4455382718708100739e-10;
constexpr Float64 kExpFactorTaylorSeries12 = 2.5678435993488195818e-11;
constexpr Float64 kExpFactorTaylorSeries13 = 1.3691488853904122045e-12;
constexpr Float64 kExpFactorTaylorSeries14 = 6.7787263548225425375e-14;
constexpr Float64 kExpFactorTaylorSeries15 = 3.1324367070884270832e-15;
constexpr Float64 kExpFactorTaylorSeries16 = 1.3570247948755140855e-16;
constexpr Float64 kExpFactorTaylorSeries17 = 5.5330465324582383199e-18;
constexpr Float64 kExpFactorTaylorSeries18 = 2.1306753354891165899e-19;
constexpr Float64 kExpFactorTaylorSeries19 = 7.7730084288573506188e-21;
constexpr Float64 kExpFactorTaylorSeries20 = 2.6939194384655817623e-22;
constexpr Float64 kExpFactorTaylorSeries21 = 8.8918222068002340135e-24;

//13 and 5
/*
    Calculate exp(x).
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD constexpr const NumberType ExpCalculateA(const NumberType _exponent) noexcept {
    NumberType pow_2_exponent = _exponent * static_cast<NumberType>(kExpFactorLn2Reciprocal);
    NumberType ans_1, ans_2;
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Int64 int_part = static_cast<Int64>(pow_2_exponent);
        NumberType decimal_part = pow_2_exponent - int_part;
        NumberType decimal_part_2 = decimal_part * decimal_part;
        ans_1 =
            decimal_part * (static_cast<NumberType>(kExpFactorTaylorSeries1) +
                decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries3) +
                    decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries3) +
                        decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries5) +
                            decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries7) +
                                decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries9) +
                                    decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries11) +
                                        decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries13) +
                                            decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries15) +
                                                decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries17) +
                                                    decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries19) +
                                                        decimal_part_2 * static_cast<NumberType>(kExpFactorTaylorSeries21))))))))))));
        ans_2 = static_cast<NumberType>(kExpFactorTaylorSeries0) +
            decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries2) +
                decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries4) +
                    decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries6) +
                        decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries8) +
                            decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries10) +
                                decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries12) +
                                    decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries14) +
                                        decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries16) +
                                            decimal_part_2 * (static_cast<NumberType>(kExpFactorTaylorSeries18) +
                                                decimal_part_2 * static_cast<NumberType>(kExpFactorTaylorSeries20))))))))));
        Size64Union ans(ans_1 + ans_2);
        ans.u_int_64_ += int_part << kFloat64ExponentSize;
        return ans.float_64_;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Int32 int_part = static_cast<Int32>(pow_2_exponent) + kFloat32MantissaOffset;
        NumberType decimal_part = pow_2_exponent - int_part;
        
    }
}

}//internal

/*
    Calculate Exp(logarithm_number).
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD constexpr const NumberType ExpA(const NumberType _exponent_number) noexcept {
    if (IsInvalid(_exponent_number)) {
        return NaN<NumberType>();
    }
    return internal::ExpCalculateA(_exponent_number);
}

/*
    Calculate Exp(logarithm_number).
*/
template<typename NumberType1, typename NumberType2>
requires std::is_arithmetic_v<NumberType1> && std::is_arithmetic_v<NumberType2>
NODISCARD constexpr const auto PowA(const NumberType1 _base_number, const NumberType2 _exponent_number) noexcept {
    return std::pow(_base_number, _exponent_number);
}

}//math
}//zengine
