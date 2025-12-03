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
#pragma once

#include "../drive.h"

#include "../t_fixed_string.h"

#include "c_log.h"

namespace zengine {
namespace log {

/*
    Base class of the log.
*/
class ZLog : public ZObject {
public:
    using FormatString_ = TFixedWString<kFormatStringMaxSize>;
    using OutputString_ = TFixedWString<kOutputStringMaxSize>;

    /*
        Create and get the log path.
    */
    CORE_DLLAPI static const WChar* CreateAndGetLogPath() noexcept;

    /*
        Override it to output different formats, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept;

    /*
        Default console output log string, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    /*
        Default file output log string, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void ConsoleOutputLog(
            const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    CORE_DLLAPI ZLog() noexcept;
    CORE_DLLAPI ZLog(
        LogTypeEnum _log_type, 
        const TimeType& _log_time, 
        const WChar* _format, ...
    ) noexcept;
    CORE_DLLAPI ZLog(
        LogTypeEnum _log_type, 
        const TimeType& _log_time, 
        const WChar* _format, 
        ArgListType _args
    ) noexcept;

    NODISCARD FORCEINLINE const LogTypeEnum LogType() const noexcept { return log_type_; }
    NODISCARD FORCEINLINE const TimeType& LogTime() const noexcept { return log_time_; }
    NODISCARD FORCEINLINE const FormatString_& LogMsgPtr() const noexcept { return log_msg_str_; }

protected:
    using SuperType_ = ZObject;

private:
    ZLog(const ZLog&) = delete;
    ZLog(ZLog&&) = delete;
    ZLog& operator=(const ZLog&) = delete;
    ZLog& operator=(ZLog&&) = delete;

    LogTypeEnum log_type_;
    TimeType log_time_;
    FormatString_ log_msg_str_;
};

}//log
}//zengine