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
    static constexpr TArray<const WChar*, kLogInfo_Max> kLogInfo_String = 
        TArray<const WChar*, kLogInfo_Max>(
            [](TArray<const WChar*, kLogInfo_Max>* _array_ptr) {
                (*_array_ptr)[0] = L"Message";
                (*_array_ptr)[1] = L"Start";
                (*_array_ptr)[2] = L"Process";
                (*_array_ptr)[3] = L"Finish";
                (*_array_ptr)[4] = L"Success";
                (*_array_ptr)[5] = L"Failure";
            });

    ZInfoLog() noexcept;
    ZInfoLog(TimeType _raw_time, LogInfoEnum _info_type, const WChar* _format, ArgListType _args) noexcept;

    /*
        Override it to output different formats.
    */
    static Void GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept;

    /*
        Console output error log string.
    */
    static Void FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    /*
        File output error log string.
    */
    static Void ConsoleOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

protected:
    using SuperType_ = ZLog;

private:
    TimeType raw_time_;
    LogInfoEnum info_type_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_Z_INFO_LOG_H_
