/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_Z_MUTEX_H_
#define Z_CORE_Z_MUTEX_H_

#include "internal/drive.h"

#include "z_object.h"

namespace zengine {

/*
    A simple mutex.
*/
class ZMutex : public ZObject {
public:
    FORCEINLINE ZMutex() noexcept : SuperType(), handle_(CreateMutex(nullptr, FALSE, nullptr)) {}
    FORCEINLINE ~ZMutex() noexcept { CloseHandle(handle_); }

    FORCEINLINE Void Lock() noexcept { WaitForSingleObject(handle_, INFINITE); }
    /*
        Try to get the lock, return true if success.
    */
    FORCEINLINE Bool TryLock() noexcept { return WaitForSingleObject(handle_, 0) == WAIT_OBJECT_0; }
    /*
        Try to get the lock in a certain time(ms), return true if success.
    */
    FORCEINLINE Bool TryLockFor(UInt32 time) noexcept { return WaitForSingleObject(handle_, time) == WAIT_OBJECT_0; }
    /*
        Try to get the lock before a certain time(ms), use clock() to get the current time, return true if success.
    */
    FORCEINLINE Bool TryLockUntil(UInt32 time) noexcept { 
        time -= clock();
        return WaitForSingleObject(handle_, time > 0 ? time : 0) == WAIT_OBJECT_0; 
    }
    FORCEINLINE Void Unlock() noexcept { ReleaseMutex(handle_); }
protected:
    using SuperType = ZObject;

private:
    ZMutex(const ZMutex&) = delete;
    ZMutex(ZMutex&&) = delete;

    ZMutex& operator=(const ZMutex&) = delete;
    ZMutex& operator=(ZMutex&&) = delete;

    Handle handle_;
};

}//zengine

#endif // !Z_CORE_Z_MUTEX_H_
