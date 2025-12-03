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

#include "../../drive.h"

#include "../z_log.h"

namespace zengine {
namespace log {

/*
    Error log info.
*/
class ZErrorLog : public ZLog {
public:
    ZErrorLog() noexcept;
    ZErrorLog(
        TimeType _log_time,
        const WChar* _proj_name,
        const Char* _file_dir,
        const Char* _func_name,
        Int32 _err_line,
        ReturnType _err_code,
        ReturnType _link_code,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    /*
        Override it to output different formats.
    */
    static Void GenerateLogString(const ZLog* _log_ptr, ZLog::OutputString_* _output_str_ptr) noexcept;

    /*
        Console output error log string.
    */
    static Void FileOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    /*
        File output error log string.
    */
    static Void ConsoleOutputLog(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

protected:
    using SuperType_ = ZLog;

private:
    const WChar* proj_name_;
    const Char* file_dir_;
    const Char* func_name_;
    Int32 err_line_;
    ReturnType err_code_;
    ReturnType link_code_;
};

}//log
}//zengine