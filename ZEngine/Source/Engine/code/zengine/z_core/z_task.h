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

#include "m_log.h"
#include "t_function.h"
#include "t_tuple.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZTaskErrorCodeEnum : ReturnType {
    kZTaskErrorCode_LinkError = kErrorCodeBase_ZTask,
    kZTaskErrorCode_SystemError,
    kZTaskErrorCode_NullptrParam,
    kZTaskErrorCode_ParamOutOfRange,
    kZTaskErrorCode_StateError,
    kZTaskErrorCode_ReturnValAlreadyGet,
    kZTaskErrorCode_TaskAlreadyExist,
    kZTaskErrorCode_CanNotBindVoidReturn
};
}//error_code
}//zengine

namespace zengine {

enum class ZTaskStateEnum : Int32 {
    kNoTask,
    kTaskSet,
    kTaskRunning, 
    kFinished
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
        if (state_ == ZTaskStateEnum::kTaskSet) {
            state_ = ZTaskStateEnum::kFinished;
        }
        if (state_ == ZTaskStateEnum::kFinished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
        mutex_.Unlock();
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == ZTaskStateEnum::kFinished; }
    FORCEINLINE NODISCARD ZTaskStateEnum State() const noexcept { return state_; }

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
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<TaskReturnType(_ArgsType...)>(_func); }) {
                return TSimpleFunction<TaskReturnType(_ArgsType...)>{};
            }
            else {
                return TFunction<TaskReturnType(_ArgsType...)>{};
            }
        }());

        operate_func_ptr_ = [](ZTaskSafe* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                TaskParamsTuple* puple_ptr;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                }
                else {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                }

                if constexpr (kSameType<TaskReturnType, Void>) {
                    puple_ptr->Apply(*func_ptr);
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        TaskReturnType* ret_val_ptr = reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_);
                        *ret_val_ptr = puple_ptr->Apply(*func_ptr);
                    }
                    else {
                        puple_ptr->Apply(*func_ptr);
                    }
                }
                _task_ptr->state_ = ZTaskStateEnum::kFinished;
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //release params
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                    puple_ptr->~TaskParamsTuple();
                }
                else {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                    delete puple_ptr;
                }

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };
        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
            new(&params_ptr_) TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        else {
            params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        ret_val_ptr_ = nullptr;
        state_ = ZTaskStateEnum::kTaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(_TaskFunction&& _func) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction>::type;
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<TaskReturnType()>(_func); }) {
                return TSimpleFunction<TaskReturnType()>{};
            }
            else {
                return TFunction<TaskReturnType()>{};
            }
        }());

        operate_func_ptr_ = [](ZTaskSafe* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);

                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*func_ptr)();
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        TaskReturnType* ret_val_ptr = reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_);
                        *ret_val_ptr = (*func_ptr)();
                    }
                    else {
                        (*func_ptr)();
                    }
                }
                _task_ptr->state_ = ZTaskStateEnum::kFinished;
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };

        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        ret_val_ptr_ = nullptr;
        state_ = ZTaskStateEnum::kTaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZTaskSafe*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskStateEnum state_;
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
        if (state_ == ZTaskStateEnum::kTaskSet) {
            state_ = ZTaskStateEnum::kFinished;
        }
        if (state_ == ZTaskStateEnum::kFinished) {
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool Finished() const noexcept { return state_ == ZTaskStateEnum::kFinished; }
    FORCEINLINE NODISCARD ZTaskStateEnum State() const noexcept { return state_; }

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
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<TaskReturnType(_ArgsType...)>(_func); }) {
                return TSimpleFunction<TaskReturnType(_ArgsType...)>{};
            }
            else {
                return TFunction<TaskReturnType(_ArgsType...)>{};
            }
        }());

        operate_func_ptr_ = [](ZTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                TaskParamsTuple* puple_ptr;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                }
                else {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                }

                if constexpr (kSameType<TaskReturnType, Void>) {
                    puple_ptr->Apply(*func_ptr);
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        TaskReturnType* ret_val_ptr = reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_);
                        *ret_val_ptr = puple_ptr->Apply(*func_ptr);
                    }
                    else {
                        puple_ptr->Apply(*func_ptr);
                    }
                }
                _task_ptr->state_ = ZTaskStateEnum::kFinished;
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //release params
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                    puple_ptr->~TaskParamsTuple();
                }
                else {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                    delete puple_ptr;
                }

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };
        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
            new(&params_ptr_) TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        else {
            params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        ret_val_ptr_ = nullptr;
        state_ = ZTaskStateEnum::kTaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(_TaskFunction&& _func) noexcept {
        using TaskReturnType = typename std::invoke_result<_TaskFunction>::type;
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<TaskReturnType()>(_func); }) {
                return TSimpleFunction<TaskReturnType()>{};
            }
            else {
                return TFunction<TaskReturnType()>{};
            }
        }());

        operate_func_ptr_ = [](ZTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);

                if constexpr (kSameType<TaskReturnType, Void>) {
                    (*func_ptr)();
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        TaskReturnType* ret_val_ptr = reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_);
                        *ret_val_ptr = (*func_ptr)();
                    }
                    else {
                        (*func_ptr)();
                    }
                }
                _task_ptr->state_ = ZTaskStateEnum::kFinished;
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };

        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        ret_val_ptr_ = nullptr;
        state_ = ZTaskStateEnum::kTaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZTask*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    Void* ret_val_ptr_;
    ZTaskStateEnum state_;
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
        if (state_ == ZTaskStateEnum::kTaskSet) {
            state_ = ZTaskStateEnum::kFinished;
            operate_func_ptr_(this);
        }
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    Void Clear() noexcept;
    
    NODISCARD ReturnType Run() noexcept;

    FORCEINLINE NODISCARD Bool TaskSet() const noexcept { return state_ == ZTaskStateEnum::kTaskSet;}
    FORCEINLINE NODISCARD ZTaskStateEnum State() const noexcept { return state_; }

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
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<Void(_ArgsType...)>(_func); }) {
                return TSimpleFunction<Void(_ArgsType...)>{};
            }
            else {
                return TFunction<Void(_ArgsType...)>{};
            }
        }());

        operate_func_ptr_ = [](ZRepeatTask* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                TaskParamsTuple* puple_ptr;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                }
                else {
                    puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                }

                puple_ptr->Apply(*func_ptr);
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //release params
                if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(&_task_ptr->params_ptr_);
                    puple_ptr->~TaskParamsTuple();
                }
                else {
                    TaskParamsTuple* puple_ptr = reinterpret_cast<TaskParamsTuple*>(_task_ptr->params_ptr_);
                    delete puple_ptr;
                }

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->params_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };
        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        if constexpr (sizeof(TaskParamsTuple) <= sizeof(Void*)) {
            new(&params_ptr_) TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        else {
            params_ptr_ = new TaskParamsTuple(std::forward<_ArgsType>(_args)...);
        }
        state_ = ZTaskStateEnum::kTaskSet;
    }

    template<typename _TaskFunction>
    Void SetTaskP(ZRepeatTask&& _func) noexcept {
        using FunctionType = decltype([] {
            if constexpr (requires{ TSimpleFunction<Void()>(_func); }) {
                return TSimpleFunction<Void()>{};
            }
            else {
                return TFunction<Void()>{};
            }
        }());

        operate_func_ptr_ = [](ZTaskSafe* _task_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                (*func_ptr)();
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                //release func
                FunctionType* func_ptr = reinterpret_cast<FunctionType*>(&_task_ptr->task_func_ptr_);
                func_ptr->~FunctionType();

                //reset
                _task_ptr->task_func_ptr_ = nullptr;
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: %d", _task_ptr->state_
                );
            }
        };

        new(&task_func_ptr_) FunctionType(std::forward<_TaskFunction>(_func));
        state_ = ZTaskStateEnum::kTaskSet;
    }

#pragma warning(pop)

    Void(*operate_func_ptr_)(ZRepeatTask*);
    Void* task_func_ptr_;
    Void* params_ptr_;
    ZTaskStateEnum state_;
};

}//zengine