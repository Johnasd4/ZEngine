/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "m_log.h"
#include "t_function.h"
#include "t_tuple.h"
#include "z_object.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to ZTask operations.
 */
enum ZTaskErrorCodeEnum : ReturnType::ValueType {
    /** @brief Error occurred during task linking. */
    kZTaskErrorCode_LinkError = kErrorCodeBase_ZTask,
    /** @brief System or external library error encountered. */
    kZTaskErrorCode_SystemOrLibraryError,
    /** @brief A required parameter was a nullptr. */
    kZTaskErrorCode_NullptrParam,
    /** @brief Parameter value is out of the valid range. */
    kZTaskErrorCode_ParamOutOfRange,
    /** @brief Task is in an invalid state for the requested operation. */
    kZTaskErrorCode_StateError,
    /** @brief The return value has already been retrieved. */
    kZTaskErrorCode_ReturnValAlreadyGet,
    /** @brief A task is already assigned to this object. */
    kZTaskErrorCode_TaskAlreadyExist,
    /** @brief Cannot bind a return value pointer for a void return type. */
    kZTaskErrorCode_CanNotBindVoidReturn
};

} // error_code
} // zengine

namespace zengine {

/**
 * @brief Enum representing the lifecycle states of a ZTask.
 */
enum class ZTaskStateEnum : Int32 {
    /** @brief No task is currently assigned. */
    kNoTask,
    /** @brief Task is assigned and ready to be executed. */
    kTaskSet,
    /** @brief Task is currently being moved to another ZTask object. */
    kTaskMove,
    /** @brief Task is currently executing. */
    kTaskRunning, 
    /** @brief Task execution has completed. */
    kFinished
};

/**
 * @brief Task class that packages a function and its parameters for deferred execution.
 * 
 * This class uses Type Erasure and Small Buffer Optimization (SBO). It creates a callable object
 * that wraps the user-provided function and arguments.
 * Implementation Principle:
 * - If the size of the function object and arguments is <= 32 bytes (kFunctionParamsSBOSize),
 *   it stores them directly in an internal buffer (`sbo_ptr_`) to avoid heap allocation.
 * - If larger, it allocates memory on the heap and stores the pointer (`ptr_`).
 * - An internal function pointer (`operate_func_ptr_`) manages the polymorphism for running,
 *   moving, and destroying the specific type of task without virtual functions.
 * The task is designed to execute multiple times. It is not thread-safe itself.
 */
class CORE_DLLAPI CACHE_ALIGN ZTask : public ZObject {
public:
    /** @brief Alias for the parent class ZObject. */
    using SuperType_ = ZObject;

private:
    /** @brief The size of the Small Buffer Optimization buffer in bytes. */
    static inline constexpr SizeType kFunctionParamsSBOSize = 32ULL;

    /**
     * @brief Union to handle storage for Small Buffer Optimization.
     */
    union FunctionParamsSBOType_ {
        /** @brief Pointer to heap memory if the task data exceeds SBO size. */
        Void* ptr_;
        /** @brief Inline buffer for small task data. */
        SIMD_ALIGN Byte sbo_ptr_[kFunctionParamsSBOSize];
    };

public:
    /**
     * @brief Default constructor. Initializes an empty task.
     */
    ZTask() noexcept;

    /** @brief Deleted copy constructor. */
    ZTask(const ZTask&) = delete;

    /**
     * @brief Move constructor. Transfers ownership of the task from another ZTask object.
     * @param _task The source task object to move from.
     */
    ZTask(ZTask&& _task) noexcept;

    /**
     * @brief Templated constructor to create a task with a function and arguments.
     * @tparam _TaskFunction The type of the function or callable object.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function or callable object to execute.
     * @param _args The arguments to pass to the function.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    ZTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept : SuperType_() {
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Destructor. Cleans up task resources.
     */
    ~ZTask() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZTask& operator=(const ZTask&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _task The source task object to move from.
     * @return Reference to this object.
     */
    ZTask& operator=(ZTask&& _task) noexcept;

    /**
     * @brief Gets the current state of the task.
     * @return The current ZTaskStateEnum.
     */
    NODISCARD FORCEINLINE ZTaskStateEnum GetState() const noexcept { return state_; }

    /**
     * @brief Binds a pointer to store the return value of the task.
     * 
     * Cannot bind the return value if the task is already running.
     * @param _ret_val_ptr Pointer to the memory where the result should be stored.
     */
    FORCEINLINE Void BindReturn(Void* _ret_val_ptr) noexcept { 
        ASSERT(
            state_ == ZTaskStateEnum::kTaskSet || state_ == ZTaskStateEnum::kNoTask,
            "ZTask: Can not bind return value when task is running!"
        );
        ret_val_ptr_ = _ret_val_ptr; 
    }

    /**
     * @brief Sets a new task with a function and arguments.
     * 
     * If a task was previously set or finished, it is cleaned up before setting the new one.
     * @tparam _TaskFunction The type of the function or callable object.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function or callable object.
     * @param _args The arguments for the function.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        Clear();
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Clears the current task and resets the state.
     */
    Void Clear() noexcept;
    
    /**
     * @brief Executes the task logic.
     * @return Returns kOK if execution is successful, otherwise returns an error code.
     */
    NODISCARD ReturnType Run() noexcept;

    /**
     * @brief Checks if the task has finished execution.
     * @return True if the task state is kFinished, false otherwise.
     */
    NODISCARD FORCEINLINE Bool Finished() const noexcept { return state_ == ZTaskStateEnum::kFinished; }

private:
    /**
     * @brief Internal helper to handle the logic of moving a task.
     * @param _task The source task to move data from.
     */
    FORCEINLINE Void MoveP(ZTask&& _task) noexcept {
        // move function and params data by move
        state_ = ZTaskStateEnum::kTaskMove;
        _task.operate_func_ptr_(this, &_task);
        // move other members
        operate_func_ptr_ = _task.operate_func_ptr_;
        ret_val_ptr_ = _task.ret_val_ptr_;
        state_ = _task.state_;
        _task.state_ = ZTaskStateEnum::kNoTask;
    }

    /**
     * @brief Internal helper to set the task data and the operation function pointer.
     * 
     * This function constructs the task storage (using SBO or heap) and generates the lambda
     * assigned to `operate_func_ptr_` which handles execution, moving, and destruction.
     * @tparam _TaskFunction The type of the function.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function object.
     * @param _args The arguments.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTaskP(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        using FunctionType = kDecayType<_TaskFunction>;
        using TaskParamsTuple = TTuple<kDecayType<_ArgsType>...>;
        using TaskReturnType = typename std::invoke_result<_TaskFunction, _ArgsType...>::type;

        struct FunctionParamsType : public ZObject {
        public:
            FORCEINLINE FunctionParamsType(FunctionParamsType&& _task) noexcept
                : SuperType_()
                , func_(std::move(_task.func_))
                , params_(std::move(_task.params_))
            {}
            FORCEINLINE FunctionParamsType(_TaskFunction&& _func, _ArgsType&&... _args) noexcept
                : SuperType_()
                , func_(std::forward<_TaskFunction>(_func))
                , params_(std::forward<_ArgsType>(_args)...)
            {}

            FORCEINLINE ~FunctionParamsType() noexcept = default;

        protected:
            using SuperType_ = ZObject;

        public:
            FunctionType func_;
            TaskParamsTuple params_;
        };

        // determine if we can use SBO
        constexpr Bool kUseSBO =
            sizeof(FunctionParamsType) <= kFunctionParamsSBOSize &&
            alignof(FunctionParamsType) <= alignof(FunctionParamsSBOType_) &&
            kIsMoveAssignable<FunctionParamsType>;

        operate_func_ptr_ = [](ZTask* _task_ptr, Void* _param_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;

                // get task
                FunctionParamsType* task_ptr;
                if constexpr (kUseSBO) {
                    task_ptr = reinterpret_cast<FunctionParamsType*>(_task_ptr->func_params_.sbo_ptr_);
                }
                else {
                    task_ptr = reinterpret_cast<FunctionParamsType*>(_task_ptr->func_params_.ptr_);
                }

                // apply
                if constexpr (kIsSameType<TaskReturnType, Void>) {
                    task_ptr->params_.Apply(task_ptr->func_);
                }
                else {
                    if (_task_ptr->ret_val_ptr_ != nullptr) {
                        TaskReturnType* ret_val_ptr = reinterpret_cast<TaskReturnType*>(_task_ptr->ret_val_ptr_);
                        *ret_val_ptr = task_ptr->params_.Apply(task_ptr->func_);
                    }
                    else {
                        task_ptr->params_.Apply(task_ptr->func_);
                    }
                }
                _task_ptr->state_ = ZTaskStateEnum::kFinished;
                break;
            }
            case ZTaskStateEnum::kTaskMove:
            {
                ZTask* src_task_ptr = reinterpret_cast<ZTask*>(_param_ptr);
                if constexpr (kUseSBO) {
                    FunctionParamsType* src_func_params_ptr = reinterpret_cast<FunctionParamsType*>(
                        src_task_ptr->func_params_.sbo_ptr_
                    );
                    new(&_task_ptr->func_params_.sbo_ptr_) FunctionParamsType(std::move(*src_func_params_ptr));
                    src_func_params_ptr->~FunctionParamsType();
                }
                else {
                    FunctionParamsType* src_func_params_ptr = reinterpret_cast<FunctionParamsType*>(
                        src_task_ptr->func_params_.ptr_
                    );
                    _task_ptr->func_params_.ptr_ = src_func_params_ptr;
                }
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                // release task
                if constexpr (kUseSBO) {
                    FunctionParamsType* task_ptr = reinterpret_cast<FunctionParamsType*>(
                        _task_ptr->func_params_.sbo_ptr_
                    );
                    task_ptr->~FunctionParamsType();
                }
                else {
                    FunctionParamsType* task_ptr = reinterpret_cast<FunctionParamsType*>(
                        _task_ptr->func_params_.ptr_
                    );
                    delete task_ptr;
                }

                // reset
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                _task_ptr->ret_val_ptr_ = nullptr;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: {}", static_cast<Int32>(_task_ptr->state_)
                );
            }
        };
        
        // initialize params
        if constexpr (kUseSBO) {
            new(&func_params_.sbo_ptr_) FunctionParamsType(
                std::forward<_TaskFunction>(_func), 
                std::forward<_ArgsType>(_args)...
            );
        }
        else {
            func_params_.ptr_ = new FunctionParamsType(
                std::forward<_TaskFunction>(_func), 
                std::forward<_ArgsType>(_args)...
            );
        }

        state_ = ZTaskStateEnum::kTaskSet;
    }

    /** @brief Current state of the task (e.g., set, running, finished). */
    ZTaskStateEnum state_;
    /** @brief Function pointer responsible for executing, moving, or cleaning up the task. */
    Void(*operate_func_ptr_)(ZTask*, Void*);
    /** @brief Storage for the function and parameters (either SBO or heap pointer). */
    FunctionParamsSBOType_ func_params_;
    /** @brief Pointer to the memory address where the return value should be written. */
    Void* ret_val_ptr_;
};

/**
 * @brief Repeatable task class that packages a function and its parameters for deferred execution.
 * 
 * This class is similar to ZTask but is designed to be executed multiple times. It uses Type Erasure
 * and Small Buffer Optimization (SBO) like ZTask, but does not support binding a return value pointer
 * since it is meant for repeated execution where the return value may not be relevant or consistent.
 * The task is designed to execute multiple times. It is not thread-safe itself.
 */
class CORE_DLLAPI CACHE_ALIGN ZRepeatTask : public ZObject {
public:
    /** @brief Alias for the parent class ZObject. */
    using SuperType_ = ZObject;

private:
    /** @brief The size of the Small Buffer Optimization buffer in bytes. */
    static inline constexpr SizeType kFunctionParamsSBOSize = 48ULL;

    /**
     * @brief Union to handle storage for Small Buffer Optimization.
     */
    union FunctionParamsSBOType_ {
        /** @brief Pointer to heap memory if the task data exceeds SBO size. */
        Void* ptr_;
        /** @brief Inline buffer for small task data. */
        SIMD_ALIGN Byte sbo_ptr_[kFunctionParamsSBOSize];
    };

public:
    /**
     * @brief Default constructor. Initializes an empty task.
     */
    ZRepeatTask() noexcept;

    /** @brief Deleted copy constructor. */
    ZRepeatTask(const ZRepeatTask&) = delete;

    /**
     * @brief Move constructor. Transfers ownership of the task from another ZRepeatTask object.
     * @param _task The source task object to move from.
     */
    ZRepeatTask(ZRepeatTask&& _task) noexcept;

    /**
     * @brief Templated constructor to create a task with a function and arguments.
     * @tparam _TaskFunction The type of the function or callable object.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function or callable object to execute.
     * @param _args The arguments to pass to the function.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    ZRepeatTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept : SuperType_() {
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Destructor. Cleans up task resources.
     */
    ~ZRepeatTask() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZRepeatTask& operator=(const ZRepeatTask&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _task The source task object to move from.
     * @return Reference to this object.
     */
    ZRepeatTask& operator=(ZRepeatTask&& _task) noexcept;

    /**
     * @brief Gets the current state of the task.
     * @return The current ZTaskStateEnum.
     */
    NODISCARD FORCEINLINE ZTaskStateEnum GetState() const noexcept { return state_; }

    /**
     * @brief Sets a new task with a function and arguments.
     * 
     * If a task was previously set or finished, it is cleaned up before setting the new one.
     * @tparam _TaskFunction The type of the function or callable object.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function or callable object.
     * @param _args The arguments for the function.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        Clear();
        SetTaskP(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Clears the current task and resets the state.
     */
    Void Clear() noexcept;
    
    /**
     * @brief Executes the task logic.
     * @return Returns kOK if execution is successful, otherwise returns an error code.
     */
    NODISCARD ReturnType Run() noexcept;

    /**
     * @brief Checks if the task has finished execution.
     * @return True if the task state is kFinished, false otherwise.
     */
    NODISCARD FORCEINLINE Bool Finished() const noexcept { return state_ == ZTaskStateEnum::kFinished; }

private:
    /**
     * @brief Internal helper to handle the logic of moving a task.
     * @param _task The source task to move data from.
     */
    FORCEINLINE Void MoveP(ZRepeatTask&& _task) noexcept {
        // move function and params data by move
        state_ = ZTaskStateEnum::kTaskMove;
        _task.operate_func_ptr_(this, &_task);
        // move other members
        operate_func_ptr_ = _task.operate_func_ptr_;
        state_ = _task.state_;
        _task.state_ = ZTaskStateEnum::kNoTask;
    }

    /**
     * @brief Internal helper to set the task data and the operation function pointer.
     * 
     * This function constructs the task storage (using SBO or heap) and generates the lambda
     * assigned to `operate_func_ptr_` which handles execution, moving, and destruction.
     * @tparam _TaskFunction The type of the function.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function object.
     * @param _args The arguments.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    Void SetTaskP(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        using FunctionType = kDecayType<_TaskFunction>;
        using TaskParamsTuple = TTuple<kDecayType<_ArgsType>...>;
        using TaskReturnType = typename std::invoke_result<_TaskFunction, _ArgsType...>::type;

        struct FunctionParamsType : public ZObject {
        public:
            FORCEINLINE FunctionParamsType(FunctionParamsType&& _task) noexcept
                : SuperType_()
                , func_(std::move(_task.func_))
                , params_(std::move(_task.params_))
            {}
            FORCEINLINE FunctionParamsType(_TaskFunction&& _func, _ArgsType&&... _args) noexcept
                : SuperType_()
                , func_(std::forward<_TaskFunction>(_func))
                , params_(std::forward<_ArgsType>(_args)...)
            {}

            FORCEINLINE ~FunctionParamsType() noexcept = default;

        protected:
            using SuperType_ = ZObject;

        public:
            FunctionType func_;
            TaskParamsTuple params_;
        };

        // determine if we can use SBO
        constexpr Bool kUseSBO =
            sizeof(FunctionParamsType) <= kFunctionParamsSBOSize &&
            alignof(FunctionParamsType) <= alignof(FunctionParamsSBOType_) &&
            kIsMoveAssignable<FunctionParamsType>;

        operate_func_ptr_ = [](ZRepeatTask* _task_ptr, Void* _param_ptr) {
            switch (_task_ptr->state_) {
            case ZTaskStateEnum::kTaskSet:
            {
                _task_ptr->state_ = ZTaskStateEnum::kTaskRunning;

                // get task
                FunctionParamsType* task_ptr;
                if constexpr (kUseSBO) {
                    task_ptr = reinterpret_cast<FunctionParamsType*>(_task_ptr->func_params_.sbo_ptr_);
                }
                else {
                    task_ptr = reinterpret_cast<FunctionParamsType*>(_task_ptr->func_params_.ptr_);
                }

                // apply
                if constexpr (kIsSameType<TaskReturnType, Void>) {
                    task_ptr->params_.Apply(task_ptr->func_);
                }
                else {
                    task_ptr->params_.Apply(task_ptr->func_);
                }
                // reset to set state for repeat use
                _task_ptr->state_ = ZTaskStateEnum::kTaskSet;
                break;
            }
            case ZTaskStateEnum::kTaskMove:
            {
                ZRepeatTask* src_task_ptr = reinterpret_cast<ZRepeatTask*>(_param_ptr);
                if constexpr (kUseSBO) {
                    FunctionParamsType* src_func_params_ptr = reinterpret_cast<FunctionParamsType*>(
                        src_task_ptr->func_params_.sbo_ptr_
                    );
                    new(&_task_ptr->func_params_.sbo_ptr_) FunctionParamsType(std::move(*src_func_params_ptr));
                    src_func_params_ptr->~FunctionParamsType();
                }
                else {
                    FunctionParamsType* src_func_params_ptr = reinterpret_cast<FunctionParamsType*>(
                        src_task_ptr->func_params_.ptr_
                    );
                    _task_ptr->func_params_.ptr_ = src_func_params_ptr;
                }
                break;
            }
            case ZTaskStateEnum::kFinished:
            {
                // release task
                if constexpr (kUseSBO) {
                    FunctionParamsType* task_ptr = reinterpret_cast<FunctionParamsType*>(
                        _task_ptr->func_params_.sbo_ptr_
                    );
                    task_ptr->~FunctionParamsType();
                }
                else {
                    FunctionParamsType* task_ptr = reinterpret_cast<FunctionParamsType*>(
                        _task_ptr->func_params_.ptr_
                    );
                    delete task_ptr;
                }

                // reset
                _task_ptr->state_ = ZTaskStateEnum::kNoTask;
                break;
            }
            default:
                Z_LOG_ERROR(
                    error_code::kZTaskErrorCode_StateError, 0, 
                    "Task state not expected! state_: {}", static_cast<Int32>(_task_ptr->state_)
                );
            }
        };
        
        // initialize params
        if constexpr (kUseSBO) {
            new(&func_params_.sbo_ptr_) FunctionParamsType(
                std::forward<_TaskFunction>(_func), 
                std::forward<_ArgsType>(_args)...
            );
        }
        else {
            func_params_.ptr_ = new FunctionParamsType(
                std::forward<_TaskFunction>(_func), 
                std::forward<_ArgsType>(_args)...
            );
        }

        state_ = ZTaskStateEnum::kTaskSet;
    }

private:
    /** @brief Current state of the task (e.g., set, running, finished). */
    ZTaskStateEnum state_;
    /** @brief Function pointer responsible for executing, moving, or cleaning up the task. */
    Void(*operate_func_ptr_)(ZRepeatTask*, Void*);
    /** @brief Storage for the function and parameters (either SBO or heap pointer). */
    FunctionParamsSBOType_ func_params_;
};

} // zengine