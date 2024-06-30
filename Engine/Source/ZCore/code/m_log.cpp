/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "m_log.h"

#include "f_console.h"
#include "t_fixed_string.h"
#include "z_file.h"
#include "z_system_time.h"
#include "log/z_log_manager.h"

namespace zengine {
namespace log {

CORE_DLLAPI ZLog::ZLog() noexcept : SuperType_(), log_msg_str_() {}

CORE_DLLAPI ZLog::ZLog(const Char* _format, ...) noexcept : SuperType_() {
    ArgListType args;
    va_start(args, _format);
    vsprintf(log_msg_str_.c_str_.DataPtr(), _format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(const Char* _format, ArgListType _args) noexcept : SuperType_() {
    vsprintf(log_msg_str_.c_str_.DataPtr(), _format, _args);
}

CORE_DLLAPI ZLog::ZLog(const WChar* _format, ...) noexcept : SuperType_() {
    ArgListType args;
    va_start(args, _format);
    vswprintf(log_msg_str_.w_str_.DataPtr(), _format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(const WChar* _format, ArgListType _args) noexcept : SuperType_() {
    vswprintf(log_msg_str_.w_str_.DataPtr(), _format, _args);
}

CORE_DLLAPI Void ZLog::GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept {
    //copy the full msg.
    memcpy(
        &(_output_str_ptr->c_str_), 
        &(_log_ptr->log_msg_str_.c_str_), 
        sizeof(_log_ptr->log_msg_str_.c_str_.Capacity()));
}

static ZFile& GetLogFile() noexcept {

    static ZFile file;
    ReturnType link_code = kOK;
    TWFixedString<ZFile::kFileNameLength> file_str;
    ZSystemTime system_time;

    file_str.SetString(
        L"%ls%04d%02d%02d%02d%02d%02d_dfault_log.log", ZLog::kPathTString,
        system_time.Year(), system_time.Month(), system_time.Day(), 
        system_time.Hour(), system_time.Min(), system_time.Sec());
    link_code = file.OpenSafe(ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendT);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::OpenSafe() link error!");
    }
    return file;
}

CORE_DLLAPI Void ZLog::FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%s\n", _output_str.w_str_.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::Print() link error!");
    }
}

CORE_DLLAPI Void ZLog::ConsoleOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    console::PrintMessage(L"%ls\n", _output_str.w_str_.DataPtr());
}

CORE_DLLAPI Void LogError(
    TimeType _raw_time,
    const Char* _err_project,
    const Char* _err_file, 
    const Char* _err_func,
    Int32 _err_line, 
    ReturnType _err_code,
    ReturnType _link_code,
    const Char* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogError(_raw_time, _err_project, _err_file, _err_func, _err_line, _err_code, _link_code, _format, args);
    va_end(args);
}

CORE_DLLAPI Void LogTrace(
    TimeType _raw_time,
    const WChar* _project,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogTrace(_raw_time, _project, _format, args);
    va_end(args);
}

CORE_DLLAPI Void LogInfo(
    TimeType _raw_time,
    LogInfoEnum _info_type,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogInfo(_raw_time, _info_type, _format, args);
    va_end(args);
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)) noexcept 
{
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id >= ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCodePortIDOutOfRange,
        "port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType UnregisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id >= ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCodePortIDOutOfRange,
        "port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerOutputFunction(
    IndexType _port_id, 
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id >= ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCodePortIDOutOfRange,
        "port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerOutputFunction(_port_id, _output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI Void UnregisterLogServerOutputFunction(
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ZLogManager::UnregisterLogServerOutputFunction(_output_func);
}

}//log
}//zengine