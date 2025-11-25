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

#include "log/type/z_info_log.h"

#include "f_console.h"
#include "m_log.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

ZInfoLog::ZInfoLog() noexcept : SuperType_(), info_type_() {}
ZInfoLog::ZInfoLog(TimeType _log_time, InfoLogTypeEnum _info_type, const WChar* _format, ArgListType _args) noexcept 
    : SuperType_(kLogType_Info, _log_time, _format, _args), info_type_(_info_type) {}

Void ZInfoLog::GenerateLogString(const ZLog* _log_ptr, ZLog::OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    const ZInfoLog& info_log = *reinterpret_cast<const ZInfoLog*>(_log_ptr);
    system_time.UpdateTimeFast(info_log.LogTime()); 
  
    _output_str_ptr->SetString(
        L"%04d/%02d/%02d-%02d:%02d:%02d | %ls | %ls",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        kInfoLogType_String[info_log.info_type_], info_log.LogMsgPtr().DataPtr()
    );
    //_output_str_ptr->SetString(
    //    L"%04d/%02d/%02d-%02d:%02d:%02d | %ls",
    //    system_time.Year(), system_time.Month(), system_time.Day(),
    //    system_time.Hour(), system_time.Min(), system_time.Sec(),
    //    info_log.LogMsgPtr().DataPtr()
    //);
}

Void ZInfoLog::FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    static ZFile& file = []() ->ZFile& {
        static ZFile file;
        ReturnType link_code = kOK;
        TFixedWString<ZFile::kFileNameLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d_Info.log", ZLog::CreateAndGetLogPath(),
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

    link_code = file.Print(L"%ls\n", _output_str.DataPtr());
    file.Flush();
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
    }
}

Void ZInfoLog::ConsoleOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept {
    ZInfoLog& info_log = *(ZInfoLog*)_log_ptr;
    switch (info_log.info_type_) {
    case kInfoLogType_Message:
        console::PrintMessage(L"%ls\n", _output_str.DataPtr());
        break;
    case kInfoLogType_Start:
        console::PrintStart(L"%ls\n", _output_str.DataPtr());
        break;
    case kInfoLogType_Process:
        console::PrintProcess(L"%ls\n", _output_str.DataPtr());
        break;
    case kInfoLogType_Finish:
        console::PrintFinish(L"%ls\n", _output_str.DataPtr());
        break;
    case kInfoLogType_Success:
        console::PrintSuccess(L"%ls\n", _output_str.DataPtr());
        break;
    case kInfoLogType_Failure:
        console::PrintFailure(L"%ls\n", _output_str.DataPtr());
        break;
    }
}

}//log
}//zengine