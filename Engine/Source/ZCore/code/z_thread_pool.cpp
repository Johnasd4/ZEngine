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
#define CORE_DLLFILE

#include "z_thread_pool.h"

#include "t_lock_guard.h"

namespace zengine {

ZThreadPool::ZThreadPool() noexcept 
        : SuperType()
        , thread_list_()
        , task_queue_()
        , pool_mutex_()
        , pool_idle_mutex_()
        , cv_()
        , max_thread_num_(0)
        , free_thread_num_(0)
        , finished_(false){
    pool_idle_mutex_.Lock();
}

ZThreadPool::ZThreadPool(Int32 thread_num_) noexcept
        : SuperType()
        , thread_list_()
        , task_queue_()
        , pool_mutex_()
        , pool_idle_mutex_()
        , cv_()
        , max_thread_num_(thread_num_)
        , free_thread_num_(0)
        , finished_(false) {
    pool_idle_mutex_.Lock();
    for (IndexType thread_index = 0; thread_index < thread_num_; ++thread_index) {
        thread_list_.EmplaceBack(ThreadFunc, Ref(*this));
    }
}

ZThreadPool::~ZThreadPool() noexcept {
    //Clear the unstarted tasks.
    pool_mutex_.Lock();
    task_queue_.Clear();
    pool_mutex_.Unlock();

    //Wait for the started tasks finish.
    pool_idle_mutex_.Lock();
    pool_idle_mutex_.Unlock();

    //Finish the thread func.
    pool_mutex_.Lock();
    finished_ = true;
    max_thread_num_ = 0;
    cv_.NotifyAll();
    pool_mutex_.Unlock();
    for (auto thread_it = thread_list_.Begin(); thread_it != thread_list_.End(); ++thread_it) {
        if (thread_it->Joinable()) {
            thread_it->Join();
        }
    }
}

NODISCARD ReturnType ZThreadPool::AddThreadNum(Int32 thread_num_) noexcept {
    ReturnType ret_val = kOK;
    if (thread_num_ < 0) {
        ret_val = error_code::kZThreadPoolErrorCodeAddNegitiveNumThread;
        Z_LOG_ERROR(ret_val, 0, "Add thread < 0! thread_num_: %d", thread_num_);
        return ret_val;
    }
    max_thread_num_ += thread_num_;
    for (IndexType thread_index = 0; thread_index < thread_num_; ++thread_index) {
        thread_list_.EmplaceBack(std::move(ZThread(ThreadFunc, Ref(*this))));
    }
    return ret_val;
}

/*
    Suspend until all the tasks are done.
*/
NODISCARD Void ZThreadPool::LockUntilTaskDone() noexcept {
    pool_idle_mutex_.Lock();
    pool_idle_mutex_.Unlock();
}

NODISCARD ReturnType ZThreadPool::AddTask(ZTask&& task) noexcept {
    ReturnType ret_val = kOK;
    TUniqueLock<ZMutex> lock(pool_mutex_);
    if (finished_) {
        ret_val = error_code::kZThreadPoolErrorCodePoolFinished;
        Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
        return ret_val;
    }
    //when idle.
    if (max_thread_num_ == free_thread_num_) {
        pool_idle_mutex_.TryLock();
    }
    task_queue_.Push(std::forward<ZTask>(task));
    cv_.NotifyOne();
    return ret_val;
}

NODISCARD ReturnType ZThreadPool::AddTask(ZTaskSafe&& task) noexcept {
    ReturnType ret_val = kOK;
    TUniqueLock<ZMutex> lock(pool_mutex_);
    if (finished_) {
        ret_val = error_code::kZThreadPoolErrorCodePoolFinished;
        Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
        return ret_val;
    }
    //when idle.
    if (max_thread_num_ == free_thread_num_) {
        pool_idle_mutex_.TryLock();
    }
    task_queue_.Push(std::forward<ZTaskSafe>(task));
    cv_.NotifyOne();
    return ret_val;
}

Void ZThreadPool::ClearTask() noexcept {
    TUniqueLock<ZMutex> lock(pool_mutex_);
    task_queue_.Clear();
}

Void ZThreadPool::ThreadFunc(ZThreadPool& thread_pool) noexcept {
    ZTask task;
    ReturnType link_code = kOK;
    while(true) {
        //atom operation, do not remove the brace.
        {
            TUniqueLock<ZMutex> lock(thread_pool.pool_mutex_);
            ++thread_pool.free_thread_num_;
            //task all done.
            if (thread_pool.task_queue_.Empty() && thread_pool.max_thread_num_ == thread_pool.free_thread_num_) {
                thread_pool.pool_idle_mutex_.Unlock();
            }
            thread_pool.cv_.Wait(lock, [&thread_pool] {
                return thread_pool.finished_ || !thread_pool.task_queue_.Empty();
            });
            --thread_pool.free_thread_num_;
            if (thread_pool.finished_) {
                return;
            }
            if (thread_pool.task_queue_.Empty()) {
                continue;
            }
            task = std::move(thread_pool.task_queue_.Front());
            thread_pool.task_queue_.Pop();
        }
        link_code = task.Run();
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kZTaskErrorCodeLinkError, link_code, "ZFastTask::Run() link error!");
        }
        task.Clear();
    }
}

}//zengine
