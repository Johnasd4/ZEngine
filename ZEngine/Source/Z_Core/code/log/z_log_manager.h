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
#ifndef Z_CORE_LOG_Z_LOG_MANAGER_H_
#define Z_CORE_LOG_Z_LOG_MANAGER_H_

#include "internal/z_drive.h"

#include "type/z_error_log.h"
#include "type/z_info_log.h"
#include "type/z_trace_log.h"
#include "t_log_queue.h"
#include "z_log_server.h"
#include "z_thread.h"

namespace zengine {
namespace log {



/*
    The log manager, log's the error and info to the console, file and any place that needs to log.
*/
class ZLogManager : public ZObject {
public:
    static constexpr IndexType kLogQueueSize = 1024;

    static constexpr IndexType kErrorLogPortID = ZLogServer::kMaxPortNum - 1;
    static constexpr IndexType kTraceLogPortID = ZLogServer::kMaxPortNum - 2;
    static constexpr IndexType kInfoLogPortID = ZLogServer::kMaxPortNum - 3;
    static constexpr IndexType kLogPortIDMin = - 3;
    static constexpr IndexType kLogPortIDMax = ZLogServer::kMaxPortNum - kLogPortIDMin;

    static Void LogError(
        TimeType _raw_time,
        const WChar* _proj_name,
        const Char* _file_dir,
        const Char* _func_name,
        Int32 _err_line,
        ReturnType _err_code,
        ReturnType _link_code,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    static Void LogTrace(
        TimeType _raw_time,
        const WChar* _proj_name,
        const Char* _file_dir,
        const Char* _func_name,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    static Void LogInfo(
        TimeType _raw_time,
        LogInfoEnum _info_type,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    /*
        Register the log server port input function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerInputFunction(
        IndexType _port_id, 
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Removes the log server port output function.
    */
    NODISCARD static ReturnType UnregisterLogServerInputFunction(
        IndexType _port_id, 
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Register the log server port output function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerOutputFunction(
        IndexType _port_id, 
        Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
    ) noexcept;

    /*
        Removes the log server port output function.
    */
    static Void UnregisterLogServerOutputFunction(
        Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
    ) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    static ZLogManager& InstanceP() noexcept;

    static Void LogThread() noexcept;

    ZLogManager(const ZLogManager&) = delete;
    ZLogManager(ZLogManager&&) = delete;
    ZLogManager& operator=(const ZLogManager&) = delete;
    ZLogManager& operator=(ZLogManager&&) = delete;

    /*
        the last port is error log and the second last port is trace log.
    */
    ZLogManager() noexcept;

    ~ZLogManager() noexcept;

    TLogQueue<ZErrorLog, kLogQueueSize> error_log_queue_;
    TLogQueue<ZTraceLog, kLogQueueSize> trace_log_queue_;
    TLogQueue<ZInfoLog, kLogQueueSize> info_log_queue_;
    TArray<TLogQueue<ZErrorLog, kLogQueueSize>, ZLogServer::kMaxPortNum - kLogPortIDMin> log_queue_array_;
    ZLogServer log_server_;
    Bool log_thread_finished_;
    ZThread log_thread_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_Z_LOG_MANAGER_H_
