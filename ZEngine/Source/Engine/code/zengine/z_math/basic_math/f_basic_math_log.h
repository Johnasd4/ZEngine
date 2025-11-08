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

namespace zengine {
namespace math {

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
constexpr const NumberType LnCalculateA(const NumberType _number) noexcept {
    Int32 k;
    NumberType f, ff_d_2;
    NumberType s, s_2, s_4;
    NumberType r, r_1, r_2;
    //calculate s f k
    k = FloatMantissa(_number);
    f = FloatExponent(_number);
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
constexpr const NumberType LnA(const NumberType _number) noexcept {
    if (IsInvalid(_number) || _number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (_number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(_number);
}

/*
    Calculate log(number) with the base of 2.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType Log2A(const NumberType _number) noexcept {
    if (IsInvalid(_number) || _number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (_number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(_number) * static_cast<NumberType>(internal::kLogFactorLn2Reciprocal);
}

/*
    Calculate log(number) with the base of 10.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType LgA(const NumberType _number) noexcept {
    if (IsInvalid(_number) || _number <= static_cast<NumberType>(0) ) {
        return NaN<NumberType>();
    }
    else if (_number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(_number) * static_cast<NumberType>(internal::kLogFactorLn10Reciprocal);
}

/*
    Calculate log(logarithm_number) with the base of base_number.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
constexpr const NumberType LogA(const NumberType _logarithm_number, const NumberType _base_number) noexcept {
    if (IsInvalid(_logarithm_number) || IsInvalid(_base_number) || 
        _logarithm_number <= static_cast<NumberType>(0) || _base_number <= static_cast<NumberType>(0)) {
        return NaN<NumberType>();
    }
    else if (_base_number == static_cast<NumberType>(1)) {
        return static_cast<NumberType>(0);
    }
    return internal::LnCalculateA(_logarithm_number) / internal::LnCalculateA(_base_number);
}

}//math
}//zengine