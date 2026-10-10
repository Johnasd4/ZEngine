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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_semaphore.h"

namespace zengine {

Void ZSemaphore::Acquire() noexcept {
    // Load the current count loosely to start the check loop
    UInt32 old_count = count_.Load(ZMemoryOrderEnum::kRelaxed);

    // Try to grab a permit if available
    while (old_count > 0) {
        // Attempt to decrement atomically (Compare-And-Swap)
        // If successful, we acquired the semaphore.
        // If failed, old_count is updated to the current value, and we retry.
        if (
            count_.CompareExchangeWeak(
                old_count,
                old_count - 1U,
                ZMemoryOrderEnum::kAcquire,
                ZMemoryOrderEnum::kRelaxed
            )
        ) {
            return;
        }
    }

    // No permits available, prepare to wait
    wait_num_.FetchAdd(1, ZMemoryOrderEnum::kSequencedConsistent);
    old_count = count_.Load(ZMemoryOrderEnum::kRelaxed);

    while (true) {
        // Try to grab a permit if available
        if (old_count > 0) {
            // Attempt to decrement atomically (Compare-And-Swap)
            // If successful, we acquired the semaphore.
            // If failed, old_count is updated to the current value, and we retry.
            if (
                count_.CompareExchangeWeak(
                    old_count,
                    old_count - 1U,
                    ZMemoryOrderEnum::kAcquire,
                    ZMemoryOrderEnum::kRelaxed
                )
            ) {
                // Successfully acquired, decrement waiter count and return
                wait_num_.FetchSub(1, ZMemoryOrderEnum::kRelaxed);
                return;
            }
        }
        // No permits available, wait efficiently
        else {
            // Blocks the thread until the value of count_ changes from old_count (which is 0).
            count_.Wait(old_count, ZMemoryOrderEnum::kRelaxed);

            // Once woken up, reload the value to try acquiring again in the next iteration.
            old_count = count_.Load(ZMemoryOrderEnum::kSequencedConsistent);
        }
    }
}

NODISCARD Bool ZSemaphore::TryAcquire() noexcept {
    UInt32 old_count = count_.Load(ZMemoryOrderEnum::kRelaxed);
        
    // Single CAS loop to handle potential race conditions during the check
    while (old_count > 0U) {
        if (
            count_.CompareExchangeWeak(
                old_count,
                old_count - 1U,
                ZMemoryOrderEnum::kAcquire,
                ZMemoryOrderEnum::kRelaxed
            )
        ) {
            return true;
        }
        // CAS failed means value changed, old_count is updated, loop checks again
    }
        
    return false;
}

NODISCARD Bool ZSemaphore::TryAcquireUntilMs(TimeType _abs_time_ms) noexcept {
    // Fast Path: Try single non-blocking acquisition first
    if (TryAcquire()) {
        return true;
    }

    const TimeType now_time = ZSystemTime::GetTimeMs();
    if (now_time >= _abs_time_ms) {
        return false;
    }

    // Prepare to wait: Register as a waiter
    // We must ensure this is decremented on ANY return path from now on.
    wait_num_.FetchAdd(1, ZMemoryOrderEnum::kRelaxed);

    // Initial load for the CAS loop
    UInt32 old_count = count_.Load(ZMemoryOrderEnum::kRelaxed);

    // Wait Loop
    while (true) {
        const TimeType current_time = ZSystemTime::GetTimeMs();

        // Check strict timeout at the start of every iteration
        if (current_time >= _abs_time_ms) {
            wait_num_.FetchSub(1, ZMemoryOrderEnum::kRelaxed);
            return false;
        }

        // Case A: Permits appear available
        if (old_count > 0U) {
            if (
                count_.CompareExchangeWeak(
                    old_count,
                    old_count - 1U,
                    ZMemoryOrderEnum::kAcquire,
                    ZMemoryOrderEnum::kRelaxed
                )
            ) {
                // Success: Unregister waiter and return
                wait_num_.FetchSub(1, ZMemoryOrderEnum::kRelaxed);
                return true;
            }
            // CAS failed: old_count is updated automatically by CompareExchangeWeak, 
            // loop continues immediately to retry.
        }
        //Case B: No permits (old_count == 0), need to sleep
        else {
            // Calculate remaining time based on absolute deadline
            TimeType remaining_time_64 = _abs_time_ms - current_time;
            UInt32 remaining_time =
                (remaining_time_64 > 0xFFFFFFFFU) ? 0xFFFFFFFFU : static_cast<UInt32>(remaining_time_64);

            // Wait until 'count_' is no longer 0 (old_count is 0 here)
            // If WaitForMs returns false, it means actual timeout occurred
            if (!count_.WaitForMs(old_count, remaining_time, ZMemoryOrderEnum::kRelaxed)) {
                wait_num_.FetchSub(1, ZMemoryOrderEnum::kRelaxed);
                return false;
            }

            // If woken up (WaitForMs returned true), reload count and loop to try CAS again
            old_count = count_.Load(ZMemoryOrderEnum::kRelaxed);
        }
    }
}

Void ZSemaphore::Release(UInt32 _update) noexcept {
    if (_update == 0U) return;

    // Atomically add the permits.
    count_.FetchAdd(_update, ZMemoryOrderEnum::kSequencedConsistent);

    // Notify waiters.
    // If we released more than 1 permit, we should ideally wake up all/multiple waiters.
    // If just 1, NotifyOne is sufficient and more efficient.
    if (wait_num_.Load(ZMemoryOrderEnum::kSequencedConsistent) > 0) {
        if (_update > 1) {
            count_.NotifyAll();
        }
        else {
            count_.NotifyOne();
        }
    }
}

} // zengine