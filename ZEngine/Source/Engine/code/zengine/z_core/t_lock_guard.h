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

namespace zengine {

/**
 * @brief Scoped lock guard for a single mutex.
 * @details Acquires the lock upon construction and releases it upon destruction (RAII pattern).
 *          This ensures that the mutex is always unlocked, even if an exception occurs.
 * @tparam _MutexType The type of the mutex to be locked.
 */
template<typename _MutexType>
class TLockGuard {
    NO_NEW_DELETE
public:
    /**
     * @brief Constructs the lock guard and acquires the lock.
     * @param _mutex The mutex to lock. It is accepted as const for convenience but cast to non-const internally.
     */
    FORCEINLINE TLockGuard(const _MutexType& _mutex) noexcept 
        : mutex_(const_cast<_MutexType&>(_mutex))
    { 
        mutex_.Lock();
    }

    /** @brief Deleted copy constructor. */
    TLockGuard(const TLockGuard&) = delete;

    /** @brief Deleted move constructor. */
    TLockGuard(TLockGuard&&) = delete;

    /** @brief Destroys the lock guard and releases the lock. */
    FORCEINLINE ~TLockGuard() noexcept { mutex_.Unlock(); }

    /** @brief Deleted copy assignment operator. */
    TLockGuard& operator=(const TLockGuard&) = delete;

    /** @brief Deleted move assignment operator. */
    TLockGuard& operator=(TLockGuard&&) = delete;

private:
    /** @brief Reference to the managed mutex. */
    _MutexType& mutex_;
};

/**
 * @brief Scoped lock guard for two mutexes.
 * @details Acquires two locks simultaneously upon construction using a deadlock-avoidance strategy,
 *          and releases both upon destruction.
 * @tparam _MutexType1 The type of the first mutex.
 * @tparam _MutexType2 The type of the second mutex.
 */
template<typename _MutexType1, typename _MutexType2>
class T2LockGuard {
    NO_NEW_DELETE
public:
    /**
     * @brief Constructs the lock guard and acquires both locks safely.
     * @param _mutex_1 The first mutex to lock.
     * @param _mutex_2 The second mutex to lock.
     */
    FORCEINLINE T2LockGuard(const _MutexType1& _mutex_1, const _MutexType2& _mutex_2) noexcept
        : mutex_1_(const_cast<_MutexType1&>(_mutex_1))
        , mutex_2_(const_cast<_MutexType2&>(_mutex_2))
    {
        Lock(mutex_1_, mutex_2_);
    }

    /** @brief Deleted copy constructor. */
    T2LockGuard(const T2LockGuard&) = delete;

    /** @brief Deleted move constructor. */
    T2LockGuard(T2LockGuard&&) = delete;

    /** @brief Destroys the lock guard and releases both locks. */
    FORCEINLINE ~T2LockGuard() noexcept { 
        Unlock(mutex_1_, mutex_2_);
    }

    /** @brief Deleted copy assignment operator. */
    T2LockGuard& operator=(const T2LockGuard&) = delete;

    /** @brief Deleted move assignment operator. */
    T2LockGuard& operator=(T2LockGuard&&) = delete;

private:
    /** @brief Reference to the first managed mutex. */
    _MutexType1& mutex_1_;
    /** @brief Reference to the second managed mutex. */
    _MutexType2& mutex_2_;
};

} // zengine