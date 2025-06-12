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

#include "m_log.h"

#include "f_console.h"
#include "f_file_system.h"
#include "z_file.h"
#include "z_string.h"
#include "z_system_time.h"

#include "log/z_log_manager.h"

namespace zengine {
namespace log {

CORE_DLLAPI ZLog::ZLog() noexcept : SuperType_(), log_msg_str_() {}

CORE_DLLAPI ZLog::ZLog(const Char* _format, ...) noexcept : SuperType_() {
    ArgListType args;
    va_start(args, _format);
    vsprintf(log_msg_str_.str_.DataPtr(), _format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(const Char* _format, ArgListType _args) noexcept : SuperType_() {
    vsprintf(log_msg_str_.str_.DataPtr(), _format, _args);
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

CORE_DLLAPI const WChar* ZLog::CreateAndGetLogPath() noexcept {
    //log file path.
    static ZWString path_str = []() -> ZWString {
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        TFixedWString<ZFile::kFileNameLength> path_str;
        path_str.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d", kLogFileRootPathDir,
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec());
        //create log path.
        ReturnType link_code = file_system::CreateDirectoryByPath(path_str.String());
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code, L"file_system::CreateDirectoryByPath() link error!");
        }

        //clear the expired log files.
        TList<ZWString> dir_list;
        file_system::GetDirectoriesByPath(kLogFileRootPathDir, &dir_list);
        IndexType del_dir_num = dir_list.Size() - log::ZLog::kLogFileMaxNum;
        for (IndexType count = 0; count < del_dir_num; ++count) {
            file_system::DeleteDirectoryByPath(dir_list.Front().String());
            dir_list.PopFront();
        }

        return ZWString(path_str.String());
    }();
    return path_str.String();
}

CORE_DLLAPI Void ZLog::GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept {
    //copy the full msg.
    memcpy(
        &(_output_str_ptr->str_), 
        &(_log_ptr->log_msg_str_.str_), 
        sizeof(_log_ptr->log_msg_str_.str_.Capacity()));
}

static ZFile& GetLogFile() noexcept {
    static ZFile& file = []() ->ZFile& { 
        static ZFile file;
        ReturnType link_code = kOK;
        TFixedWString<ZFile::kFileNameLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d_default.log", ZLog::CreateAndGetLogPath(),
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec());
        link_code = file.Open(file_dir.DataPtr(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
        }
        return file;
    }();
    return file;
}

CORE_DLLAPI Void ZLog::FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%ls\n", _output_str.w_str_.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
    }
}

CORE_DLLAPI Void ZLog::ConsoleOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    console::PrintMessage(L"%ls\n", _output_str.w_str_.DataPtr());
}

CORE_DLLAPI Void LogError(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir, 
    const Char* _func_name,
    Int32 _err_line, 
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogError(_raw_time, _proj_name, _file_dir, _func_name, _err_line, _err_code, _link_code, _format, args);
    va_end(args);
}

CORE_DLLAPI Void LogTrace(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogTrace(_raw_time, _proj_name, _file_dir, _func_name, _format, args);
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
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
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
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
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
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerOutputFunction(_port_id, _output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
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