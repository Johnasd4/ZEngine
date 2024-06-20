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

#include "z_trace_log.h"

#include "f_console.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZTraceLog::ZTraceLog() noexcept : raw_time_(), project_(), SuperType() {}
ZTraceLog::ZTraceLog(TimeType raw_time, const WChar* project, const WChar* format, ArgListType args) noexcept 
        : raw_time_(raw_time), project_(project), SuperType(format, args) {}

Void ZTraceLog::GenerateLogString(const ZLog* log_ptr, OutputString* outpuw_str_ptr) noexcept {
    static ZSystemTime system_time;
    ZTraceLog& trace_log = *(ZTraceLog*)log_ptr;
    system_time.UpdateTimeFast(trace_log.raw_time_);
    outpuw_str_ptr->w_str.SetString(
        L"%04d/%02d/%02d-%02d:%02d:%02d | <%ls> %ls",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        trace_log.project_, trace_log.LogMsgPtr().w_str.DataPtr());
}

static ZFile& GetLogFile() noexcept {
    static ZFile file;
    ReturnType link_code = kOK;
    TWFixedString<ZFile::kFileNameLength> file_str;
    ZSystemTime system_time;

    file_str.SetString(L"%lstrace_%04d%02d%02d%02d%02d%02d.log", ZLog::kPathTString,
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec());
    link_code = file.OpenSafe(ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendT);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::OpenSafe() link error!");
    }
    return file;
}

Void ZTraceLog::FileOutputLogString(const ZLog* log_ptr, const ZLog::OutputString& output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%ls\n", output_str.w_str.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::Print() link error!");
    }
}

Void ZTraceLog::ConsoleOutputLogString(const ZLog* log_ptr, const ZLog::OutputString& output_str) noexcept {
    console::PrintTrace("%ls\n", output_str.w_str.DataPtr());
}

}//log
}//zengine