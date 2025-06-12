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

#include "internal/z_drive.h"

#include "z_object.h"

namespace zengine {

/*
    A simple mutex.
*/
class CORE_DLLAPI ZMutex : public ZObject {
public:
    FORCEINLINE ZMutex() noexcept : SuperType_(), handle_(CreateMutex(nullptr, FALSE, nullptr)) {}
    FORCEINLINE ZMutex(ZMutex&& _mutex) noexcept : SuperType_(std::forward<ZMutex>(_mutex)) { 
        MoveP(std::forward<ZMutex>(_mutex)); 
    }
    FORCEINLINE ~ZMutex() noexcept { CloseHandle(handle_); }

    FORCEINLINE ZMutex& operator=(ZMutex&& _mutex) noexcept {
        MoveP(std::forward<ZMutex>(_mutex));
        return *this;
    }

    FORCEINLINE Void Lock() noexcept { WaitForSingleObject(handle_, INFINITE);}
    /*
        Try to get the lock, return true if success.
    */
    FORCEINLINE Bool TryLock() noexcept { return WaitForSingleObject(handle_, 0) == WAIT_OBJECT_0; }
    /*
        Try to get the lock in a certain time(ms), return true if success.
    */
    FORCEINLINE Bool TryLockFor(UInt32 _time) noexcept { return WaitForSingleObject(handle_, _time) == WAIT_OBJECT_0; }
    /*
        Try to get the lock before a certain time(ms), use clock() to get the current time, return true if success.
    */
    FORCEINLINE Bool TryLockUntil(UInt32 _time) noexcept { 
        _time -= clock();
        return WaitForSingleObject(handle_, _time > 0 ? _time : 0) == WAIT_OBJECT_0; 
    }
    FORCEINLINE Void Unlock() noexcept { ReleaseMutex(handle_); }
protected:
    using SuperType_ = ZObject;

private:
    ZMutex(const ZMutex&) = delete;
    ZMutex& operator=(const ZMutex&) = delete;

    FORCEINLINE Void MoveP(ZMutex&& _mutex) noexcept {
        handle_ = _mutex.handle_;
        _mutex.handle_ = nullptr;
    }

    Handle handle_;
};

}//zengine
