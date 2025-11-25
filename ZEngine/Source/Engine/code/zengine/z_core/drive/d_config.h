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

#include "d_constant.h"
#include "d_lib.h"
#include "d_type.h"

// Shows the use of the memory pool, includes the memory block left when the 
// program emds, the total memory blocks appplyed and the peak situation of
// the memory pool. Using this test will slightly reduce the performance of 
// the program.
#define USE_MEMORY_POOL_TEST true
//Wheather the memory pool is thread safe.
#define MEMORY_POOL_THREAD_SAFE true
//Wheather the smart pointer is thread safe.
#define SMART_POINTER_THREAD_SAFE true

//Log type.
#ifdef _DEBUG
#define USE_DEBUG_LOG true
#define USE_CONSOLE_PRINT true
#define USE_CONSOLE_LOG true
#define USE_FILE_LOG true
#else
#define USE_DEBUG_LOG false
#define USE_CONSOLE_PRINT false
#define USE_CONSOLE_LOG false
#define USE_FILE_LOG true
#endif//_DEBUG

namespace zengine {

//The unit size when applying memory.
inline constexpr SizeType kHeapMemoryUnitSize = 4 * kKB;

//the root path of the log files.
inline constexpr WChar kLogFileRootPathDir[] = L".\\Log";

//the time before program exiting.
inline constexpr TimeType kLogFinishFlushMaxTime = 5000LL;

}//zengine