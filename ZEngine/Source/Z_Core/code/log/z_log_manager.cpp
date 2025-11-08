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

#include "z_log_manager.h"

namespace zengine {
namespace log {

Void ZLogManager::LogError(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ArgListType _args
) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.error_log_queue_.EmplaceBack(
        _raw_time, _proj_name, _file_dir, _func_name, _err_line, _err_code, _link_code, _format, _args
    );
}

Void ZLogManager::LogTrace(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format,
    ArgListType _args
) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.trace_log_queue_.EmplaceBack(
        _raw_time, _proj_name, _file_dir, _func_name, _format, _args
    );
}

Void ZLogManager::LogInfo(
    TimeType _raw_time,
    InfoLogTypeEnum _info_type,
    const WChar* _format,
    ArgListType _args
) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.info_log_queue_.EmplaceBack(
        _raw_time, _info_type, _format, _args
    );
}

NODISCARD ReturnType ZLogManager::RegisterLogServerInputFunction(
    SizeType _port_id, Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    _port_id = (_port_id + kLogMaxPortNum) % kLogMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.RegisterInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogServer::RegisterInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZLogManager::UnregisterLogServerInputFunction(
    SizeType _port_id, Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    _port_id = (_port_id + kLogMaxPortNum) % kLogMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.UnregisterInputFunction(_port_id, _input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogServer::UnregisterInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZLogManager::RegisterLogServerOutputFunction(
    SizeType _port_id, Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    _port_id = (_port_id + kLogMaxPortNum) % kLogMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.RegisterOutputFunction(_port_id, _output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogServer::RegisterOutputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

Void ZLogManager::UnregisterLogServerOutputFunction(
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ZLogManager::InstanceP().log_server_.UnregisterOutputFunction(_output_func);
}

ZLogManager& ZLogManager::InstanceP() noexcept {
    static ZLogManager log_manager;
    return log_manager;
}

Void ZLogManager::LogThread() noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    Bool if_log = true;

    while (log_manager.log_thread_finished_ == false || if_log) {
        if_log = false;
        //err log
        if (!log_manager.error_log_queue_.Empty()) {
            log_manager.log_server_.OutputLog(kErrorLogPortID, &log_manager.error_log_queue_.Front());
            log_manager.error_log_queue_.PopFront();
            if_log = true;
        }

        //trace log
        if (!log_manager.trace_log_queue_.Empty()) {
            log_manager.log_server_.OutputLog(kTraceLogPortID, &log_manager.trace_log_queue_.Front());
            log_manager.trace_log_queue_.PopFront();
            if_log = true;
        }

        //info log
        if (!log_manager.info_log_queue_.Empty()) {
            log_manager.log_server_.OutputLog(kInfoLogPortID, &log_manager.info_log_queue_.Front());
            log_manager.info_log_queue_.PopFront();
            if_log = true;
        }

        //log
        for (SizeType port_id = 0; port_id < log_manager.log_queue_array_.Capacity(); ++port_id) {
            if (!log_manager.log_queue_array_[port_id].Empty()) {
                log_manager.log_server_.OutputLog(port_id, &log_manager.log_queue_array_[port_id].Front());
                log_manager.log_queue_array_[port_id].PopFront();
                if_log = true;
            }
        }

        //log str
        if (!if_log) {
            SleepMs(1);
        }
    }
}

ZLogManager::ZLogManager() noexcept 
        : SuperType_() 
        , error_log_queue_()
        , log_queue_array_()
        , log_server_()
        , log_thread_finished_(false)
        , log_thread_(&ZLogManager::LogThread) {
    ReturnType link_code = kOK;

    //Register the the default ports.
    link_code = log_server_.RegisterInputFunction(kErrorLogPortID, ZErrorLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code, 
            L"ZLogServer::RegisterInputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterInputFunction(kTraceLogPortID, ZTraceLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code, 
            L"ZLogServer::RegisterInputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterInputFunction(kInfoLogPortID, ZInfoLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterInputFunction() link error!"
        );
    }
#if USE_FILE_LOG
    link_code = log_server_.RegisterOutputFunction(kErrorLogPortID, ZErrorLog::FileOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code, 
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterOutputFunction(kTraceLogPortID, ZTraceLog::FileOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterOutputFunction(kInfoLogPortID, ZInfoLog::FileOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
#endif
#if USE_CONSOLE_LOG
    link_code = log_server_.RegisterOutputFunction(kErrorLogPortID, ZErrorLog::ConsoleOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterOutputFunction(kTraceLogPortID, ZTraceLog::ConsoleOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
    link_code = log_server_.RegisterOutputFunction(kInfoLogPortID, ZInfoLog::ConsoleOutputLog);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kMLogErrorCode_LinkError, link_code,
            L"ZLogServer::RegisterOutputFunction() link error!"
        );
    }
#endif
}

ZLogManager::~ZLogManager() noexcept {
    log_thread_finished_ = true;
    if (log_thread_.Joinable()) {
        log_thread_.Join();
    }
}

}//log
}//zengine