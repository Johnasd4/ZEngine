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

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace math {

//The Float64 version of PI.
inline constexpr Float64 kPI64 = 3.1415926535897932384626433832795;
//The Float32 version of PI.
inline constexpr Float32 kPI32 = static_cast<Float32>(kPI64);
//The Float64 version of 2PI.
inline constexpr Float64 k2PI64 = kPI64 * 2.0;
//The Float32 version of 2PI.
inline constexpr Float32 k2PI32 = static_cast<Float32>(k2PI64);
//The Float64 version of half PI.
inline constexpr Float64 kHalfPI64 = kPI64 * 0.5;
//The Float32 version of half PI.
inline constexpr Float32 kHalfPI32 = static_cast<Float32>(kHalfPI64);
//The Float32 version of half PI.
inline constexpr Float64 Sqrt3 = 1.732050807568877;
inline constexpr Float64 Sqrt3Ref = 0.5773502691896257;
inline constexpr Float64 Sqrt2 = 1.414213562373095;
inline constexpr Float64 Sqrt2Ref = 0.7071067811865476;

}//math
}//zengine

namespace zengine {
namespace math {
namespace internal {

inline constexpr Int32 kFloat64MantissaOffset = 1023;
inline constexpr UInt64 kFloat64MantissaMask = 0X7ff0000000000000ULL;
inline constexpr UInt64 kFloat64ExponentWithoutSymbolMask = 0X000fffffffffffffULL;
inline constexpr UInt64 kFloat64ExponentWithSymbolMask = 0X800fffffffffffffULL;
inline constexpr UInt64 kFloat64MantissaInvalidValueBinary = 0X7ff0000000000000ULL;
inline constexpr UInt64 kFloat64ExponentInvalidValueBinary = 0X0000000000000000ULL;
inline constexpr Int64 kFloat64ExponentSize = 52LL;
inline constexpr UInt64 kFloat64MantissaZeroValue = 0X3ff0000000000000ULL;
inline constexpr UInt64 kFloat64NaNBinary = 0XffffffffffffffffULL;
inline constexpr UInt64 kFloat64InfPositiveBinary = 0X7ff0000000000000ULL;
inline constexpr UInt64 kFloat64InfNegativeBinary = 0Xfff0000000000000ULL;

inline constexpr Int32 kFloat32MantissaOffset = 127;
inline constexpr UInt32 kFloat32MantissaMask = 0X7f800000U;
inline constexpr UInt32 kFloat32ExponentWithoutSymbolMask = 0X007fffffU;
inline constexpr UInt32 kFloat32ExponentWithSymbolMask = 0X807fffffU;
inline constexpr UInt32 kFloat32MantissaInvalidValueBinary = 0X7f800000U;
inline constexpr UInt32 kFloat32ExponentInvalidValueBinary = 0X00000000U;
inline constexpr Int32 kFloat32ExponentSize = 23;
inline constexpr UInt32 kFloat32MantissaZeroValue = 0X3f800000U;
inline constexpr UInt32 kFloat32NaNBinary = 0XffffffffU;
inline constexpr UInt32 kFloat32InfPositiveBinary = 0X7f800000U;
inline constexpr UInt32 kFloat32InfNegativeBinary = 0Xff800000U;

}//internal
}//math
}//zengine