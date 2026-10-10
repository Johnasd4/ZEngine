/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "m_log.h"
#include "z_object.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration for TUniqueLock error codes.
 *
 * Defines specific error codes used within the TUniqueLock class for debugging and logging purposes.
 */
enum TUniqueLockErrorCodeEnum : ReturnType::ValueType {
    /** @brief Base error code linking to TUniqueLock errors. */
    kTUniqueLockErrorCode_LinkError = kErrorCodeBase_TUniqueLock,
    /** @brief Error indicating a system or external library failure. */
    kTUniqueLockErrorCode_SystemOrLibraryError,
    /** @brief Error indicating a null pointer parameter was passed. */
    kTUniqueLockErrorCode_NullptrParam,
    /** @brief Error indicating a parameter is out of the valid range. */
    kTUniqueLockErrorCode_ParamOutOfRange
};
} // error_code
} // zengine

namespace zengine {

/**
 * @brief Unique lock class, controls the mutex lifecycle via RAII mechanism.
 *
 * This class acts as a wrapper for a mutex, providing exclusive ownership semantics.
 * It automatically manages the locking and unlocking of the underlying mutex.
 * When the object is destroyed, it ensures the mutex is unlocked if it is still owned.
 * This class is movable but not copyable.
 *
 * @tparam _MutexType The type of the mutex to be managed (e.g., ZMutex, std::mutex).
 */
template<typename _MutexType>
class TUniqueLock : public ZObject {
public:
    /** @brief Type definition for the base class. */
    using SuperType_ = ZObject;

private:
    /** @brief Internal tag type for defer-locking construction. */
    struct NoLockType_ { Int32 value_; };
    /** @brief Internal tag type for try-locking construction. */
    struct TryLockType_ { Int32 value_; };

public:
    /** @brief Constant tag object for defer-locking. */
    static inline constexpr NoLockType_ kNoLock = { 0 };
    /** @brief Constant tag object for try-locking. */
    static inline constexpr TryLockType_ kTryLock = { 0 };

    /**
     * @brief Default constructor.
     *
     * Constructs a TUniqueLock with no associated mutex.
     */
    TUniqueLock() noexcept : SuperType_(), mutex_ptr_(nullptr), owns_lock_(false) {}

    /** @brief Deleted copy constructor. */
    TUniqueLock(const TUniqueLock&) = delete;

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the mutex and its lock state from another TUniqueLock object.
     *
     * @param _unique_lock The TUniqueLock object to move from.
     */
    TUniqueLock(TUniqueLock&& _unique_lock) noexcept : SuperType_(std::move(_unique_lock)) {
        MoveP(std::move(_unique_lock));
    }

    /**
     * @brief Locking constructor.
     *
     * Associates with the given mutex and immediately locks it.
     *
     * @param _mutex The mutex to manage and lock.
     */
    TUniqueLock(_MutexType& _mutex) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        mutex_ptr_->Lock();
        owns_lock_ = true;
    }

    /**
     * @brief Defer-lock constructor.
     *
     * Associates with the given mutex but does not lock it.
     *
     * @param _mutex The mutex to manage.
     * @param _value Tag parameter to indicate no lock operation.
     */
    explicit TUniqueLock(_MutexType& _mutex, NoLockType_ _value) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        owns_lock_ = false;
    }

    /**
     * @brief Try-lock constructor.
     *
     * Associates with the given mutex and attempts to lock it immediately.
     *
     * @param _mutex The mutex to manage.
     * @param _value Tag parameter to indicate try-lock operation.
     */
    explicit TUniqueLock(_MutexType& _mutex, TryLockType_ _value) noexcept : SuperType_(), mutex_ptr_(&_mutex) {
        owns_lock_ = mutex_ptr_->TryLock();
    }

    /**
     * @brief Destructor.
     *
     * Unlocks the associated mutex if it is currently owned by this object.
     */
    ~TUniqueLock() noexcept {
        if (owns_lock_ == true) {
            mutex_ptr_->Unlock();
        }
    }

    /** @brief Deleted copy assignment operator. */
    const TUniqueLock& operator=(const TUniqueLock&) = delete;

    /**
     * @brief Move assignment operator.
     *
     * If the current object owns a lock, it is unlocked before taking ownership from the source.
     *
     * @param _unique_lock The TUniqueLock object to move from.
     * @return TUniqueLock& Reference to this object.
     */
    TUniqueLock& operator=(TUniqueLock&& _unique_lock) noexcept {
        if (this == &_unique_lock) {
            return *this;
        }
        if (owns_lock_ && mutex_ptr_) {
            mutex_ptr_->Unlock();
        }
        SuperType_::operator=(std::move(_unique_lock));
        MoveP(std::move(_unique_lock));
        return *this;
    }

    /**
     * @brief Gets the pointer to the associated mutex.
     *
     * @return _MutexType* Pointer to the managed mutex, or nullptr if none.
     */
    NODISCARD FORCEINLINE _MutexType* MutexPtr() noexcept { return mutex_ptr_; }

    /**
     * @brief Checks if this object currently owns the lock.
     *
     * @return Bool True if the mutex is locked by this object, false otherwise.
     */
    NODISCARD FORCEINLINE Bool OwnsLock() noexcept { return owns_lock_; }

    /**
     * @brief Locks the associated mutex.
     *
     * Validates the state before locking. Logs an error if the mutex is null or already owned.
     */
    Void Lock() noexcept {
        if (!mutex_ptr_) {
            ASSERT(false, "TUniqueLock: mutex_ptr_ is nullptr!");
            return;
        }
        if (owns_lock_) {
            ASSERT(false, "TUniqueLock: already owns the mutex!");
            return;
        }
        mutex_ptr_->Lock(); 
        owns_lock_ = true;
    }

    /**
     * @brief Attempts to lock the associated mutex without blocking.
     *
     * Validates the state before trying to lock.
     *
     * @return Bool True if the lock was acquired, false if failed or if an error occurred.
     */
    NODISCARD Bool TryLock() noexcept {
        if (!mutex_ptr_) {
            ASSERT(false, "TUniqueLock: mutex_ptr_ is nullptr!");
            return false;
        }
        if (owns_lock_) {
            ASSERT(false, "TUniqueLock: already owns the mutex!");
            return false;
        }
        owns_lock_ = mutex_ptr_->TryLock();
        return owns_lock_;
    }

    /**
     * @brief Unlocks the associated mutex.
     *
     * Validates the state before unlocking. Logs an error if the mutex is null or not currently owned.
     */
    Void Unlock() noexcept {
        if (!mutex_ptr_) {
            ASSERT(false, "TUniqueLock: mutex_ptr_ is nullptr!");
            return;
        }
        if (!owns_lock_) {
            ASSERT(false, "TUniqueLock: doesn't owns the mutex!");
            return;
        }
        mutex_ptr_->Unlock();
        owns_lock_ = false;
    }

    /**
     * @brief Swaps the state with another TUniqueLock object.
     *
     * @param _unique_lock The other TUniqueLock object to swap with.
     */
    Void Swap(TUniqueLock& _unique_lock) noexcept {
        _MutexType* temp_mutex_ptr_ = mutex_ptr_;
        Bool temp_own = owns_lock_;
        mutex_ptr_ = _unique_lock.mutex_ptr_;
        owns_lock_ = _unique_lock.owns_lock_;
        _unique_lock.mutex_ptr_ = temp_mutex_ptr_;
        _unique_lock.owns_lock_ = temp_own;
    }

    /**
     * @brief Releases the association with the mutex without unlocking it.
     *
     * The caller becomes responsible for unlocking the returned mutex.
     *
     * @return _MutexType* Pointer to the previously managed mutex.
     */
    _MutexType* Release() noexcept {
        _MutexType* temp_mutex_ptr_ = mutex_ptr_;
        mutex_ptr_ = nullptr;
        owns_lock_ = false;
        return temp_mutex_ptr_;
    }

private:
    /**
     * @brief Helper function to implement move semantics.
     *
     * Moves the mutex pointer and ownership status from the source object,
     * resetting the source object to a default state.
     *
     * @param _unique_lock The source object to move from.
     */
    FORCEINLINE Void MoveP(TUniqueLock&& _unique_lock) noexcept {
        mutex_ptr_ = _unique_lock.mutex_ptr_;
        owns_lock_ = _unique_lock.owns_lock_;
        _unique_lock.mutex_ptr_ = nullptr;
        _unique_lock.owns_lock_ = false;
    }

private:
    /** @brief Pointer to the managed mutex. */
    _MutexType* mutex_ptr_;
    /** @brief Flag indicating whether this object currently owns the lock. */
    Bool owns_lock_;
};

} // zengine