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

#include "t_atom.h"

namespace zengine {

/** @brief A simple mutex wrapper class providing non-recursive locking mechanisms. */
class CORE_DLLAPI ZMutex {
    NO_NEW_DELETE
public:
    /** @brief Default constructor. Initializes the mutex instance. */
    FORCEINLINE ZMutex() noexcept 
        : mutex_()
    {}

    /** @brief Deleted copy constructor. */
    ZMutex(const ZMutex&) = delete;

    /** @brief Deleted move constructor. */
    ZMutex(ZMutex&&) = delete;

    /** 
     * @brief Destructor. Cleans up the mutex instance. 
     */
    FORCEINLINE ~ZMutex() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZMutex& operator=(const ZMutex&) = delete;

    /** @brief Deleted move assignment operator. */
    ZMutex& operator=(ZMutex&&) = delete;

    /** @brief Locks the mutex, blocking the calling thread until the lock is obtained. */
    Void Lock() noexcept;

    /**
     * @brief Tries to lock the mutex without blocking.
     * @return True if the lock was acquired, false otherwise.
     */
    NODISCARD FORCEINLINE Bool TryLock() noexcept {  
        UInt32 expected = 0U;
        // Only transition to 1 if current is 0.
        return mutex_.CompareExchangeStrong(expected, 1U, ZMemoryOrderEnum::kAcquire, ZMemoryOrderEnum::kRelaxed);
    }

    /**
     * @brief Tries to lock the mutex for a specified duration.
     * This is the base implementation for timed locking.
     * @param _timeout_ms The maximum duration in milliseconds to wait for the lock.
     * @return True if the lock was acquired, false if the timeout occurred.
     */
    NODISCARD FORCEINLINE Bool TryLockForMs(TimeType _timeout_ms) noexcept {
        return TryLockUntilMs(ZSystemTime::GetTimeMs() + _timeout_ms);
    }

    /**
     * @brief Tries to lock the mutex until the specified absolute time is reached.
     * Delegates to TryLockFor.
     * @param _abs_time_ms The absolute time in milliseconds (based on ZSystemTime) to wait until.
     * @return True if the lock was acquired, false if the timeout occurred.
     */
    NODISCARD Bool TryLockUntilMs(TimeType _abs_time_ms) noexcept;

    /** @brief Releases the lock held by the mutex. */
    FORCEINLINE Void Unlock() noexcept { 
        // Release the lock (set to 0) and check previous state.
        if (mutex_.Exchange(0U, ZMemoryOrderEnum::kRelease) == 2U) {
            mutex_.NotifyOne();
        }
    }

private:
    /** @brief The underlying standard mutex used for locking. */
    TAtom<UInt32> mutex_;
};

} // zengine