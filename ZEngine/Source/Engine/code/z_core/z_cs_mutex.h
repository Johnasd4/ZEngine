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
#ifndef Z_CORE_Z_CRITICAL_SECTION_H_
#define Z_CORE_Z_CRITICAL_SECTION_H_

#include "internal/z_drive.h"

#include "z_object.h"

namespace zengine {

/*
    Critical section class.
*/
class ZCSMutex : public ZObject {
public:
    FORCEINLINE ZCSMutex() noexcept : SuperType_() { InitializeCriticalSection(&mutex_); }
    FORCEINLINE ~ZCSMutex() noexcept { DeleteCriticalSection(&mutex_); }

    FORCEINLINE Void Lock() noexcept { EnterCriticalSection(&mutex_); }

    FORCEINLINE Void Unlock() noexcept { LeaveCriticalSection(&mutex_); }

protected:
    using SuperType_ = ZObject;

private:
    friend class ZConditionVariable;

    ZCSMutex(const ZCSMutex&) = delete;
    ZCSMutex(ZCSMutex&&) = delete;

    ZCSMutex& operator=(const ZCSMutex&) = delete;
    ZCSMutex& operator=(ZCSMutex&&) = delete;

    CRITICAL_SECTION mutex_;
};

}//zengine

#endif // !Z_CORE_Z_CRITICAL_SECTION_H_
