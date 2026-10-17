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
#include "t_unique_lock.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A condition variable synchronization primitive based on atomic operations.
 *
 * Implementation Principle:
 * This condition variable implements thread waiting and signaling mechanisms using atomic primitives
 * rather than relying directly on heavyweight OS-specific condition variable structures.
 * It coordinates synchronization using two atomic variables:
 * - signal_: Acts as a notification version/generation counter.
 * - wait_num_: Tracks the number of active waiting threads.
 *
 * When a thread waits, it increments wait_num_, samples the current signal_ value, releases the user-supplied
 * lock, and executes an atomic wait operation on signal_ (e.g., futex wait or equivalent platform primitive).
 * Upon waking, it reacquires the lock and decrements wait_num_.
 * Notification operations (NotifyOne and NotifyAll) check whether wait_num_ is non-zero, update signal_,
 * and wake up sleeping threads via atomic wake operations.
 */
class CORE_DLLAPI ZConditionVariable : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor.
     *
     * Initializes the signal counter and waiting thread counter to zero.
     */
    FORCEINLINE ZConditionVariable() noexcept
        : SuperType_()
        , signal_(0U)
        , wait_num_(0U)
    {}

    ZConditionVariable(const ZConditionVariable&) = delete;
    ZConditionVariable(ZConditionVariable&&) = delete;

    /**
     * @brief Default destructor.
     */
    FORCEINLINE ~ZConditionVariable() noexcept = default;

    ZConditionVariable& operator=(const ZConditionVariable&) = delete;
    ZConditionVariable& operator=(ZConditionVariable&&) = delete;

    /**
     * @brief Gets the number of threads currently waiting on this condition variable.
     *
     * @return The number of waiting threads.
     */
    NODISCARD FORCEINLINE UInt32 WaitThreadNum() noexcept {
        return wait_num_.Load(ZMemoryOrderEnum::kRelaxed);
    }

    /**
     * @brief Checks if there are any threads waiting on this condition variable.
     *
     * @return True if the waiter count is zero, false otherwise.
     */
    NODISCARD FORCEINLINE Bool Empty() noexcept {
        return wait_num_.Load(ZMemoryOrderEnum::kRelaxed) == 0U;
    }

    /**
     * @brief Waits for the condition variable to be notified.
     *
     * Atomically releases the lock, blocks the current thread, and adds it to the list of
     * threads waiting on this condition variable. The thread unblocks when notified.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @param _lock A unique lock controlling the mutex.
     */
    template<typename _MutexType>
    Void Wait(TUniqueLock<_MutexType>& _lock) noexcept {
        wait_num_.FetchAdd(1U, ZMemoryOrderEnum::kSequencedConsistent);
        UInt32 pre_signal_val = signal_.Load(ZMemoryOrderEnum::kAcquire);
        _lock.Unlock();
        signal_.Wait(pre_signal_val, ZMemoryOrderEnum::kAcquire);
        _lock.Lock();
        wait_num_.FetchSub(1U, ZMemoryOrderEnum::kRelaxed);
    }

    /**
     * @brief Waits for the condition variable to be notified or for the predicate to become true.
     *
     * This method handles spurious wakeups by testing the predicate in a loop.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @tparam PredicateFunction The type of the predicate function.
     * @param _lock A unique lock controlling the mutex.
     * @param _func A callable object that returns a boolean indicating whether the condition is met.
     */
    template <typename _MutexType, typename PredicateFunction>
    Void Wait(
        TUniqueLock<_MutexType>& _lock, 
        PredicateFunction&& _func
    ) noexcept {
        if (_func()) {
            return;
        }

        wait_num_.FetchAdd(1U, ZMemoryOrderEnum::kSequencedConsistent);
        while (!_func()) {
            UInt32 pre_signal_val = signal_.Load(ZMemoryOrderEnum::kAcquire);
            _lock.Unlock();
            signal_.Wait(pre_signal_val, ZMemoryOrderEnum::kAcquire);
            _lock.Lock();
        }
        wait_num_.FetchSub(1U, ZMemoryOrderEnum::kRelaxed);
    }

    /**
     * @brief Waits for the condition variable to be notified, or until the timeout expires.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @param _lock A unique lock controlling the mutex.
     * @param _timeout_ms The maximum relative duration to wait, in milliseconds.
     * @return True if notified before the timeout expired, false on timeout.
     */
    template<typename _MutexType>
    NODISCARD FORCEINLINE Bool WaitForMs(
        TUniqueLock<_MutexType>& _lock,
        TimeType _timeout_ms
    ) noexcept {
        return WaitUntilMs(_lock, ZSystemTime::GetSteadyTimeMs() + _timeout_ms);
    }

    /**
     * @brief Waits for the condition variable or for the predicate to become true, or until timeout.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @tparam PredicateFunction The type of the predicate function.
     * @param _lock A unique lock controlling the mutex.
     * @param _timeout_ms The maximum relative duration to wait, in milliseconds.
     * @param _func A callable object that returns a boolean indicating whether the condition is met.
     * @return True if the predicate evaluated to true, false if timeout occurred before predicate became true.
     */
    template <typename _MutexType, typename PredicateFunction>
    NODISCARD FORCEINLINE Bool WaitForMs(
        TUniqueLock<_MutexType>& _lock,
        TimeType _timeout_ms,
        PredicateFunction&& _func
    ) noexcept {
        return WaitUntilMs(
            _lock, ZSystemTime::GetSteadyTimeMs() + _timeout_ms, std::forward<PredicateFunction>(_func));
    }

    /**
     * @brief Waits until the absolute time point is reached.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @param _lock A unique lock controlling the mutex.
     * @param _abs_time_ms The absolute time point to wait until. 
     * Call ZSystemTime::GetSteadyTimeMs() to get current time.
     * @return True if notified before the absolute time point was reached, false on timeout.
     */
    template<typename _MutexType>
    NODISCARD Bool WaitUntilMs(
        TUniqueLock<_MutexType>& _lock,
        TimeType _abs_time_ms
    ) noexcept {
        const TimeType now_time = ZSystemTime::GetSteadyTimeMs();
        if (now_time >= _abs_time_ms) {
            return false;
        }

        wait_num_.FetchAdd(1U, ZMemoryOrderEnum::kSequencedConsistent);

        // Calculate remaining time for the underlying system wait
        TimeType remaining_time_64 = _abs_time_ms - now_time;
        // Clamp to UInt32 if necessary (assuming underlying WaitForMs takes UInt32)
        UInt32 wait_time = (remaining_time_64 > 0xFFFFFFFFU) ? 0xFFFFFFFFU : static_cast<UInt32>(remaining_time_64);

        UInt32 pre_signal_val = signal_.Load(ZMemoryOrderEnum::kAcquire);

        _lock.Unlock();
        // Wait returns false on timeout, true on wakeup
        Bool result = signal_.WaitForMs(pre_signal_val, wait_time, ZMemoryOrderEnum::kAcquire);
        _lock.Lock();

        wait_num_.FetchSub(1U, ZMemoryOrderEnum::kRelaxed);

        return result;
    }

    /**
     * @brief Waits until the absolute time point is reached or predicate becomes true.
     *
     * @tparam _MutexType The type of the mutex being locked.
     * @tparam PredicateFunction The type of the predicate function.
     * @param _lock A unique lock controlling the mutex.
     * @param _abs_time_ms The absolute time point to wait until.
     * Call ZSystemTime::GetSteadyTimeMs() to get current time.
     * @param _func A callable object that returns a boolean indicating whether the condition is met.
     * @return True if the predicate evaluated to true, false if timeout occurred before predicate became true.
     */
    template <typename _MutexType, typename PredicateFunction>
    NODISCARD Bool WaitUntilMs(
        TUniqueLock<_MutexType>& _lock,
        TimeType _abs_time_ms,
        PredicateFunction&& _func
    ) noexcept {
        // Quick check
        if (_func()) {
            return true;
        }

        wait_num_.FetchAdd(1U, ZMemoryOrderEnum::kSequencedConsistent);

        while (!_func()) {
            const TimeType now_time = ZSystemTime::GetSteadyTimeMs();

            // Check absolute timeout
            if (now_time >= _abs_time_ms) {
                // IMPORTANT: We must check predicate one last time.
                // Standard dictates: return predicate state at timeout.
                bool result = _func();
                wait_num_.FetchSub(1U, ZMemoryOrderEnum::kRelaxed);
                return result;
            }

            // Calculate remaining time
            TimeType remaining_time_64 = _abs_time_ms - now_time;
            UInt32 remaining_time =
                (remaining_time_64 > 0xFFFFFFFFU) ? 0xFFFFFFFFU : static_cast<UInt32>(remaining_time_64);

            UInt32 pre_signal_val = signal_.Load(ZMemoryOrderEnum::kAcquire);

            _lock.Unlock();
            signal_.WaitForMs(pre_signal_val, remaining_time, ZMemoryOrderEnum::kAcquire);
            _lock.Lock();
        }

        // Predicate is true
        wait_num_.FetchSub(1U, ZMemoryOrderEnum::kRelaxed);
        return true;
    }

    /**
     * @brief Unblocks one of the threads waiting on this condition variable.
     *
     * If no threads are waiting, this function does nothing.
     */
    Void NotifyOne() noexcept;

    /**
     * @brief Unblocks all threads currently waiting on this condition variable.
     *
     * If no threads are waiting, this function does nothing.
     */
    Void NotifyAll() noexcept;

private:
    /** @brief Atomic variable used for signaling state changes to waiters. */
    TAtom<UInt32> signal_;
    /** @brief Atomic variable tracking the count of waiting threads. */
    TAtom<UInt32> wait_num_;
};

} // zengine