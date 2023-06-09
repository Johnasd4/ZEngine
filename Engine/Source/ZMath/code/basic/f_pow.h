#ifndef Z_MATH_F_POW_H_
#define Z_MATH_F_POW_H_

#include "internal\drive.h"

#include "z_interpolation_table.h"

namespace zengine {
namespace math {

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

}//math
}//zengine

#endif //!Z_MATH_F_POW_H_