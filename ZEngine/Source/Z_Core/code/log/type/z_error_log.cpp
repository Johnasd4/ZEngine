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

#include "log/type/z_error_log.h"

#include "f_console.h"
#include "f_string.h"
#include "z_file.h"
#include "z_string.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZErrorLog::ZErrorLog() noexcept 
    : SuperType_(), proj_name_(), file_dir_(), func_name_(), err_line_(), err_code_(), link_code_() {}
ZErrorLog::ZErrorLog(
    TimeType _log_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ArgListType _args
) noexcept 
    : SuperType_(kLogType_Error, _log_time, _format, _args)
    , proj_name_(_proj_name)
    , file_dir_(_file_dir)
    , func_name_(_func_name)
    , err_line_(_err_line)
    , err_code_(_err_code)
    , link_code_(_link_code)
 {}

Void ZErrorLog::GenerateLogString(const ZLog* _log_ptr, ZLog::OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    const ZErrorLog& err_log = *reinterpret_cast<const ZErrorLog*>(_log_ptr);
    ZWString file_dir = string::String2WString(err_log.file_dir_);
    system_time.UpdateTimeFast(err_log.LogTime());
    _output_str_ptr->SetString(
        L"Time: %04d/%02d/%02d-%02d:%02d:%02d\nProject: %ls\nFile: %ls\nFunction: %ls\nLine: %d\nError Code: 0x%x\nLink Code: 0x%x\nMessage: %ls",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        err_log.proj_name_, file_dir.String(), string::String2WString(err_log.func_name_).String(), err_log.err_line_,
        err_log.err_code_, err_log.link_code_, err_log.LogMsgPtr().DataPtr()
    );
}

Void ZErrorLog::FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static constexpr WChar log_head_end[] =
        L"--------------------------------------------------------------------------------\n";
    static ZFile& file = std::invoke([]() ->ZFile& {
        static ZFile file;
        ReturnType link_code = kOK;
        TFixedWString<ZFile::kFileNameLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d_Error.log", ZLog::CreateAndGetLogPath(),
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec()
        );
        link_code = file.Open(file_dir.DataPtr(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Open() link error!");
        }
        return file;
    });
    ReturnType link_code = kOK;

    if (file.IfOpen()) {
        link_code = file.Print(L"%ls%ls\n%ls", log_head_end, _output_str.DataPtr(), log_head_end);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            link_code = file.Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Close() link error!");
            }
            return;
        }
        file.Flush();
    }
}

Void ZErrorLog::ConsoleOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static constexpr WChar log_head_end[] =
        L"--------------------------------------------------------------------------------\n";
    console::PrintError(L"%ls%ls\n%ls", log_head_end, _output_str.DataPtr(), log_head_end);
}

}//log
}//zengine