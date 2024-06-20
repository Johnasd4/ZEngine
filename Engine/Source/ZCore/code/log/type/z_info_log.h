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
#ifndef Z_CORE_LOG_Z_INFO_LOG_H_
#define Z_CORE_LOG_Z_INFO_LOG_H_

#include "internal/z_drive.h"

#include "m_log.h"
#include "t_array.h"

namespace zengine {
namespace log {

/*
    Error log info.
*/
class ZInfoLog : public ZLog {
public:
    static constexpr TArray<const WChar*, kLogInfoMax> kLogInfoString = 
        TArray<const WChar*, kLogInfoMax>(
            [](TArray<const WChar*, kLogInfoMax>* array_ptr) {
                (*array_ptr)[0] = L"Message";
                (*array_ptr)[1] = L"Start";
                (*array_ptr)[2] = L"Process";
                (*array_ptr)[3] = L"Finish";
                (*array_ptr)[4] = L"Success";
                (*array_ptr)[5] = L"Failure";
            });

    ZInfoLog() noexcept;
    ZInfoLog(TimeType raw_time,
             LogInfoEnum info_type,
             const WChar* format,
             ArgListType args) noexcept;

    /*
        Override it to output different formats.
    */
    static Void GenerateLogString(const ZLog* log_ptr, OutputString* output_str_ptr) noexcept;

    /*
        Console output error log string.
    */
    static Void FileOutputLogString(const ZLog* log_ptr, const ZLog::OutputString& output_str) noexcept;

    /*
        File output error log string.
    */
    static Void ConsoleOutputLogString(const ZLog* log_ptr, const ZLog::OutputString& output_str) noexcept;

protected:
    using SuperType = ZLog;

private:
    TimeType raw_time_;
    LogInfoEnum info_type_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_Z_INFO_LOG_H_
