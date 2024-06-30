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

#include "z_error_log.h"

#include "f_console.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZErrorLog::ZErrorLog() noexcept 
    : raw_time_(), err_project_(), err_file_(), err_func_(), err_line_(), err_code_(), link_code_(), SuperType_() {}
ZErrorLog::ZErrorLog(
    TimeType _raw_time,
    const Char* _err_project,
    const Char* _err_file,
    const Char* _err_func,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const Char* _format,
    ArgListType _args
) noexcept 
    : raw_time_(_raw_time)
    , err_project_(_err_project)
    , err_file_(_err_file)
    , err_func_(_err_func)
    , err_line_(_err_line)
    , err_code_(_err_code)
    , link_code_(_link_code)
    , SuperType_(_format, _args) {}

Void ZErrorLog::GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime system_time;
    ZErrorLog& err_log = *(ZErrorLog*)_log_ptr;
    system_time.UpdateTimeFast(err_log.raw_time_);
    _output_str_ptr->c_str_.SetString(
        "--------------------------------------------------------------------------------\nTime: %04d/%02d/%02d-%02d:%02d:%02d\nProject: %s\nFile: %s\nFunction: %s\nLine: %d\nError Code: 0x%x\nLink Code: 0x%x\nMessage: %s\n--------------------------------------------------------------------------------",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        err_log.err_project_, err_log.err_file_, err_log.err_func_, err_log.err_line_, 
        err_log.err_code_, err_log.link_code_, err_log.LogMsgPtr().c_str_.DataPtr());
}

static ZFile& GetLogFile() noexcept {
    static ZFile file;
    ReturnType link_code = kOK;
    TWFixedString<ZFile::kFileNameLength> file_str;
    ZSystemTime system_time;

    file_str.SetString(L"%ls%04d%02d%02d%02d%02d%02d_error.log", ZLog::kPathTString,
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec());
    link_code = file.OpenSafe(ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendT);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::OpenSafe() link error!");
    }
    return file;
}

Void ZErrorLog::FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = GetLogFile();
    ReturnType link_code = kOK;

    link_code = file.Print("%s\n", _output_str.c_str_.DataPtr());
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::Print() link error!");
    }
}

Void ZErrorLog::ConsoleOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    console::PrintError("%s\n", _output_str.c_str_.DataPtr());
}

}//log
}//zengine