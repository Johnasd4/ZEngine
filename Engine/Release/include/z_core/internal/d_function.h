/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_INTERNAL_D_FUNCTION_H_
#define Z_CORE_INTERNAL_D_FUNCTION_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

template<typename ObjectType>
FORCEINLINE constexpr Void Swap(ObjectType* object_1, ObjectType* object_2) {
    ObjectType temp_object(std::move(*object_1));
    *object_1 = std::move(*object_2);
    *object_2 = std::move(temp_object);
}

template<typename ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(ObjectType& object) {
    return std::ref(object);
}

template<typename ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(const ObjectType& object) {
    return std::ref(object);
}

template<typename ObjectType>
NODISCARD FORCEINLINE constexpr decltype(auto) Ref(std::reference_wrapper<ObjectType> object) {
    return std::ref(object);
}

}//zengine

#endif // !Z_CORE_INTERNAL_D_FUNCTION_H_