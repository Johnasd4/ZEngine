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
#ifndef Z_CORE_T_Task_H_
#define Z_CORE_T_Task_H_

#include "internal/z_drive.h"

#include <type_traits>

#include "m_log.h"
#include "t_lock_guard.h"
#include "t_tuple.h"
#include "z_object.h"
#include "z_mutex.h"

namespace zengine {

namespace error_code {

enum TTaskErrorCode : ReturnType {
    kTTaskErrorCodeLinkError = kErrorCodeBasePCore,
    kTTaskErrorCodeTaskStateError,
    kTTaskErrorCodeReturnValAlreadyGet,
    kTTaskErrorCodeTaskAlreadyExist,
    kTTaskErrorCodeCanNotBindVoidReturn
};

}//error_code

enum TTaskState : IndexType {
    kTTaskStateNoTask =         0x1,
    kTTaskStateTaskSet =        0x2,
    kTTaskStateTaskRunning =    0x4, 
    kTTaskStateReturnReady =    0x8,
};

/*
    Task class, package a function and it's params, the task can only run one time.
*/

class TTask : public ZObject {
public:
    using TaskReturnType = typename std::invoke_result<TaskFunction, ArgsType...>::type;
    using TaskParamsTuple = TTuple<ArgsType...>;

    TTask() noexcept 
            : SuperType()
            , func_(nullptr)
            , params_ptr_(nullptr)
            , ret_val_ptr_(nullptr)
            , mutex_()
            , state_(kTTaskStateNoTask) {}
    TTask(TTask&& task) noexcept {
        task.mutex_.Lock();
        func_ = std::move(task.func_);
        params_ptr_ = task.params_ptr_;
        task.params_ptr_ = nullptr;
        if constexpr (kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        else {
            ret_val_ptr_ = task.ret_val_ptr_;
            task.ret_val_ptr_ = nullptr;
        }
        state_ = task.state_;
        task.state_ = kTTaskStateNoTask;
        task.mutex_.Unlock();
    }
    template<typename TaskFunction, typename... ArgsType>
    TTask(TaskFunction func, ArgsType&&... args) noexcept 
            : SuperType()
            , func_(std::forward<TaskFunction>(func))
            , params_ptr_(new TaskParamsTuple(std::forward<ArgsType>(args)...)) 
            , ret_val_ptr_(nullptr)
            , mutex_() 
            , state_(kTTaskStateTaskSet) {}
    ~TTask() noexcept {
        if (params_ptr_ != nullptr) {
            delete params_ptr_;
        }
    }

    TTask& operator=(TTask&& task) noexcept {
        task.mutex_.Lock();
        mutex_.Lock();
        func_ = std::move(task.func_);
        params_ptr_ = task.params_ptr_;
        task.params_ptr_ = nullptr;
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = task.ret_val_ptr_;
            task.ret_val_ptr_ = nullptr;
        }
        state_ = task.state_;
        task.state_ = kTTaskStateNoTask;
        mutex_.Unlock();
        task.mutex_.Unlock();
        return *this;
    }

    FORCEINLINE Void Swap(TTask& task) noexcept {
        zengine::Swap(this, &task);
    }

    /*
        Binds the return value to the given pointer, can not bind return when the task is running.
        WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
        task is running.
    */
    NODISCARD ReturnType BindReturn(TaskReturnType* ret_val_ptr) noexcept {
        ReturnType ret_val = kOK;
        if (IN_STATE(state_, kTTaskStateTaskRunning)) {
            ret_val = error_code::kTTaskErrorCodeTaskStateError;
            Z_LOG_ERROR(ret_val, 0, "Task Running, can not bind return! state_: %d", state_);
            return ret_val;
        }
        TLockGuard<ZMutex> lock_guard(mutex_);
        if constexpr (kSameType<TaskReturnType, Void>) {
            ret_val = error_code::kTTaskErrorCodeCanNotBindVoidReturn;
            Z_LOG_ERROR(ret_val, 0, "Can not bind a Void return!");
            return ret_val;
        }
        if (IN_STATE(state_, kTTaskStateReturnReady)) {
            state_ = kTTaskStateNoTask;
        }
        ret_val_ptr_ = ret_val_ptr;
        return ret_val;
    }

    Void SetTask(TaskFunction&& func, ArgsType&&... args) noexcept {
        mutex_.Lock();    
        func_ = std::forward<TaskFunction>(func);
        if (params_ptr_ != nullptr) {
            params_ptr_->~TTuple();
        }
        new((Void*)params_ptr_) TaskParamsTuple(std::forward<ArgsType>(args)...);
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        state_ = kTTaskStateTaskSet;
        mutex_.Unlock();
    }

    NODISCARD Bool Clear() noexcept {
        mutex_.Lock();
        if (params_ptr_ != nullptr) {
            delete params_ptr_;
            params_ptr_ = nullptr;
        }
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        state_ = kTTaskStateNoTask;
        mutex_.Unlock();
    }

#pragma warning(push)
#pragma warning(disable: 6031)

    NODISCARD ReturnType Run() noexcept {
        ReturnType ret_val = kOK;
        TLockGuard<ZMutex> lock_guard(mutex_);
        if (!IN_STATE(state_, kTTaskStateTaskSet)) {
            ret_val = error_code::kTTaskErrorCodeTaskStateError;
            Z_LOG_ERROR(ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d", 
                        state_, kTTaskStateTaskSet);
            return ret_val;
        }
        state_ = kTTaskStateTaskRunning;
        if constexpr (kSameType<TaskReturnType, Void>) {
            params_ptr_->Apply(func_);
        }
        else {
            if (ret_val_ptr_ != nullptr) {
                *ret_val_ptr_ = params_ptr_->Apply(func_);
            }
            else {
                params_ptr_->Apply(func_);
            }
        }
        state_ = kTTaskStateReturnReady;
        delete params_ptr_;
        params_ptr_ = nullptr;
        return ret_val;
    }

#pragma warning(pop)

    FORCEINLINE Bool ReturnReady() const noexcept { return state_ == kTTaskStateReturnReady; }
    FORCEINLINE TTaskState State() const noexcept { return state_; }

protected:
    using SuperType = ZObject;

private:
    TTask(const TTask&) = delete;

    TTask& operator=(const TTask&) = delete;

    Void(*run_func_)(TTask*);
    Void(*clear_func_)(TTask*);
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZMutex mutex_;
    TTaskState state_;
};

namespace task {

template<typename TaskFunction, typename... ArgsType>
NODISCARD FORCEINLINE TTask<TaskFunction, ArgsType...> MakeTask(TaskFunction&& func, ArgsType&&... args) noexcept {
    return TTask<TaskFunction, ArgsType...>(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
}

//template<typename TaskFunction>
//NODISCARD FORCEINLINE TTask<TaskFunction> MakeTask(TaskFunction&& func) noexcept {
//    return TTask<TaskFunction>(std::forward<TaskFunction>(func));
//}
//
//template<typename TaskFunction, typename... ArgsType>
//NODISCARD FORCEINLINE TTask<TaskFunction, ArgsType...> MakeTaskFast(TaskFunction&& func, ArgsType&&... args) noexcept {
//    return TTaskFast<TaskFunction, ArgsType...>(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
//}
//
//template<typename TaskFunction>
//NODISCARD FORCEINLINE TTask<TaskFunction> MakeTaskFast(TaskFunction&& func) noexcept {
//    return TTaskFast<TaskFunction>(std::forward<TaskFunction>(func));
//}

}//task
}//zengine

#endif //!Z_CORE_T_Task_H_