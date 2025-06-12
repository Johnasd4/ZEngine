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

#include "internal/z_drive.h"

#include "m_log.h"
#include "t_list.h"
#include "t_queue.h"
#include "t_unique_lock.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_sem_mutex.h"
#include "z_task.h"
#include "z_thread.h"

namespace zengine {

namespace error_code {

enum ZThreadPoolErrorCode : ReturnType {
    kZThreadPoolErrorCode_LinkError = kErrorCodeBase_ZThreadPool,
    kZThreadPoolErrorCode_AddNegitiveNumThread,
    kZThreadPoolErrorCode_PoolFinished
};

}//error_code

/*
    Thread pool class.
*/
class CORE_DLLAPI ZThreadPool : public ZObject {
public:
    using ThreadIDType_ = UInt32;

    ZThreadPool() noexcept;
    ZThreadPool(Int32 _thread_num) noexcept;

    ~ZThreadPool() noexcept;

    NODISCARD FORCEINLINE Int32 MaxThreadNum() const noexcept { return max_thread_num_; }
    NODISCARD FORCEINLINE Int32 FreeThreadNum() const noexcept { return free_thread_num_; }

    /*
        Adds the working thread num(>0).
    */
    NODISCARD ReturnType AddThreadNum(Int32 _thread_num) noexcept;

    /*
        Suspend until all the tasks are done.
    */
    NODISCARD Void LockUntilTaskDone() noexcept;

    template<typename _TaskFunction, typename... _ArgsType>
    NODISCARD ReturnType AddTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        ReturnType ret_val = kOK;
        TUniqueLock<ZMutex> lock(pool_mutex_);
        if (finished_) {
            ret_val = error_code::kZThreadPoolErrorCode_PoolFinished;
            Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
            return ret_val;
        }
        //when idle.
        if (max_thread_num_ == free_thread_num_) {
            pool_idle_mutex_.TryLock();
        }
        task_queue_.Push(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
        cv_.NotifyOne();
        return ret_val;
    }

    NODISCARD ReturnType AddTask(ZTask&& _task) noexcept;
    NODISCARD ReturnType AddTask(ZTaskSafe&& _task) noexcept;

    Void ClearTask() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    static Void SubThread(ZThreadPool& _thread_pool) noexcept;

    ZThreadPool(const ZThreadPool&) = delete;
    ZThreadPool(ZThreadPool&&) = delete;
    ZThreadPool& operator=(const ZThreadPool&) = delete;
    ZThreadPool& operator=(ZThreadPool&&) = delete;

    ReturnType AddTask(const ZTask&) = delete;

    TList<ZThread> thread_list_;
    TQueue<ZTask> task_queue_;
    ZMutex pool_mutex_;
    ZSemMutex pool_idle_mutex_;
    ZConditionVariable cv_;
    Int32 max_thread_num_;
    Int32 free_thread_num_;
    Bool finished_;
};

}//zengine