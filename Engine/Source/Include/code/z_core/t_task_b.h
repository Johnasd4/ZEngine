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

        enum ZTaskErrorCode : ReturnType {
            kZTaskErrorCodeLinkError = kErrorCodeBasePCore,
            kZTaskErrorCodeTaskStateError,
            kZTaskErrorCodeReturnValAlreadyGet,
            kZTaskErrorCodeTaskAlreadyExist,
            kZTaskErrorCodeCanNotBindVoidReturn
        };

    }//error_code

    enum ZTaskState : IndexType {
        kZTaskStateNoTask = 0x1,
        kZTaskStateTaskSet = 0x2,
        kZTaskStateTaskRunning = 0x4,
        kZTaskStateReturnReady = 0x8,
    };

    /*
        Task class, package a function and it's params, the task can only run one time.
    */
    template<typename TaskFunction, typename... ArgsType>
    class ZTask : public ZObject {
    public:
        using TaskReturnType = typename std::invoke_result<TaskFunction, ArgsType...>::type;
        using TaskParamsTuple = TTuple<ArgsType...>;

        ZTask() noexcept
            : SuperType()
            , func_(nullptr)
            , params_ptr_(nullptr)
            , ret_val_ptr_(nullptr)
            , mutex_()
            , state_(kZTaskStateNoTask) {}
        ZTask(ZTask&& task) noexcept {
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
            task.state_ = kZTaskStateNoTask;
            task.mutex_.Unlock();
        }
        ZTask(TaskFunction func, ArgsType&&... args) noexcept
            : SuperType()
            , func_(std::forward<TaskFunction>(func))
            , params_ptr_(new TaskParamsTuple(std::forward<ArgsType>(args)...))
            , ret_val_ptr_(nullptr)
            , mutex_()
            , state_(kZTaskStateTaskSet) {}
        ~ZTask() noexcept {
            if (params_ptr_ != nullptr) {
                delete params_ptr_;
            }
        }

        ZTask& operator=(ZTask&& task) noexcept {
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
            task.state_ = kZTaskStateNoTask;
            mutex_.Unlock();
            task.mutex_.Unlock();
            return *this;
        }

        FORCEINLINE Void Swap(ZTask& task) noexcept {
            zengine::Swap(this, &task);
        }

        /*
            Binds the return value to the given pointer, can not bind return when the task is running.
            WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
            task is running.
        */
        NODISCARD ReturnType BindReturn(TaskReturnType* ret_val_ptr) noexcept {
            ReturnType ret_val = kOK;
            if (IN_STATE(state_, kZTaskStateTaskRunning)) {
                ret_val = error_code::kZTaskErrorCodeTaskStateError;
                Z_LOG_ERROR(ret_val, 0, "Task Running, can not bind return! state_: %d", state_);
                return ret_val;
            }
            TLockGuard<ZMutex> lock_guard(mutex_);
            if constexpr (kSameType<TaskReturnType, Void>) {
                ret_val = error_code::kZTaskErrorCodeCanNotBindVoidReturn;
                Z_LOG_ERROR(ret_val, 0, "Can not bind a Void return!");
                return ret_val;
            }
            if (IN_STATE(state_, kZTaskStateReturnReady)) {
                state_ = kZTaskStateNoTask;
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
            state_ = kZTaskStateTaskSet;
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
            state_ = kZTaskStateNoTask;
            mutex_.Unlock();
        }

#pragma warning(push)
#pragma warning(disable: 6031)

        NODISCARD ReturnType Run() noexcept {
            ReturnType ret_val = kOK;
            TLockGuard<ZMutex> lock_guard(mutex_);
            if (!IN_STATE(state_, kZTaskStateTaskSet)) {
                ret_val = error_code::kZTaskErrorCodeTaskStateError;
                Z_LOG_ERROR(ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
                    state_, kZTaskStateTaskSet);
                return ret_val;
            }
            state_ = kZTaskStateTaskRunning;
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
            state_ = kZTaskStateReturnReady;
            delete params_ptr_;
            params_ptr_ = nullptr;
            return ret_val;
        }

#pragma warning(pop)

        FORCEINLINE Bool ReturnReady() const noexcept { return state_ == kZTaskStateReturnReady; }
        FORCEINLINE ZTaskState State() const noexcept { return state_; }

    protected:
        using SuperType = ZObject;

    private:
        ZTask(const ZTask&) = delete;

        ZTask& operator=(const ZTask&) = delete;

        TaskFunction func_;
        TaskParamsTuple* params_ptr_;
        TaskReturnType* ret_val_ptr_;
        ZMutex mutex_;
        ZTaskState state_;
    };

    /*
        Task class, package a function and it's params, the task can only run one time.
    */
    template<typename TaskFunction>
    class ZTask<TaskFunction> : public ZObject {
    public:
        using TaskReturnType = typename std::invoke_result<TaskFunction>::type;

        ZTask() noexcept
            : SuperType()
            , func_(nullptr)
            , ret_val_ptr_(nullptr)
            , mutex_()
            , state_(kZTaskStateNoTask) {}
        ZTask(ZTask&& task) noexcept {
            task.mutex_.Lock();
            func_ = std::move(task.func_);
            if constexpr (kSameType<TaskReturnType, Void>) {
                ret_val_ptr_ = nullptr;
            }
            else {
                ret_val_ptr_ = task.ret_val_ptr_;
                task.ret_val_ptr_ = nullptr;
            }
            state_ = task.state_;
            task.state_ = kZTaskStateNoTask;
            task.mutex_.Unlock();
        }
        ZTask(TaskFunction func) noexcept
            : SuperType()
            , func_(std::forward<TaskFunction>(func))
            , ret_val_ptr_(nullptr)
            , mutex_()
            , state_(kZTaskStateTaskSet) {}
        ~ZTask() noexcept {}

        ZTask& operator=(ZTask&& task) noexcept {
            task.mutex_.Lock();
            mutex_.Lock();
            func_ = std::move(task.func_);
            if constexpr (!kSameType<TaskReturnType, Void>) {
                ret_val_ptr_ = task.ret_val_ptr_;
                task.ret_val_ptr_ = nullptr;
            }
            state_ = task.state_;
            task.state_ = kZTaskStateNoTask;
            mutex_.Unlock();
            task.mutex_.Unlock();
            return *this;
        }

        FORCEINLINE Void Swap(ZTask& task) noexcept {
            zengine::Swap(this, &task);
        }

        /*
            Binds the return value to the given pointer, can not bind return when the task is running.
            WARNING: Have a slight chance suspending the current thread until the task is finished if tring to bind when the
            task is running.
        */
        NODISCARD ReturnType BindReturn(TaskReturnType* ret_val_ptr) noexcept {
            ReturnType ret_val = kOK;
            if (IN_STATE(state_, kZTaskStateTaskRunning)) {
                ret_val = error_code::kZTaskErrorCodeTaskStateError;
                Z_LOG_ERROR(ret_val, 0, "Task Running, can not bind return! state_: %d", state_);
                return ret_val;
            }
            TLockGuard<ZMutex> lock_guard(mutex_);
            if constexpr (kSameType<TaskReturnType, Void>) {
                ret_val = error_code::kZTaskErrorCodeCanNotBindVoidReturn;
                Z_LOG_ERROR(ret_val, 0, "Can not bind a Void return!");
                return ret_val;
            }
            if (IN_STATE(state_, kZTaskStateReturnReady)) {
                state_ = kZTaskStateNoTask;
            }
            ret_val_ptr_ = ret_val_ptr;
            return ret_val;
        }

        Void SetTask(TaskFunction&& func) noexcept {
            mutex_.Lock();
            func_ = std::forward<TaskFunction>(func);
            if constexpr (!kSameType<TaskReturnType, Void>) {
                ret_val_ptr_ = nullptr;
            }
            state_ = kZTaskStateTaskSet;
            mutex_.Unlock();
        }

        NODISCARD Bool Clear() noexcept {
            mutex_.Lock();
            if constexpr (!kSameType<TaskReturnType, Void>) {
                ret_val_ptr_ = nullptr;
            }
            state_ = kZTaskStateNoTask;
            mutex_.Unlock();
        }

#pragma warning(push)
#pragma warning(disable: 6031)

        NODISCARD ReturnType Run() noexcept {
            ReturnType ret_val = kOK;
            TLockGuard<ZMutex> lock_guard(mutex_);
            if (!IN_STATE(state_, kZTaskStateTaskSet)) {
                ret_val = error_code::kZTaskErrorCodeTaskStateError;
                Z_LOG_ERROR(ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
                    state_, kZTaskStateTaskSet);
                return ret_val;
            }
            state_ = kZTaskStateTaskRunning;
            if constexpr (kSameType<TaskReturnType, Void>) {
                func_();
            }
            else {
                if (ret_val_ptr_ != nullptr) {
                    *ret_val_ptr_ = func_();
                }
                else {
                    func_();
                }
            }
            state_ = kZTaskStateReturnReady;
            return ret_val;
        }

#pragma warning(pop)

        FORCEINLINE Bool ReturnReady() const noexcept { return state_ == kZTaskStateReturnReady; }
        FORCEINLINE ZTaskState State() const noexcept { return state_; }

    protected:
        using SuperType = ZObject;

    public:
        ZTask(const ZTask&) = delete;

        ZTask& operator=(const ZTask&) = delete;

        TaskFunction func_;
        TaskReturnType* ret_val_ptr_;
        ZMutex mutex_;
        ZTaskState state_;
    };

    /*
        Task class, package a function and it's params, no returns, no state check, not thread safe.
    */
    template<typename TaskFunction, typename... ArgsType>
    class ZTaskFast : public ZObject {
    public:
        using TaskParamsTuple = TTuple<ArgsType...>;

        ZTaskFast() noexcept
            : SuperType()
            , func_(nullptr)
            , params_ptr_(nullptr) {}
        ZTaskFast(ZTaskFast&& task) noexcept
            : SuperType()
            , func_(std::move(task.func_))
            , params_ptr_(task.params_ptr_) {
            task.params_ptr_ = nullptr;
        }
        ZTaskFast(TaskFunction func, ArgsType&&... args) noexcept
            : SuperType()
            , func_(std::forward<TaskFunction>(func))
            , params_ptr_(new TaskParamsTuple(std::forward<ArgsType>(args)...)) {}
        ~ZTaskFast() noexcept {
            if (params_ptr_ != nullptr) {
                delete params_ptr_;
            }
        }

        ZTaskFast& operator=(ZTaskFast&& task) noexcept {
            func_ = std::move(task.func_);
            params_ptr_ = task.params_ptr_;
            task.params_ptr_ = nullptr;
            return *this;
        }

        FORCEINLINE Void Swap(ZTaskFast& task) noexcept {
            zengine::Swap(this, &task);
        }

        Void SetTask(TaskFunction&& func, ArgsType&&... args) noexcept {
            func_ = std::forward<TaskFunction>(func);
            if (params_ptr_ != nullptr) {
                params_ptr_->~TTuple();
            }
            new((Void*)params_ptr_) TaskParamsTuple(std::forward<ArgsType>(args)...);
        }

        Void Clear() noexcept {
            if (params_ptr_ != nullptr) {
                delete params_ptr_;
                params_ptr_ = nullptr;
            }
        }

#pragma warning(push)
#pragma warning(disable: 6031)

        Void Run() noexcept {
            params_ptr_->Apply(func_);
            delete params_ptr_;
            params_ptr_ = nullptr;
        }

#pragma warning(pop)

    protected:
        using SuperType = ZObject;

    private:
        ZTaskFast(const ZTaskFast&) = delete;

        ZTaskFast& operator=(const ZTaskFast&) = delete;

        TaskFunction func_;
        TaskParamsTuple* params_ptr_;
    };

    /*
        Task class, package a function and it's params, no returns, no state check, not thread safe.
    */
    template<typename TaskFunction>
    class ZTaskFast<TaskFunction> : public ZObject {
    public:
        FORCEINLINE ZTaskFast() noexcept
            : SuperType()
            , func_(nullptr) {}
        FORCEINLINE ZTaskFast(ZTaskFast&& task) noexcept
            : SuperType()
            , func_(std::move(task.func_)) {
        }
        FORCEINLINE ZTaskFast(TaskFunction func) noexcept
            : SuperType()
            , func_(std::forward<TaskFunction>(func)) {}
        FORCEINLINE ~ZTaskFast() noexcept {}

        FORCEINLINE ZTaskFast& operator=(ZTaskFast&& task) noexcept {
            func_ = std::move(task.func_);
            return *this;
        }

        FORCEINLINE Void Swap(ZTaskFast& task) noexcept {
            zengine::Swap(this, &task);
        }

        FORCEINLINE Void SetTask(TaskFunction&& func) noexcept {
            func_ = std::forward<TaskFunction>(func);
        }

        FORCEINLINE Void Clear() noexcept {}

#pragma warning(push)
#pragma warning(disable: 6031)

        FORCEINLINE Void Run() noexcept { func_(); }

#pragma warning(pop)

    protected:
        using SuperType = ZObject;

    private:
        ZTaskFast(const ZTaskFast&) = delete;

        ZTaskFast& operator=(const ZTaskFast&) = delete;

        TaskFunction func_;
    };

    namespace task {

        template<typename TaskFunction, typename... ArgsType>
        NODISCARD FORCEINLINE ZTask<TaskFunction, ArgsType...> MakeTask(TaskFunction&& func, ArgsType&&... args) noexcept {
            return ZTask<TaskFunction, ArgsType...>(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
        }

        template<typename TaskFunction>
        NODISCARD FORCEINLINE ZTask<TaskFunction> MakeTask(TaskFunction&& func) noexcept {
            return ZTask<TaskFunction>(std::forward<TaskFunction>(func));
        }

        template<typename TaskFunction, typename... ArgsType>
        NODISCARD FORCEINLINE ZTask<TaskFunction, ArgsType...> MakeTaskFast(TaskFunction&& func, ArgsType&&... args) noexcept {
            return ZTaskFast<TaskFunction, ArgsType...>(std::forward<TaskFunction>(func), std::forward<ArgsType>(args)...);
        }

        template<typename TaskFunction>
        NODISCARD FORCEINLINE ZTask<TaskFunction> MakeTaskFast(TaskFunction&& func) noexcept {
            return ZTaskFast<TaskFunction>(std::forward<TaskFunction>(func));
        }

    }//task
}//zengine

#endif //!Z_CORE_T_Task_H_