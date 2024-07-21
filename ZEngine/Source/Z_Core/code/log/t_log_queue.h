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
#ifndef Z_CORE_LOG_T_LOG_QUEUE_H_
#define Z_CORE_LOG_T_LOG_QUEUE_H_

#include "internal/z_drive.h"

#include "m_log.h"
#include "t_fixed_queue.h"
#include "z_cs_mutex.h"

namespace zengine {
namespace log {

/*
    Log queue.
*/
template<typename LogType, IndexType kCapacity>
class TLogQueue : public ZObject {
public:
    TLogQueue() noexcept : log_queue_(), log_mutex_() {}

    NODISCARD FORCEINLINE LogType& Front() noexcept { return log_queue_.Front(); }
    NODISCARD FORCEINLINE const LogType& Front() const noexcept { return log_queue_.Front(); }
    NODISCARD FORCEINLINE LogType& Back() noexcept { return log_queue_.Back(); }
    NODISCARD FORCEINLINE const LogType& Back() const noexcept { return log_queue_.Back(); }
    NODISCARD FORCEINLINE Bool Empty() noexcept { return log_queue_.Empty(); }

    Void Pop() noexcept {
        log_mutex_.Lock();
        log_queue_.PopFront();
        log_mutex_.Unlock();
    }

    Void Push(const LogType& _log) noexcept {
        log_mutex_.Lock();
        log_queue_.Push(_log);
        if (log_queue_.Size() > log_queue_.Capacity()) {
            log_queue_.Clear();
            Z_LOG_ERROR(error_code::kMLogErrorCodeLogQueueOverflow, 0, L"Log queue overflow! Clear all logs!");
        }
        log_mutex_.Unlock();
    }
    template<typename... ArgsType>
    Void Push(ArgsType&&... _args) noexcept {
        log_mutex_.Lock();
        log_queue_.EmplaceBack(std::forward<ArgsType>(_args)...);
        if (log_queue_.Size() > log_queue_.Capacity()) {
            log_queue_.Clear();
            Z_LOG_ERROR(error_code::kMLogErrorCodeLogQueueOverflow, 0, L"Log queue overflow! Clear all logs!");
        }
        log_mutex_.Unlock();
    }

protected:
    using SuperType_ = ZObject;

private:
    TLogQueue(const TLogQueue&) = delete;
    TLogQueue(TLogQueue&&) = delete;
    TLogQueue& operator=(const TLogQueue&) = delete;
    TLogQueue& operator=(TLogQueue&&) = delete;

    TFixedQueue<LogType, kCapacity> log_queue_;
    ZCSMutex log_mutex_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_T_LOG_QUEUE_H_
