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
namespace error_code {
enum FBasicMath : ReturnType {
    kFBasicMathRandom_LinkError = kErrorCodeBase_FBasicMathRandom
};
}//error_code
}//zengine

namespace zengine {
namespace math {

/*
    Gets a random Int32 num. Range: [_lower_bound, _upper_bound]
*/
MATH_DLLAPI NODISCARD const Int32 RandInt32(const Int32 _lower_bound, const Int32 _upper_bound) noexcept;

/*
    Gets a random Int64 num. Range: [_lower_bound, _upper_bound]
*/
MATH_DLLAPI NODISCARD const Int64 RandInt64(const Int64 _lower_bound, const Int64 _upper_bound) noexcept;

/*
    Gets a random Float32 num. Range: [_lower_bound, _upper_bound]
*/
MATH_DLLAPI NODISCARD const Float32 RandFloat32(const Float32 _lower_bound, const Float32 _upper_bound) noexcept;

/*
    Gets a random Float64 num. Range: [_lower_bound, _upper_bound]
*/
MATH_DLLAPI NODISCARD const Float64 RandFloat64(const Float64 _lower_bound, const Float64 _upper_bound) noexcept;

}//math
}//zengine
