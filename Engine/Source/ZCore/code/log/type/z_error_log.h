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
#ifndef Z_CORE_LOG_Z_ERROR_LOG_H_
#define Z_CORE_LOG_Z_ERROR_LOG_H_

#include "internal/z_drive.h"

#include "m_log.h"

namespace zengine {
namespace log {

/*
    Error log info.
*/
class ZErrorLog : public ZLog {
public:
    ZErrorLog() noexcept;
    ZErrorLog(TimeType raw_time,
              const Char* err_project,
              const Char* err_file,
              const Char* err_func,
              Int32 err_line,
              ReturnType err_code,
              ReturnType link_code,
              const Char* format,
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
    const Char* err_project_;
    const Char* err_file_;
    const Char* err_func_;
    Int32 err_line_;
    ReturnType err_code_;
    ReturnType link_code_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_Z_ERROR_LOG_H_
