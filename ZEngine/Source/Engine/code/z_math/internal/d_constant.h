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
constexpr Float64 kPI64 = 3.1415926535897932384626433832795;
//The Float32 version of PI.
constexpr Float32 kPI32 = static_cast<Float32>(kPI64);
//The Float64 version of 2PI.
constexpr Float64 k2PI64 = kPI64 * 2.0;
//The Float32 version of 2PI.
constexpr Float32 k2PI32 = static_cast<Float32>(k2PI64);
//The Float64 version of half PI.
constexpr Float64 kHalfPI64 = kPI64 * 0.5;
//The Float32 version of half PI.
constexpr Float32 kHalfPI32 = static_cast<Float32>(kHalfPI64);
//The Float32 version of half PI.
constexpr Float64 Sqrt3 = 1.732050807568877;
constexpr Float64 Sqrt3Ref = 0.5773502691896257;
constexpr Float64 Sqrt2 = 1.414213562373095;
constexpr Float64 Sqrt2Ref = 0.7071067811865476;

}//math
}//zengine