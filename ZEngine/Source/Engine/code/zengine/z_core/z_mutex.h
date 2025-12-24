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

#include <atomic>

#include "z_object.h"

namespace zengine {

/** @brief A simple mutex wrapper class providing non-recursive locking mechanisms. */
class ZMutex {
public:
    /** @brief Default constructor. Initializes the mutex instance. */
    FORCEINLINE ZMutex() noexcept 
        : mutex_()
    {}

    /** @brief Destructor. Cleans up the mutex instance. */
    FORCEINLINE ~ZMutex() noexcept {}

    /** @brief Locks the mutex, blocking the calling thread until the lock is obtained. */
    FORCEINLINE Void Lock() noexcept { 
        //try to acquire the lock
        while (mutex_.exchange(1, std::memory_order_acquire) != 0) {
            //wait until the mutex appears to be unlocked
            while (mutex_.load(std::memory_order_relaxed) == 1) {
                mutex_.wait(1, std::memory_order_relaxed);
            }
        }
    }

    /**
     * @brief Tries to lock the mutex without blocking.
     * @return True if the lock was acquired, false otherwise.
     */
    FORCEINLINE Bool TryLock() noexcept {  
        return mutex_.exchange(1, std::memory_order_acquire) == 0;
    }

    /** @brief Releases the lock held by the mutex. */
    FORCEINLINE Void Unlock() noexcept { 
        mutex_.store(0, std::memory_order_release);
        mutex_.notify_one();
    }

private:
    static Void* operator new(SizeType) = delete;
    static Void operator delete(Void*) = delete;
    ZMutex(const ZMutex&) = delete;
    ZMutex(ZMutex&&) = delete;
    ZMutex& operator=(const ZMutex&) = delete;
    ZMutex& operator=(ZMutex&&) = delete;

    /** @brief The underlying standard mutex used for locking. */
    std::atomic<UInt32> mutex_;
};

}//zengine