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

#include "../internal/z_drive.h"

#include "z_log.h"

namespace zengine {
namespace log {

/*
    Log error message and error location.
*/
CORE_DLLAPI Void LogError(
    TimeType _log_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ...
) noexcept;

/*
    Log trace message and trace location.
*/
CORE_DLLAPI Void LogTrace(
    TimeType _log_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format,
    ...
) noexcept;

/*
    Log info message.
*/
CORE_DLLAPI Void LogInfo(
    TimeType _log_time,
    InfoLogTypeEnum _info_type,
    const WChar* _format,
    ...
) noexcept;

/*
    Register the log server port input function, the function will be called when log happens.
    Each port can have 1 input function and 8 output function.
    Port -1(max port num - 1) is error log, 2 output function used. 
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port -3(max port num - 3) is info log, 2 output function used.
    Port 0~4 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType RegisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept;

/*
    Removes the log server port output function.
    Each port can have 1 input function and 8 output function.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port -3(max port num - 3) is info log, 2 output function used.
    Port 0~4 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType UnregisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept;

/*
    Register the log server port output function, the function will be called when log happens.
    Each port can have 1 input function and 8 output function.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port -3(max port num - 3) is info log, 2 output function used.
    Port 0~4 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType RegisterLogServerOutputFunction(
    IndexType _port_id, 
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept;

/*
    Removes the log server port output function.
    Each port can have 1 input function and 8 output function.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port -3(max port num - 3) is info log, 2 output function used.
    Port 0~4 is not used.
*/
CORE_DLLAPI Void UnregisterLogServerOutputFunction(
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept;

}//log
}//zengine