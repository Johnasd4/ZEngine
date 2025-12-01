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

#include "z_object.h"

namespace zengine {

/*
    A semaphore mutex.
*/
class ZSemMutex : public ZObject {
public:
    FORCEINLINE ZSemMutex(Int32 _max_count = 1) noexcept
        : SuperType_()
        , handle_(CreateSemaphore(nullptr, _max_count, _max_count, nullptr))
    {}
    FORCEINLINE ZSemMutex(ZSemMutex&& _mutex) noexcept : SuperType_(std::forward<ZSemMutex>(_mutex)) {
        MoveP(std::forward<ZSemMutex>(_mutex));
    }
    FORCEINLINE ~ZSemMutex() noexcept { CloseHandle(handle_); }

    FORCEINLINE ZSemMutex& operator=(ZSemMutex&& _mutex) noexcept {
        SuperType_::operator=(std::forward<ZSemMutex>(_mutex));
        MoveP(std::forward<ZSemMutex>(_mutex));
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
    FORCEINLINE Bool TryLockFor(TimeType _time) noexcept { 
        return WaitForSingleObject(handle_, static_cast<DWORD>(_time)) == WAIT_OBJECT_0;
    }
    /*
        Try to get the lock before a certain time(ms), use clock() to get the current time, return true if success.
    */
    FORCEINLINE Bool TryLockUntil(TimeType _time) noexcept {
        _time -= clock();
        return WaitForSingleObject(handle_, static_cast<DWORD>(_time > 0LL ? _time : 0LL)) == WAIT_OBJECT_0;
    }
    FORCEINLINE Void Unlock() noexcept { ReleaseSemaphore(handle_, 1LL, NULL); }
protected:
    using SuperType_ = ZObject;

private:
    ZSemMutex(const ZSemMutex&) = delete;
    ZSemMutex& operator=(const ZSemMutex&) = delete;

    FORCEINLINE Void MoveP(ZSemMutex&& _mutex) noexcept {
        handle_ = _mutex.handle_;
        _mutex.handle_ = nullptr;
    }

    Handle handle_;
};

}//zengine