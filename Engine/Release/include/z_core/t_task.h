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
    kTTaskErrorCodeReturnValAlreadyGet,
    kTTaskErrorCodeTaskNotExist,
    kTTaskErrorCodeTaskAlreadyExist,
    kTTaskErrorCodeCanNotBindVoidReturn,
};

}//error_code

//TODO
//template<typename TaskFunction>
//class TTask : public ZObject {};

/*
    Task class, package a function and it's params, the task can only run one time.
*/
template<typename TaskFunction, typename... ArgsType>
class TTask : public ZObject {
public:
    using TaskReturnType = typename std::invoke_result<TaskFunction, ArgsType...>::type;
    using TaskParamsTuple = TTuple<ArgsType...>;

    enum TaskState : IndexType {
        kTaskStateNoTask,
        kTaskStateWaiting,
        kTaskStateOperating,
        kTaskStateFinished,

    };

    TTask() noexcept 
            : SuperType()
            , func_(nullptr)
            , params_ptr_(nullptr)
            , ret_val_ptr_(nullptr)
            , mutex_(nullptr) {}
    TTask(TTask&& task) noexcept
            : SuperType()
            , func_(std::move(task.func_))
            , params_ptr_(task.params_ptr_)
            , mutex_(std::move(task.mutex_)) {
        task.params_ptr_ = nullptr;
        if constexpr (kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        else {
            ret_val_ptr_ = task.ret_val_ptr_;
            task.ret_val_ptr_ = nullptr;
        }
    }
    TTask(TaskFunction func, ArgsType&&... args) noexcept 
            : SuperType()
            , func_(std::forward<TaskFunction>(func))
            , params_ptr_(new TaskParamsTuple(std::forward<ArgsType>(args)...)) 
            , ret_val_ptr_(nullptr)
            , mutex_() {}
    ~TTask() noexcept {
        if constexpr (!kSameType<TaskReturnType, Void>) {
            if (params_ptr_ != nullptr) {
                delete params_ptr_;
            }
        }
    }

    TTask& operator=(TTask&& task) noexcept {
        func_ = task.func_;
        params_ptr_ = task.params_ptr_;
        task.params_ptr_ = nullptr;
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = task.ret_val_ptr_;
            task.ret_val_ptr_ = nullptr;
        }
        mutex_ = std::move(task.mutex_);
        return *this;
    }

    FORCEINLINE Void Swap(TTask& task) noexcept {
        zengine::Swap(this, &task);
    }

    NODISCARD ReturnType BindReturn(TaskReturnType* ret_val_ptr) noexcept {
        ReturnType ret_val = kOK;
        if constexpr (kSameType<TaskReturnType, Void>) {
            ret_val = error_code::kTTaskErrorCodeCanNotBindVoidReturn;
            Z_LOG_ERROR(ret_val, 0, "Can not bind a Void return!");
            return ret_val;
        }
        mutex_.Lock();
        ret_val_ptr_ = ret_val_ptr;
        mutex_.Unlock();
        return ret_val;
    }

    Void SetTask(TaskFunction&& func, ArgsType&&... args) noexcept {
        mutex_.Lock();
        func_ = std::forward<TaskFunction>(func);
        if (params_ptr_ != nullptr) {
            delete params_ptr_;
        }
        params_ptr_ = new TaskParamsTuple(std::forward<ArgsType>(args)...);
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        mutex_.Unlock();
    }

    Void Clear() noexcept {
        mutex_.Lock();
        if (params_ptr_ != nullptr) {
            delete params_ptr_;
            params_ptr_ = nullptr;
        }
        if constexpr (!kSameType<TaskReturnType, Void>) {
            ret_val_ptr_ = nullptr;
        }
        mutex_.Unlock();
    }

#pragma warning(push)
#pragma warning(disable: 6031)

    NODISCARD ReturnType Run() noexcept {
        ReturnType ret_val = kOK;
        TLockGuard<ZMutex> lock_guard(mutex_);
        if (params_ptr_ == nullptr) {
            ret_val = error_code::kTTaskErrorCodeTaskNotExist;
            Z_LOG_ERROR(ret_val, 0, "Task not exist! ");
            return ret_val;
        }
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
        delete params_ptr_;
        params_ptr_ = nullptr;
        return ret_val;
    }

#pragma warning(pop)

    FORCEINLINE Bool Finished() noexcept { return params_ptr_ == nullptr; }

protected:
    using SuperType = ZObject;

private:
    TTask(const TTask&) = delete;

    TTask& operator=(const TTask&) = delete;

    TaskFunction func_;
    TaskParamsTuple* params_ptr_;
    TaskReturnType* ret_val_ptr_;
    ZMutex mutex_;
};

namespace task {

template<typename TaskFunction, typename... ArgsType>
NODISCARD FORCEINLINE TTask<TaskFunction, ArgsType...> MakeTask(TaskFunction&& func, ArgsType&&... args) noexcept {
    return TTask<TaskFunction, ArgsType...>(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
}

}//task
}//zengine

#endif //!Z_CORE_T_Task_H_