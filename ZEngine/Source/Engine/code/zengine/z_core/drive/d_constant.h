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

inline constexpr Int32 kInt32Max = 2147483647;
inline constexpr Int32 kInt32Min = -2147483647; 
inline constexpr Int64 kInt64Max = 9223372036854775807LL;
inline constexpr Int64 kInt64Min = -9223372036854775807LL;
inline constexpr UInt64 kUInt64Max = 18446744073709551615ULL;
inline constexpr UInt64 kUInt64Min = 0ULL;
inline constexpr Float32 kFloat32Max = 3.402823466e+38F;
inline constexpr Float32 kFloat32Min = -3.402823466e+38F;

inline constexpr TimeType kTimeTypeMax = kInt64Max;
inline constexpr TimeType kTimeTypeMin = kInt64Min;
inline constexpr SizeType kSizeTypeMax = kUInt64Max;
inline constexpr SizeType kSizeTypeMin = kUInt64Min;

inline constexpr SizeType kB = 1;
inline constexpr SizeType kKB = 1024;
inline constexpr SizeType kMB =  1048576;
inline constexpr SizeType kGB = 1073741824;

}//zengine