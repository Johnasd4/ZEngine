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
inline constexpr Float32 kFloat32Max = 3.402823466e+38F;
inline constexpr Float32 kFloat32Min = -3.402823466e+38F;

inline constexpr Int32 kIndexTypeMax = kInt32Max;
inline constexpr Int32 kIndexTypeMin = kInt32Min;

inline constexpr MemoryType kB = 1;
inline constexpr MemoryType kKB = 1024;
inline constexpr MemoryType kMB = 1048576;
inline constexpr MemoryType kGB = 1073741824;

//The unit size when applying memory.
inline constexpr MemoryType kHeapMemoryUnitSize = 4 * kKB;

//the root folder of the code file.
inline constexpr WChar kCodeFileRootDirWString[] = L"\\code\\";

//the root path of the log files.
inline constexpr WChar kLogFileRootPathDir[] = L".\\log";

//the time before program exiting.
inline constexpr UInt32 kTimeBeforeProgramExit = 500U;

}//zengine