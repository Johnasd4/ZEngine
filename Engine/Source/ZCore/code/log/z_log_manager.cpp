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
#define CORE_DLLFILE

#include "z_log_manager.h"

namespace zengine {
namespace log {

Void ZLogManager::LogError(TimeType raw_time,
                           const CChar* err_project,
                           const CChar* err_file,
                           const CChar* err_func,
                           Int32 err_line,
                           ReturnType err_code,
                           ReturnType link_code,
                           const CChar* format,
                           ArgListType args) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.error_log_queue_.Push(
        raw_time, err_project, err_file, err_func, err_line, err_code, link_code, format, args);
}

Void ZLogManager::LogTrace(TimeType raw_time,
                           const TChar* project,
                           const TChar* format,
                           ArgListType args) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.trace_log_queue_.Push(raw_time, project, format, args);
}

Void ZLogManager::LogInfo(TimeType raw_time,
                          LogInfoEnum info_type,
                          const TChar* format,
                          ArgListType args) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.info_log_queue_.Push(raw_time, info_type, format, args);
}

NODISCARD ReturnType ZLogManager::RegisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    port_id = (port_id + ZLogServer::kMaxPortNum) % ZLogServer::kMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.RegisterInputFunction(port_id, input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogServer::RegisterInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZLogManager::UnregisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    port_id = (port_id + ZLogServer::kMaxPortNum) % ZLogServer::kMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.UnregisterInputFunction(port_id, input_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogServer::UnregisterInputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZLogManager::RegisterLogServerOutputFunction(
        IndexType port_id, Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    port_id = (port_id + ZLogServer::kMaxPortNum) % ZLogServer::kMaxPortNum;
    link_code = ZLogManager::InstanceP().log_server_.RegisterOutputFunction(port_id, output_func);
    if (link_code != kOK) {
        ret_val = error_code::kMLogErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogServer::RegisterOutputFunction() link error!");
        return ret_val;
    }

    return ret_val;
}

Void ZLogManager::UnregisterLogServerOutputFunction(
        Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept {
    ZLogManager::InstanceP().log_server_.UnregisterOutputFunction(output_func);
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
            log_manager.error_log_queue_.Pop();
            if_log = true;
        }

        //trace log
        if (!log_manager.trace_log_queue_.Empty()) {
            log_manager.log_server_.OutputLog(kTraceLogPortID, &log_manager.trace_log_queue_.Front());
            log_manager.trace_log_queue_.Pop();
            if_log = true;
        }

        //info log
        if (!log_manager.info_log_queue_.Empty()) {
            log_manager.log_server_.OutputLog(kInfoLogPortID, &log_manager.info_log_queue_.Front());
            log_manager.info_log_queue_.Pop();
            if_log = true;
        }

        //log
        for (IndexType port_id = 0; port_id < log_manager.log_queue_array_.Capacity(); ++port_id) {
            if (!log_manager.log_queue_array_[port_id].Empty()) {
                log_manager.log_server_.OutputLog(port_id, &log_manager.log_queue_array_[port_id].Front());
                log_manager.log_queue_array_[port_id].Pop();
                if_log = true;
            }
        }

        //log str
        if (!if_log) {
            Sleep(1);
        }
    }
}

ZLogManager::ZLogManager() noexcept : SuperType(), 
        error_log_queue_(), log_queue_array_(), 
        log_server_(), log_thread_finished_(false), log_thread_(&ZLogManager::LogThread) {
    ReturnType link_code = kOK;
    link_code = log_server_.RegisterInputFunction(kErrorLogPortID, ZErrorLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterInputFunction() link error!");
    }
    link_code = log_server_.RegisterInputFunction(kTraceLogPortID, ZTraceLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterInputFunction() link error!");
    }
    link_code = log_server_.RegisterInputFunction(kInfoLogPortID, ZInfoLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterInputFunction() link error!");
    }
#if USE_FILE_LOG
    link_code = log_server_.RegisterOutputFunction(kErrorLogPortID, ZErrorLog::FileOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(kTraceLogPortID, ZTraceLog::FileOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(kInfoLogPortID, ZInfoLog::FileOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
#endif
#if USE_CONSOLE_LOG
    link_code = log_server_.RegisterOutputFunction(kErrorLogPortID, ZErrorLog::ConsoleOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(kTraceLogPortID, ZTraceLog::ConsoleOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(kInfoLogPortID, ZInfoLog::ConsoleOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
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