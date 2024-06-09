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

CORE_DLLAPI ZLog::ZLog() noexcept : SuperType(), log_msg_str_() {}

CORE_DLLAPI ZLog::ZLog(const CChar* format, ...) noexcept : SuperType() {
    ArgListType args;
    va_start(args, format);
    vsprintf(log_msg_str_.c_str.DataPtr(), format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(const CChar* format, ArgListType args) noexcept : SuperType() {
    vsprintf(log_msg_str_.c_str.DataPtr(), format, args);
}

CORE_DLLAPI ZLog::ZLog(const TChar* format, ...) noexcept : SuperType() {
    ArgListType args;
    va_start(args, format);
    vswprintf(log_msg_str_.t_str.DataPtr(), format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(const TChar* format, ArgListType args) noexcept : SuperType() {
    vswprintf(log_msg_str_.t_str.DataPtr(), format, args);
}

CORE_DLLAPI Void ZLog::GenerateLogString(const ZLog* log_ptr, OutputString* output_str_ptr) noexcept {
    //copy the full msg.
    memcpy(&(output_str_ptr->c_str), &(log_ptr->log_msg_str_.c_str), sizeof(log_ptr->log_msg_str_.c_str.Capacity()));
}

static ZFile& GetLogFile() noexcept {

    static ZFile file;
    ReturnType link_code = kOK;
    TFixedString<TChar, ZFile::kFileNameLength> file_str;
    ZSystemTime system_time;

    file_str.SetString(L"%lsdefault_log_%04d%02d%02d%02d%02d%02d.log", ZLog::kPathTString,
                       system_time.Year(), system_time.Month(), system_time.Day(), 
                       system_time.Hour(), system_time.Min(), system_time.Sec());
    link_code = file.OpenSafe(ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendT);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::OpenSafe() link error!");
    }
    return file;
}

CORE_DLLAPI Void ZLog::FileOutputLogString(const ZLog::OutputString& output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%s\n", output_str.t_str.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::Print() link error!");
    }
}

CORE_DLLAPI Void ZLog::ConsoleOutputLogString(const ZLog::OutputString& output_str) noexcept {
    console::PrintMessage(L"%ls\n", output_str.t_str.DataPtr());
}

CORE_DLLAPI Void LogError(TimeType raw_time,
                          const CChar* err_project,
                          const CChar* err_file, 
                          const CChar* err_func,
                          Int32 err_line, 
                          ReturnType err_code,
                          ReturnType link_code,
                          const CChar* format,
                          ...) noexcept {
    ArgListType args;
    va_start(args, format);
    ZLogManager::LogError(raw_time, err_project, err_file, err_func, err_line, err_code, link_code, format, args);
    va_end(args);
}

CORE_DLLAPI Void LogTrace(TimeType raw_time,
                          const TChar* project,
                          const TChar* format,
                          ...) noexcept {
    ArgListType args;
    va_start(args, format);
    ZLogManager::LogTrace(raw_time, project, format, args);
    va_end(args);
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZLogManager::RegisterLogServerInputFunction(port_id, input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType UnregisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZLogManager::RegisterLogServerInputFunction(port_id, input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerOutputFunction(
        IndexType port_id, Void(*output_func)(const ZLog::OutputString&)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZLogManager::RegisterLogServerOutputFunction(port_id, output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI Void UnregisterLogServerOutputFunction(Void(*output_func)(const ZLog::OutputString&)) noexcept {
    ZLogManager::UnregisterLogServerOutputFunction(output_func);
}

}//log
}//zengine