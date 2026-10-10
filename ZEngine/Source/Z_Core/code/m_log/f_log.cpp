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

#include "m_log/f_log.h"

#include "z_log_manager.h"

namespace zengine {
namespace log {
namespace internal {

CORE_DLLAPI Void LogErrorP(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_path,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    static ZLogManager& manager = ZLogManager::GetInstance();
    if (!manager.IsRunning()) {
        return;
    }
    TUniquePointer<ZErrorLog> log_ptr = MakeUnique<ZErrorLog>(
        _log_time,
        _proj_name,
        _file_path,
        _func_name,
        _err_line,
        _err_code,
        _link_code
    );
    ZErrorLog::LogString_& log_string = log_ptr->LogString();
    log_string.size_ = string::internal::GenerateStringNoEnd(
        log_string.string_.GetDataPtr(),
        log_string.string_.GetCapacity(),
        _format,
        _args
    );
    manager.Log(std::move(log_ptr));
}

CORE_DLLAPI Void LogTraceP(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_path,
    ZStringView _func_name,
    Int32 _trace_line,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    static ZLogManager& manager = ZLogManager::GetInstance();
    if (!manager.IsRunning()) {
        return;
    }
    TUniquePointer<ZTraceLog> log_ptr = MakeUnique<ZTraceLog>(
        _log_time,
        _proj_name,
        _file_path,
        _func_name,
        _trace_line
    );
    ZTraceLog::LogString_& log_string = log_ptr->LogString();
    log_string.size_ = string::internal::GenerateStringNoEnd(
        log_string.string_.GetDataPtr(),
        log_string.string_.GetCapacity(),
        _format,
        _args
    );
    manager.Log(std::move(log_ptr));
}

CORE_DLLAPI Void LogInfoP(
    TimeType _log_time,
    InfoLogTypeEnum _info_type,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    static ZLogManager& manager = ZLogManager::GetInstance();
    if (!manager.IsRunning()) {
        return;
    }
    TUniquePointer<ZInfoLog> log_ptr = MakeUnique<ZInfoLog>(
        _log_time,
        _info_type
    );
    ZInfoLog::LogString_& log_string = log_ptr->LogString();
    log_string.size_ = string::internal::GenerateStringNoEnd(
        log_string.string_.GetDataPtr(),
        log_string.string_.GetCapacity(),
        _format,
        _args
    );
    manager.Log(std::move(log_ptr));
}

CORE_DLLAPI Void RegisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept {
    ZLogManager::GetInstance().RegisterLogOutputFunction(
        _output_func,
        output_func_array_ptr_
    );
}

CORE_DLLAPI Void UnregisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept {
    ZLogManager::GetInstance().UnregisterLogOutputFunction(
        _output_func,
        output_func_array_ptr_
    );
}

} // internal
} // log
} // zengine

namespace zengine {
namespace log {

CORE_DLLAPI Void FinishFlush() noexcept {
    ZLogManager::GetInstance().FinishFlush();
}

} // log
} // zengine