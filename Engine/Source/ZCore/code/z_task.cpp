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

#include "z_task.h"

namespace zengine {

ZTaskSafe::ZTaskSafe() noexcept
    : SuperType_()
    , operate_func_ptr_(nullptr)
    , task_func_ptr_(nullptr)
    , params_ptr_(nullptr)
    , ret_val_ptr_(nullptr)
    , state_(kZTaskStateNoTask)
    , mutex_() {}

ZTaskSafe::ZTaskSafe(ZTaskSafe&& _task) noexcept : SuperType_() , mutex_() {
    MoveP(std::forward<ZTaskSafe>(_task));
}

ZTaskSafe::~ZTaskSafe() noexcept { Clear(); }

ZTaskSafe& ZTaskSafe::operator=(ZTaskSafe&& _task) noexcept {
    mutex_.Lock();
    MoveP(std::forward<ZTaskSafe>(_task));
    mutex_.Unlock();
    return *this;
}

NODISCARD ReturnType ZTaskSafe::operator()() noexcept {
    ReturnType ret_val = kOK;
    if (!IN_STATE(state_, kZTaskStateTaskSet)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(
            ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
            state_, kZTaskStateTaskSet);
        return ret_val;
    }
    operate_func_ptr_(this);
    return ret_val;
}

NODISCARD ReturnType ZTaskSafe::BindReturn(Void* _ret_val_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (IN_STATE(state_, kZTaskStateTaskRunning)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(ret_val, 0, "Task Running, can not bind return! state_: %d", state_);
        return ret_val;
    }
    mutex_.Lock();
    if (IN_STATE(state_, kZTaskStateFinished)) {
        operate_func_ptr_(this);
    }
    ret_val_ptr_ = _ret_val_ptr;
    mutex_.Unlock();
    return ret_val;
}

Void ZTaskSafe::Clear() noexcept {
    TLockGuard<ZMutex> lock_guard(mutex_);
    if (state_ == kZTaskStateTaskSet) {
        state_ = kZTaskStateFinished;
    }
    if (state_ == kZTaskStateFinished) {
        operate_func_ptr_(this);
    }
    state_ = kZTaskStateNoTask;
    mutex_.Unlock();
}

NODISCARD ReturnType ZTaskSafe::Run() noexcept {
    ReturnType ret_val = kOK;
    TLockGuard<ZMutex> lock_guard(mutex_);
    if (!IN_STATE(state_, kZTaskStateTaskSet)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
            state_, kZTaskStateTaskSet);
        return ret_val;
    }
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
    _task.state_ = kZTaskStateNoTask;
    _task.mutex_.Unlock();
}

ZTask::ZTask() noexcept
        : SuperType_()
        , operate_func_ptr_(nullptr)
        , task_func_ptr_(nullptr)
        , params_ptr_(nullptr)
        , ret_val_ptr_(nullptr)
        , state_(kZTaskStateNoTask) {}

ZTask::ZTask(ZTask&& _task) noexcept : SuperType_() {
    MoveP(std::forward<ZTask>(_task));
}

ZTask::ZTask(ZTaskSafe&& _task) noexcept : SuperType_() {
    MoveP(std::move(*reinterpret_cast<ZTask*>(&(_task))));
}

ZTask::~ZTask() noexcept { Clear(); }

ZTask& ZTask::operator=(ZTask&& _task) noexcept {
    MoveP(std::forward<ZTask>(_task));
    return *this;
}
ZTask& ZTask::operator=(ZTaskSafe&& _task) noexcept {
    MoveP(std::move(*reinterpret_cast<ZTask*>(&(_task))));
    return *this;
}

NODISCARD ReturnType ZTask::operator()() noexcept {
    ReturnType ret_val = kOK;
    if (!IN_STATE(state_, kZTaskStateTaskSet)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(
            ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
            state_, kZTaskStateTaskSet);
        return ret_val;
    }
    operate_func_ptr_(this);
    return ret_val;
}

NODISCARD ReturnType ZTask::BindReturn(Void* _ret_val_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (IN_STATE(state_, kZTaskStateTaskRunning)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(ret_val, 0, "Task Running, can not bind return! state_: %d", state_);
        return ret_val;
    }
    if (IN_STATE(state_, kZTaskStateFinished)) {
        operate_func_ptr_(this);
    }
    ret_val_ptr_ = _ret_val_ptr;
    return ret_val;
}

Void ZTask::Clear() noexcept {
    if (state_ == kZTaskStateTaskSet) {
        state_ = kZTaskStateFinished;
    }
    if (state_ == kZTaskStateFinished) {
        operate_func_ptr_(this);
    }
    state_ = kZTaskStateNoTask;
}

NODISCARD ReturnType ZTask::Run() noexcept {
    ReturnType ret_val = kOK;
    if (!IN_STATE(state_, kZTaskStateTaskSet)) {
        ret_val = error_code::kZTaskErrorCodeTaskStateError;
        Z_LOG_ERROR(ret_val, 0, "Task stata error, can not run! state_: %d expect state: %d",
            state_, kZTaskStateTaskSet);
        return ret_val;
    }
    operate_func_ptr_(this);
    return ret_val;
}

Void ZTask::MoveP(ZTask&& _task) noexcept {
    operate_func_ptr_ = _task.operate_func_ptr_;
    task_func_ptr_ = _task.task_func_ptr_;
    params_ptr_ = _task.params_ptr_;
    ret_val_ptr_ = _task.ret_val_ptr_;
    state_ = _task.state_;
    _task.params_ptr_ = nullptr;
    _task.state_ = kZTaskStateNoTask;
}

}//zengine
