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
        const CChar* err_file,
        const CChar* err_func,
        Int32 err_line,
        ReturnType err_code,
        ReturnType link_code,
        const CChar* format,
        ArgListType args) noexcept {
    static ZLogManager& log_manager = ZLogManager::InstanceP();
    log_manager.error_log_queue_.Push(raw_time, err_file, err_func, err_line, err_code, link_code, format, args);
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
            log_manager.log_server_.OutputLog(0 ,&log_manager.error_log_queue_.Front());
            if_log = true;
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
    link_code = log_server_.RegisterInputFunction(ErrorLogPortID, ZErrorLog::GenerateLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterInputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(ErrorLogPortID, ZErrorLog::FileOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
    link_code = log_server_.RegisterOutputFunction(ErrorLogPortID, ZErrorLog::ConsoleOutputLogString);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZLogServer::RegisterOutputFunction() link error!");
    }
}

ZLogManager::~ZLogManager() noexcept {
    log_thread_finished_ = true;
    if (log_thread_.Joinable()) {
        log_thread_.Join();
    }
}

}//log
}//zengine