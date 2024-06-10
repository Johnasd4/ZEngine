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
    static constexpr IndexType kLogQueueSize = 128;

    static constexpr IndexType kErrorLogPortID = ZLogServer::kMaxPortNum - 1;
    static constexpr IndexType kTraceLogPortID = ZLogServer::kMaxPortNum - 2;
    static constexpr IndexType kInfoLogPortID = ZLogServer::kMaxPortNum - 3;
    static constexpr IndexType kLogPortIDMin = - 3;
    static constexpr IndexType kLogPortIDMax = ZLogServer::kMaxPortNum - kLogPortIDMin;

    static Void LogError(TimeType raw_time,
                         const CChar* err_project,
                         const CChar* err_file, 
                         const CChar* err_func,
                         Int32 err_line, 
                         ReturnType err_code,
                         ReturnType link_code,
                         const CChar* format,
                         ArgListType args) noexcept;

    static Void LogTrace(TimeType raw_time,
                         const TChar* project,
                         const TChar* format,
                         ArgListType args) noexcept;

    static Void LogInfo(TimeType raw_time,
                        LogInfoEnum info_type,
                        const TChar* format,
                        ArgListType args) noexcept;

    /*
        Register the log server port input function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept;

    /*
        Removes the log server port output function.
    */
    NODISCARD static ReturnType UnregisterLogServerInputFunction(
        IndexType port_id, Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept;

    /*
        Register the log server port output function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerOutputFunction(
        IndexType port_id, Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept;

    /*
        Removes the log server port output function.
    */
    static Void UnregisterLogServerOutputFunction(Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept;

protected:
    using SuperType = ZObject;

private:
    static ZLogManager& InstanceP() noexcept;

    static Void LogThread() noexcept;

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
