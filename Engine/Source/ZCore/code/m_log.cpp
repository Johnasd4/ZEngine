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

#include "m_log.h"

#include "f_console.h"
#include "t_fixed_queue.h"
#include "t_fixed_string.h"
#include "z_mutex.h"
#include "z_system_time.h"
#include "z_thread.h"

namespace zengine {
namespace internal {

/*
    The log manager, log's the error and info to the console, file and any place that needs to log.
*/
class ZLogManager : public ZObject {
public:
    static constexpr Int32 klogLen = 4096;

    static constexpr Int32 kErrMsgMaxLen = 512;
    static constexpr Int32 kErrMsgMaxNum = 128;

    struct ErrorInfo {
    public:
        TimeType raw_time;
        const CChar* err_file;
        const CChar* err_func;
        Int32 err_line;
        ReturnType err_code;
        ReturnType link_code;
        TFixedString<CChar, kErrMsgMaxLen> err_msg;

        ErrorInfo& operator=(ErrorInfo&& info) {
            raw_time = info.raw_time;
        }
        ErrorInfo() noexcept : raw_time(), err_file(), err_func(), err_line(), err_code(), link_code(), err_msg() {}
        ErrorInfo(TimeType raw_time,
                  const CChar* err_file,
                  const CChar* err_func,
                  Int32 err_line,
                  ReturnType err_code,
                  ReturnType link_code,
                  const CChar* format,
                  ArgListType args) noexcept :
            raw_time(raw_time),
            err_file(err_file),
            err_func(err_func),
            err_line(err_line),
            err_code(err_code),
            link_code(link_code)
        {
            err_msg.SetString(format, args);
        }
    };

    static Void LogError(TimeType raw_time,
                         const CChar* err_file, 
                         const CChar* err_func,
                         Int32 err_line, 
                         ReturnType err_code,
                         ReturnType link_code,
                         const CChar* format,
                         ArgListType args) noexcept {
        static ZLogManager& log_manager = ZLogManager::InstanceP();
        log_manager.err_log_mutex_.Lock();
        log_manager.err_info_queue_.Push(raw_time, err_file, err_func, err_line, err_code, link_code, format, args);
        log_manager.err_log_mutex_.Unlock();
    }


protected:
    using SuperType = ZObject;

private:
    static ZLogManager& InstanceP() noexcept {
        static ZLogManager log_manager;
        return log_manager;
    }

    static Void LogThread() noexcept {
        static ZLogManager& log_manager = ZLogManager::InstanceP();
        static ZSystemTime system_time;
        TFixedString<CChar, klogLen> log_str;
        Bool if_str_logged = true;
        while (log_manager.log_thread_finished_ == false || !log_manager.err_info_queue_.Empty()) {
            //genrate err log str
            if (log_manager.err_info_queue_.Size() != 0) {
                ErrorInfo& err_info = log_manager.err_info_queue_.Front();
                system_time.UpdateTimeFast(err_info.raw_time);
                log_str.SetString(
                    "\nTime: %04d/%02d/%02d-%02d:%02d:%02d\nFile: %s\nFunction: %s\nLine: %d\nError Code: 0x%x\nLink Code: 0x%x\nMessage: %s\n",
                    system_time.Year(), system_time.Month(), system_time.Day(), 
                    system_time.Hour(), system_time.Min(), system_time.Sec(),
                    err_info.err_file, err_info.err_func, err_info.err_line, 
                    err_info.err_code,err_info.link_code, err_info.err_msg.DataPtr());
                log_manager.err_log_mutex_.Lock();
                log_manager.err_info_queue_.Pop();
                log_manager.err_log_mutex_.Unlock();
                if_str_logged = false;
            }
 
            //log str
            if (if_str_logged == false) {
#if USE_CONSOLE_LOG
                console::PrintError(log_str.DataPtr());
#endif
                if_str_logged = true;
            }
            else {
                Sleep(1);
            }
        }
    }

    ZLogManager() noexcept : SuperType(), 
        err_info_queue_(), log_thread_finished_(false), err_log_mutex_(), msg_log_mutex_(),
        log_thread_(&ZLogManager::LogThread) {}

    ~ZLogManager() noexcept {
        log_thread_finished_ = true;
        if (log_thread_.Joinable()) {
            log_thread_.Join();
        }
    }

    TFixedQueue<ErrorInfo, kErrMsgMaxNum> err_info_queue_;
    Bool log_thread_finished_;
    ZMutex err_log_mutex_;
    ZMutex msg_log_mutex_;
    ZThread log_thread_;
};

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogError(TimeType raw_time,
                                 const CChar* err_file, 
                                 const CChar* err_func,
                                 Int32 err_line, 
                                 ReturnType err_code,
                                 ReturnType link_code,
                                 const CChar* format,
                                 ...) noexcept {
    ArgListType args;
    va_start(args, format);
    ZLogManager::LogError(raw_time, err_file, err_func, err_line, err_code, link_code,  format, args);
    va_end(args);
}

}//internal
}//zengine