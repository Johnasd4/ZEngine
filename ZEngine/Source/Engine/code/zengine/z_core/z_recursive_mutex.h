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
#include "z_mutex.h"
#include "z_system_time.h"

namespace zengine {

/**
 * @brief A lightweight, cache-friendly recursive mutex (16 bytes) providing re-entrant locking.
 *
 * Implementation Principle:
 * This class implements recursive locking via the owner pattern by composing an underlying non-recursive
 * futex mutex (ZMutex), a recursion depth counter (count_), and an atomic owner thread identifier (owner_thread_id_).
 * When acquiring the lock, the calling thread first checks if it already owns the mutex. If re-entering, it increments
 * the recursion counter directly on an inlined fast path without atomic bus locks or kernel system calls. Otherwise,
 * it falls back to the slow path to lock the underlying ZMutex, records its thread ID as the owner, and initializes
 * the counter to 1. On release, the recursion counter is decremented; only when the counter reaches zero is the owner
 * thread ID reset and the underlying mutex released, ensuring minimal memory footprint and optimal cache locality.
 */
class CORE_DLLAPI ZRecursiveMutex {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor. Initializes the recursive mutex instance in an unlocked state.
     */
    FORCEINLINE ZRecursiveMutex() noexcept
        : mutex_()
        , count_(0U)
        , owner_thread_id_()
    {}

	/** @brief Deleted copy constructor. */
    ZRecursiveMutex(const ZRecursiveMutex&) = delete;

	/** @brief Deleted move constructor. */
    ZRecursiveMutex(ZRecursiveMutex&&) = delete;

    /**
     * @brief Destructor. Cleans up the recursive mutex instance.
     */
    FORCEINLINE ~ZRecursiveMutex() noexcept = default;

	/** @brief Deleted copy assignment operator. */
    ZRecursiveMutex& operator=(const ZRecursiveMutex&) = delete;

	/** @brief Deleted move assignment operator. */
    ZRecursiveMutex& operator=(ZRecursiveMutex&&) = delete;

    /**
     * @brief Gets current recursion depth for the calling thread.
     * @return The recursion count if locked by the calling thread; otherwise, 0.
     */
    NODISCARD FORCEINLINE UInt32 GetRecursionCount() const noexcept {
        return IsLockedByCurrentThread() ? count_ : 0U;
    }

    /**
     * @brief Checks whether the mutex is currently locked by the calling thread.
     * @return True if the mutex is owned by the calling thread, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsLockedByCurrentThread() const noexcept {
        return owner_thread_id_.Load(ZMemoryOrderEnum::kRelaxed) == ZThread::GetCurrentThreadID();
    }

    /**
     * @brief Locks the recursive mutex, blocking the calling thread until ownership is acquired.
     * Re-entrant calls by the owner thread succeed immediately on the fast path without blocking.
     */
    FORCEINLINE Void Lock() noexcept {
        const ZThread::ThreadIDType_ current_thread_id = ZThread::GetCurrentThreadID();

        // Fast Path: Re-entrant acquisition
        if (owner_thread_id_.Load(ZMemoryOrderEnum::kRelaxed) == current_thread_id) {
            ++count_;
            return;
        }

        LockSlowPathP(current_thread_id);
    }

    /**
     * @brief Tries to lock the mutex without blocking.
     * @return True if the lock was acquired or re-entered, false otherwise.
     */
    NODISCARD FORCEINLINE Bool TryLock() noexcept {
        const ZThread::ThreadIDType_ current_thread_id = ZThread::GetCurrentThreadID();

        // Fast Path: Re-entrant acquisition
        if (owner_thread_id_.Load(ZMemoryOrderEnum::kRelaxed) == current_thread_id) {
            ++count_;
            return true;
        }

        // Try to acquire the underlying mutex
        if (mutex_.TryLock()) {
            owner_thread_id_.Store(current_thread_id, ZMemoryOrderEnum::kRelaxed);
            count_ = 1U;
            return true;
        }

        return false;
    }

    /**
     * @brief Tries to lock the mutex for a specified duration.
     * @param _timeout_ms The maximum duration in milliseconds to wait.
     * @return True if acquired or re-entered, false on timeout.
     */
    NODISCARD FORCEINLINE Bool TryLockForMs(TimeType _timeout_ms) noexcept {
        return TryLockUntilMs(ZSystemTime::GetTimeMs() + _timeout_ms);
    }

    /**
     * @brief Tries to lock the mutex until the specified absolute time.
     * @param _abs_time_ms The absolute time in milliseconds (based on ZSystemTime) to wait until.
     * @return True if acquired or re-entered, false on timeout.
     */
    NODISCARD Bool TryLockUntilMs(TimeType _abs_time_ms) noexcept;

    /**
     * @brief Releases the lock held by the mutex.
     * If recursion count > 1, decrements the count; releases the underlying lock when count reaches 0.
     */
    FORCEINLINE Void Unlock() noexcept {
        ASSERT(
            IsLockedByCurrentThread() && count_ > 0, 
            "Unlock called by thread that does not own the mutex or recursion count is zero."
        );

        // Decrement recursion count (only called by the thread holding the lock)
        if (count_ > 0U) {
            --count_;
            if (count_ == 0U) {
                // Clear owner before releasing the underlying lock.
                // Sequenced-before relation ensures other threads observe this store.
                owner_thread_id_.Store(ZThread::ThreadIDType_(), ZMemoryOrderEnum::kRelaxed);
                mutex_.Unlock();
            }
        }
    }

private:
    /**
     * @brief Acquires the underlying mutex on contention or first acquisition and establishes thread ownership.
     * @param _current_thread_id The identifier of the calling thread attempting to acquire ownership.
     */
    Void LockSlowPathP(const ZThread::ThreadIDType_ _current_thread_id) noexcept;

private:
    /** @brief Underlying non-recursive futex mutex. */
    ZMutex mutex_;
    /** @brief Recursion depth counter for the owning thread. */
    UInt32 count_;
    /** @brief ID of the thread currently owning the mutex (unowned if empty/default). */
    TAtom<ZThread::ThreadIDType_> owner_thread_id_;
};

} // zengine