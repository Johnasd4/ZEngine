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

// windows
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <synchapi.h>   // WaitOnAddress
#pragma comment(lib, "Synchronization.lib") // For WaitOnAddress and WakeByAddress functions 
// linux
#elif defined(__linux__)
#include <unistd.h>       // For syscall
#include <sys/syscall.h>  // For SYS_futex
#include <linux/futex.h>  // For FUTEX_WAIT_PRIVATE
#include <errno.h>        // For errno, ETIMEDOUT
#include <sys/time.h>     // For struct timespec
#include <errno.h>
#include <climits>
#endif

#include "z_object.h"
#include "z_system_time.h"
#include "z_thread.h"

namespace zengine {

/** @brief Enumeration of memory ordering constraints for atomic operations. */
enum class ZMemoryOrderEnum : Int32 {
    /** @brief Relaxed operation: no synchronization or ordering constraints. */
    kRelaxed = std::memory_order::relaxed,
    /** @brief Acquire operation: ensures that subsequent loads are not reordered before this load. */
    kAcquire = std::memory_order::acquire,
    /** @brief Release operation: ensures that previous stores are not reordered after this store. */
    kRelease = std::memory_order::release,
    /** @brief Acquire-Release operation: combines the effects of both Acquire and Release. */
    kAcquireRelease = std::memory_order::acq_rel,
    /** @brief Sequentially Consistent operation: enforces total ordering with all other seq_cst operations. */
    kSequencedConsistent = std::memory_order::seq_cst,
};

/**
 * @brief Atom template specialization for basic types using std::atomic.
 * This class wraps std::atomic to provide a unified interface for atomic operations on simple types.
 * It extends standard functionality by implementing arithmetic (multiply, divide, modulo) and bitwise
 * shift operations using Compare-And-Swap (CAS) loops, ensuring thread safety for operations not
 * natively supported by std::atomic. It also provides platform-optimized waiting and notifying mechanisms.
 * @tparam _ObjectType The basic type to be wrapped. Must be a simple type.
 */
template<typename _ObjectType>
class TAtom : public ZObject {
    STATIC_ASSERT(kIsTriviallyCopyable<_ObjectType>, "TAtom: Simple types only.");
public:
    /** @brief Parent type definition. */
    using SuperType_ = ZObject;
    /** @brief Type definition for the underlying std::atomic type. */
    using STDAtom_ = std::atomic<_ObjectType>;

private:
    /** @brief The number of times to spin and yield before sleeping in wait loops on generic platforms. */
    static inline constexpr Int32 kWaitBackoffSpinYieldTimes = 10;

public:
    /**
     * @brief Default constructor.
     * Initializes the atomic object with a default-constructed value of _ObjectType.
     */
    FORCEINLINE TAtom() noexcept : SuperType_(), obj_() {}

    /** @brief Deleted copy constructor. */
    TAtom(const TAtom&) = delete;

    /** @brief Deleted move constructor. */
    TAtom(TAtom&&) = delete;

    /**
     * @brief Constructor with initial value.
     * @param _obj The initial value to store in the atomic object.
     */
    FORCEINLINE TAtom(const _ObjectType& _obj) noexcept : SuperType_(), obj_(_obj) {}

    /**
     * @brief Move constructor with initial value.
     * @param _obj The initial value to move into the atomic object.
     */
    FORCEINLINE TAtom(_ObjectType&& _obj) noexcept : SuperType_(), obj_(std::move(_obj)) {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TAtom() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    TAtom& operator=(const TAtom&) = delete;

    /** @brief Deleted move assignment operator. */
    TAtom& operator=(TAtom&&) = delete;

    /**
     * @brief Assignment operator from a value.
     * Atomically stores the value into the atomic object.
     * @param _obj The value to assign.
     * @return A reference to this atomic object.
     */
    FORCEINLINE TAtom& operator=(const _ObjectType& _obj) noexcept {
        obj_ = _obj;
        return *this;
    }

    /**
     * @brief Move assignment operator from a value.
     * Atomically moves and stores the value into the atomic object.
     * @param _obj The value to move assign.
     * @return A reference to this atomic object.
     */
    FORCEINLINE TAtom& operator=(_ObjectType&& _obj) noexcept {
        obj_ = std::move(_obj);
        return *this;
    }

    /**
     * @brief Conversion operator to the underlying type.
     * Loads the value from the atomic object.
     * @return The current value of the atomic object.
     */
    FORCEINLINE operator _ObjectType() const noexcept {
        return obj_.load();
    }

    /**
     * @brief Atomically adds a value to the current value.
     * @param _obj The value to add.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    FORCEINLINE _ObjectType FetchAdd(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.fetch_add(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically subtracts a value from the current value.
     * @param _obj The value to subtract.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    FORCEINLINE _ObjectType FetchSub(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.fetch_sub(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically performs bitwise OR with a value.
     * @param _obj The value to OR with the current value.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    FORCEINLINE _ObjectType FetchOr(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.fetch_or(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically performs bitwise AND with a value.
     * @param _obj The value to AND with the current value.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    FORCEINLINE _ObjectType FetchAnd(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.fetch_and(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically performs bitwise XOR with a value.
     * @param _obj The value to XOR with the current value.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    FORCEINLINE _ObjectType FetchXOr(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.fetch_xor(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically loads the stored value.
     * @param _memory_order The memory order constraints to enforce.
     * @return The current value of the atomic object.
     */
    NODISCARD FORCEINLINE _ObjectType Load(
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.load(static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically loads the stored value (const version).
     * @param _memory_order The memory order constraints to enforce.
     * @return The current value of the atomic object.
     */
    NODISCARD FORCEINLINE const _ObjectType Load(
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) const noexcept {
        return obj_.load(static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically replaces the stored value.
     * @param _obj The value to store.
     * @param _memory_order The memory order constraints to enforce.
     */
    FORCEINLINE Void Store(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        obj_.store(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically replaces the stored value and returns the old value.
     * @param _obj The value to store.
     * @param _memory_order The memory order constraints to enforce.
     * @return The value of the atomic object immediately preceding the effects of this function.
     */
    NODISCARD FORCEINLINE _ObjectType Exchange(
        const _ObjectType& _obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        return obj_.exchange(_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically compares the object representation with an expected value and exchanges it if equal (weak).
     * This function may fail spuriously.
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeWeak(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj
    ) noexcept {
        return obj_.compare_exchange_weak(_expect_obj, _desire_obj);
    }

    /**
     * @brief Atomically compares the object with expected value and exchanges it if equal (weak), with memory order.
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @param _memory_order The memory order constraints for both success and failure.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeWeak(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj,
        ZMemoryOrderEnum _memory_order
    ) noexcept {
        return obj_.compare_exchange_weak(_expect_obj, _desire_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically compares the object with expected value and exchanges it if equal (weak), with separate orders.
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @param _success_memory_order The memory order constraints if the operation succeeds.
     * @param _fail_memory_order The memory order constraints if the operation fails.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeWeak(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj,
        ZMemoryOrderEnum _success_memory_order,
        ZMemoryOrderEnum _fail_memory_order
    ) noexcept {
        return obj_.compare_exchange_weak(
            _expect_obj, 
            _desire_obj, 
            static_cast<std::memory_order>(_success_memory_order), 
            static_cast<std::memory_order>(_fail_memory_order)          
        );
    }

    /**
     * @brief Atomically compares the object representation with an expected value and exchanges it if equal (strong).
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeStrong(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj
    ) noexcept {
        return obj_.compare_exchange_strong(_expect_obj, _desire_obj);
    }

    /**
     * @brief Atomically compares the object with expected value and exchanges it if equal (strong), with memory order.
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @param _memory_order The memory order constraints for both success and failure.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeStrong(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj,
        ZMemoryOrderEnum _memory_order
    ) noexcept {
        return obj_.compare_exchange_strong(_expect_obj, _desire_obj, static_cast<std::memory_order>(_memory_order));
    }

    /**
     * @brief Atomically compares the object with expected value and exchanges it if equal (strong), 
     * with separate orders.
     * @param _expect_obj Reference to the expected value. Updated to the actual value if comparison fails.
     * @param _desire_obj The value to store if comparison succeeds.
     * @param _success_memory_order The memory order constraints if the operation succeeds.
     * @param _fail_memory_order The memory order constraints if the operation fails.
     * @return True if the exchange occurred, false otherwise.
     */
    NODISCARD FORCEINLINE Bool CompareExchangeStrong(
        _ObjectType& _expect_obj,
        const _ObjectType& _desire_obj,
        ZMemoryOrderEnum _success_memory_order,
        ZMemoryOrderEnum _fail_memory_order
    ) noexcept {
        return obj_.compare_exchange_strong(
            _expect_obj,
            _desire_obj,
            static_cast<std::memory_order>(_success_memory_order),
            static_cast<std::memory_order>(_fail_memory_order)
        );
    }

    /**
     * @brief Blocks the current thread until the value of the atomic object changes from the expected value.
     * Uses optimized system calls (WaitOnAddress/futex) on supported platforms.
     * @param _expect_obj The value expected to be currently stored. The thread waits while the value equals this.
     * @param _memory_order The memory order constraints to enforce.
     */
    FORCEINLINE Void Wait(
        const _ObjectType& _expect_obj,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
// Windows platform
        STATIC_ASSERT(
            sizeof(_ObjectType) == 4,
            "TAtom: Wait only supports 4 byte types."
        );
#if defined(_WIN32)
        WaitOnAddress((volatile PVOID)&obj_, (PVOID)&_expect_obj, sizeof(_ObjectType), INFINITE);
// Linux platform
#elif defined(__linux__)
        syscall(SYS_futex, reinterpret_cast<Int32*>(&obj_), FUTEX_WAIT_PRIVATE, (Int32)_expect_obj, NULL, NULL, 0);
// Other platforms
#else
        Int32 backoff_spin_count = 0;
        while (Load(_memory_order) == _expect_obj) {
            if (backoff_spin_count <= kWaitBackoffSpinYieldTimes) {
                // Exponential backoff spin
                ZThread::YieldThread();
                ++backoff_spin_count;
            }
            else {
                ZThread::SleepMs(1LL);
            }
        }
#endif
    }

    /**
     * @brief Blocks the current thread until the value changes or the timeout expires.
     * @param _expect_obj The value expected to be currently stored.
     * @param _timeout_ms The maximum time to wait in milliseconds.
     * @param _memory_order The memory order constraints to enforce.
     * @return True if the value changed before timeout, false if the timeout occurred.
     */
    NODISCARD Bool WaitForMs(
        const _ObjectType& _expect_obj,
        UInt32 _timeout_ms,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
// Windows platform
        STATIC_ASSERT(
            sizeof(_ObjectType) == 4,
            "TAtom: WaitForMs only supports 4 byte types."
        );
#if defined(_WIN32)
        // Windows: WaitOnAddress returns TRUE if woken, FALSE if failed (e.g. timeout)
        if (WaitOnAddress((volatile PVOID)&obj_, (PVOID)&_expect_obj, sizeof(_ObjectType), (DWORD)_timeout_ms)) {
            return true;
        }
        // Failed - check if due to timeout, other errors count as spurious wakeups
        return GetLastError() != ERROR_TIMEOUT;
// Linux platform
#elif defined(__linux__)
        // Linux: Use futex syscall for efficient waiting
        // Convert timeout from milliseconds to timespec(second and nanosecond)
        struct timespec time_data;
        time_data.tv_sec = static_cast<time_t>(_timeout_ms / 1000);
        time_data.tv_nsec = static_cast<long>((_timeout_ms % 1000) * 1000000);

        // SYS_futex returns 0 on wakeup, -1 on error
        if (
            syscall(
                SYS_futex, 
                reinterpret_cast<Int32*>(&obj_), 
                FUTEX_WAIT_PRIVATE, 
                (Int32)_expect_obj, 
                &time_data, NULL, 0
            ) == 0
        ) {
            return true;
        }
        // Failed - check if due to timeout, other errors count as spurious wakeups
        return errno != ETIMEDOUT;
// Other platforms
#else
        // Fallback: Busy-wait loop (Least efficient, but compatible)
        TimeType start_time = ZSystemTime::GetTimeMs();
        TimeType timeout_ms = static_cast<TimeType>(_timeout_ms);
        Int32 backoff_spin_count = 0;
        while (Load(_memory_order) == _expect_obj) {
            if (ZSystemTime::GetTimeMs() - start_time >= timeout_ms) {
                return false;
            }
            if(backoff_spin_count <= kWaitBackoffSpinYieldTimes) {
                // Exponential backoff spin
                ZThread::YieldThread();
                ++backoff_spin_count;
            }
            else {
                ZThread::SleepMs(1LL);
            }
        }
        return true;
#endif
    }

    /**
     * @brief Blocks the current thread until the value changes or an absolute time is reached.
     * @param _expect_obj The value expected to be currently stored.
     * @param _abs_time_ms The absolute time in milliseconds (system time) to wait until.
     * @param _memory_order The memory order constraints to enforce.
     * @return True if the value changed before the time was reached, false otherwise.
     */
    NODISCARD Bool WaitUntilMs(
        const _ObjectType& _expect_obj,
        UInt32 _abs_time_ms,
        ZMemoryOrderEnum _memory_order = ZMemoryOrderEnum::kSequencedConsistent
    ) noexcept {
        TimeType now_ms = ZSystemTime::GetTimeMs();
        if (now_ms >= _abs_time_ms) {
            return false; // Already timed out
        }
        // Delegate to WaitForMs with calculated relative time
        return WaitForMs(_expect_obj, _abs_time_ms - now_ms, _memory_order);
    }

    /**
     * @brief Notifies one thread waiting on this atomic object to wake up.
     * Uses WakeByAddressSingle on Windows or FUTEX_WAKE_PRIVATE on Linux.
     */
    FORCEINLINE Void NotifyOne() noexcept {
#if defined(_WIN32)
        WakeByAddressSingle((PVOID)&obj_);
#elif defined(__linux__)
        syscall(SYS_futex, &obj_, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
#endif
    }

    /**
     * @brief Notifies all threads waiting on this atomic object to wake up.
     * Uses WakeByAddressAll on Windows or FUTEX_WAKE_PRIVATE on Linux.
     */
    FORCEINLINE Void NotifyAll() noexcept {
#if defined(_WIN32)
        WakeByAddressAll((PVOID)&obj_);
#elif defined(__linux__)
        syscall(SYS_futex, &obj_, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
#endif
    }

    /**
     * @brief Pre-increment operator.
     * Atomically increments the current value by 1.
     * @return The new value of the atomic object.
     */
    FORCEINLINE _ObjectType operator++() noexcept {
        return ++obj_;
    }

    /**
     * @brief Post-increment operator.
     * Atomically increments the current value by 1.
     * @param Unnamed integer parameter to distinguish from pre-increment.
     * @return The value of the atomic object before the increment.
     */
    FORCEINLINE _ObjectType operator++(int) noexcept {
        return obj_++;
    }

    /**
     * @brief Pre-decrement operator.
     * Atomically decrements the current value by 1.
     * @return The new value of the atomic object.
     */
    FORCEINLINE _ObjectType operator--() noexcept {
        return --obj_;
    }

    /**
     * @brief Post-decrement operator.
     * Atomically decrements the current value by 1.
     * @param Unnamed integer parameter to distinguish from pre-decrement.
     * @return The value of the atomic object before the decrement.
     */
    FORCEINLINE _ObjectType operator--(int) noexcept {
        return obj_--;
    }

    /**
     * @brief Addition assignment operator.
     * Atomically adds the operand to the current value.
     * @tparam _OtherObjectType The type of the value to add.
     * @param _obj The value to add.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator+=(const _OtherObjectType& _obj) noexcept {
        return obj_ += _obj;
    }

    /**
     * @brief Subtraction assignment operator.
     * Atomically subtracts the operand from the current value.
     * @tparam _OtherObjectType The type of the value to subtract.
     * @param _obj The value to subtract.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator-=(const _OtherObjectType& _obj) noexcept {
        return obj_ -= _obj;
    }

    /**
     * @brief Multiplication assignment operator.
     * Atomically multiplies the current value by the operand using a Compare-Exchange loop.
     * @tparam _OtherObjectType The type of the multiplier.
     * @param _obj The value to multiply by.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator*=(const _OtherObjectType& _obj) noexcept {
        _ObjectType old_obj = Load(ZMemoryOrderEnum::kRelaxed);
        _ObjectType new_obj;
        do {
            new_obj = old_obj * _obj;
        } while (
            !CompareExchangeWeak(old_obj, new_obj, ZMemoryOrderEnum::kSequencedConsistent, ZMemoryOrderEnum::kRelaxed)
        );
        return new_obj;
    }

    /**
     * @brief Division assignment operator.
     * Atomically divides the current value by the operand using a Compare-Exchange loop.
     * @tparam _OtherObjectType The type of the divisor.
     * @param _obj The value to divide by.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator/=(const _OtherObjectType& _obj) noexcept {
        _ObjectType old_obj = Load(ZMemoryOrderEnum::kRelaxed);
        _ObjectType new_obj;
        do {
            new_obj = old_obj / _obj;
        } while (
            !CompareExchangeWeak(old_obj, new_obj, ZMemoryOrderEnum::kSequencedConsistent, ZMemoryOrderEnum::kRelaxed)
        );
        return new_obj;
    }

    /**
     * @brief Modulo assignment operator.
     * Atomically applies the modulo operation to the current value using a Compare-Exchange loop.
     * @tparam _OtherObjectType The type of the operand.
     * @param _obj The value to compute modulo with.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator%=(const _OtherObjectType& _obj) noexcept {
        _ObjectType old_obj = Load(ZMemoryOrderEnum::kRelaxed);
        _ObjectType new_obj;
        do {
            new_obj = old_obj % _obj;
        } while (
            !CompareExchangeWeak(old_obj, new_obj, ZMemoryOrderEnum::kSequencedConsistent, ZMemoryOrderEnum::kRelaxed)
        );
        return new_obj;
    }

    /**
     * @brief Bitwise AND assignment operator.
     * Atomically performs bitwise AND with the operand.
     * @tparam _OtherObjectType The type of the operand.
     * @param _obj The value to AND with.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator&=(const _OtherObjectType& _obj) noexcept {
        return obj_ &= _obj;
    }

    /**
     * @brief Bitwise OR assignment operator.
     * Atomically performs bitwise OR with the operand.
     * @tparam _OtherObjectType The type of the operand.
     * @param _obj The value to OR with.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator|=(const _OtherObjectType& _obj) noexcept {
        return obj_ |= _obj;
    }

    /**
     * @brief Bitwise XOR assignment operator.
     * Atomically performs bitwise XOR with the operand.
     * @tparam _OtherObjectType The type of the operand.
     * @param _obj The value to XOR with.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator^=(const _OtherObjectType& _obj) noexcept {
        return obj_ ^= _obj;
    }

    /**
     * @brief Right shift assignment operator.
     * Atomically performs right bitwise shift using a Compare-Exchange loop.
     * @tparam _OtherObjectType The type of the shift amount.
     * @param _obj The number of bits to shift.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator>>=(const _OtherObjectType& _obj) noexcept {
        _ObjectType old_obj = Load(ZMemoryOrderEnum::kRelaxed);
        _ObjectType new_obj;
        do {
            new_obj = old_obj >> _obj;
        } while (
            !CompareExchangeWeak(old_obj, new_obj, ZMemoryOrderEnum::kSequencedConsistent, ZMemoryOrderEnum::kRelaxed)
        );
        return new_obj;
    }

    /**
     * @brief Left shift assignment operator.
     * Atomically performs left bitwise shift using a Compare-Exchange loop.
     * @tparam _OtherObjectType The type of the shift amount.
     * @param _obj The number of bits to shift.
     * @return The new value of the atomic object.
     */
    template<typename _OtherObjectType>
    FORCEINLINE _ObjectType operator<<=(const _OtherObjectType& _obj) noexcept {
        _ObjectType old_obj = Load(ZMemoryOrderEnum::kRelaxed);
        _ObjectType new_obj;
        do {
            new_obj = old_obj << _obj;
        } while (
            !CompareExchangeWeak(old_obj, new_obj, ZMemoryOrderEnum::kSequencedConsistent, ZMemoryOrderEnum::kRelaxed)
        );
        return new_obj;
    }

    /**
     * @brief Bitwise NOT operator.
     * Retrieves the current value and returns its bitwise complement.
     * @return The result of bitwise NOT on the value.
     */
    NODISCARD FORCEINLINE auto operator~() noexcept {
        return ~obj_;
    }

    /**
     * @brief Logical NOT operator.
     * Retrieves the current value and returns its logical negation.
     * @return The result of logical NOT on the value.
     */
    NODISCARD FORCEINLINE auto operator!() noexcept {
        return !obj_;
    }

private:
    /** @brief The underlying atomic object. */
    STDAtom_ obj_;
};

} // zengine