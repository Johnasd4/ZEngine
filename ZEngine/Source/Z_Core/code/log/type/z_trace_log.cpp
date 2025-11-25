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

#include "log/type/z_trace_log.h"

#include "f_console.h"
#include "f_string.h"
#include "z_file.h"
#include "z_string.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZTraceLog::ZTraceLog() noexcept : SuperType_(), proj_name_(), file_dir_(), func_name_() {}
ZTraceLog::ZTraceLog(
    TimeType _log_time, 
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format, 
    ArgListType _args
) noexcept 
    : SuperType_(kLogType_Trace, _log_time, _format, _args)
    , proj_name_(_proj_name)
    , file_dir_(_file_dir)
    , func_name_(_func_name) {}

Void ZTraceLog::GenerateLogString(const ZLog* _log_ptr, ZLog::OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    const ZTraceLog& trace_log = *reinterpret_cast<const ZTraceLog*>(_log_ptr);
    ZWString file_dir = string::String2WString(trace_log.file_dir_);
    system_time.UpdateTimeFast(trace_log.LogTime());
    _output_str_ptr->SetString(
        L"Time: %04d/%02d/%02d-%02d:%02d:%02d\nProject: %ls\nFile: %ls\nFunction: %ls\nMessage: %ls",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        trace_log.proj_name_, file_dir.String(), string::String2WString(trace_log.func_name_).String(), 
        trace_log.LogMsgPtr().DataPtr()
    );
}

Void ZTraceLog::FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static constexpr WChar log_head_end[] =
        L"--------------------------------------------------------------------------------\n";
    static ZFile& file = []() ->ZFile& {
        static ZFile file;
        ReturnType link_code = kOK;
        TFixedWString<ZFile::kFileNameLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d_Trace.log", ZLog::CreateAndGetLogPath(),
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec()
        );
        link_code = file.Open(file_dir.DataPtr(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
        }
        return file;
    }();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%ls%ls\n%ls", log_head_end, _output_str.DataPtr(), log_head_end);
    file.Flush();
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
        return;
    }
}

Void ZTraceLog::ConsoleOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static constexpr WChar log_head_end[] =
        L"--------------------------------------------------------------------------------\n";
    console::PrintTrace(L"%ls%ls\n%ls", log_head_end, _output_str.DataPtr(), log_head_end);
}

}//log
}//zengine