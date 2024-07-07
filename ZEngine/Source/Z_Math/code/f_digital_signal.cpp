#define MATH_DLLFILE

#include "f_digital_signal.h"

#include "f_basic.h"

namespace zengine {
namespace math {

namespace internal{

constexpr IndexType kLowPassFilterSecondOrderMinCalculateSize = 3;


//TODO(Johnasd4):Break the function into two function.
/*
    Input : X(n)
    Output : Y(n)
    Y(n) * a0 = b0 * X(n) + b1* X(n - 1) + b2 * X(n - 2) - a1 * Y(n - 1) - a2 * Y(n - 2)
    Y(n) = (b0 * X(n) + b1* X(n - 1) + b2 * X(n - 2) - a1 * Y(n - 1) - a2 * Y(n - 2)) / a0

    0 < pole_damping < 1 :

        w = 2 * pi * pole_frequency                         o = pole_damping * w
        w_d = w * sqrt(1 - pole_damping * pole_damping)     e = exp(-o * sample_frquency)
        c = Cos(w_d * sample_frquency)                      e_2 = e * e

        a_0 = 1                                             b_0 = (1 - 2 * e * c + o_2) * 0.25
        a_1 = -2 * e * c                                    b_1 = 2 * b_0
        a_2 = o * o                                         b_2 = b_0

    pole_damping == 1 :

        w = 2 * pi * pole_frequency                         e = exp(-w * sample_frquency)
                                                            e_2 = e * e                               
    pole_damping > 1 :

        w = 2 * pi * pole_frequency                         o = pole_damping * w
        w_d = w * sqrt(pole_damping * pole_damping - 1)     e_1 = exp((-o - w_d) * sample_frquency)
                                                            e_2 = exp((-o + w_d) * sample_frquency)

        a_0 = 1                                             b_0 = (1 + a_1 + a_2) * 0.25
        a_1 = -(e_1 + e_2)                                  b_1 = 2 * b_0
        a_2 = e_1 * e_2                                     b_2 = b_0
*/
template<typename NumberType>
FORCEINLINE Void DigitalSignalLPF2Calculate(ZArray<NumberType>* ans_array_ptr, const ZArray<NumberType>& src_array, 
                                              const NumberType sample_period, const NumberType pole_frequency,
                                              const NumberType pole_damping) noexcept {
    /***** Prepare for calculation *****/
    ans_array_ptr->Clear();
    ans_array_ptr->PushEmpty(src_array.size());

    /***** Calculate the parameters. *****/
    NumberType a_1, a_2, b_0, b_1, b_2;
    //pole_damping < 0
    if (pole_damping < static_cast<NumberType>(0)) {

    }
    //pole_damping == 0
    else if (pole_damping == static_cast<NumberType>(0)) {

    }
    //0 < pole_damping < 1
    else if (pole_damping < static_cast<NumberType>(1)) {
        NumberType w = static_cast<NumberType>(k2PI64) * pole_frequency;
        NumberType o = pole_damping * w;
        NumberType w_d = w * sqrt(1 - pole_damping * pole_damping);
        NumberType e = exp(-o * sample_period);

        a_1 = static_cast<NumberType>(-2) * e * Cos(w_d * sample_period);
        a_2 = e * e;
    }
    //pole_damping == 1
    else if (pole_damping == static_cast<NumberType>(1)) {
        NumberType w = static_cast<NumberType>(k2PI64) * pole_frequency;
        NumberType e = exp(-w * sample_period);

        a_1 = static_cast<NumberType>(-2) * e;
        a_2 = e * e;
    }
    //pole_damping > 1
    else {
        NumberType w = static_cast<NumberType>(k2PI64) * pole_frequency;
        NumberType o = pole_damping * w;
        NumberType w_d = w * sqrt(pole_damping * pole_damping - 1);
        NumberType e_1 = exp((-o - w_d) * sample_period);
        NumberType e_2 = exp((-o + w_d) * sample_period);

        a_1 = -(e_1 + e_2);
        a_2 = e_1 * e_2;
    }

    b_0 = (static_cast<NumberType>(1) + a_1 + a_2) * static_cast<NumberType>(0.25);
    b_1 = static_cast<NumberType>(2) * b_0;
    b_2 = b_0;

    /***** Calculate the answer. *****/
    if (pole_frequency < kLowPassFilterSecondOrderMinCalculateSize) {
        memcpy(reinterpret_cast<Address>(&(*ans_array_ptr)(0)), 
               reinterpret_cast<Address>(const_cast<NumberType*>((&src_array(0)))),
               src_array.size() * sizeof(NumberType));
    }
    else {
        (*ans_array_ptr)(0) = b_0 * src_array(0);
        (*ans_array_ptr)(1) = b_0 * src_array(1) + b_1 * src_array(0) - a_1 * (*ans_array_ptr)(0);
        for (IndexType index = 2; index < ans_array_ptr->size(); ++index) {
            (*ans_array_ptr)(index) = b_0 * src_array(index) + b_1 * src_array(index - 1) + b_1 * src_array(index - 2) -
                                      a_1 * (*ans_array_ptr)(index - 1) - a_2 * (*ans_array_ptr)(index - 2);
        }
    }


}

}//internal

/*
    Float32 version of Second order low pass filter.
*/
MATH_DLLAPI Void DigitalSignalLPF2(ZArray<Float32>* ans_array_ptr, const ZArray<Float32>& src_array,
                                   const Float32 sample_frquency, const Float32 pole_frequency, 
                                   const Float32 pole_damping) noexcept {
    internal::DigitalSignalLPF2Calculate(ans_array_ptr, src_array, sample_frquency,
                                                         pole_frequency, pole_damping);
}

/*
    Float64 version of Second order low pass filter.
*/
MATH_DLLAPI Void DigitalSignalLPF2(ZArray<Float64>* ans_array_ptr, const ZArray<Float64>& src_array,
                                   const Float64 sample_frquency, const Float64 pole_frequency,
                                   const Float64 pole_damping) noexcept {
    internal::DigitalSignalLPF2Calculate(ans_array_ptr, src_array, sample_frquency,
                                                         pole_frequency, pole_damping);
}



}//math
}//zengine
