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
#ifndef Z_CORE_Z_THREAD_POOL_H_
#define Z_CORE_Z_THREAD_POOL_H_

#pragma warning(disable : 26439)

#include "internal/z_drive.h"

#include <functional>
#include <future>

#include "t_list.h"
#include "t_queue.h"
#include "t_unique_lock.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_sem_mutex.h"
#include "z_thread.h"

namespace zengine {

namespace error_code {

enum ZThreadPoolErrorCode : ReturnType {
    kZThreadPoolErrorCodeLinkError = kErrorCodeBaseZThreadPool,
    kZThreadPoolErrorCodeAddNegitiveNumThread,
    kZThreadPoolErrorCodePoolFinished
};

}//error_code

/*
    Thread pool class.
*/
class ZThreadPool : public ZObject {
public:
    using ThreadIDType = UInt32;

    CORE_DLLAPI ZThreadPool() noexcept;
    CORE_DLLAPI ZThreadPool(Int32 thread_num_) noexcept;

    CORE_DLLAPI ~ZThreadPool() noexcept;

    NODISCARD FORCEINLINE Int32 MaxThreadNum() const noexcept { return max_thread_num_; }
    NODISCARD FORCEINLINE Int32 FreeThreadNum() const noexcept { return free_thread_num_; }

    /*
        Adds the working thread num(>0).
    */
    CORE_DLLAPI NODISCARD ReturnType AddThreadNum(Int32 thread_num_) noexcept;

    /*
        Suspend until all the tasks are done.
    */
    CORE_DLLAPI NODISCARD Void LockUntilTaskDone() noexcept;

    template<typename TaskFunction, typename... ArgsType>
    NODISCARD ReturnType AddTask(TaskFunction&& func, ArgsType&&... args) noexcept {
        ReturnType ret_val = kOK;
        TUniqueLock<ZMutex> lock(pool_mutex_);
        if (finished_) {
            ret_val = error_code::kZThreadPoolErrorCodePoolFinished;
            Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
            return ret_val;
        }
        //when idle.
        if (max_thread_num_ == free_thread_num_) {
            pool_idle_mutex_.TryLock();
        }
        task_queue_.Push([func = std::forward<TaskFunction>(func), ...args = std::forward<ArgsType>(args)]() mutable {
            func(std::forward<ArgsType>(args)...);
        });
        cv_.NotifyOne();
        return ret_val;
    }

    template<typename TaskType>
    NODISCARD ReturnType AddTask(TaskType& task) noexcept {
        ReturnType ret_val = kOK;
        TUniqueLock<ZMutex> lock(pool_mutex_);
        if (finished_) {
            ret_val = error_code::kZThreadPoolErrorCodePoolFinished;
            Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
            return ret_val;
        }
        //when idle.
        if (max_thread_num_ == free_thread_num_) {
            pool_idle_mutex_.TryLock();
        }
        task_queue_.Push([&]() mutable { task.Run(); });
        cv_.NotifyOne();
        return ret_val;
    }

    CORE_DLLAPI Void ClearTask() noexcept;

protected:
    using SuperType = ZObject;

private:
    static Void ThreadFunc(ZThreadPool& thread_pool) noexcept;

    ZThreadPool(const ZThreadPool&) = delete;
    ZThreadPool(ZThreadPool&&) = delete;

    ZThreadPool& operator=(const ZThreadPool&) = delete;
    ZThreadPool& operator=(ZThreadPool&&) = delete;

    TList<ZThread> thread_list_;
    TQueue<std::function<Void()>> task_queue_;
    ZMutex pool_mutex_;
    ZSemMutex pool_idle_mutex_;
    ZConditionVariable cv_;
    Int32 max_thread_num_;
    Int32 free_thread_num_;
    Bool finished_;
};

}//zengine

#endif // !Z_CORE_Z_THREAD_POOL_H_