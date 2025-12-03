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

/*
    Returns a NaN. Can't be used to compare with another NaN, 
    Use IsNaN() to judge if a number is NaN.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType NaN() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(internal::kFloat64NaNBinary);
        return temp_number.float_64_;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(internal::kFloat32NaNBinary);
        return temp_number.float_32_;
    }
}

/*
    Returns inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType InfP() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(internal::kFloat64InfPositiveBinary);
        return temp_number.float_64_;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(internal::kFloat32InfPositiveBinary);
        return temp_number.float_32_;
    }
}

/*
    Returns -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const NumberType InfN() {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(internal::kFloat64InfNegativeBinary);
        return temp_number.float_64_;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(internal::kFloat32InfNegativeBinary);
        return temp_number.float_32_;
    }
}

/*
    If number is not NaN or inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsValid(const NumberType _number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(_number);
        return (temp_number.u_int_64_ & internal::kFloat64MantissaMask) != internal::kFloat64MantissaInvalidValueBinary;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(_number);
        return (temp_number.u_int_32_ & internal::kFloat32MantissaMask) != internal::kFloat32MantissaInvalidValueBinary;
    }
}

/*
    If number is NaN or inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsInvalid(const NumberType _number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(_number);
        return (temp_number.u_int_64_ & internal::kFloat64MantissaMask) == internal::kFloat64MantissaInvalidValueBinary;
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(_number);
        return (temp_number.u_int_32_ & internal::kFloat32MantissaMask) == internal::kFloat32MantissaInvalidValueBinary;
    }
}

/*
    If number is NaN.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsNaN(const NumberType _number) {
    if constexpr (std::is_same_v<NumberType, Float64>) {
        Size64Union temp_number(_number);
        return
            ((temp_number.u_int_64_ & internal::kFloat64MantissaMask) == internal::kFloat64MantissaInvalidValueBinary) &&
            ((temp_number.u_int_64_ & internal::kFloat64ExponentWithoutSymbolMask) !=
            internal::kFloat64ExponentInvalidValueBinary);
    }
    else if constexpr (std::is_same_v<NumberType, Float32>) {
        Size32Union temp_number(_number);
        return
            ((temp_number.u_int_32_ & internal::kFloat32MantissaMask) == internal::kFloat32MantissaInvalidValueBinary) &&
            ((temp_number.u_int_32_ & internal::kFloat32ExponentWithoutSymbolMask) !=
            internal::kFloat32ExponentInvalidValueBinary);
    }
}

/*
    If number is inf or -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsInf(const NumberType _number) {
    return _number == InfP<NumberType>() || _number == InfN<NumberType>();
}

/*
    If number is inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsInfP(const NumberType _number) {
    return _number == InfP<NumberType>();
}

/*
    If number is -inf.
*/
template<typename NumberType>
requires std::is_floating_point_v<NumberType>
NODISCARD FORCEINLINE constexpr const Bool IsInfN(const NumberType _number) {
    return _number == InfN<NumberType>();
}

}//math
}//zengine