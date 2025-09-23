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

#include "z_math/internal/z_drive.h"

namespace zengine {
namespace math {

/*
    Returns the mantissa of the Float number.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Int32 FloatMantissa(const NumberType _number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(_number);
        return 
            static_cast<Int32>(
                (temp_number.u_int_64_ & internal::kFloat64MantissaMask) >> internal::kFloat64ExponentSize)  -
            internal::kFloat64MantissaOffset;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(_number);
        return 
            static_cast<Int32>(
                (temp_number.u_int_32_ & internal::kFloat32MantissaMask) >> internal::kFloat32ExponentSize) -
            internal::kFloat32MantissaOffset;
    }
}

/*
    Returns the exponent of the Float number, the value's range is [1,2) U {0}.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType FloatExponent(const NumberType _number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(_number);
        if (temp_number.int_64_ != 0) {
            temp_number.u_int_64_ = (temp_number.u_int_64_ & internal::kFloat64ExponentWithoutSymbolMask) |
                internal::kFloat64MantissaZeroValue;
        }
        return temp_number.float_64_;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(_number);
        if (temp_number.int_32_ != 0) {
            temp_number.u_int_32_ = (temp_number.u_int_32_ & internal::kFloat32ExponentWithoutSymbolMask) |
                internal::kFloat32MantissaZeroValue;
        }
        return temp_number.float_32_;
    }
}

/*
    Returns  1 when number >= 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr NumberType Sgn2(const NumberType _number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return static_cast<NumberType>(1);
    }
    else {
        return static_cast<NumberType>((_number >= static_cast<NumberType>(0)) - (_number < static_cast<NumberType>(0)));
    }
}

/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr NumberType Sgn3(const NumberType _number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return _number == static_cast<NumberType>(0) ? static_cast<NumberType>(0) : static_cast<NumberType>(1);
    }
    else {
        return static_cast<NumberType>((_number > static_cast<NumberType>(0)) - (_number < static_cast<NumberType>(0)));
    }
}

/*
    Returns the absolute value of the number
*/
template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr NumberType Abs(const NumberType _number) {
    if constexpr (std::is_unsigned_v<NumberType>) {
        return _number;
    }
    else {
        return _number < static_cast<NumberType>(0) ? -_number : _number;
    }
}

}//math
}//zengine