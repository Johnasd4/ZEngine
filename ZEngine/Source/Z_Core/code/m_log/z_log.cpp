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

#include "f_console.h"
#include "f_file_system.h"
#include "z_file.h"
#include "z_string.h"
#include "z_system_time.h"

#include "z_log_manager.h"

namespace zengine {
namespace log {

CORE_DLLAPI ZLog::ZLog() noexcept : SuperType_(), log_msg_str_(), log_time_(), log_type_() {}

CORE_DLLAPI ZLog::ZLog(
    LogTypeEnum _log_type,
    const TimeType& _log_time,
    const WChar* _format, ...
) noexcept 
    : SuperType_() 
    , log_type_(_log_type)
    , log_time_(_log_time)
{
    ArgListType args;
    va_start(args, _format);
    vswprintf(log_msg_str_.DataPtr(), _format, args);
    va_end(args);
}

CORE_DLLAPI ZLog::ZLog(
    LogTypeEnum _log_type,
    const TimeType& _log_time,
    const WChar* _format,
    ArgListType _args
) noexcept
    : SuperType_()
    , log_type_(_log_type)
    , log_time_(_log_time)
{
    vswprintf(log_msg_str_.DataPtr(), _format, _args);
}

CORE_DLLAPI const WChar* ZLog::CreateAndGetLogPath() noexcept {
    //log file path.
    static ZWString path_str = std::invoke([]() -> ZWString {
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
        for (SizeType count = log::kLogFileMaxNum; count < dir_list.Size(); ++count) {
            file_system::DeleteDirectoryByPath(dir_list.Front().String());
            dir_list.PopFront();
        }

        return ZWString(path_str.String());
    });
    return path_str.String();
}

CORE_DLLAPI Void ZLog::GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept {
    //copy the full msg.
    Copy(
        &(_output_str_ptr), 
        &(_log_ptr->log_msg_str_), 
        sizeof(_log_ptr->log_msg_str_.Capacity())
    );
}

static ZFile& GetLogFile() noexcept {
    static ZFile& file = std::invoke([]() ->ZFile& {
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
    });
    return file;
}

CORE_DLLAPI Void ZLog::FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%ls\n", _output_str.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
    }
}

CORE_DLLAPI Void ZLog::ConsoleOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    console::PrintMessage(L"%ls\n", _output_str.DataPtr());
}

}//log
}//zengine