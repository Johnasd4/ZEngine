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

/**
 * @brief Checks if the current context is a compile-time constant evaluation.
 * @return True if in a constant evaluation context, false otherwise.
 */
FORCEINLINE consteval Bool IsCompileTimeConstant() {
    return std::is_constant_evaluated();
}

/**
 * @brief Copies memory from the source object to the destination object.
 * @tparam _DstObjectType Type of the destination object.
 * @tparam _SrcObjectType Type of the source object.
 * @param _dst_obj Pointer to the destination object.
 * @param _src_obj Pointer to the source object.
 * @param _size Size in bytes to copy. Defaults to the size of the destination type.
 * @return Void.
 */
template<typename _DstObjectType, typename _SrcObjectType>
FORCEINLINE constexpr Void Copy(
    _DstObjectType* _dst_obj,
    const _SrcObjectType* _src_obj,
    SizeType _size = sizeof(_DstObjectType)
) {
    memcpy(_dst_obj, _src_obj, _size);
}

/**
 * @brief Swaps the values of two objects using move semantics.
 * @tparam _ObjectType Type of the objects.
 * @param _obj_1 Pointer to the first object.
 * @param _obj_2 Pointer to the second object.
 * @return Void.
 */
template<typename _ObjectType>
FORCEINLINE constexpr Void Swap(_ObjectType* _obj_1, _ObjectType* _obj_2) {
    _ObjectType temp_obj(std::move(*_obj_1));
    *_obj_1 = std::move(*_obj_2);
    *_obj_2 = std::move(temp_obj);
}

/**
 * @brief Generates a reference wrapper for the given object.
 * @tparam _ObjectType Type of the object.
 * @param _obj The object to create a reference to.
 * @return A reference wrapper to the object.
 */
template<typename _ObjectType>
FORCEINLINE constexpr decltype(auto) Ref(_ObjectType&& _obj) {
    return std::ref(std::forward<_ObjectType>(_obj));
}

/**
 * @brief Locks two mutexes in a deadlock-free manner.
 * Implementation Principle: The locking order is determined by the memory addresses of the mutexes to ensure a
 * consistent global locking order, thereby preventing circular wait conditions.
 * @tparam _MutexType1 Type of the first mutex.
 * @tparam _MutexType2 Type of the second mutex.
 * @param _mutex_1 The first mutex to lock.
 * @param _mutex_2 The second mutex to lock.
 * @return Void.
 */
template<typename _MutexType1, typename _MutexType2>
Void Lock(_MutexType1& _mutex_1, _MutexType2& _mutex_2) {
    if (reinterpret_cast<Void*>(&_mutex_1) < reinterpret_cast<Void*>(&_mutex_2)) {
        _mutex_1.Lock();
        _mutex_2.Lock();
    }
    else if (reinterpret_cast<Void*>(&_mutex_1) > reinterpret_cast<Void*>(&_mutex_2)) {
        _mutex_2.Lock();
        _mutex_1.Lock();
    }
    else {
        _mutex_1.Lock();
    }
}

/**
 * @brief Unlocks two mutexes in a deadlock-free manner.
 * @tparam _MutexType1 Type of the first mutex.
 * @tparam _MutexType2 Type of the second mutex.
 * @param _mutex_1 The first mutex to unlock.
 * @param _mutex_2 The second mutex to unlock.
 * @return Void.
 */
template<typename _MutexType1, typename _MutexType2>
FORCEINLINE Void Unlock(_MutexType1& _mutex_1, _MutexType2& _mutex_2) {
    if (reinterpret_cast<Void*>(&_mutex_1) < reinterpret_cast<Void*>(&_mutex_2)) {
        _mutex_2.Unlock();
        _mutex_1.Unlock();
    }
    else if (reinterpret_cast<Void*>(&_mutex_1) > reinterpret_cast<Void*>(&_mutex_2)) {
        _mutex_1.Unlock();
        _mutex_2.Unlock();
    }
    else {
        _mutex_1.Unlock();
    }
}

/**
 * @brief Compares two objects and returns the larger one.
 * @tparam _ObjectType1 Type of the first object.
 * @tparam _ObjectType2 Type of the second object.
 * @param _obj_1 The first object.
 * @param _obj_2 The second object.
 * @return The larger of the two objects. The return type is automatically deduced.
 */
template<typename _ObjectType1, typename _ObjectType2>
NODISCARD FORCEINLINE constexpr auto Max(
    const _ObjectType1& _obj_1, 
    const _ObjectType2& _obj_2
) {
    return _obj_1 > _obj_2 ? _obj_1 : _obj_2;
}

/**
 * @brief Compares multiple objects and returns the largest one.
 * @tparam _ObjectType Type of the first object.
 * @tparam _OtherObjectTypes Types of the other objects.
 * @param _obj The first object.
 * @param _other_objs The other objects to compare.
 * @return The largest object among the inputs. The return type is automatically deduced.
 */
template<typename _ObjectType1, typename _ObjectType2, typename... _OtherObjectTypes>
NODISCARD FORCEINLINE constexpr auto Max(
    const _ObjectType1& _obj_1,
    const _ObjectType2& _obj_2,
    _OtherObjectTypes&&... _other_objs
) {
    return Max(Max(_obj_1, _obj_2), std::forward<_OtherObjectTypes>(_other_objs)...);
}

/**
 * @brief Compares two objects and returns the smaller one.
 * @tparam _ObjectType1 Type of the first object.
 * @tparam _ObjectType2 Type of the second object.
 * @param _obj_1 The first object.
 * @param _obj_2 The second object.
 * @return The smaller of the two objects. The return type is automatically deduced.
 */
template<typename _ObjectType1, typename _ObjectType2>
NODISCARD FORCEINLINE constexpr auto Min(
    const _ObjectType1& _obj_1,
    const _ObjectType2& _obj_2
) {
    return _obj_1 < _obj_2 ? _obj_1 : _obj_2;
}

/**
 * @brief Compares multiple objects and returns the smallest one.
 * @tparam _ObjectType Type of the first object.
 * @tparam _OtherObjectTypes Types of the other objects.
 * @param _obj The first object.
 * @param _other_objs The other objects to compare.
 * @return The smallest object among the inputs. The return type is automatically deduced.
 */
template<typename _ObjectType, typename... _OtherObjectTypes>
NODISCARD FORCEINLINE constexpr auto Min(
    const _ObjectType& _obj,
    _OtherObjectTypes&&... _other_objs
) {
    return Min(_obj, Min(std::forward<_OtherObjectTypes>(_other_objs)...));
}

/**
 * @brief Exits the program with the given return value.
 * @param _ret_val The return value. Returns kOK if successful; otherwise, returns an error code.
 * @return Void.
 */
CORE_DLLAPI NODISCARD Void Exit(ReturnType _ret_val) noexcept;

} // zengine