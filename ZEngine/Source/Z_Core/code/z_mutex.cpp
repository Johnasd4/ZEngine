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

#include "z_mutex.h"

#pragma warning(disable: 4834)

namespace zengine {

Void ZMutex::Lock() noexcept {
    // Fast Path: Try to acquire lock (0 -> 1)
    // We use CompareExchangeStrong here. If we used Weak, a spurious failure
    // would cause us to fall through to the expensive slow path unnecessarily.
    UInt32 expected = 0;
    if (mutex_.CompareExchangeStrong(expected, 1U, ZMemoryOrderEnum::kAcquire, ZMemoryOrderEnum::kRelaxed)) {
        return;
    }

    // Slow Path: The lock is currently held (state is 1 or 2).
    // We atomically swap the state to 2 to indicate contention.
    // Exchange returns the previous value.
    while (mutex_.Exchange(2U, ZMemoryOrderEnum::kAcquire) != 0U) {
        // If we are here, the lock was held.
        // Wait while the value remains 2. 
        // If the value changes (e.g., to 0 by Unlock), we wake up and retry the loop.
        mutex_.Wait(2U, ZMemoryOrderEnum::kRelaxed);
    }
}

NODISCARD Bool ZMutex::TryLockUntilMs(TimeType _abs_time_ms) noexcept {
    // Fast Path: Attempt to acquire immediately
    // This covers the case where the lock is free when we enter, regardless of time
    if (TryLock()) {
        return true;
    }

    // Slow Path: Loop until acquired or timeout
    while (true) {
        // We are in the contended path. The state is likely 1 or 2.
        // TryLock() (CAS 0->1) fails if the state is 2. 
        // Exchange(2) does two things:
        // 1. Tries to acquire the lock (if it returns 0, we got it).
        // 2. Ensures the state is 2 (Contended), so the holder knows to Notify us.
        if (mutex_.Exchange(2U, ZMemoryOrderEnum::kAcquire) == 0U) {
            return true;
        }

        // Check timeout
        const TimeType now_time = ZSystemTime::GetTimeMs();
        if (now_time >= _abs_time_ms) {
            return false;
        }

        // Calculate remaining time
        TimeType remaining_time_64 = _abs_time_ms - now_time;
        UInt32 remaining_time =
            (remaining_time_64 > 0xFFFFFFFFU) ? 0xFFFFFFFFU : static_cast<UInt32>(remaining_time_64);

        // We just executed Exchange(2) above. 
        // If we didn't get the lock, the value in memory is now guaranteed to be 2.
        // We wait until it changes (e.g., set to 0 by Unlock).
        mutex_.WaitForMs(2U, remaining_time, ZMemoryOrderEnum::kRelaxed);

        // After waking up (either by Notify or Timeout), we loop back.
        // - If Notify: Exchange(2) will likely succeed next iteration.
        // - If Timeout: The time check at the top of the loop will return false.
    }
}

} // zengine

#pragma warning(default: 4834)