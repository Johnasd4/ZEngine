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

//the time before program exiting.
inline constexpr TimeType kLogFinishFlushMaxTime = 5000LL;

//the time before program exiting.
inline constexpr SizeType kMaxFileDirLength = 4096LL;

}//zengine