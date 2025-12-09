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
 * @brief Copies memory from the source object to the destination object.
 * 
 * @tparam _DstObjectType Type of the destination object.
 * @tparam _SrcObjectType Type of the source object.
 * @param _dst_obj Pointer to the destination object.
 * @param _src_obj Pointer to the source object.
 * @param _size Size in bytes to copy. Defaults to the size of the destination type.
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
 * @brief Swaps the values of two objects.
 * 
 * @tparam _ObjectType Type of the objects.
 * @param _obj_1 Pointer to the first object.
 * @param _obj_2 Pointer to the second object.
 */
template<typename _ObjectType>
FORCEINLINE constexpr Void Swap(_ObjectType* _obj_1, _ObjectType* _obj_2) {
    _ObjectType temp_obj(std::move(*_obj_1));
    *_obj_1 = std::move(*_obj_2);
    *_obj_2 = std::move(temp_obj);
}

/**
 * @brief Decays basic types and forwards others.
 * 
 * If the argument is a fundamental type, pointer, or enum, it returns a copy (decays).
 * Otherwise, it performs a standard forward.
 * 
 * @tparam _ArgType Type of the argument.
 * @param arg The argument to forward.
 * @return decltype(auto) The forwarded or decayed value.
 */
template<typename _ArgType>
NODISCARD FORCEINLINE constexpr decltype(auto) NonBasicTypeForward(_ArgType&& arg) {
    using DecayArgType = std::remove_reference_t<_ArgType>;
    if constexpr (
        std::is_fundamental_v<DecayArgType> ||
        std::is_pointer_v<DecayArgType> ||
        std::is_enum_v<DecayArgType>
    ) {
        return static_cast<DecayArgType>(arg);
    }
    else {
        return std::forward<_ArgType>(arg);
    }
}

/**
 * @brief Suspends the current thread for the specified time in seconds.
 * 
 * @param _time The time to sleep in seconds.
 */
CORE_DLLAPI NODISCARD Void SleepSec(TimeType _time) noexcept;

/**
 * @brief Suspends the current thread for the specified time in milliseconds.
 * 
 * @param _time The time to sleep in milliseconds.
 */
CORE_DLLAPI NODISCARD Void SleepMs(TimeType _time) noexcept;

/**
 * @brief Exits the program with the given return value.
 * 
 * @param _ret_val The return value. Returns kOK if successful; otherwise, returns an error code.
 */
CORE_DLLAPI NODISCARD Void Exit(ReturnType _ret_val) noexcept;

}//zengine