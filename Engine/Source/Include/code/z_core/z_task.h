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
#ifndef Z_CORE_Z_TASK_H_
#define Z_CORE_Z_TASK_H_

#include "internal/z_drive.h"

#include <type_traits>

#include "m_log.h"
#include "t_lock_guard.h"
#include "t_tuple.h"
#include "z_object.h"
#include "z_mutex.h"

namespace zengine {

namespace error_code {

enum ZTaskErrorCode : ReturnType {
    kZTaskErrorCodeLinkError = kErrorCodeBasePCore,
    kZTaskErrorCodeTaskStateError,
    kZTaskErrorCodeReturnValAlreadyGet,
    kZTaskErrorCodeTaskAlreadyExist,
    kZTaskErrorCodeCanNotBindVoidReturn
};

}//error_code

enum ZTaskState : IndexType {
    kZTaskStateNoTask =         0x0,
    kZTaskStateTaskSet =        0x1,
    kZTaskStateTaskRunning =    0x2, 
    kZTaskStateFinished =       0x4
};

/*
    Task class, package a function and it's params, the task can only run one time, thread safe.
    Use ZTaskFast instead for thread pool tasks.
*/
class CORE_DLLAPI ZTask : public ZObject {
public:
    ZTask() noexcept;
    ZTask(ZTask&& task) noexcept;
    template<typename TaskFunction, typename... ArgsType>
    ZTask(TaskFunction&& func, ArgsType&&... args) noexcept 
            : SuperType()
            , mutex_() {
        SetTaskP(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
    }
    ~ZTask() noexcept;

    ZTask& operator=(ZTask&& task) noexcept;

    NODISCARD ReturnType operator()() noexcept;

    /*
        Binds the return value to the given pointer, can not bind return when the task is running.
        WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
        task is running.
    */
    NODISCARD ReturnType BindReturn(Void* ret_val_ptr) noexcept;

    template<typename TaskFunction, typename... ArgsType>
    Void SetTask(TaskFunction&& func, ArgsType&&... args) noexcept {
        mutex_.Lock();
        if (state_ == kZTaskStateTaskSet) {
            state_ = kZTaskStateFinished;
        }
        if (state_ == kZTaskStateFinished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
        mutex_.Unlock();
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == kZTaskStateFinished; }
    FORCEINLINE NODISCARD ZTaskState State() const noexcept { return state_; }

protected:
    using SuperType = ZObject;

private:
    ZTask(const ZTask&) = delete;

    ZTask& operator=(const ZTask&) = delete;

#pragma warning(push)
#pragma warning(disable: 6031)
#pragma warning(disable: 4834)

    template<typename TaskFunction, typename... ArgsType>
    Void SetTaskP(TaskFunction&& func, ArgsType&&... args) noexcept {
        using TaskReturnType = typename std::invoke_result<TaskFunction, ArgsType...>::type;
        using TaskParamsTuple = TTuple<ArgsType...>;

        operate_func_ptr_ = [](ZTask* task_ptr) {
            switch (task_ptr->state_) {
            case kZTaskStateTaskSet:
                task_ptr->state_ = kZTaskStateTaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                        *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                }
                else {
                    if (task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(task_ptr->ret_val_ptr_) =
                            (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                                *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                    }
                    else {
                        (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                            *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                    }
                }
                task_ptr->state_ = kZTaskStateFinished;
                break;
            case kZTaskStateFinished:
                delete reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_);
                task_ptr->params_ptr_ = nullptr;
                task_ptr->state_ = kZTaskStateNoTask;
                break;
            default:
                Z_LOG_ERROR(error_code::kZTaskErrorCodeTaskStateError, 0, 
                            "Task state not expected! state_: %d", task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&func);
        params_ptr_ = new TaskParamsTuple(std::forward<ArgsType>(args)...);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskStateTaskSet;
    }

    template<typename TaskFunction>
    Void SetTaskP(TaskFunction&& func) noexcept {
        using TaskReturnType = typename std::invoke_result<TaskFunction>::type;

        operate_func_ptr_ = [](ZTask* task_ptr) {
            switch (task_ptr->state_) {
            case kZTaskStateTaskSet:
                task_ptr->state_ = kZTaskStateTaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                }
                else {
                    if (task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(task_ptr->ret_val_ptr_) =
                            (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                    }
                    else {
                        (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                    }
                }
                task_ptr->state_ = kZTaskStateNoTask;
                break;
            default:
                Z_LOG_ERROR(error_code::kZTaskErrorCodeTaskStateError, 0, 
                            "Task state not expected! state_: %d", task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&func);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskStateTaskSet;
    }

#pragma warning(pop)

    Void MoveP(ZTask&& task) noexcept;

    Void(*operate_func_ptr_)(ZTask*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskState state_;
    ZMutex mutex_;
};

/*
    Task class, package a function and it's params, the task can only run one time, not thread safe, 
    Used if for thread pool tasks.
*/
class CORE_DLLAPI ZTaskFast : public ZObject {
public:
    ZTaskFast() noexcept;
    ZTaskFast(ZTaskFast&& task) noexcept;
    ZTaskFast(ZTask&& task) noexcept;
    template<typename TaskFunction, typename... ArgsType>
    ZTaskFast(TaskFunction&& func, ArgsType&&... args) noexcept : SuperType() {
        SetTaskP(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
    }
    ~ZTaskFast() noexcept;

    ZTaskFast& operator=(ZTaskFast&& task) noexcept;
    ZTaskFast& operator=(ZTask&& task) noexcept;

    NODISCARD ReturnType operator()() noexcept;

    /*
        Binds the return value to the given pointer, can not bind return when the task is running.
        WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
        task is running.
    */
    NODISCARD ReturnType BindReturn(Void* ret_val_ptr) noexcept;

    template<typename TaskFunction, typename... ArgsType>
    Void SetTask(TaskFunction&& func, ArgsType&&... args) noexcept {
        if (state_ == kZTaskStateTaskSet) {
            state_ = kZTaskStateFinished;
        }
        if (state_ == kZTaskStateFinished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == kZTaskStateFinished; }
    FORCEINLINE NODISCARD ZTaskState State() const noexcept { return state_; }

protected:
    using SuperType = ZObject;

private:
    ZTaskFast(const ZTaskFast&) = delete;

    ZTaskFast& operator=(const ZTaskFast&) = delete;

#pragma warning(push)
#pragma warning(disable: 6031)
#pragma warning(disable: 4834)

    template<typename TaskFunction, typename... ArgsType>
    Void SetTaskP(TaskFunction&& func, ArgsType&&... args) noexcept {
        using TaskReturnType = typename std::invoke_result<TaskFunction, ArgsType...>::type;
        using TaskParamsTuple = TTuple<ArgsType...>;

        operate_func_ptr_ = [](ZTaskFast* task_ptr) {
            switch (task_ptr->state_) {
            case kZTaskStateTaskSet:
                task_ptr->state_ = kZTaskStateTaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                        *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                }
                else {
                    if (task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(task_ptr->ret_val_ptr_) =
                            (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                                *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                    }
                    else {
                        (reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_))->Apply(
                            *reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_));
                    }
                }
                task_ptr->state_ = kZTaskStateFinished;
                break;
            case kZTaskStateFinished:
                delete reinterpret_cast<TaskParamsTuple*>(task_ptr->params_ptr_);
                task_ptr->params_ptr_ = nullptr;
                task_ptr->state_ = kZTaskStateNoTask;
                break;
            default:
                Z_LOG_ERROR(error_code::kZTaskErrorCodeTaskStateError, 0, 
                            "Task state not expected! state_: %d", task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&func);
        params_ptr_ = new TaskParamsTuple(std::forward<ArgsType>(args)...);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskStateTaskSet;
    }

    template<typename TaskFunction>
    Void SetTaskP(TaskFunction&& func) noexcept {
        using TaskReturnType = typename std::invoke_result<TaskFunction>::type;

        operate_func_ptr_ = [](ZTaskFast* task_ptr) {
            switch (task_ptr->state_) {
            case kZTaskStateTaskSet:
                task_ptr->state_ = kZTaskStateTaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                }
                else {
                    if (task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(task_ptr->ret_val_ptr_) =
                            (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                    }
                    else {
                        (*reinterpret_cast<std::remove_reference<TaskFunction>::type*>(task_ptr->task_func_ptr_))();
                    }
                }
                task_ptr->state_ = kZTaskStateNoTask;
                break;
            default:
                Z_LOG_ERROR(error_code::kZTaskErrorCodeTaskStateError, 0, 
                            "Task state not expected! state_: %d", task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&func);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskStateTaskSet;
    }

#pragma warning(pop)

    Void MoveP(ZTaskFast&& task) noexcept;

    Void(*operate_func_ptr_)(ZTaskFast*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskState state_;
};

}//zengine

#endif //!Z_CORE_Z_TASK_H_