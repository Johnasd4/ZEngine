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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_thread_pool.h"

#include "m_log.h"

namespace zengine {

NODISCARD ZThreadPool& ZThreadPool::GetInstance() noexcept {
    static ZThreadPool instance;
    return instance;
}

ZThreadPool::ZThreadPool() noexcept 
    : SuperType_()
    , pool_mutex_()
    , total_thread_num_(0U)
    , free_thread_num_(0U)
    , finished_(false)
    , task_queue_()
    , work_thread_cv_()
    , thread_list_mutex_()
    , thread_list_()
    , task_finished_cv_()
{}

ZThreadPool::ZThreadPool(UInt32 _thread_num) noexcept
    : SuperType_()
    , pool_mutex_()
    , total_thread_num_(0U)
    , free_thread_num_(0U)
    , finished_(false)
    , task_queue_()
    , work_thread_cv_()
    , thread_list_mutex_()
    , thread_list_()
    , task_finished_cv_()
{
    for (SizeType thread_index = 0; thread_index < _thread_num; ++thread_index) {
        thread_list_.EmplaceBack(TaskThreadFuncP, this);
    }
}

ZThreadPool::~ZThreadPool() noexcept {
    // Clear the unstarted tasks.
    {
        TLockGuard lock_guard(pool_mutex_);
        task_queue_.Clear();
        finished_ = true;
    }
    work_thread_cv_.NotifyAll();

    TLockGuard lock_guard(thread_list_mutex_);
    for (auto& iter : thread_list_) {
        if (iter.IsJoinable()) {
            iter.Join();
        }
    }
}

Void ZThreadPool::AddWorkThread(UInt32 _thread_num) noexcept {
    TLockGuard lock_guard(thread_list_mutex_);
    for (SizeType thread_index = 0; thread_index < _thread_num; ++thread_index) {
        thread_list_.EmplaceBack(TaskThreadFuncP, this);
    }
}

Void ZThreadPool::WaitUntilTaskFinished() noexcept {
    TUniqueLock<ZMutex> lock(pool_mutex_);
    task_finished_cv_.Wait(lock, [this]() {
        return task_queue_.IsEmpty() && (free_thread_num_ == total_thread_num_);
    });
}

Void ZThreadPool::AddTask(ZTask&& _task) noexcept {
    TLockGuard lock_guard(pool_mutex_);
    task_queue_.PushBack(std::move(_task));
    work_thread_cv_.NotifyOne();
}

Void ZThreadPool::TaskThreadFuncP(ZThreadPool* _thread_pool_ptr) noexcept {
    ReturnType link_code = kOK;
    ZTask task;

    {
        TLockGuard lock_guard(_thread_pool_ptr->pool_mutex_);
        ++_thread_pool_ptr->total_thread_num_;
    }

    for (;;) {
        // atom operation, do not remove the brace.
        {
            TUniqueLock<ZMutex> lock(_thread_pool_ptr->pool_mutex_);

            // get task
            while (!_thread_pool_ptr->task_queue_.TryPopFront(task)) {
                ++_thread_pool_ptr->free_thread_num_;
                if (_thread_pool_ptr->total_thread_num_ == _thread_pool_ptr->free_thread_num_) {
                    // all threads are idle, unlock the pool idle mutex.
                    _thread_pool_ptr->task_finished_cv_.NotifyOne();
                }
                // check if finished.
                if (_thread_pool_ptr->finished_) {
                    return;
                }
                _thread_pool_ptr->work_thread_cv_.Wait(lock);
                --_thread_pool_ptr->free_thread_num_;
            }

        }

        link_code = task.Run();
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kZThreadPoolErrorCode_LinkError, link_code, "ZTask::Run() link error!");
        }
        task.Clear();
    };
}

} // zengine
