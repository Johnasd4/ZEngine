/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

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