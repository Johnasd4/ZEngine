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

#include "z_task.h"

#include "t_lock_guard.h"

namespace zengine {

ZTaskSafe::ZTaskSafe() noexcept
    : SuperType_()
    , operate_func_ptr_(nullptr)
    , task_func_ptr_(nullptr)
    , params_ptr_(nullptr)
    , ret_val_ptr_(nullptr)
    , state_(kZTaskState_NoTask)
    , mutex_() {}

ZTaskSafe::ZTaskSafe(ZTaskSafe&& _task) noexcept : SuperType_(std::forward<ZTaskSafe>(_task)) , mutex_() {
    MoveP(std::forward<ZTaskSafe>(_task));
}

ZTaskSafe::~ZTaskSafe() noexcept { Clear(); }

ZTaskSafe& ZTaskSafe::operator=(ZTaskSafe&& _task) noexcept {
    SuperType_::operator=(std::forward<ZTaskSafe>(_task));
    mutex_.Lock();
    MoveP(std::forward<ZTaskSafe>(_task));
    mutex_.Unlock();
    return *this;
}

NODISCARD ReturnType ZTaskSafe::operator()() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

NODISCARD ReturnType ZTaskSafe::BindReturn(Void* _ret_val_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (state_ == kZTaskState_TaskRunning) {
        ret_val = error_code::kZTaskErrorCode_StateError;
        Z_LOG_ERROR(ret_val, 0, L"Task Running, can not bind return! state_: %d", state_);
        return ret_val;
    }
    mutex_.Lock();
    if (state_ == kZTaskState_Finished) {
        operate_func_ptr_(this);
    }
    ret_val_ptr_ = _ret_val_ptr;
    mutex_.Unlock();
    return ret_val;
}

Void ZTaskSafe::Clear() noexcept {
    TLockGuard<ZMutex> lock_guard(mutex_);
    if (state_ == kZTaskState_TaskSet) {
        state_ = kZTaskState_Finished;
    }
    if (state_ == kZTaskState_Finished) {
        operate_func_ptr_(this);
    }
    state_ = kZTaskState_NoTask;
    mutex_.Unlock();
}

NODISCARD ReturnType ZTaskSafe::Run() noexcept {
    ReturnType ret_val = kOK;
    TLockGuard<ZMutex> lock_guard(mutex_);
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

Void ZTaskSafe::MoveP(ZTaskSafe&& _task) noexcept {
    _task.mutex_.Lock();
    operate_func_ptr_ = _task.operate_func_ptr_;
    task_func_ptr_ = _task.task_func_ptr_;
    params_ptr_ = _task.params_ptr_;
    ret_val_ptr_ = _task.ret_val_ptr_;
    state_ = _task.state_;
    _task.params_ptr_ = nullptr;
    _task.state_ = kZTaskState_NoTask;
    _task.mutex_.Unlock();
}

ZTask::ZTask() noexcept
        : SuperType_()
        , operate_func_ptr_(nullptr)
        , task_func_ptr_(nullptr)
        , params_ptr_(nullptr)
        , ret_val_ptr_(nullptr)
        , state_(kZTaskState_NoTask) {}

ZTask::ZTask(ZTask&& _task) noexcept : SuperType_(std::forward<ZTask>(_task)) {
    MoveP(std::forward<ZTask>(_task));
}

ZTask::~ZTask() noexcept { Clear(); }

ZTask& ZTask::operator=(ZTask&& _task) noexcept {
    SuperType_::operator=(std::forward<ZTask>(_task));
    MoveP(std::forward<ZTask>(_task));
    return *this;
}

NODISCARD ReturnType ZTask::operator()() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

NODISCARD ReturnType ZTask::BindReturn(Void* _ret_val_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (state_ == kZTaskState_TaskRunning) {
        ret_val = error_code::kZTaskErrorCode_StateError;
        Z_LOG_ERROR(ret_val, 0, L"Task Running, can not bind return! state_: %d", state_);
        return ret_val;
    }
    if (state_ == kZTaskState_Finished) {
        operate_func_ptr_(this);
    }
    ret_val_ptr_ = _ret_val_ptr;
    return ret_val;
}

Void ZTask::Clear() noexcept {
    if (state_ == kZTaskState_TaskSet) {
        state_ = kZTaskState_Finished;
    }
    if (state_ == kZTaskState_Finished) {
        operate_func_ptr_(this);
    }
    state_ = kZTaskState_NoTask;
}

NODISCARD ReturnType ZTask::Run() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

Void ZTask::MoveP(ZTask&& _task) noexcept {
    operate_func_ptr_ = _task.operate_func_ptr_;
    task_func_ptr_ = _task.task_func_ptr_;
    params_ptr_ = _task.params_ptr_;
    state_ = _task.state_;
    _task.params_ptr_ = nullptr;
    _task.state_ = kZTaskState_NoTask;
}

ZRepeatTask::ZRepeatTask() noexcept
    : SuperType_()
    , operate_func_ptr_(nullptr)
    , task_func_ptr_(nullptr)
    , params_ptr_(nullptr)
    , state_(kZTaskState_NoTask) {
}

ZRepeatTask::ZRepeatTask(ZRepeatTask&& _task) noexcept : SuperType_(std::forward<ZRepeatTask>(_task)) {
    MoveP(std::forward<ZRepeatTask>(_task));
}

ZRepeatTask::~ZRepeatTask() noexcept { Clear(); }

ZRepeatTask& ZRepeatTask::operator=(ZRepeatTask&& _task) noexcept {
    SuperType_::operator=(std::forward<ZRepeatTask>(_task));
    MoveP(std::forward<ZRepeatTask>(_task));
    return *this;
}

NODISCARD ReturnType ZRepeatTask::operator()() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

Void ZRepeatTask::Clear() noexcept {
    if (state_ == kZTaskState_TaskSet) {
        state_ = kZTaskState_Finished;
    }
    if (state_ == kZTaskState_Finished) {
        operate_func_ptr_(this);
    }
    state_ = kZTaskState_NoTask;
}

NODISCARD ReturnType ZRepeatTask::Run() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ != kZTaskState_TaskSet,
        error_code::kZTaskErrorCode_StateError,
        L"Task state error! state: %d expect state: %d",
        state_, kZTaskState_TaskSet
    );

    operate_func_ptr_(this);
    return ret_val;
}

Void ZRepeatTask::MoveP(ZRepeatTask&& _task) noexcept {
    operate_func_ptr_ = _task.operate_func_ptr_;
    task_func_ptr_ = _task.task_func_ptr_;
    params_ptr_ = _task.params_ptr_;
    state_ = _task.state_;
    _task.params_ptr_ = nullptr;
    _task.state_ = kZTaskState_NoTask;
}

}//zengine
