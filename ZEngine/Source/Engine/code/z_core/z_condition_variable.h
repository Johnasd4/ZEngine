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
#ifndef Z_CORE_Z_CONDITION_VARIABLE_H_
#define Z_CORE_Z_CONDITION_VARIABLE_H_

#include "internal/z_drive.h"

#include "t_stack.h"
#include "t_unique_lock.h"
#include "z_cs_mutex.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_thread.h"

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
    Void WaitFor(TUniqueLock<ZMutex>& _mutex, UInt32 _time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename _PredicateFunction>
    Void WaitFor(TUniqueLock<ZMutex>& _mutex, UInt32 _time, _PredicateFunction _func) noexcept {
        LockP(_mutex);
        while (!_func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, _time);
        }
        UnlockP(_mutex);
    }
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    Void WaitUntil(TUniqueLock<ZMutex>& _mutex, UInt32 _time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename _PredicateFunction>
    Void WaitUntil(TUniqueLock<ZMutex>& _mutex, UInt32 _time, _PredicateFunction _func) noexcept {
        _time -= clock();
        LockP(_mutex);
        while (!_func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, _time);
        }
        UnlockP(_mutex);
    }

    Void NotifyOne() noexcept;
    Void NotifyAll() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    Void LockP(TUniqueLock<ZMutex>& _mutex) noexcept;
    Void UnlockP(TUniqueLock<ZMutex>& _mutex) noexcept;

    ZCSMutex cs_mutex_;
    CONDITION_VARIABLE cv_;
    Int32 wait_thread_num_;
    Bool cv_finished_;
};

}//zengine

#endif // !Z_CORE_Z_CONDITION_VARIABLE_H_