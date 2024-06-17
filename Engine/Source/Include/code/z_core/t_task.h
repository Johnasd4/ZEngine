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

#include "m_log.h"
#include "t_tuple.h"
#include "z_object.h"
#include "z_sem_mutex.h"

namespace zengine {

namespace error_code {

enum TTaskErrorCode : ReturnType {
    kTTaskErrorCodeLinkError = kErrorCodeBasePCore,
    kTTaskErrorCodeReturnValAlreadyGet
};

}//error_code

/*
    Task class, package a function and it's params, can run at any time.
*/
template<Bool kNeedReturn, typename TaskFunction, typename... ArgsType>
class TTask : public ZObject {
public:
    using ReturnType = std::invoke_result<TaskFunction, ArgsType...>::type;
    using TaskParamsTuple = TTuple<ArgsType...>;

    TTask() noexcept : SuperType(), task_func_ptr_(nullptr), task_params_ptr_(nullptr), ret_val_ptr_(nullptr) {}
    TTask(const TTask& task) noexcept 
            : SuperType()
            , task_func_ptr_(task.task_func_ptr_)
            , task_params_ptr_(nullptr)
            , ret_val_ptr_(nullptr) {
        if (task.task_params_ptr_ != nullptr) {
            task_params_ptr_ = new TaskParamsTuple(*task.task_params_ptr_);
        }
        if (task.ret_val_ptr_ != nullptr) {
            ret_val_ptr_ = new ReturnType(*task.ret_val_ptr_);
        }
    }
    TTask(TTask&& task) noexcept 
            : SuperType()
            , task_func_ptr_(task.task_func_ptr_)
            , task_params_ptr_(task.task_params_ptr_)
            , ret_val_ptr_(task.ret_val_ptr_) {
        task.task_func_ptr_ = nullptr;
        task.task_params_ptr_ = nullptr;
        task.ret_val_ptr_ = nullptr;
    }
    TTask(TaskFunction&& func, ArgsType&&... args) noexcept 
            : SuperType()
            , task_func_ptr_(&func)
            , task_params_ptr_(new TaskParamsTuple(std::forward<ArgsType>(args)...)) 
            , ret_val_ptr_(nullptr) {}
    ~TTask() noexcept {
        if (task_params_ptr_ != nullptr) {
            delete task_params_ptr_;
        }
        if (ret_val_ptr_ != nullptr) {
            delete ret_val_ptr_;
        }
    }

    TTask& operator=(const TTask& task) noexcept {
        task_func_ptr_ = task.task_func_ptr_;
        if (task.task_params_ptr_ != nullptr) {
            task_params_ptr_ = new TaskParamsTuple(*task.task_params_ptr_);
        }
        else {
            task_params_ptr_ = nullptr;
        }
        if (task.ret_val_ptr_ != nullptr) {
            ret_val_ptr_ = new ReturnType(*task.ret_val_ptr_);
        }
        else {
            ret_val_ptr_ = nullptr;
        }
        return *this;
    }
    TTask& operator=(TTask&& task) noexcept {
        task_func_ptr_ = task.task_func_ptr_;
        task_params_ptr_ = task.task_params_ptr_;
        ret_val_ptr_ = task.ret_val_ptr_;
        task.task_func_ptr_ = nullptr;
        task.task_params_ptr_ = nullptr;
        task.ret_val_ptr_ = nullptr;
        return *this;
    }

    template<typename ObjectType>
    FORCEINLINE Void Swap(TTask& task) noexcept {
        zengine::Swap(this, &task);
    }

    NODISCARD ReturnType* GetReturn() noexcept {
        ReturnType* temp_ret_val_ptr = ret_val_ptr_;
        if (ret_val_ptr_ == nullptr) {
            Z_LOG_ERROR(error_code::kTTaskErrorCodeReturnValAlreadyGet, 0, "Return value already get! ");
            return nullptr;
        }
        ret_val_ptr_ = nullptr;
        return temp_ret_val_ptr;
    }

    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(const TaskObjectType<kIndex>& object) noexcept {
        std::get<kIndex>(task_) = object;
    }
    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(TaskObjectType<kIndex>&& object) noexcept {
        std::get<kIndex>(task_) = std::forward<TaskObjectType<kIndex>>(object);
    }
    template<typename ObjectType>
    FORCEINLINE constexpr Void Set(const ObjectType& object) noexcept {
        std::get<ObjectType>(task_) = object;
    }
    template<typename ObjectType>
    FORCEINLINE constexpr Void Set(ObjectType&& object) noexcept {
        std::get<ObjectType>(task_) = std::forward<ObjectType>(object);
    }

    NODISCARD FORCEINLINE constexpr const IndexType Size() const noexcept {
        return static_cast<IndexType>(std::task_size<STDTask>::value);
    }
    template<typename Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(Function&& func) noexcept {
        return std::apply(std::forward<Function>(func), std::move(task_));
    }


protected:
    using SuperType = ZObject;

private:
    template<typename... OtherArgsType>
    friend class TTask;

    TaskFunction* task_func_ptr_;
    TaskParamsTuple* task_params_ptr_;
    ReturnType* ret_val_ptr_;
    ZSemMutex task_mutex_;
};

namespace task {

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr TTask<ArgsType...> MakeTask(ArgsType&&... args) noexcept {
    return TTask(std::forward<ArgsType>(args)...);
}

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr TTask<ArgsType&...> Tie(ArgsType&... args) noexcept {
    return TTask<ArgsType&...>(args...);
}

template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTask<ArgsType...>& task) noexcept {
    return task.Get<kIndex>();
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTask<ArgsType...>& task) noexcept {
    return task.Get<kIndex>();
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTask<ArgsType...>&& task) noexcept {
    return std::move(task.Get<kIndex>());
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTask<ArgsType...>&& task) noexcept {
    return std::move(task.Get<kIndex>());
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr ObjectType& Get(TTask<ArgsType...>& task) noexcept {
    return task.Get<ObjectType>();
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr const ObjectType& Get(const TTask<ArgsType...>& task) noexcept {
    return task.Get<ObjectType>();
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr ObjectType&& Get(TTask<ArgsType...>&& task) noexcept {
    return std::move(task.Get<ObjectType>());
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr const ObjectType&& Get(const TTask<ArgsType...>&& task) noexcept {
    return std::move(task.Get<ObjectType>());
}

template<IndexType kIndex, typename ObjectType, typename... ArgsType>
FORCEINLINE constexpr Void Set(TTask<ArgsType...>* task, ObjectType&& object) noexcept {
    task->Set<kIndex>(std::forward<ObjectType>(object));
}

template<typename ObjectType, typename... ArgsType>
FORCEINLINE constexpr Void Set(TTask<ArgsType...>* task, ObjectType&& object) noexcept {
    task->Set<ObjectType>(std::forward<ObjectType>(object));
}

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr const IndexType Size(const TTask<ArgsType...>& task) noexcept {
    return task.Size();
}

template<typename Function,typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(Function&& func, TTask<ArgsType...>&& task) noexcept {
    return task.Apply(std::forward<Function>(func));
}

}//task
}//zengine

#endif //!Z_CORE_T_Task_H_