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

template<typename _LeftObjectType,typename _RightObjectType>
concept kSameType = std::is_same_v<_LeftObjectType, _RightObjectType>;

template<typename _ObjectType>
concept kIsClass = std::is_class_v<_ObjectType>;

template<typename _CharType>
concept kIsChar = kSameType<_CharType, Char> || kSameType<_CharType, WChar>;

template<typename _NumberType>
concept kIsSignedInt = 
    kSameType<_NumberType, Int8> 
    || kSameType<_NumberType, Int16> 
    || kSameType<_NumberType, Int32> 
    || kSameType<_NumberType, Int64>;

template<typename _NumberType>
concept kIsUnsignedInt = 
    kSameType<_NumberType, UInt8> 
    || kSameType<_NumberType, UInt16> 
    || kSameType<_NumberType, UInt32> 
    || kSameType<_NumberType, UInt64>;

template<typename _NumberType>
concept kIsInt = kIsSignedInt<_NumberType> || kIsUnsignedInt<_NumberType>;

template<typename _NumberType>
concept kIsFloat = kSameType<_NumberType, Float32> || kSameType<_NumberType, Float64>;

template<typename _NumberType>
concept kIsNumber = kIsInt<_NumberType> || kIsFloat<_NumberType>;

template<auto kNumber>
concept kIsZero = kNumber == 0;

template<auto kNumber>
concept kIsNotZero = kNumber != 0;

template<typename _ObjectType>
concept kIsComparable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 == _obj_2;
    _obj_1 != _obj_2;
    _obj_1 > _obj_2;
    _obj_1 >= _obj_2;
    _obj_1 < _obj_2;
    _obj_1 <= _obj_2;
};

template<typename _ObjectType>
concept kIsCopyable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 = _obj_2;
};

template<typename _ObjectType>
concept kIsMovable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 = std::move(_obj_2);
};

template<typename _Function, typename _ObjectType>
concept kIsPredicateFunction = requires(_Function _func, _ObjectType _obj) {
    { _func(_obj, _obj) } -> kSameType<Bool>;
};

}//zengine

#endif // !Z_CORE_INTERNAL_D_CONCEPT_H_