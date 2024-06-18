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
    A simple mutex.
*/
class ZConditionVariable : public ZObject {
public:
    CORE_DLLAPI ZConditionVariable() noexcept;
    CORE_DLLAPI ~ZConditionVariable() noexcept;

    CORE_DLLAPI NODISCARD Int32 WaitThreadNum() noexcept;
    CORE_DLLAPI NODISCARD Bool Empty() noexcept;

    CORE_DLLAPI Void Wait(TUniqueLock<ZMutex>& mutex) noexcept;
    template <typename PredicateFunction>
    Void Wait(TUniqueLock<ZMutex>& mutex, PredicateFunction func) noexcept {
        LockP(mutex);
        while (!func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, INFINITE);
        }
        UnlockP(mutex);
    }
    /*
       Wait for a certain time(ms).
    */
    CORE_DLLAPI Void WaitFor(TUniqueLock<ZMutex>& mutex, UInt32 time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename PredicateFunction>
    Void WaitFor(TUniqueLock<ZMutex>& mutex, UInt32 time, PredicateFunction func) noexcept {
        LockP(mutex);
        while (!func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, time);
        }
        UnlockP(mutex);
    }
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    CORE_DLLAPI Void WaitUntil(TUniqueLock<ZMutex>& mutex, UInt32 time) noexcept;
    /*
       Wait for a certain time(ms), use clock() to get the current time.
    */
    template <typename PredicateFunction>
    Void WaitUntil(TUniqueLock<ZMutex>& mutex, UInt32 time, PredicateFunction func) noexcept {
        time -= clock();
        LockP(mutex);
        while (!func() && !cv_finished_) {
            SleepConditionVariableCS(&cv_, &cs_mutex_.mutex_, time);
        }
        UnlockP(mutex);
    }

    CORE_DLLAPI Void NotifyOne() noexcept;
    CORE_DLLAPI Void NotifyAll() noexcept;

protected:
    using SuperType = ZObject;

private:
    CORE_DLLAPI Void LockP(TUniqueLock<ZMutex>& mutex) noexcept;
    CORE_DLLAPI Void UnlockP(TUniqueLock<ZMutex>& mutex) noexcept;

    ZCSMutex cs_mutex_;
    CONDITION_VARIABLE cv_;
    Int32 wait_thread_num_;
    Bool cv_finished_;
};

}//zengine

#endif // !Z_CORE_Z_CONDITION_VARIABLE_H_