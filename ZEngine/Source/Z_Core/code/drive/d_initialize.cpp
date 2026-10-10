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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "drive/d_initialize.h"

#include "m_log.h"
#include "m_log/z_log_manager.h"

#include "f_memory_pool/z_generic_memory_pool_performance_test.h"

#pragma warning(disable : 4834)

namespace zengine {
namespace internal {

static Void SystemAbortHandler(Int32 _signal) noexcept {
    Exit(error_code::kErrorCode_SystemAbort);
}

} // internal
} // zengine

namespace zengine {

CORE_DLLAPI Void Initialize() noexcept { 
    // register crash signal handler
    std::signal(SIGABRT, internal::SystemAbortHandler);

    // Initialize the ID of the main thread
    ZThread::GetMainThreadID();

#if USE_MEMORY_POOL_PERFORMANCE_TEST
    // call instance to initialize, must be called before any memory pool used
    memory_pool::ZGenericMemoryPoolPerformanceTest::GetInstance();
#endif // USE_MEMORY_POOL_PERFORMANCE_TEST

    // register log output function
#if USE_FILE_LOG
    log::RegisterLogOutputFunction<log::ZErrorLog>(log::ZErrorLog::FileOutputLog);
    log::RegisterLogOutputFunction<log::ZInfoLog>(log::ZInfoLog::FileOutputLog);
    log::RegisterLogOutputFunction<log::ZTraceLog>(log::ZTraceLog::FileOutputLog);
#endif
#if USE_CONSOLE_LOG
    log::RegisterLogOutputFunction<log::ZErrorLog>(log::ZErrorLog::ConsoleOutputLog);
    log::RegisterLogOutputFunction<log::ZInfoLog>(log::ZInfoLog::ConsoleOutputLog);
    log::RegisterLogOutputFunction<log::ZTraceLog>(log::ZTraceLog::ConsoleOutputLog);
#endif

    // Must be called in the current thread, 
    // sub threads calling this function will cause sync issues when exiting the program.
    log::ZLog::CreateAndGetLogPath();

    // start log thread
    log::ZLogManager::GetInstance().StartThread();

    Z_LOG_ERROR(0, 0, "Error...");
    Z_LOG_TRACE("Trace...");
    Z_LOG_MESSAGE("Message...");
    Z_LOG_START("Start...");
    Z_LOG_PROCESS("Process...");
    Z_LOG_FINISH("Finish...");
    Z_LOG_FAILURE("Failure...");
    Z_LOG_SUCCESS("Success...");

    Z_LOG_SUCCESS("ZEngine initialize success!");
}

} // zengine

#pragma warning(default : 4834)