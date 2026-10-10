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

/** 
 * @brief A counting semaphore synchronization primitive built upon TAtom.
 * 
 * Semaphores are used to control access to a common resource by multiple threads.
 * Unlike a mutex, a semaphore allows up to N threads to access the resource simultaneously.
 */
class CORE_DLLAPI ZSemaphore {
    NO_NEW_DELETE
public:
    /**
     * @brief Constructor.
     * @param _init_count The initial number of permits available. Must be non-negative.
     */
    FORCEINLINE explicit ZSemaphore(UInt32 _init_count) noexcept
        : count_(_init_count)
        , wait_num_(0U)
    {}

    /** @brief Deleted copy constructor. */
    ZSemaphore(const ZSemaphore&) = delete;

    /** @brief Deleted move constructor. */
    ZSemaphore(ZSemaphore&&) = delete;

    /** @brief Destructor. */
    FORCEINLINE ~ZSemaphore() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZSemaphore& operator=(const ZSemaphore&) = delete;

    /** @brief Deleted move assignment operator. */
    ZSemaphore& operator=(ZSemaphore&&) = delete;

    /**
     * @brief Acquires a permit from the semaphore (P operation).
     * 
     * Decrements the internal counter by 1. If the counter is 0, this call blocks
     * the calling thread until a permit becomes available (via Release).
     */
    Void Acquire() noexcept;

    /**
     * @brief Tries to acquire a permit without blocking.
     * 
     * @return True if a permit was acquired (count decremented), False if count was 0.
     */
    NODISCARD Bool TryAcquire() noexcept;

    /**
     * @brief Tries to acquire a permit within the specified duration.
     *
     * @param _timeout_ms The maximum duration in milliseconds to wait.
     * @return True if acquired, False if timed out.
     */
    NODISCARD FORCEINLINE Bool TryAcquireForMs(TimeType _timeout_ms) noexcept {
        return TryAcquireUntilMs(ZSystemTime::GetTimeMs() + _timeout_ms);
    }

    /**
     * @brief Tries to acquire a permit until the specified absolute time.
     *
     * @param _abs_time_ms The absolute time in milliseconds.
     * @return True if acquired, False if timed out.
     */
    NODISCARD Bool TryAcquireUntilMs(TimeType _abs_time_ms) noexcept;

    /**
     * @brief Releases permits to the semaphore (V operation).
     * 
     * Increments the internal counter and notifies waiting threads.
     * @param _update The number of permits to release. Default is 1.
     */
    Void Release(UInt32 _update = 1U) noexcept;

private:
    /** @brief The underlying atomic counter. */
    TAtom<UInt32> count_;
    /**
     * @brief Tracks the number of threads currently waiting (or about to wait).
     * Used to avoid unnecessary syscalls (Notify) when there is no contention.
     */
    TAtom<UInt32> wait_num_;
};

} // zengine