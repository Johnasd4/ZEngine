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

template<typename _ObjectType>
FORCEINLINE constexpr Void Swap(_ObjectType* _obj_1, _ObjectType* _obj_2) {
    _ObjectType temp_obj(std::move(*_obj_1));
    *_obj_1 = std::move(*_obj_2);
    *_obj_2 = std::move(temp_obj);
}

template<typename _ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(_ObjectType& _obj) {
    return std::ref(_obj);
}

template<typename _ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(const _ObjectType& _obj) {
    return std::ref(_obj);
}

template<typename _ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(std::reference_wrapper<_ObjectType> _obj) {
    return std::ref(_obj);
}

NODISCARD FORCEINLINE TimeType Time() noexcept { return time(nullptr); }

}//zengine