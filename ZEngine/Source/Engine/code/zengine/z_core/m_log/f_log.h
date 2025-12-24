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

#include "../drive.h"

#include "../internal/l_fmt.h"

#include "../t_smart_pointer.h"
#include "../z_string_view.h"

#include "c_log.h"
#include "z_log/z_error_log.h"
#include "z_log/z_info_log.h"
#include "z_log/z_trace_log.h"
#include "z_log/z_log.h"

namespace zengine {
namespace log {
namespace internal {

/**
 * @brief Internal implementation to process and dispatch an error log.
 * @param _log_time The timestamp of the log.
 * @param _proj_name The project name.
 * @param _file_dir The file directory/path.
 * @param _func_name The function name.
 * @param _err_line The line number.
 * @param _err_code The error code (returns kOK if successful, if failed returns error code).
 * @param _link_code The secondary link error code.
 * @param _format The format string.
 * @param _args The pre-formatted arguments.
 */
CORE_DLLAPI Void LogErrorP(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    ZStringView _format,
    fmt::format_args _args
) noexcept;

/**
 * @brief Internal implementation to process and dispatch a trace log.
 * @param _log_time The timestamp of the log.
 * @param _proj_name The project name.
 * @param _file_dir The file directory/path.
 * @param _func_name The function name.
 * @param _trace_line The line number.
 * @param _format The format string.
 * @param _args The pre-formatted arguments.
 */
CORE_DLLAPI Void LogTraceP(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _trace_line,
    ZStringView _format,
    fmt::format_args _args
) noexcept;

/**
 * @brief Internal implementation to process and dispatch an information log.
 * @param _log_time The timestamp of the log.
 * @param _info_type The type/category of the info log.
 * @param _format The format string.
 * @param _args The pre-formatted arguments.
 */
CORE_DLLAPI Void LogInfoP(
    TimeType _log_time,
    InfoLogTypeEnum _info_type,
    ZStringView _format,
    fmt::format_args _args
) noexcept;

/**
 * @brief Internal helper to register a log output function.
 * @param _output_func The function pointer to register.
 * @param output_func_array_ptr_ Pointer to the array storing output functions.
 */
CORE_DLLAPI Void RegisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept;

/**
 * @brief Internal helper to unregister a log output function.
 * @param _output_func The function pointer to unregister.
 * @param output_func_array_ptr_ Pointer to the array storing output functions.
 */
CORE_DLLAPI Void UnregisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept;


}//internal
}//log
}//zengine

namespace zengine {
namespace log {

/**
 * @brief Logs an error message with formatting support.
 * @tparam _ArgsType Variadic argument types for formatting.
 * @param _log_time The timestamp of the log.
 * @param _proj_name The project name.
 * @param _file_dir The file directory/path.
 * @param _func_name The function name.
 * @param _err_line The line number.
 * @param _err_code The error code (returns kOK if successful, if failed returns error code).
 * @param _link_code The secondary link error code.
 * @param _format The format string.
 * @param _args The arguments to format into the string.
 */
template<typename... _ArgsType>
FORCEINLINE Void LogError(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    internal::LogErrorP(
        _log_time,
        _proj_name,
        _file_dir,
        _func_name,
        _err_line,
        _err_code,
        _link_code,
        _format,
        fmt::make_format_args(_args...)
    );
}

/**
 * @brief Logs a trace message with formatting support.
 * @tparam _ArgsType Variadic argument types for formatting.
 * @param _log_time The timestamp of the log.
 * @param _proj_name The project name.
 * @param _file_dir The file directory/path.
 * @param _func_name The function name.
 * @param _trace_line The line number.
 * @param _format The format string.
 * @param _args The arguments to format into the string.
 */
template<typename... _ArgsType>
FORCEINLINE Void LogTrace(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _trace_line,
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    internal::LogTraceP(
        _log_time,
        _proj_name,
        _file_dir,
        _func_name,
        _trace_line,
        _format,
        fmt::make_format_args(_args...)
    );
}

/**
 * @brief Logs an informational message with formatting support.
 * @tparam _ArgsType Variadic argument types for formatting.
 * @param _log_time The timestamp of the log.
 * @param _info_type The type/category of the info log.
 * @param _format The format string.
 * @param _args The arguments to format into the string.
 */
template<typename... _ArgsType>
FORCEINLINE Void LogInfo(
    TimeType _log_time,
    InfoLogTypeEnum _info_type,
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    internal::LogInfoP(
        _log_time,
        _info_type,
        _format,
        fmt::make_format_args(_args...)
    );
}

/**
 * @brief Registers an output callback function for a specific log type.
 * @tparam _LogType The class type of the log (e.g., ZErrorLog, ZInfoLog).
 * @param _output_func The function pointer to register.
 */
template<typename _LogType>
Void RegisterLogOutputFunction(ZLog::OutputFunction_ _output_func) noexcept {
    internal::RegisterLogOutputFunctionP(
        _output_func,
        &_LogType::OutputFunctionArrayInstance()
    );
}

/**
 * @brief Unregisters an output callback function for a specific log type.
 * @tparam _LogType The class type of the log (e.g., ZErrorLog, ZInfoLog).
 * @param _output_func The function pointer to unregister.
 */
template<typename _LogType>
Void UnregisterLogOutputFunction(ZLog::OutputFunction_ _output_func) noexcept {
    internal::RegisterLogOutputFunctionP(
        _output_func,
        &_LogType::OutputFunctionArrayInstance()
    );
}

/**
 * @brief Flushes all pending logs and waits for completion.
 *
 * Call at the end of the program or when exiting the program to ensure no logs are lost.
 * @param _max_wait_time_ms The maximum time to wait for flushing in milliseconds.
 */
CORE_DLLAPI Void FinishFlush() noexcept;

}//log
}//zengine