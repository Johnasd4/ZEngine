/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

/** @brief Concept to check if two types are the same, ignoring const/volatile and references. */
template<typename _LeftObjectType, typename _RightObjectType>
concept kSameType = std::is_same_v<std::decay_t<_LeftObjectType>, std::decay_t<_RightObjectType>>;

/** @brief Concept to check if two types are different, ignoring const/volatile and references. */
template<typename _LeftObjectType, typename _RightObjectType>
concept kNotType = !std::is_same_v<std::decay_t<_LeftObjectType>, std::decay_t<_RightObjectType>>;

/** @brief Concept to check if a type is a class or struct type. */
template<typename _ObjectType>
concept kIsClassType = std::is_class_v<_ObjectType>;

/** @brief Concept to check if a type is a basic type (fundamental, pointer, or enum). */
template<typename _ObjectType>
concept kIsBasicType = 
    std::is_fundamental_v<_ObjectType> ||
    std::is_pointer_v<_ObjectType> ||
    std::is_enum_v<_ObjectType>;

/** @brief Concept to check if a type is a character type (Char or WChar). */
template<typename _CharType>
concept kIsChar = kSameType<_CharType, Char> || kSameType<_CharType, WChar>;

/** @brief Concept to check if a type is a signed integer type (Int8, Int16, Int32, Int64). */
template<typename _NumberType>
concept kIsSignedInt = 
    kSameType<_NumberType, Int8> 
    || kSameType<_NumberType, Int16> 
    || kSameType<_NumberType, Int32> 
    || kSameType<_NumberType, Int64>;

/** @brief Concept to check if a type is an unsigned integer type (UInt8, UInt16, UInt32, UInt64). */
template<typename _NumberType>
concept kIsUnsignedInt = 
    kSameType<_NumberType, UInt8> 
    || kSameType<_NumberType, UInt16> 
    || kSameType<_NumberType, UInt32> 
    || kSameType<_NumberType, UInt64>;

/** @brief Concept to check if a type is any integer type (signed or unsigned). */
template<typename _NumberType>
concept kIsInt = kIsSignedInt<_NumberType> || kIsUnsignedInt<_NumberType>;

/** @brief Concept to check if a type is a floating-point type (Float32 or Float64). */
template<typename _NumberType>
concept kIsFloat = kSameType<_NumberType, Float32> || kSameType<_NumberType, Float64>;

/** @brief Concept to check if a type is a number (integer or floating-point). */
template<typename _NumberType>
concept kIsNumber = kIsInt<_NumberType> || kIsFloat<_NumberType>;

/** @brief Concept to check if a compile-time constant number is zero. */
template<auto kNumber>
concept kIsZero = kNumber == 0;

/** @brief Concept to check if a compile-time constant number is not zero. */
template<auto kNumber>
concept kIsNotZero = kNumber != 0;

/** @brief Concept to check if a type supports comparison operators (==, !=, >, >=, <, <=). */
template<typename _ObjectType>
concept kIsComparable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 == _obj_2;
    _obj_1 != _obj_2;
    _obj_1 > _obj_2;
    _obj_1 >= _obj_2;
    _obj_1 < _obj_2;
    _obj_1 <= _obj_2;
};

/** @brief Concept to check if a type is copy assignable. */
template<typename _ObjectType>
concept kIsCopyable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 = _obj_2;
};

/** @brief Concept to check if a type is move assignable. */
template<typename _ObjectType>
concept kIsMovable = requires(_ObjectType _obj_1, _ObjectType _obj_2) {
    _obj_1 = std::move(_obj_2);
};

/** @brief Type alias for std::decay_t, used to remove const, volatile, and references. */
template<typename _ObjectType>
using kDecayType = std::decay_t<_ObjectType>;

namespace internal {

/** @brief Helper structure to determine type with reference removed if applicable. */
template<typename _ObjectType>
struct RemoveBasicTypeReferenceStruct {
    /** @brief The resulting type (default is original type). */
    using Type_ = _ObjectType;
};

/** @brief Specialization of RemoveBasicTypeReferenceStruct for basic types. */
template<typename _ObjectType> requires kIsBasicType<std::remove_reference_t<_ObjectType>>
struct RemoveBasicTypeReferenceStruct<_ObjectType> {
    /** @brief The resulting type with reference removed. */
    using Type_ = std::remove_reference_t<_ObjectType>;
};

}//internal

/** @brief Alias to remove references from a type only if it is a basic type. */
template<typename _ObjectType>
using kRemoveBasicTypeReferenceType = internal::RemoveBasicTypeReferenceStruct<_ObjectType>::Type_;

}//zengine