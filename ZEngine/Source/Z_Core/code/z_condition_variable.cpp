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

#include "z_condition_variable.h"

namespace zengine {

ZConditionVariable::ZConditionVariable() noexcept 
        : SuperType_(), cs_mutex_(), wait_thread_num_(0), cv_finished_(false){
    InitializeConditionVariable(&cv_);
}

ZConditionVariable::~ZConditionVariable() noexcept {
    cv_finished_ = true;
    WakeAllConditionVariable(&cv_);
}

NODISCARD Int32 ZConditionVariable::WaitThreadNum() noexcept {
    cs_mutex_.Lock();
    Int32 thread_num = wait_thread_num_;
    cs_mutex_.Unlock();
    return thread_num;
}

NODISCARD Bool ZConditionVariable::Empty() noexcept {
    cs_mutex_.Lock();
    Bool empty = (wait_thread_num_ == 0);
    cs_mutex_.Unlock();
    return empty;
}

Void ZConditionVariable::Wait(TUniqueLock<ZMutex>& _mutex) noexcept {
    LockP(_mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
    }
    UnlockP(_mutex);
}

Void ZConditionVariable::WaitFor(TUniqueLock<ZMutex>& _mutex, TimeType _time) noexcept {
    LockP(_mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, static_cast<UInt32>(_time));
    }
    UnlockP(_mutex);
}

Void ZConditionVariable::WaitUntil(TUniqueLock<ZMutex>& _mutex, TimeType _time) noexcept {
    _time -= clock();
    LockP(_mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, static_cast<UInt32>(_time));
    }
    UnlockP(_mutex);
}

Void ZConditionVariable::NotifyOne() noexcept {
    WakeConditionVariable(&cv_);
}
Void ZConditionVariable::NotifyAll() noexcept {
    WakeAllConditionVariable(&cv_);
}

Void ZConditionVariable::LockP(TUniqueLock<ZMutex>& _mutex) noexcept {
    _mutex.Unlock();
    cs_mutex_.Lock();
    ++wait_thread_num_;
}

Void ZConditionVariable::UnlockP(TUniqueLock<ZMutex>& _mutex) noexcept {
    --wait_thread_num_;
    cs_mutex_.Unlock();
    _mutex.Lock();
}

}//zengine
