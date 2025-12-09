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

#include "m_log.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum TUniqueLockErrorCodeEnum : ReturnType {
    kTUniqueLockErrorCode_LinkError = kErrorCodeBase_TUniqueLock,
    kTUniqueLockErrorCode_SystemError,
    kTUniqueLockErrorCode_NullptrParam,
    kTUniqueLockErrorCode_ParamOutOfRange,
    kTUniqueLockErrorCode_MutexNotExist,
    kTUniqueLockErrorCode_MutexAlreadyOwn,
    kTUniqueLockErrorCode_MutexDoNotOwn
};
}//error_code
}//zengine

namespace zengine {

/*
    Unique lock class, controls the mutex.
*/
template<typename _MutexType>
class TUniqueLock : public ZObject {
private:
    struct NoLockType_ { Int32 value_; };
    struct TryLockType_ { Int32 value_; };
    struct TryLockForType_ { Int32 value_; };
    struct TryLockUntilType_ { Int32 value_; };

public:
    static inline constexpr NoLockType_ kNoLock = { 0 };
    static inline constexpr TryLockType_ kTryLock = { 0 };
    static inline constexpr TryLockForType_ kTryLockFor = { 0 };
    static inline constexpr TryLockUntilType_ kTryLockUntil = { 0 };

    TUniqueLock() noexcept : SuperType_(), mutex_ptr_(nullptr), owns_lock_(false) {}
    TUniqueLock(TUniqueLock&& _unique_lock) noexcept : SuperType_(std::forward<TUniqueLock>(_unique_lock)) {
        MoveP(std::forward<TUniqueLock>(_unique_lock));
    }
    /*
        Locks the mutex.
    */
    TUniqueLock(_MutexType& _mutex) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        mutex_ptr_->Lock();
        owns_lock_ = true;
    }
    /*
        Does not lock the mutex.
    */
    explicit TUniqueLock(_MutexType& _mutex, NoLockType_ _value) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        owns_lock_ = false;
    }
    /*
        Try to get the lock.
    */
    explicit TUniqueLock(_MutexType& _mutex, TryLockType_ _value) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        owns_lock_ = mutex_ptr_->TryLock();
    }
    /*
         Try to get the lock in a certain time(ms).
    */
    explicit TUniqueLock(_MutexType& _mutex, TryLockForType_ _value, TimeType _time) noexcept
        : SuperType_(), mutex_ptr_(&_mutex) 
    {
        owns_lock_ = mutex_ptr_->TryLockFor(_time);
    }
    /*
         Try to get the lock before a certain time(ms), use clock() to get the current time.
    */
    explicit TUniqueLock(_MutexType& _mutex, TryLockUntilType_ _value, TimeType _time) noexcept
        : SuperType_(), mutex_ptr_(&_mutex) 
    {
        owns_lock_ = mutex_ptr_->TryLockUntil(_time);
    }

    /*
        Unlocks the mutex if owns.
    */
    ~TUniqueLock() noexcept {
        if (owns_lock_ == true) {
            mutex_ptr_->Unlock();
        }
    }

    TUniqueLock& operator=(TUniqueLock&& _unique_lock) noexcept {
        if (this == &_unique_lock) {
            return *this;
        }
        SuperType_::operator=(std::forward<TUniqueLock>(_unique_lock));
        MoveP(std::forward<TUniqueLock>(_unique_lock));
        return *this;
    }

    NODISCARD FORCEINLINE _MutexType* MutexPtr() noexcept { return mutex_ptr_; }
    NODISCARD FORCEINLINE Bool OwnsLock() noexcept { return owns_lock_; }

    Void Lock() noexcept { 
        ReturnType link_code = LockValidCheckP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kTUniqueLockErrorCode_LinkError, link_code, "TUniqueLock::LockValidCheckP() link error!");
            return;
        }
        mutex_ptr_->Lock(); 
        owns_lock_ = true;
    }
    /*
        Try to get the lock, return true if success.
    */
    NODISCARD Bool TryLock() noexcept { 
        ReturnType link_code = LockValidCheckP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kTUniqueLockErrorCode_LinkError, link_code, "TUniqueLock::LockValidCheckP() link error!");
            return false;
        }
        owns_lock_ = mutex_ptr_->TryLock();
        return owns_lock_;
    }
    /*
        Try to get the lock in a certain time(ms), return true if success.
    */
    NODISCARD Bool TryLockFor(TimeType _time) noexcept {
        ReturnType link_code = LockValidCheckP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kTUniqueLockErrorCode_LinkError, link_code, "TUniqueLock::LockValidCheckP() link error!");
            return false;
        }
        owns_lock_ = mutex_ptr_->TryLockFor(static_cast<UInt32>(_time));
        return owns_lock_;
    }
    /*
        Try to get the lock before a certain time(ms), use clock() to get the current time, return true if success.
    */
    NODISCARD Bool TryLockUntil(TimeType _time) noexcept {
        ReturnType link_code = LockValidCheckP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kTUniqueLockErrorCode_LinkError, link_code, "TUniqueLock::LockValidCheckP() link error!");
            return false;
        }
        owns_lock_ = mutex_ptr_->TryLockUntil(static_cast<UInt32>(_time));
        return owns_lock_;
    }
    Void Unlock() noexcept { 
        ReturnType link_code = UnlockValidCheckP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kTUniqueLockErrorCode_LinkError, link_code, "TUniqueLock::UnlockValidCheckP() link error!");
            return;
        }
        mutex_ptr_->Unlock();
        owns_lock_ = false;
    }

    Void Swap(TUniqueLock& _unique_lock) noexcept {
        _MutexType* temp_mutex_ptr_ = mutex_ptr_;
        Bool temp_own = owns_lock_;
        mutex_ptr_ = _unique_lock.mutex_ptr_;
        owns_lock_ = _unique_lock.owns_lock_;
        _unique_lock.mutex_ptr_ = temp_mutex_ptr_;
        _unique_lock.owns_lock_ = temp_own;
    }

    _MutexType* Release() noexcept {
        _MutexType* temp_mutex_ptr_ = mutex_ptr_;
        mutex_ptr_ = nullptr;
        owns_lock_ = false;
        return temp_mutex_ptr_;
    }

protected:
    using SuperType_ = ZObject;

private:
    TUniqueLock(const TUniqueLock&) = delete;
    const TUniqueLock& operator=(const TUniqueLock&) = delete;

    FORCEINLINE Void MoveP(TUniqueLock&& _unique_lock) noexcept {
        mutex_ptr_ = _unique_lock.mutex_ptr_;
        owns_lock_ = _unique_lock.owns_lock_;
        _unique_lock.mutex_ptr_ = nullptr;
        _unique_lock.owns_lock_ = true;
    }

    /*
        Checks if the mutex is able to lock.
    */
    NODISCARD ReturnType LockValidCheckP() noexcept {
        ReturnType ret_val = kOK;
        if (!mutex_ptr_) {
            ret_val = error_code::kTUniqueLockErrorCode_MutexNotExist;
            Z_LOG_ERROR(ret_val, 0, "Mutex pointer is null!");
            return false;
        }
        if (owns_lock_) {
            ret_val = error_code::kTUniqueLockErrorCode_MutexAlreadyOwn;
            Z_LOG_ERROR(ret_val, 0, "Already owns the mutex!");
            return false;
        }
        return ret_val;
    }

    /*
        Checks if the mutex is able to unlock.
    */
    NODISCARD ReturnType UnlockValidCheckP() noexcept {
        ReturnType ret_val = kOK;
        if (!mutex_ptr_) {
            ret_val = error_code::kTUniqueLockErrorCode_MutexNotExist;
            Z_LOG_ERROR(ret_val, 0, "Mutex pointer is null!");
            return false;
        }
        if (!owns_lock_) {
            ret_val = error_code::kTUniqueLockErrorCode_MutexDoNotOwn;
            Z_LOG_ERROR(ret_val, 0, "Doesn't owns the mutex!");
            return false;
        }
        return ret_val;
    }

    _MutexType* mutex_ptr_;
    Bool owns_lock_;
};

}//zengine