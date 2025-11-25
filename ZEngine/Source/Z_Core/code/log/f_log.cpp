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

#include "log/f_log.h"

#include "f_console.h"
#include "z_string.h"
#include "z_system_time.h"

#include "z_log_manager.h"

namespace zengine {
namespace log {

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
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogError(_log_time, _proj_name, _file_dir, _func_name, _err_line, _err_code, _link_code, _format, args);
    va_end(args);
}

CORE_DLLAPI Void LogTrace(
    TimeType _log_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogTrace(_log_time, _proj_name, _file_dir, _func_name, _format, args);
    va_end(args);
}

CORE_DLLAPI Void LogInfo(
    TimeType _log_time,
    InfoLogTypeEnum _info_type,
    const WChar* _format,
    ...
) noexcept {
    ArgListType args;
    va_start(args, _format);
    ZLogManager::LogInfo(_log_time, _info_type, _format, args);
    va_end(args);
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerInputFunction(
    SizeType _port_id,
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)) noexcept 
{
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id > ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType UnregisterLogServerInputFunction(
    SizeType _port_id,
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id > ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RegisterLogServerOutputFunction(
    SizeType _port_id,
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _port_id < ZLogManager::kLogPortIDMin || _port_id > ZLogManager::kLogPortIDMax, 
        error_code::kMLogErrorCode_PortIDOutOfRange,
        L"port_id %d out of range!", 
        _port_id);

    link_code = ZLogManager::RegisterLogServerOutputFunction(_port_id, _output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogManager::RegisterLogServerInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI Void UnregisterLogServerOutputFunction(
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ZLogManager::UnregisterLogServerOutputFunction(_output_func);
}

CORE_DLLAPI Void FinishFlush(TimeType _max_wait_time_ms) noexcept {
    ZLogManager::FinishFlush(_max_wait_time_ms);
}

}//log
}//zengine