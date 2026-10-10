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
#include "t_lock_guard.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_system_time.h"
#include "z_task.h"
#include "z_thread.h"

namespace zengine {
namespace error_code {

/** @brief Error codes specific to the ZTimer class. */
enum ZTimerErrorCodeEnum : ReturnType::ValueType {
    /** @brief Error occurred during the link phase of the task. */
    kZTimerErrorCode_LinkError = kErrorCodeBase_ZTimer,
    /** @brief Error occurred in the system or underlying library. */
    kZTimerErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was passed where it is not allowed. */
    kZTimerErrorCode_NullptrParam,
    /** @brief A parameter is out of the valid range. */
    kZTimerErrorCode_ParamOutOfRange,
    /** @brief The operation cannot be performed in the current timer state. */
    kZTimerErrorCode_StateError
};
} //  error_code
} //  zengine

namespace zengine {
namespace internal {

/** @brief Internal data structure for timer details. */
struct ZTimerData;

} //  internal
} //  zengine

namespace zengine {

/**
 * @brief Timer class.
 *
 * Implementation Principle:
 * This class utilizes a dedicated thread (ZThread) to manage timing events. To prevent time drift (accumulation of
 * error), it calculates the next execution point based on absolute time increments (Start + n * Interval) rather than
 * relative delays. Thread synchronization is handled via a condition variable to support efficient waiting, pausing,
 * and stopping.
 */
class CORE_DLLAPI ZTimer : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

    /** @brief Constant representing infinite tick repetitions. */
    static inline constexpr UInt32 kTickForever = kUInt32Max;

private:
    /** @brief Default delay before starting the first tick (0ms). */
    static inline constexpr TimeType kDefaultDelayStartTimeMs = 0LL;
    /** @brief Default interval between ticks (0ms). */
    static inline constexpr TimeType kDefaultIntervalMs = 0LL;
    /** @brief Default number of tick repetitions (1 time). */
    static inline constexpr TimeType kDefaultTickTimes = 1LL;

    /** @brief Internal state enumeration for the timer. */
    enum class TimerStateEnum_ : Int32 {
        /** @brief No task has been set. */
        kNoTask,
        /** @brief Task is set but the timer is not running. */
        kIdle,
        /** @brief Timer is currently running. */
        kExecute,
        /** @brief Timer is paused. */
        kPause,
        /** @brief Timer has finished execution. */
        kFinished
    };

public:
    /** @brief Default constructor. */
    ZTimer() noexcept;

    /** @brief Deleted copy constructor. */
    ZTimer(const ZTimer&) = delete;

    /** @brief Deleted move constructor. */
    ZTimer(ZTimer&&) = delete;

    /** @brief Destructor. Stops the timer and joins the thread if running. */
    ~ZTimer() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZTimer& operator=(const ZTimer&) = delete;

    /** @brief Deleted move assignment operator. */
    ZTimer& operator=(ZTimer&&) = delete;

    /**
     * @brief Gets the interval between ticks.
     * @return TimeType The interval in milliseconds.
     */
    NODISCARD TimeType GetIntervalMs() noexcept;

    /**
     * @brief Sets the delay time before the first execution.
     * @param _delay_start_time_ms The delay time in milliseconds.
     */
    Void SetDelayStartTimeMs(TimeType _delay_start_time_ms) noexcept;

    /**
     * @brief Sets the interval between ticks.
     * @param _interval_ms The interval in milliseconds.
     */
    Void SetIntervalMs(TimeType _interval_ms) noexcept;

    /**
     * @brief Sets the number of times the task should repeat.
     * @param _tick_times The number of repetitions. Use kTickForever for infinite loops.
     */
    Void SetTickTimes(UInt32 _tick_times) noexcept;

    /**
     * @brief Sets the task to be executed by the timer (Move semantics).
     * @param _task The task object to be moved into the timer.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    NODISCARD ReturnType SetTickTask(ZRepeatTask&& _task) noexcept;

    /**
     * @brief Sets the task to be executed by the timer using a function and arguments.
     * @tparam _Function The type of the function to execute.
     * @tparam _ArgsType The types of the arguments to pass to the function.
     * @param _func The function to execute.
     * @param _args The arguments to pass to the function.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    template <typename _Function, typename... _ArgsType>
    NODISCARD ReturnType SetTickTask(_Function&& _func, _ArgsType&&... _args) noexcept {
        ReturnType ret_val = kOK;
        TLockGuard lock_guard(timer_mutex_);
        Z_CHECK(
            state_ == TimerStateEnum_::kNoTask || state_ == TimerStateEnum_::kIdle,
            error_code::kZTimerErrorCode_StateError,
            "Timer state not valid! state: {} expect state: {} or {}",
            static_cast<Int32>(state_),
            static_cast<Int32>(TimerStateEnum_::kNoTask),
            static_cast<Int32>(TimerStateEnum_::kIdle)
        );
        tick_task_.SetTask(std::forward<_Function>(_func), std::forward<_ArgsType>(_args)...);
        state_ = TimerStateEnum_::kIdle;
        return ret_val;
    }

    /**
     * @brief Waits until the timer thread has finished execution.
     */
    FORCEINLINE Void WaitUntilFinished() noexcept { 
        if (timer_thread_.IsJoinable()) {
            timer_thread_.Join();
        }
    }

    /**
     * @brief Starts the timer.
     * @return ReturnType Returns kOK if started successfully, otherwise returns an error code.
     */
    NODISCARD ReturnType Start() noexcept;

    /**
     * @brief Stops the timer.
     * Interrupts the wait and signals the thread to finish.
     */
    Void Stop() noexcept;

    /**
     * @brief Pauses the timer.
     * Only effective when the timer is currently running.
     */
    Void Pause() noexcept;

    /**
     * @brief Continues the timer execution.
     * Only effective when the timer is currently paused.
     */
    Void Continue() noexcept;

    /**
     * @brief Resets the timer to the idle state.
     * Stops the current execution and waits for the thread to join.
     */
    Void Reset() noexcept;

private:
    /**
     * @brief The thread function that executes the timer logic.
     * @param _timer_ptr Pointer to the ZTimer instance.
     */
    static Void TimerThreadFuncP(ZTimer* _timer_ptr) noexcept;

private:
    /** @brief Condition variable for thread synchronization and waiting. */
    ZConditionVariable timer_cv_;
    /** @brief Mutex for protecting timer state and shared data. */
    ZMutex timer_mutex_;
    /** @brief Delay before the first tick in milliseconds. */
    TimeType delay_start_time_ms_;
    /** @brief Interval between ticks in milliseconds. */
    TimeType interval_ms_;
    /** @brief Total number of times to execute the task. */
    UInt32 tick_times_;
    /** @brief Current state of the timer. */
    TimerStateEnum_ state_;
    /** @brief The thread object running the timer loop. */
    ZThread timer_thread_;
    /** @brief The task object to be executed. */
    ZRepeatTask tick_task_;
};

} //  zengine