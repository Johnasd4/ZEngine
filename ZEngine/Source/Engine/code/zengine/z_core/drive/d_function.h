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

template<typename _DstObjectType, typename _SrcObjectType>
FORCEINLINE constexpr Void Copy(
    _DstObjectType* _dst_obj,
    const _SrcObjectType* _src_obj,
    SizeType _size = sizeof(_DstObjectType)
) {
    memcpy(_dst_obj, _src_obj, _size);
}

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

/*
    The current thread suspends the given time(s)
*/
CORE_DLLAPI NODISCARD Void SleepSec(TimeType _time) noexcept;

/*
    The current thread suspends the given time(ms)
*/
CORE_DLLAPI NODISCARD Void SleepMs(TimeType _time) noexcept;

/*
    Exit the program with the given return value.
*/
CORE_DLLAPI NODISCARD Void Exit(ReturnType _ret_val) noexcept;

}//zengine