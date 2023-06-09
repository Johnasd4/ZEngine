#define MATH_DLLFILE

#include "f_fourier.h"

#include "z_interpolation_table.h"

#include "basic/f_triganometric.h"

namespace zengine {
namespace math {

namespace internal{

constexpr IndexType FourierFFTButterflyTramsformOneDigitMask = 0x00000001;

//The max order of the butterfly tramsform index pre-calculated.
constexpr IndexType kButterflyTransformMaxOrder = 10;
constexpr IndexType kButterflyTransformTableOffset = 0;
constexpr IndexType kButterflyTransformTableStepSize = 1;
constexpr IndexType kButterflyTransformTableSize = 0x1 << (kButterflyTransformMaxOrder + 1);

/*
    Contains the butterfly tramsform index arrays.
*/
constexpr ZInterpolationTable<IndexType, kButterflyTransformTableSize> kButterflyTransformTable =
    ZInterpolationTable<IndexType, kButterflyTransformTableSize>(
        kButterflyTransformTableOffset, kButterflyTransformTableStepSize,
        [](ZInterpolationTable<IndexType, kButterflyTransformTableSize>* table_ptr) {
            (*table_ptr)(0) = 0;
            (*table_ptr)(1) = 0;
            for (IndexType order = 0; order < kButterflyTransformMaxOrder; ++order) {
                IndexType index_offset = 1 << (order + 1);
                (*table_ptr)(index_offset) = 0;
                for (IndexType index = 1; index < index_offset; ++index) {
                    (*table_ptr)(index + index_offset) = ((*table_ptr)((index >> 1) + index_offset) >> 1) | 
                        ((index & FourierFFTButterflyTramsformOneDigitMask) << order);
                }
            }

        });

template<typename NumberType, Bool kIsFFT>
static __forceinline Void FourierFFTAndIFFTCalculate(ZArray<ZComplex<NumberType>>* ans_array_ptr,
                                                     const ZArray<ZComplex<NumberType>>& src_array) {
    /***** Calculate the best size of the array. *****/
    IndexType index_type_temp;
    IndexType best_calculate_size = 1;
    //log2(size) - 1
    IndexType best_calculate_size_log2_m_1 = -1;    
    index_type_temp = src_array.size() - 1;
    while (index_type_temp != 0) {
        index_type_temp = index_type_temp >> 1;
        best_calculate_size = best_calculate_size << 1;
        ++best_calculate_size_log2_m_1;
    }

    /***** Prepare for the calculation. *****/
    //Initialize the ans array.
    ans_array_ptr->Clear();
    ans_array_ptr->PushEmpty(best_calculate_size);
    memset(&(*ans_array_ptr)(0), 0, sizeof(ZComplex<NumberType>) * best_calculate_size);

    /***** Butterfly Transform *****/
    //Use the bufferfly transform interpolation table.
    if (best_calculate_size_log2_m_1 + 1 <= kButterflyTransformMaxOrder) {
        IndexType index_offset = 1 << (best_calculate_size_log2_m_1 + 2);
        //Puts the number into the array by butterfly_transform_index_array.
        for (IndexType index = 0; index < src_array.size(); ++index) {
            (*ans_array_ptr)(kButterflyTransformTable.SearchTable(index + index_offset)) = src_array(index);
        }
    }
    //Calculate the bufferfly transform array.
    else {
        //Generate the butterfly transform array
        ZArray<IndexType> butterfly_transform_index_array(best_calculate_size);
        butterfly_transform_index_array.PushEmpty(best_calculate_size);
        butterfly_transform_index_array(0) = 0;
        //Only calculate to the size of the source array.
        for (IndexType index = 1; index < src_array.size(); ++index) {
            butterfly_transform_index_array(index) = (butterfly_transform_index_array(index >> 1) >> 1) |
                ((index & FourierFFTButterflyTramsformOneDigitMask) << best_calculate_size_log2_m_1);
        }
        //Puts the number into the array by butterfly_transform_index_array.
        for (IndexType index = 0; index < src_array.size(); ++index) {
            (*ans_array_ptr)(butterfly_transform_index_array(index)) = src_array(index);
        }
    }

    /***** Calculates the iteration. *****/
    IndexType group_num = best_calculate_size >> 1;
    //Loop log2(best_array_size) times.
    for (IndexType group_size_d_2 = 1; group_size_d_2 < best_calculate_size; group_size_d_2 = group_size_d_2 << 1) {
        NumberType group_size_d_2_reciprocal = static_cast<NumberType>(1) / static_cast<NumberType>(group_size_d_2);
        ZComplex<NumberType> w_n;
        if constexpr (kIsFFT) {
            w_n.real = Cos(static_cast<NumberType>(kPI64) * group_size_d_2_reciprocal);
            w_n.imag = Sin(static_cast<NumberType>(kPI64) * group_size_d_2_reciprocal);
        }
        else {
            w_n.real = Cos(static_cast<NumberType>(kPI64) * group_size_d_2_reciprocal);
            w_n.imag = -Sin(static_cast<NumberType>(kPI64) * group_size_d_2_reciprocal);
        }
        for (IndexType ans_index = 0; ans_index < best_calculate_size;) {
            ZComplex<NumberType> w_0(static_cast<NumberType>(1), static_cast<NumberType>(0));
            for (IndexType count = 0; count < group_size_d_2; ++count) {
                ZComplex<NumberType> term_1 = (*ans_array_ptr)(ans_index);
                ZComplex<NumberType> term_2 = w_0 * (*ans_array_ptr)(ans_index + group_size_d_2);
                (*ans_array_ptr)(ans_index) = term_1 + term_2;
                (*ans_array_ptr)(ans_index + group_size_d_2) = term_1 - term_2;
                w_0 *= w_n;
                ++ans_index;
            }
            ans_index += group_size_d_2;
        }
        group_num = group_num >> 1;
    }
}

}//internal

/*
    Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierFFT(ZArray<ZComplex<Float32>>* ans_array_ptr,
                            const ZArray<ZComplex<Float32>>& src_array) noexcept {
    internal::FourierFFTAndIFFTCalculate<Float32, true>(ans_array_ptr, src_array);
}

/*
    Float64 version of Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierFFT(ZArray<ZComplex<Float64>>* ans_array_ptr,
                            const ZArray<ZComplex<Float64>>& src_array) noexcept {
    internal::FourierFFTAndIFFTCalculate<Float64, true>(ans_array_ptr, src_array);
}

/*
    Float32 version of Inverse Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierIFFT(ZArray<ZComplex<Float32>>* ans_array_ptr,
                             const ZArray<ZComplex<Float32>>& src_array) noexcept {
    internal::FourierFFTAndIFFTCalculate<Float32, false>(ans_array_ptr, src_array);
}

/*
    Float32 version of Inverse Fast Fourier Tramsform.
*/
MATH_DLLAPI Void FourierIFFT(ZArray<ZComplex<Float64>>* ans_array_ptr,
                             const ZArray<ZComplex<Float64>>& src_array) noexcept {
    internal::FourierFFTAndIFFTCalculate<Float64, false>(ans_array_ptr, src_array);
}

}//math
}//zengine
