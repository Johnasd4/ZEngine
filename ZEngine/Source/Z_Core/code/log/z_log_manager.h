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

#include "drive.h"

#include "log/type/z_error_log.h"
#include "log/type/z_info_log.h"
#include "log/type/z_trace_log.h"
#include "t_atom.h"
#include "t_fixed_queue.h"
#include "z_log_server.h"
#include "z_thread.h"

namespace zengine {
namespace log {



/*
    The log manager, log's the error and info to the console, file and any place that needs to log.
*/
class ZLogManager : public ZObject {
public:
    static constexpr SizeType kLogQueueSize = 1024;

    static constexpr SizeType kLogPortIDMin = 0;
    static constexpr SizeType kLogPortIDMax = kLogMaxPortNum - 1;

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
        InfoLogTypeEnum _info_type,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    /*
        Register the log server port input function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerInputFunction(
        SizeType _port_id, 
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Removes the log server port output function.
    */
    NODISCARD static ReturnType UnregisterLogServerInputFunction(
        SizeType _port_id,
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Register the log server port output function, the function will be called when log happens.
    */
    NODISCARD static ReturnType RegisterLogServerOutputFunction(
        SizeType _port_id,
        Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
    ) noexcept;

    /*
        Removes the log server port output function.
    */
    static Void UnregisterLogServerOutputFunction(
        Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
    ) noexcept;

    /*
        Called at the end of the program or when exiting the program.
    */
    static Void FinishFlush(TimeType _max_wait_time_ms) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    static ZLogManager& InstanceP() noexcept;

    static Void LogThread(ZLogManager* _log_manager_ptr) noexcept;

    ZLogManager(const ZLogManager&) = delete;
    ZLogManager(ZLogManager&&) = delete;
    ZLogManager& operator=(const ZLogManager&) = delete;
    ZLogManager& operator=(ZLogManager&&) = delete;

    ZLogManager() noexcept;

    ~ZLogManager() noexcept;

    TFixedQueueSafe<ZErrorLog, kLogQueueSize> error_log_queue_;
    TFixedQueueSafe<ZTraceLog, kLogQueueSize> trace_log_queue_;
    TFixedQueueSafe<ZInfoLog, kLogQueueSize> info_log_queue_;
    TArray<TFixedQueueSafe<ZLog, kLogQueueSize>, kLogMaxPortNum> log_queue_array_;
    ZLogServer log_server_;
    TAtom<Bool> log_thread_finished_;
    ZThread log_thread_;
};

}//log
}//zengine