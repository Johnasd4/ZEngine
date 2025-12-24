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

#include "../../z_core/t_lookup_table.h"

#include "f_basic_math_valid.h"

namespace zengine {
namespace math {

namespace internal {

constexpr inline SizeType kFactorialTableSize = 171;
constexpr inline Float64 kFactorialTableRadianStepDistance = 1.0;
constexpr inline Float64 kFactorialTableOffset = 0.0;

/*
    Contains the value of factorial 1 - 170.
*/
inline constexpr TLookupTable<Float64, kFactorialTableSize> kFactorialTable = TLookupTable<Float64, kFactorialTableSize>(
    [](TLookupTable<Float64, kFactorialTableSize>* _table_ptr) {
        (*_table_ptr)[0] = 1.0;
        for (SizeType index = 1; index < _table_ptr->GetSize(); ++index) {
            (*_table_ptr)[index] = (*_table_ptr)[index - 1] * static_cast<Float64>(index);
        }
    });

/*
    Contains the value of the reciprocal of factorial 1 - 170.
*/
inline constexpr TLookupTable<Float64, kFactorialTableSize> kFactorialReciprocalTable =
TLookupTable<Float64, kFactorialTableSize>(
    [](TLookupTable<Float64, kFactorialTableSize>* _table_ptr) {
        for (SizeType index = 0; index < _table_ptr->GetSize(); ++index) {
            (*_table_ptr)[index] = 1.0 / kFactorialTable[index];
        }
    });

inline constexpr SizeType kFactorialFloat64MaxIndex = 170;
inline constexpr SizeType kFactorialFloat32MaxIndex = 34;

}//internal

/*
    Return the factorial of the given order.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr NumberType Factorial(const NumberType _order) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        if (_order > internal::kFactorialFloat64MaxIndex) {
            return InfP<Float64>();
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        if (_order > internal::kFactorialFloat32MaxIndex) {
            return InfP<Float64>();
        }
    }
    else{
        if (_order > internal::kFactorialFloat64MaxIndex) {
            return static_cast<NumberType>(0);
        }
    }
    return static_cast<NumberType>(internal::kFactorialTable.At(_order));
}

/*
    Return the reciprocal factorial of the given order.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr NumberType FactorialReciprocal(const NumberType _order) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        if (_order > internal::kFactorialFloat64MaxIndex) { 
            return static_cast<NumberType>(0);
        }
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        if (_order > internal::kFactorialFloat32MaxIndex) {
            return static_cast<NumberType>(0);
        }
    }
    return static_cast<NumberType>(internal::kFactorialReciprocalTable.At(_order));
}

}//math
}//zengine