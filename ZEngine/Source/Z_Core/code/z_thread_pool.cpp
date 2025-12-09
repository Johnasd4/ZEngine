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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_thread_pool.h"

namespace zengine {

ZThreadPool::ZThreadPool() noexcept 
    : SuperType_()
    , thread_list_()
    , task_queue_()
    , pool_mutex_()
    , pool_idle_mutex_()
    , cv_()
    , max_thread_num_(0)
    , free_thread_num_(0)
    , finished_(false)
{
    pool_idle_mutex_.Lock();
}

ZThreadPool::ZThreadPool(Int32 _thread_num) noexcept
    : SuperType_()
    , thread_list_()
    , task_queue_()
    , pool_mutex_()
    , pool_idle_mutex_()
    , cv_()
    , max_thread_num_(_thread_num)
    , free_thread_num_(0)
    , finished_(false) 
{
    pool_idle_mutex_.Lock();
    for (SizeType thread_index = 0; thread_index < _thread_num; ++thread_index) {
        thread_list_.EmplaceBack(SubThread, std::ref(*this));
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

NODISCARD ReturnType ZThreadPool::AddThreadNum(Int32 _thread_num) noexcept {
    ReturnType ret_val = kOK;
    if (_thread_num < 0) {
        ret_val = error_code::kZThreadPoolErrorCode_AddNegitiveNumThread;
        Z_LOG_ERROR(ret_val, 0, "Add thread < 0! thread_num_: %d", _thread_num);
        return ret_val;
    }
    max_thread_num_ += _thread_num;
    for (SizeType thread_index = 0; thread_index < _thread_num; ++thread_index) {
        thread_list_.EmplaceBack(std::move(ZThread(SubThread, std::ref(*this))));
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

NODISCARD ReturnType ZThreadPool::AddTask(ZTask&& _task) noexcept {
    ReturnType ret_val = kOK;
    TUniqueLock<ZMutex> lock(pool_mutex_);
    if (finished_) {
        ret_val = error_code::kZThreadPoolErrorCode_PoolFinished;
        Z_LOG_ERROR(ret_val, 0, "Thread pool finished, can't add task!");
        return ret_val;
    }
    //when idle.
    if (max_thread_num_ == free_thread_num_) {
        pool_idle_mutex_.TryLock();
    }
    task_queue_.PushBack(std::forward<ZTask>(_task));
    cv_.NotifyOne();
    return ret_val;
}

Void ZThreadPool::ClearTask() noexcept {
    TUniqueLock<ZMutex> lock(pool_mutex_);
    task_queue_.Clear();
}

Void ZThreadPool::SubThread(ZThreadPool& _thread_pool) noexcept {
    ZTask task;
    ReturnType link_code = kOK;
    while(true) {
        //atom operation, do not remove the brace.
        {
            TUniqueLock<ZMutex> lock(_thread_pool.pool_mutex_);
            ++_thread_pool.free_thread_num_;
            //task all done.
            if (_thread_pool.task_queue_.Empty() && _thread_pool.max_thread_num_ == _thread_pool.free_thread_num_) {
                _thread_pool.pool_idle_mutex_.Unlock();
            }
            _thread_pool.cv_.Wait(lock, [&_thread_pool] {
                return _thread_pool.finished_ || !_thread_pool.task_queue_.Empty();
            });
            --_thread_pool.free_thread_num_;
            if (_thread_pool.finished_) {
                return;
            }
            if (_thread_pool.task_queue_.Empty()) {
                continue;
            }
            task = std::move(_thread_pool.task_queue_.Front());
            _thread_pool.task_queue_.PopFront();
        }
        link_code = task.Run();
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kZThreadPoolErrorCode_LinkError, link_code, "ZTask::Run() link error!");
        }
        task.Clear();
    }
}

}//zengine
