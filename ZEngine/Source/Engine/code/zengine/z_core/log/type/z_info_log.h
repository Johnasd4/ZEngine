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

#include "../../t_array.h"

#include "../z_log.h"

namespace zengine {
namespace log {

/*
    Error log info.
*/
class ZInfoLog : public ZLog {
public:
    static constexpr TArray<const WChar*, kInfoLogType_Max> kInfoLogType_String = 
        TArray<const WChar*, kInfoLogType_Max>(
            [](TArray<const WChar*, kInfoLogType_Max>* _array_ptr) {
                (*_array_ptr)[kInfoLogType_Message] = L"Message";
                (*_array_ptr)[kInfoLogType_Start] = L"Start  ";
                (*_array_ptr)[kInfoLogType_Process] = L"Process";
                (*_array_ptr)[kInfoLogType_Finish] = L"Finish ";
                (*_array_ptr)[kInfoLogType_Success] = L"Success";
                (*_array_ptr)[kInfoLogType_Failure] = L"Failure";
            });

    ZInfoLog() noexcept;
    ZInfoLog(TimeType _log_time, InfoLogTypeEnum _info_type, const WChar* _format, ArgListType _args) noexcept;

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

    NODISCARD FORCEINLINE const InfoLogTypeEnum InfoType() const noexcept { return info_type_; }

protected:
    using SuperType_ = ZLog;

private:
    InfoLogTypeEnum info_type_;
};

}//log
}//zengine