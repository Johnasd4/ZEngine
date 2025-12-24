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
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    static ZLogManager& manager = ZLogManager::Instance();
    if (manager.IsFinished()) {
        return;
    }
    TUniquePointer<ZLog> log_ptr = MakeUnique<ZErrorLog>(
        _log_time,
        _proj_name,
        _file_dir,
        _func_name,
        _err_line,
        _err_code,
        _link_code
    );
    ZLog::LogString_* log_string_ptr = log_ptr->LogStringPtr();
    log_string_ptr->size_ = string::internal::GenerateStringNoEnd(
        log_string_ptr->log_str_ptr_->GetDataPtr(),
        log_string_ptr->log_str_ptr_->GetCapacity(),
        _format,
        _args
    );
    manager.Log(std::move(log_ptr));
}

CORE_DLLAPI Void LogTraceP(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _trace_line,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    static ZLogManager& manager = ZLogManager::Instance();
    if (manager.IsFinished()) {
        return;
    }
    TUniquePointer<ZLog> log_ptr = MakeUnique<ZTraceLog>(
        _log_time,
        _proj_name,
        _file_dir,
        _func_name,
        _trace_line
    );
    ZLog::LogString_* log_string_ptr = log_ptr->LogStringPtr();
    log_string_ptr->size_ = string::internal::GenerateStringNoEnd(
        log_string_ptr->log_str_ptr_->GetDataPtr(),
        log_string_ptr->log_str_ptr_->GetCapacity(),
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
    static ZLogManager& manager = ZLogManager::Instance();
    if (manager.IsFinished()) {
        return;
    }
    TUniquePointer<ZLog> log_ptr = MakeUnique<ZInfoLog>(
        _log_time,
        _info_type
    );
    ZLog::LogString_* log_string_ptr = log_ptr->LogStringPtr();
    log_string_ptr->size_ = string::internal::GenerateStringNoEnd(
        log_string_ptr->log_str_ptr_->GetDataPtr(),
        log_string_ptr->log_str_ptr_->GetCapacity(),
        _format,
        _args
    );
    manager.Log(std::move(log_ptr));
}

CORE_DLLAPI Void RegisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept {
    ZLogManager::Instance().RegisterLogOutputFunction(
        _output_func,
        output_func_array_ptr_
    );
}

CORE_DLLAPI Void UnregisterLogOutputFunctionP(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* output_func_array_ptr_
) noexcept {
    ZLogManager::Instance().UnregisterLogOutputFunction(
        _output_func,
        output_func_array_ptr_
    );
}

}//internal
}//log
}//zengine

namespace zengine {
namespace log {

CORE_DLLAPI Void FinishFlush() noexcept {
    ZLogManager::Instance().FinishFlush();
}

}//log
}//zengine