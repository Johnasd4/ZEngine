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
#ifndef Z_CORE_INTERNAL_D_CONCEPT_H_
#define Z_CORE_INTERNAL_D_CONCEPT_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

template<typename ObjectType1,typename ObjectType2>
concept kSameType = std::is_same_v<ObjectType1, ObjectType2>;

template<typename ObjectType>
concept kIsClass = std::is_class_v<ObjectType>;

template<typename CharType>
concept kIsChar = kSameType<CharType, CChar> || kSameType<CharType, TChar>;

template<typename NumberType>
concept kIsSignedInt = kSameType<NumberType, Int8> || kSameType<NumberType, Int16> || kSameType<NumberType, Int32> || 
                       kSameType<NumberType, Int64>;

template<typename NumberType>
concept kIsUnsignedInt = kSameType<NumberType, UInt8> || kSameType<NumberType, UInt16> || 
                         kSameType<NumberType, UInt32> || kSameType<NumberType, UInt64>;

template<typename NumberType>
concept kIsInt = kIsSignedInt<NumberType> || kIsUnsignedInt<NumberType>;

template<typename NumberType>
concept kIsFloat = kSameType<NumberType, Float32> || kSameType<NumberType, Float64>;

template<typename NumberType>
concept kIsNumber = kIsInt<NumberType> || kIsFloat<NumberType>;

template<auto number>
concept kIsZero = number == 0;

template<auto number>
concept kIsNotZero = number != 0;

template<typename ObjectType>
concept kIsComparable = requires(ObjectType object_1, ObjectType object_2) {
    object_1 == object_2;
    object_1 != object_2;
    object_1 > object_2;
    object_1 >= object_2;
    object_1 < object_2;
    object_1 <= object_2;
};

template<typename ObjectType>
concept kIsCopyable = requires(ObjectType object_1, ObjectType object_2) {
    object_1 = object_2;
};

template<typename ObjectType>
concept kIsMovable = requires(ObjectType object_1, ObjectType object_2) {
    object_1 = std::move(object_2);
};

template<typename Function, typename ObjectType>
concept kIsCompareFunction = requires(Function function, ObjectType object) {
    { function(object, object) } -> kSameType<Bool>;
};

}//zengine

#endif // !Z_CORE_INTERNAL_D_CONCEPT_H_