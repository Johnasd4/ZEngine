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
#pragma once

#include "drive.h"

#include "t_unique_lock.h"
#include "z_cs_mutex.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    A condition variable class.
*/
class CORE_DLLAPI ZConditionVariable : public ZObject {
public:
    ZConditionVariable() noexcept;
    ~ZConditionVariable() noexcept;

    NODISCARD Int32 WaitThreadNum() noexcept;
    NODISCARD Bool Empty() noexcept;

    Void Wait(TUniqueLock<ZMutex>& _mutex) noexcept;
    template <typename PredicateFunction>
    Void Wait(TUniqueLock<ZMutex>& _mutex, PredicateFunction _func) noexcept {
        LockP(_mutex);
        while (!_func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
        }
        UnlockP(_mutex);
    }
    /*
       Wait for a certain time(ms).
    */
    Void WaitFor(TUniqueLock<ZMutex>& _mutex, TimeType _time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename _PredicateFunction>
    Void WaitFor(TUniqueLock<ZMutex>& _mutex, TimeType _time, _PredicateFunction _func) noexcept {
        LockP(_mutex);
        while (!_func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, static_cast<UInt32>(_time));
        }
        UnlockP(_mutex);
    }
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    Void WaitUntil(TUniqueLock<ZMutex>& _mutex, TimeType _time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename _PredicateFunction>
    Void WaitUntil(TUniqueLock<ZMutex>& _mutex, TimeType _time, _PredicateFunction _func) noexcept {
        _time -= clock();
        LockP(_mutex);
        while (!_func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, static_cast<UInt32>(_time));
        }
        UnlockP(_mutex);
    }

    Void NotifyOne() noexcept;
    Void NotifyAll() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZConditionVariable(const ZConditionVariable&) = delete;
    ZConditionVariable(ZConditionVariable&&) = delete;
    ZConditionVariable& operator=(const ZConditionVariable&) = delete;
    ZConditionVariable& operator=(ZConditionVariable&&) = delete;

    Void LockP(TUniqueLock<ZMutex>& _mutex) noexcept;
    Void UnlockP(TUniqueLock<ZMutex>& _mutex) noexcept;

    ZCSMutex cs_mutex_;
    CONDITION_VARIABLE cv_;
    Int32 wait_thread_num_;
    Bool cv_finished_;
};

}//zengine