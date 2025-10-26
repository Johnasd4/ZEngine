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

#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Locks the mutex when constructed and release when destructed.
*/
template<typename _MutexType>
class TLockGuard : public ZObject {
public:
    FORCEINLINE TLockGuard(_MutexType& _mutex) noexcept 
        : SuperType_()
        , mutex_ptr_(&_mutex) 
    { 
        mutex_ptr_->Lock(); 
    }
    FORCEINLINE TLockGuard(TLockGuard&& _lock_guard) noexcept 
        : SuperType_()
        , mutex_ptr_(_lock_guard.mutex_ptr_)
    { 
        _lock_guard.mutex_ptr_ = nullptr;
    }
    FORCEINLINE ~TLockGuard() noexcept { mutex_ptr_->Unlock(); }


    FORCEINLINE TLockGuard& operator=(TLockGuard&& _lock_guard) noexcept {
        if (mutex_ptr_ != nullptr) {
            mutex_ptr_->Unlock();
        }
        mutex_ptr_ = _lock_guard.mutex_ptr_;
        _lock_guard.mutex_ptr_ = nullptr;
        return *this;
    }

protected:
    using SuperType_ = ZObject;

private:
    TLockGuard() = delete;
    TLockGuard(const TLockGuard&) = delete;

    TLockGuard& operator=(const TLockGuard&) = delete;

    _MutexType* mutex_ptr_;
};

}//zengine