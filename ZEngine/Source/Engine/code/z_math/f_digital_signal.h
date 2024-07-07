#ifndef Z_MATH_F_DIGITAL_SIGNAL_H_
#define Z_MATH_F_DIGITAL_SIGNAL_H_

#include "internal/drive.h"

#include "z_core/z_array.h"

#include "z_complex.h"

namespace zengine {
namespace math {

/*
    Float32 version of Second order low pass filter.
*/
MATH_DLLAPI Void DigitalSignalLPF2(ZArray<Float32>* ans_array_ptr, const ZArray<Float32>& src_array,
                                          const Float32 sample_period, const Float32 pole_frequency,
                                          const Float32 pole_damping) noexcept;

/*
    Float64 version of Second order low pass filter.
*/
MATH_DLLAPI Void DigitalSignalLPF2(ZArray<Float64>* ans_array_ptr, const ZArray<Float64>& src_array,
                                          const Float64 sample_period, const Float64 pole_frequency,
                                          const Float64 pole_damping) noexcept;
}//math
}//zengine


#endif //!Z_MATH_F_DIGITAL_SIGNAL_H_
