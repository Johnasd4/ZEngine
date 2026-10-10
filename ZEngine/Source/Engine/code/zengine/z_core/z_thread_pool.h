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

#include "t_list.h"
#include "t_queue.h"
#include "t_unique_lock.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_task.h"
#include "z_thread.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to the ZThreadPool class.
 */
enum ZThreadPoolErrorCodeEnum : ReturnType::ValueType {
    /** @brief Error occurred during task linking or execution. */
    kZThreadPoolErrorCode_LinkError = kErrorCodeBase_ZThreadPool,
    /** @brief System or external library error. */
    kZThreadPoolErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was provided. */
    kZThreadPoolErrorCode_NullptrParam,
    /** @brief A parameter was out of the valid range. */
    kZThreadPoolErrorCode_ParamOutOfRange
};
} // error_code
} // zengine

namespace zengine {

/**
 * @brief Thread pool class.
 *
 * This class implements a thread pool pattern, managing a collection of worker threads
 * and a synchronized task queue. It works on the Producer-Consumer principle where
 * tasks are added to a queue and idle threads consume and execute them.
 */
class CORE_DLLAPI CACHE_ALIGN ZThreadPool : public ZObject {
public:
    /** @brief Type definition for thread ID. */
    using ThreadIDType_ = UInt32;
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

public:
    NODISCARD static ZThreadPool& GetInstance() noexcept;

    /**
     * @brief Default constructor.
     *
     * Initializes an empty thread pool.
     */
    ZThreadPool() noexcept;

    /** @brief Deleted copy constructor. */
    ZThreadPool(const ZThreadPool&) = delete;

    /** @brief Deleted move constructor. */
    ZThreadPool(ZThreadPool&&) = delete;

    /**
     * @brief Constructor with initial thread count.
     *
     * @param _thread_num The number of worker threads to initialize the pool with.
     */
    ZThreadPool(UInt32 _thread_num) noexcept;

    /**
     * @brief Destructor.
     *
     * Clears pending tasks, notifies all threads, and joins them to ensure clean shutdown.
     */
    ~ZThreadPool() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZThreadPool& operator=(const ZThreadPool&) = delete;

    /** @brief Deleted move assignment operator. */
    ZThreadPool& operator=(ZThreadPool&&) = delete;

    /**
     * @brief Gets the total number of threads in the pool.
     * @return The total count of worker threads.
     */
    NODISCARD FORCEINLINE UInt32 TotalThreadNum() const noexcept {
        TUniqueLock<ZMutex> lock(pool_mutex_);
        return total_thread_num_; 
    }

    /**
     * @brief Gets the number of currently free (idle) threads.
     * @return The count of threads waiting for tasks.
     */
    NODISCARD FORCEINLINE UInt32 FreeThreadNum() const noexcept { 
        TUniqueLock<ZMutex> lock(pool_mutex_);
        return free_thread_num_; 
    }

    /**
     * @brief Adds more working threads to the pool.
     *
     * @param _thread_num The number of new threads to add (must be > 0).
     */
    Void AddWorkThread(UInt32 _thread_num) noexcept;

    /**
     * @brief Suspends the calling thread until all tasks in the pool are finished.
     *
     * This function waits until the task queue is empty and all threads are in the idle state.
     */
    Void WaitUntilTaskFinished() noexcept;

    /**
     * @brief Adds a task to the thread pool with variadic arguments.
     *
     * @tparam _TaskFunction The type of the function or callable object.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function or callable object to execute.
     * @param _args The arguments to pass to the function.
     */
    template<typename _TaskFunction, typename... _ArgsType>
    Void AddTask(_TaskFunction&& _func, _ArgsType&&... _args) noexcept {
        TUniqueLock<ZMutex> lock(pool_mutex_);
        task_queue_.EmplaceBack(std::forward<_TaskFunction>(_func), std::forward<_ArgsType>(_args)...);
        work_thread_cv_.NotifyOne();
    }

    /**
     * @brief Adds a ZTask object to the thread pool.
     *
     * @param _task The task object to move into the queue.
     */
    Void AddTask(ZTask&& _task) noexcept;

    /**
     * @brief Clears all pending tasks from the queue.
     *
     * Does not stop currently running tasks.
     */
    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(pool_mutex_);
        task_queue_.Clear(); 
    }

private:
    /**
     * @brief The worker thread entry point function.
     *
     * @param _thread_pool_ptr Pointer to the ZThreadPool instance.
     */
    static Void TaskThreadFuncP(ZThreadPool* _thread_pool_ptr) noexcept;

private:
    /** @brief Mutex to protect pool state and task queue. */
    CACHE_ALIGN mutable ZMutex pool_mutex_;
    /** @brief The total number of worker threads managed by the pool. */
    UInt32 total_thread_num_;
    /** @brief The number of threads currently waiting for tasks. */
    UInt32 free_thread_num_;
    /** @brief Flag indicating if the pool is being destroyed. */
    Bool finished_;
    /** @brief Queue containing tasks waiting to be executed. */
    TQueue<ZTask> task_queue_;
    /** @brief Condition variable to wake up worker threads when a task is added. */
    ZConditionVariable work_thread_cv_;

    /** @brief Mutex to protect the list of thread objects. */
    CACHE_ALIGN mutable ZMutex thread_list_mutex_;
    /** @brief List of worker thread objects. */
    TList<ZThread> thread_list_;
    /** @brief Condition variable to notify when all tasks are finished. */
    ZConditionVariable task_finished_cv_;
};

} // zengine