/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "z_error_log.h"

#include "f_console.h"
#include "t_vector.h"
#include "z_file.h"
#include "z_string.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZErrorLog::ZErrorLog() noexcept 
    : raw_time_(), proj_str_(), file_str_(), func_str_(), err_line_(), err_code_(), link_code_(), SuperType_() {}
ZErrorLog::ZErrorLog(
    TimeType _raw_time,
    const WChar* _proj_str,
    const Char* _file_str,
    const Char* _func_str,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ArgListType _args
) noexcept 
    : raw_time_(_raw_time)
    , proj_str_(_proj_str)
    , file_str_(_file_str)
    , func_str_(_func_str)
    , err_line_(_err_line)
    , err_code_(_err_code)
    , link_code_(_link_code)
    , SuperType_(_format, _args) {}

Void ZErrorLog::GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime system_time;
    static TVector<WChar> file_str;
    static TVector<WChar> func_str;
    const ZErrorLog& err_log = *reinterpret_cast<const ZErrorLog*>(_log_ptr);
    string::String2WString(err_log.file_str_, &file_str);
    string::String2WString(err_log.func_str_, &func_str);
    system_time.UpdateTimeFast(err_log.raw_time_);
    _output_str_ptr->w_str_.SetString(
        L"--------------------------------------------------------------------------------\nTime: %04d/%02d/%02d-%02d:%02d:%02d\nProject: %ls\nFile: %ls\nFunction: %ls\nLine: %d\nError Code: 0x%x\nLink Code: 0x%x\nMessage: %ls\n--------------------------------------------------------------------------------",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        err_log.proj_str_, file_str.DataPtr(), func_str.DataPtr(), err_log.err_line_,
        err_log.err_code_, err_log.link_code_, err_log.LogMsgPtr().str_.DataPtr());
}

static ZFile& GetLogFile() noexcept {
    static ZFile& file = []() ->ZFile& {
        static ZFile file;
        ReturnType link_code = kOK;
        TWFixedString<ZFile::kFileNameLength> file_str;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_str.SetString(
            L"%ls%04d%02d%02d%02d%02d%02d_error.log", ZLog::kPathTString,
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec());
        link_code = file.OpenSafe(ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendW);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, L"ZFile::OpenSafe() link error!");
        }
        return file;
    }();
    return file;
}

Void ZErrorLog::FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print(L"%ls\n", _output_str.w_str_.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, L"ZFile::Print() link error!");
    }
}

Void ZErrorLog::ConsoleOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    console::PrintError(L"%ls\n", _output_str.w_str_.DataPtr());
}

}//log
}//zengine