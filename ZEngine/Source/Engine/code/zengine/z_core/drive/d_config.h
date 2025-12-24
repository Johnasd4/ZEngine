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

/**
 * @brief Enables memory pool usage statistics, including remaining blocks, total applied, and peak usage.
 * @note Using this test will slightly reduce the performance of the program.
 */
#define USE_MEMORY_POOL_PERFORMANCE_TEST true
 /** @brief Determines whether the memory pool is thread-safe. */
#define MEMORY_POOL
/** @brief Determines whether the memory pool is thread-safe. */
#define MEMORY_POOL_THREAD_SAFE true
/** @brief Determines whether the smart pointer is thread-safe. */
#define SMART_POINTER_THREAD_SAFE true

//Log type.
#ifdef _DEBUG
/** @brief Determines whether to use macros starting with Z_DEBUG_LOG for log output. */
#define USE_DEBUG_LOG true
/** @brief Determines whether to use the Z_PRINT macro for output. */
#define USE_CONSOLE_PRINT true
/** @brief Determines whether to print logs to the console. */
#define USE_CONSOLE_LOG true
/** @brief Determines whether to print logs to a file. */
#define USE_FILE_LOG true
#else
/** @brief Determines whether to use macros starting with Z_DEBUG_LOG for log output. */
#define USE_DEBUG_LOG false
/** @brief Determines whether to use the Z_PRINT macro for output. */
#define USE_CONSOLE_PRINT false
/** @brief Determines whether to print logs to the console. */
#define USE_CONSOLE_LOG false
/** @brief Determines whether to print logs to a file. */
#define USE_FILE_LOG true
#endif//_DEBUG

#define DEFAULT_USE_GLOBAL_MEMORY_POOL true

namespace zengine {

/** @brief The delay time in milliseconds before the program exits. */
inline constexpr TimeType kExitTimeMs = 50LL;
/** @brief The maximum length allowed for a file directory path. */
inline constexpr SizeType kMaxFileDirLength = 4096LL;

}//zengine