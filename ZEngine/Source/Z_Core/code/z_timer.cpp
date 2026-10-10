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

#include "z_timer.h"

#include "t_unique_lock.h"

namespace zengine {

ZTimer::ZTimer() noexcept 
    : SuperType_()
    , timer_cv_()
    , timer_mutex_()
    , delay_start_time_ms_(kDefaultDelayStartTimeMs)
    , interval_ms_(kDefaultIntervalMs)
    , tick_times_(kDefaultTickTimes)
    , state_(TimerStateEnum_::kNoTask)
    , timer_thread_()
    , tick_task_()
{}

ZTimer::~ZTimer() noexcept {
    Stop();
    if (timer_thread_.IsJoinable()) {
        timer_thread_.Join();
    }
}

NODISCARD TimeType ZTimer::GetIntervalMs() noexcept {
    TLockGuard lock_guard(timer_mutex_);
    return interval_ms_;
}

Void ZTimer::SetDelayStartTimeMs(TimeType _delay_start_time_ms) noexcept {
    TLockGuard lock_guard(timer_mutex_);
    delay_start_time_ms_ = _delay_start_time_ms;
}

Void ZTimer::SetIntervalMs(TimeType _interval_ms) noexcept {
    TLockGuard lock_guard(timer_mutex_);
    interval_ms_ = _interval_ms;
}

Void ZTimer::SetTickTimes(UInt32 _tick_times) noexcept {
    TLockGuard lock_guard(timer_mutex_);
    tick_times_ = _tick_times;
}

NODISCARD ReturnType ZTimer::SetTickTask(ZRepeatTask&& _task) noexcept {
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
    Z_CHECK(
        _task.GetState() == ZTaskStateEnum::kTaskSet,
        error_code::kZTimerErrorCode_StateError,
        "Tick task state not valid! state: {} expect state: {}",
        static_cast<Int32>(_task.GetState()), static_cast<Int32>(ZTaskStateEnum::kTaskSet)
    );
    tick_task_ = std::move(_task);
    state_ = TimerStateEnum_::kIdle;
    return ret_val;
}

NODISCARD ReturnType ZTimer::Start() noexcept {
    ReturnType ret_val = kOK;
    TLockGuard lock_guard(timer_mutex_);

    Z_CHECK(
        state_ == TimerStateEnum_::kIdle,
        error_code::kZTimerErrorCode_StateError,
        "Timer state error! state: {} expect state: {}",
        static_cast<Int32>(state_), static_cast<Int32>(TimerStateEnum_::kIdle)
    );

    state_ = TimerStateEnum_::kExecute;
    timer_thread_ = ZThread(TimerThreadFuncP, this);

    return ret_val;
}

Void ZTimer::Stop() noexcept {
    TLockGuard lock_guard(timer_mutex_);
    if (state_ == TimerStateEnum_::kExecute || state_ == TimerStateEnum_::kPause) {
        state_ = TimerStateEnum_::kFinished;
        timer_cv_.NotifyOne();
    }
}

Void ZTimer::Pause() noexcept {
    TLockGuard lock_guard(timer_mutex_);
    if (state_ == TimerStateEnum_::kExecute) {
        state_ = TimerStateEnum_::kPause;
    }
}

Void ZTimer::Continue() noexcept {
    TLockGuard lock_guard(timer_mutex_);
    if (state_ == TimerStateEnum_::kPause) {
        state_ = TimerStateEnum_::kExecute;
        timer_cv_.NotifyOne();
    }
}

Void ZTimer::Reset() noexcept {
    {
        TLockGuard lock_guard(timer_mutex_);
        if (state_ == TimerStateEnum_::kExecute || state_ == TimerStateEnum_::kPause) {
            state_ = TimerStateEnum_::kFinished;
            timer_cv_.NotifyOne();
        }
    }
    if (timer_thread_.IsJoinable()) {
        timer_thread_.Join();
    }
    {
        TLockGuard lock_guard(timer_mutex_);
        state_ = TimerStateEnum_::kIdle;
    }
}

Void ZTimer::TimerThreadFuncP(ZTimer* _timer_ptr) noexcept {
    ReturnType link_code = kOK;
    Bool finished = false;
    UInt32 repeat_times_count = 0U;
    TimeType next_tick_time;

    // init
    {
        TLockGuard lock_guard(_timer_ptr->timer_mutex_);
        repeat_times_count = 0U;
        next_tick_time = ZSystemTime::GetTimeMs() + _timer_ptr->delay_start_time_ms_;
    }

    for (;;) {
        // timer state update
        {
            TUniqueLock unique_lock(_timer_ptr->timer_mutex_);

            // if finished 
            if (_timer_ptr->state_ == TimerStateEnum_::kFinished) {
                break;
            }

            // if finished all repeats
            if (repeat_times_count >= _timer_ptr->tick_times_ && repeat_times_count != kTickForever) {
                break;
            }

            // sleep
            TimeType current_time = ZSystemTime::GetTimeMs();
            while (next_tick_time >= current_time) {
                // end
                if (_timer_ptr->timer_cv_.WaitForMs(unique_lock, next_tick_time - current_time)) {
                    break;
                }
                current_time = ZSystemTime::GetTimeMs();
            }

            // if finished 
            if (_timer_ptr->state_ == TimerStateEnum_::kFinished) {
                break;
            }

            // if paused
            if (_timer_ptr->state_ == TimerStateEnum_::kPause) {
                current_time = ZSystemTime::GetTimeMs();
                do {
                    _timer_ptr->timer_cv_.Wait(unique_lock);
                } while (_timer_ptr->state_ == TimerStateEnum_::kPause);
                next_tick_time += ZSystemTime::GetTimeMs() - current_time;
                continue;
            }

            // update next tick time
            next_tick_time += _timer_ptr->interval_ms_;
        }

        ++repeat_times_count;

        // call tick func
        link_code = _timer_ptr->tick_task_.Run();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZTimerErrorCode_LinkError, 0,
                "ZRepeatTask::Run() link error!"
            );
        }
    }
    
    // finished
    {
        TLockGuard lock_guard(_timer_ptr->timer_mutex_);
        _timer_ptr->state_ = TimerStateEnum_::kFinished;
    }
}

} // zengine