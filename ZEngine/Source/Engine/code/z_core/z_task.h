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
#include "t_tuple.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZTaskErrorCode : ReturnType {
    kZTaskErrorCode_LinkError = kErrorCodeBase_ZTask,
    kZTaskErrorCode_SystemError,
    kZTaskErrorCode_NullptrParam,
    kZTaskErrorCode_ParamOutOfRange,
    kZTaskErrorCode_TaskStateError,
    kZTaskErrorCode_ReturnValAlreadyGet,
    kZTaskErrorCode_TaskAlreadyExist,
    kZTaskErrorCode_CanNotBindVoidReturn
};
}//error_code
}//zengine

namespace zengine {

enum ZTaskState : IndexType {
    kZTaskState_NoTask =         0x0,
    kZTaskState_TaskSet =        0x1,
    kZTaskState_TaskRunning =    0x2, 
    kZTaskState_Finished =       0x4
};

/*
    Task class, package a function and it's params, the task can only execute one time, thread safe.
    Use ZTask instead for thread pool tasks.
*/
class CORE_DLLAPI ZTaskSafe : public ZObject {
public:
    ZTaskSafe() noexcept;
    ZTaskSafe(ZTaskSafe&& _task) noexcept;
    template<typename _TaskFunction, typename... _ArgsType>
    ZTaskSafe(_TaskFunction&& _func, _ArgsType&&... _args) noexcept : SuperType_() , mutex_() {
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }
    ~ZTaskSafe() noexcept;

    ZTaskSafe& operator=(ZTaskSafe&& _task) noexcept;

    NODISCARD ReturnType operator()() noexcept;

    /*
        Binds the return value to the given pointer, can not bind return when the task is running.
        WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
        task is running.
    */
    NODISCARD ReturnType BindReturn(Void* _ret_val_ptr) noexcept;

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        mutex_.Lock();
        if (state_ == kZTaskState_TaskSet) {
            state_ = kZTaskState_Finished;
        }
        if (state_ == kZTaskState_Finished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
        mutex_.Unlock();
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == kZTaskState_Finished; }
    FORCEINLINE NODISCARD ZTaskState State() const noexcept { return state_; }

protected:
    using SuperType_ = ZObject;

private:
    ZTaskSafe(const ZTaskSafe&) = delete;
    ZTaskSafe& operator=(const ZTaskSafe&) = delete;

    Void MoveP(ZTaskSafe&& _task) noexcept;

#pragma warning(push)
#pragma warning(disable: 6031)
#pragma warning(disable: 4834)

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTaskP(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction, _ArgsType...>::type;
        using TaskParamsTuple = TTuple<_ArgsType...>;

        operate_func_ptr_ = [](ZTaskSafe* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                _task_ptr->state_ = kZTaskState_TaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                        *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_) =
                            (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                                *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                    }
                    else {
                        (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                            *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                    }
                }
                _task_ptr->state_ = kZTaskState_Finished;
                break;
            case kZTaskState_Finished:
                delete reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskState_TaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(_TaskFunction&& _func) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction>::type;

        operate_func_ptr_ = [](ZTaskSafe* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                _task_ptr->state_ = kZTaskState_TaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_) =
                            (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                    }
                    else {
                        (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                    }
                }
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskState_TaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZTaskSafe*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskState state_;
    ZMutex mutex_;
};

/*
    Task class, package a function and it's params, the task can only execute one time, not thread safe, 
    Used if for thread pool tasks.
*/
class CORE_DLLAPI ZTask : public ZObject {
public:
    ZTask() noexcept;
    ZTask(ZTask&& _task) noexcept;
    template<typename _TaskFunction, typename... _ArgsType>
    ZTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept : SuperType_() {
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }
    ~ZTask() noexcept;

    ZTask& operator=(ZTask&& _task) noexcept;

    NODISCARD ReturnType operator()() noexcept;

    /*
        Binds the return value to the given pointer, can not bind return when the task is running.
        WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
        task is running.
    */
    NODISCARD ReturnType BindReturn(Void* _ret_val_ptr) noexcept;

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        if (state_ == kZTaskState_TaskSet) {
            state_ = kZTaskState_Finished;
        }
        if (state_ == kZTaskState_Finished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == kZTaskState_Finished; }
    FORCEINLINE NODISCARD ZTaskState State() const noexcept { return state_; }

protected:
    using SuperType_ = ZObject;

private:
    ZTask(const ZTask&) = delete;
    ZTask& operator=(const ZTask&) = delete;

    Void MoveP(ZTask&& _task) noexcept;

#pragma warning(push)
#pragma warning(disable: 6031)
#pragma warning(disable: 4834)

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTaskP(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction, _ArgsType...>::type;
        using TaskParamsTuple = TTuple<_ArgsType...>;

        operate_func_ptr_ = [](ZTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                _task_ptr->state_ = kZTaskState_TaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                        *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_) =
                            (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                                *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                    }
                    else {
                        (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                            *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                    }
                }
                _task_ptr->state_ = kZTaskState_Finished;
                break;
            case kZTaskState_Finished:
                delete reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskState_TaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(_TaskFunction&& _func) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction>::type;

        operate_func_ptr_ = [](ZTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                _task_ptr->state_ = kZTaskState_TaskRunning;
                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        *reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_) =
                            (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                    }
                    else {
                        (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                    }
                }
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_);
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        ret_val_ptr_ = nullptr;
        state_ = kZTaskState_TaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZTask*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskState state_;
};

/*
    Task class, package a function and it's params, the task can execute multiple times.
*/
class CORE_DLLAPI ZRepeatTask : public ZObject {
public:
    ZRepeatTask() noexcept;
    ZRepeatTask(ZRepeatTask&& _task) noexcept;
    template<typename _TaskFunction, typename... _ArgsType>
    ZRepeatTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept : SuperType_() {
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }
    ~ZRepeatTask() noexcept;

    ZRepeatTask& operator=(ZRepeatTask&& _task) noexcept;

    NODISCARD ReturnType operator()() noexcept;

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        if (state_ == kZTaskState_TaskSet) {
            state_ = kZTaskState_Finished;
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool TaskSet() const noexcept { return state_ == kZTaskState_TaskSet;}
    FORCEINLINE NODISCARD ZTaskState State() const noexcept { return state_; }

protected:
    using SuperType_ = ZObject;

private:
    ZRepeatTask(const ZRepeatTask&) = delete;
    ZRepeatTask& operator=(const ZRepeatTask&) = delete;

    Void MoveP(ZRepeatTask&& _task) noexcept;

#pragma warning(push)
#pragma warning(disable: 6031)
#pragma warning(disable: 4834)

    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTaskP(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        using TaskParamsTuple = TTuple<_ArgsType...>;

        operate_func_ptr_ = [](ZRepeatTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                (reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_))->Apply(
                    *reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_));
                break;
            case kZTaskState_Finished:
                delete reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        state_ = kZTaskState_TaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(_TaskFunction&& _func) noexcept {
        operate_func_ptr_ = [](ZRepeatTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case kZTaskState_TaskSet:
                (*reinterpret_cast<std::remove_reference<_TaskFunction>::type*>(_task_ptr->task_func_ptr_))();
                break;
            case kZTaskState_Finished:
                _task_ptr->state_ = kZTaskState_NoTask;
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_TaskStateError, 0, 
                    L"Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };
        task_func_ptr_ = reinterpret_cast<Void*>(&_func);
        state_ = kZTaskState_TaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZRepeatTask*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    ZTaskState state_;
};

}//zengine