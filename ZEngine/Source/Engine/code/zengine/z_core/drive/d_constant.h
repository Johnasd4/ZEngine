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

/** @brief Maximum value for a 8-bit signed integer. */
inline constexpr Int8 kInt8Max = 127;
/** @brief Minimum value for a 8-bit signed integer. */
inline constexpr Int8 kInt8Min = -128;
/** @brief Maximum value for a 8-bit unsigned integer. */
inline constexpr UInt8 kUInt8Max = 255U;
/** @brief Minimum value for a 8-bit unsigned integer. */
inline constexpr UInt8 kUInt8Min = 0U;
/** @brief Maximum value for a 16-bit signed integer. */
inline constexpr Int16 kInt16Max = 32767;
/** @brief Minimum value for a 16-bit signed integer. */
inline constexpr Int16 kInt16Min = -32768;
/** @brief Maximum value for a 16-bit unsigned integer. */
inline constexpr UInt16 kUInt16Max = 65535U;
/** @brief Minimum value for a 16-bit unsigned integer. */
inline constexpr UInt16 kUInt16Min = 0U;
/** @brief Maximum value for a 32-bit signed integer. */
inline constexpr Int32 kInt32Max = 2147483647;
/** @brief Minimum value for a 32-bit signed integer. */
inline constexpr Int32 kInt32Min = -2147483647; 
/** @brief Maximum value for a 32-bit unsigned integer. */
inline constexpr UInt32 kUInt32Max = 4294967295U;
/** @brief Minimum value for a 32-bit unsigned integer. */
inline constexpr UInt32 kUInt32Min = 0U;
/** @brief Maximum value for a 64-bit signed integer. */
inline constexpr Int64 kInt64Max = 9223372036854775807LL;
/** @brief Minimum value for a 64-bit signed integer. */
inline constexpr Int64 kInt64Min = -9223372036854775807LL;
/** @brief Maximum value for a 64-bit unsigned integer. */
inline constexpr UInt64 kUInt64Max = 18446744073709551615ULL;
/** @brief Minimum value for a 64-bit unsigned integer. */
inline constexpr UInt64 kUInt64Min = 0ULL;
/** @brief Maximum value for a 32-bit floating point number. */
inline constexpr Float32 kFloat32Max = 3.402823466e+38F;
/** @brief Minimum value for a 32-bit floating point number. */
inline constexpr Float32 kFloat32Min = -3.402823466e+38F;

/** @brief Maximum value for the TimeType alias. */
inline constexpr TimeType kTimeTypeMax = kInt64Max;
/** @brief Minimum value for the TimeType alias. */
inline constexpr TimeType kTimeTypeMin = kInt64Min;
/** @brief Maximum value for the SizeType alias. */
inline constexpr SizeType kSizeTypeMax = kUInt64Max;
/** @brief Minimum value for the SizeType alias. */
inline constexpr SizeType kSizeTypeMin = kUInt64Min;

/** @brief Constant representing 1 Byte. */
inline constexpr SizeType kB = 1;
/** @brief Constant representing 1 Kilobyte (1024 Bytes). */
inline constexpr SizeType kKB = 1024;
/** @brief Constant representing 1 Megabyte (1024 Kilobytes). */
inline constexpr SizeType kMB =  1048576;
/** @brief Constant representing 1 Gigabyte (1024 Megabytes). */
inline constexpr SizeType kGB = 1073741824;

} // zengine