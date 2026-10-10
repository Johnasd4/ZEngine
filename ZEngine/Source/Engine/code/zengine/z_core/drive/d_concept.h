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
concept kIsSameType = std::is_same_v<std::decay_t<_LeftObjectType>, std::decay_t<_RightObjectType>>;

/** @brief Concept to check if two types are convertible, ignoring const/volatile and references. */
template<typename _LeftObjectType, typename _RightObjectType>
concept kIsConvertibleType = std::is_convertible_v<std::decay_t<_LeftObjectType>, std::decay_t<_RightObjectType>>;

/** @brief Concept to check if a type is a left value reference. */
template<typename _ObjectType>
concept kIsRightValue = std::is_rvalue_reference_v<_ObjectType>;

/** @brief Concept to check if a type is a class or struct type. */
template<typename _ObjectType>
concept kIsClass = std::is_class_v<_ObjectType>;

/** @brief Concept to check if a type is trivially constructible. */
template<typename _ObjectType>
concept kIsTriviallyConstructible = std::is_trivially_constructible_v<_ObjectType>;

/** @brief Concept to check if a type is trivially copyable. */
template<typename _ObjectType>
concept kIsTriviallyCopyable = std::is_trivially_copyable_v<_ObjectType>;

/** @brief Concept to check if a type is trivially destructible. */
template<typename _ObjectType>
concept kIsTriviallyDestructible = std::is_trivially_destructible_v<_ObjectType>;

/** @brief Concept to check if a type is a basic type (fundamental, pointer, or enum). */
template<typename _ObjectType>
concept kIsBasicType = 
    std::is_fundamental_v<_ObjectType> ||
    std::is_pointer_v<_ObjectType> ||
    std::is_enum_v<_ObjectType>;

/** @brief Concept to check if a type is a character type (Char or WChar). */
template<typename _CharType>
concept kIsChar = kIsSameType<_CharType, Char> || kIsSameType<_CharType, WChar>;

/** @brief Concept to check if a type is a signed integer type (Int8, Int16, Int32, Int64). */
template<typename _NumberType>
concept kIsSignedInt = 
    kIsSameType<_NumberType, Int8> 
    || kIsSameType<_NumberType, Int16> 
    || kIsSameType<_NumberType, Int32> 
    || kIsSameType<_NumberType, Int64>;

/** @brief Concept to check if a type is an unsigned integer type (UInt8, UInt16, UInt32, UInt64). */
template<typename _NumberType>
concept kIsUnsignedInt = 
    kIsSameType<_NumberType, UInt8> 
    || kIsSameType<_NumberType, UInt16> 
    || kIsSameType<_NumberType, UInt32> 
    || kIsSameType<_NumberType, UInt64>;

/** @brief Concept to check if a type is any integer type (signed or unsigned). */
template<typename _NumberType>
concept kIsInt = kIsSignedInt<_NumberType> || kIsUnsignedInt<_NumberType>;

/** @brief Concept to check if a type is a floating-point type (Float32 or Float64). */
template<typename _NumberType>
concept kIsFloat = kIsSameType<_NumberType, Float32> || kIsSameType<_NumberType, Float64>;

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

/** @brief Concept to check if a type is copy constructible. */
template<typename _ObjectType>
concept kIsCopyConstructible = std::is_copy_constructible_v<_ObjectType>;

/** @brief Concept to check if a type is copy assignable. */
template<typename _ObjectType>
concept kIsCopyAssignable = std::is_copy_assignable_v<_ObjectType>;

/** @brief Concept to check if a type is move constructible. */
template<typename _ObjectType>
concept kIsMoveConstructible = std::is_move_constructible_v<_ObjectType>;

/** @brief Concept to check if a type is move assignable. */
template<typename _ObjectType>
concept kIsMoveAssignable = std::is_move_assignable_v<_ObjectType>;


/** @brief Type alias for std::decay_t, used to remove const, volatile, and references. */
template<typename _ObjectType>
using kDecayType = std::decay_t<_ObjectType>;

} // zengine