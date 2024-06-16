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

#include "z_condition_variable.h"

namespace zengine {

CORE_DLLAPI ZConditionVariable::ZConditionVariable() noexcept 
        : SuperType(), cs_mutex_(), wait_thread_num_(0), cv_finished_(false){
    InitializeConditionVariable(&cv_);
}

CORE_DLLAPI ZConditionVariable::~ZConditionVariable() noexcept {
    cv_finished_ = true;
    WakeAllConditionVariable(&cv_);
}

CORE_DLLAPI NODISCARD Int32 ZConditionVariable::WaitThreadNum() noexcept {
    cs_mutex_.Lock();
    Int32 thread_num = wait_thread_num_;
    cs_mutex_.Unlock();
    return thread_num;
}

CORE_DLLAPI NODISCARD Bool ZConditionVariable::Empty() noexcept {
    cs_mutex_.Lock();
    Bool empty = (wait_thread_num_ == 0);
    cs_mutex_.Unlock();
    return empty;
}

CORE_DLLAPI Void ZConditionVariable::Wait(TUniqueLock<ZMutex>& mutex) noexcept {
    LockP(mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
    }
    UnlockP(mutex);
}

CORE_DLLAPI Void ZConditionVariable::WaitFor(TUniqueLock<ZMutex>& mutex, UInt32 time) noexcept {
    LockP(mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
    }
    UnlockP(mutex);
}

CORE_DLLAPI Void ZConditionVariable::WaitUntil(TUniqueLock<ZMutex>& mutex, UInt32 time) noexcept {
    time -= clock();
    LockP(mutex);
    if (!cv_finished_) {
        SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
    }
    UnlockP(mutex);
}

CORE_DLLAPI Void ZConditionVariable::NotifyOne() noexcept {
    WakeConditionVariable(&cv_);
}
CORE_DLLAPI Void ZConditionVariable::NotifyAll() noexcept {
    WakeAllConditionVariable(&cv_);
}

CORE_DLLAPI Void ZConditionVariable::LockP(TUniqueLock<ZMutex>& mutex) noexcept {
    mutex.Unlock();
    cs_mutex_.Lock();
    ++wait_thread_num_;
}

CORE_DLLAPI Void ZConditionVariable::UnlockP(TUniqueLock<ZMutex>& mutex) noexcept {
    --wait_thread_num_;
    cs_mutex_.Unlock();
    mutex.Lock();
}

}//zengine
