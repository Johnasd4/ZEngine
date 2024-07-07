#ifndef Z_MATH_F_FOURIER_H_
#define Z_MATH_F_FOURIER_H_

#include "internal/drive.h"

#include "z_core/z_array.h"

#include "z_complex.h"

namespace zengine {
namespace math {

/*
    Float32 version of Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierFFT(ZArray<ZComplex<Float32>>* ans_array_ptr,
                            const ZArray<ZComplex<Float32>>& src_array) noexcept;

/*
    Float64 version of Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierFFT(ZArray<ZComplex<Float64>>* ans_array_ptr,
                            const ZArray<ZComplex<Float64>>& src_array) noexcept;

/*
    Float32 version of Inverse Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierIFFT(ZArray<ZComplex<Float32>>* ans_array_ptr,
                             const ZArray<ZComplex<Float32>>& src_array) noexcept;

/*
    Float32 version of Inverse Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierIFFT(ZArray<ZComplex<Float64>>* ans_array_ptr,
                             const ZArray<ZComplex<Float64>>& src_array) noexcept;

}//math
}//zengine


#endif //!Z_MATH_F_FOURIER_H_
