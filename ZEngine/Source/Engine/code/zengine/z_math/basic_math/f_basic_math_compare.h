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

template<typename NumberType>
NODISCARD FORCEINLINE constexpr const NumberType Max(const NumberType _number_1, const NumberType _number_2) {
    return _number_1 > _number_2 ? _number_1 : _number_2;
}

template<typename NumberType, typename... NumberTypes>
NODISCARD FORCEINLINE constexpr const NumberType Max(const NumberType _number, NumberTypes&&... _other_numbers) {
    return Max(_number, Max(std::forward<NumberTypes>(_other_numbers)...));
}

template<typename NumberType>
NODISCARD FORCEINLINE constexpr const NumberType Min(const NumberType _number_1, const NumberType _number_2) {
    return _number_1 < _number_2 ? _number_1 : _number_2;
}

template<typename NumberType, typename... NumberTypes>
NODISCARD FORCEINLINE constexpr const NumberType Min(const NumberType _number, NumberTypes&&... _other_numbers) {
    return Min(_number, Min(std::forward<NumberTypes>(_other_numbers)...));
}

}//math
}//zengine
