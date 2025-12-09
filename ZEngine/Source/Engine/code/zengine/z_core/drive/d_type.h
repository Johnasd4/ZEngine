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

namespace zengine {

using Char = char;
using WChar = wchar_t;

//range: -128~127
using Int8 = int8_t;
//range: -3,2768~3,2767
using Int16 = int16_t;
//range: -21,4748,3648~21,4748,3647
using Int32 = int32_t;
//range: -922,3372,0368,5477,5808~922,3372,0368,5477,5807
using Int64 = int64_t;

//range: 0~255
using UInt8 = uint8_t;
//range: 0~6,5535
using UInt16 = uint16_t;
//range: 0~42,9496,7295
using UInt32 = uint32_t;
//range: 0~1844,6744,0737,0955,1616
using UInt64 = uint64_t;

//range: -3.40E+38 ~ +3.40E+38
using Float32 = float;
//range: -1.79E+308 ~ +1.79E+308
using Float64 = double;

//Same as size_t.
using SizeType = size_t;
//The type of the pointer when translated to number.
using PointerType = SizeType;
//The type of return value. Format: 0xSPPTTTEE(S: Solution, P: Project, T: Type, E: Error)
using ReturnType = UInt32;
//The type of time.
using TimeType = Int64;
//The type of time.
using ArgListType = va_list;
//The type of nullptr.
using NullptrType = std::nullptr_t;

using Bool = bool;
using Byte = unsigned char;
using Void = void;
using Handle = void*;

union Size32Union {
    FORCEINLINE constexpr Size32Union(const Int32 _int_32) : int_32_(_int_32) {}
    FORCEINLINE constexpr Size32Union(const UInt32 _u_int_32) : u_int_32_(_u_int_32) {}
    FORCEINLINE constexpr Size32Union(const Float32 _float_32) : float_32_(_float_32) {}

    Int32 int_32_;
    UInt32 u_int_32_;
    Float32 float_32_;
};

union Size64Union {
    FORCEINLINE constexpr Size64Union(const Int64 _int_64) : int_64_(_int_64) {}
    FORCEINLINE constexpr Size64Union(const UInt64 _u_int_64) : u_int_64_(_u_int_64) {}
    FORCEINLINE constexpr Size64Union(const Float64 _float_64) : float_64_(_float_64) {}

    Int64 int_64_;
    UInt64 u_int_64_;
    Float64 float_64_;
};

enum class StringEnum : Int32 {
    kString,
    kWString
};

}//zengine