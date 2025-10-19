/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "z_timer.h"

#include "m_log.h"

namespace zengine {

ZTimer::ZTimer() noexcept 
    : SuperType_()
    , delay_start_time_(0)
    , interval_ms_(kDefaultInterval)
    , repeat_times_(kTimerNeverEnd)
    , state_(kTimerState_Idle)
    , temp_tick_func_()
    , timer_thread_()
    , timer_mutex_()
{}

ZTimer::~ZTimer() noexcept {
    timer_mutex_.Lock();
    state_ = kTimerState_Finished;
    timer_mutex_.Unlock();
    if (timer_thread_.Joinable()) {
        timer_thread_.Join();
    }
}

NODISCARD TimeType ZTimer::IntervalMs() noexcept { 
    timer_mutex_.Lock();
    TimeType interval = interval_ms_;
    timer_mutex_.Unlock();
    return interval_ms_; 
}

NODISCARD Void ZTimer::SetIntervalMs(TimeType _interval_ms) noexcept {
    timer_mutex_.Lock();
    interval_ms_ = _interval_ms;
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::SetTickFunc(const TFunction<Void()>& _tick_func) noexcept {
    timer_mutex_.Lock();
    temp_tick_func_ = _tick_func;
    timer_mutex_.Unlock();
}
NODISCARD Void ZTimer::SetTickFunc(TFunction<Void()>&& _tick_func) noexcept {
    timer_mutex_.Lock();
    temp_tick_func_ = std::move(_tick_func);
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::SetRepeatTimes(Int32 _repeat_times) noexcept {
    timer_mutex_.Lock();
    repeat_times_ = _repeat_times < 0 ? -1 : _repeat_times;
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::SetDelayStartTime(Int32 _delay_start_time) noexcept {
    timer_mutex_.Lock();
    delay_start_time_ = _delay_start_time;
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::WaitUntilFinished() noexcept {
    if (timer_thread_.Joinable()) {
        timer_thread_.Join();
    }
}

NODISCARD Bool ZTimer::ReadyToStart() noexcept {
    timer_mutex_.Lock();
    Bool ready = (state_ == kTimerState_Idle);
    timer_mutex_.Unlock();
    return ready;
}

NODISCARD ReturnType ZTimer::Start() noexcept {
    ReturnType ret_val = kOK;
    if (state_ != kTimerState_Idle) {
        ret_val = error_code::kZTimerErrorCode_TimerStateError;
        Z_LOG_ERROR(
            ret_val, 0, L"Timer state error! state: %d expect state: %d",
            state_, kTimerState_Idle
        );
        return ret_val;
    }
    timer_mutex_.Lock();
    state_ = kTimerState_Execute;
    timer_mutex_.Unlock();
    timer_thread_ = ZThread(TimerThreadFunc, this);
    return ret_val;
}

NODISCARD Void ZTimer::End() noexcept {
    timer_mutex_.Lock();
    if (state_ != kTimerState_Idle) {
        state_ = kTimerState_Finished;
    }
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::Pause() noexcept {
    timer_mutex_.Lock();
    if (state_ == kTimerState_Execute) {
        state_ = kTimerState_Pause;
    }
    timer_mutex_.Unlock();
}

NODISCARD Void ZTimer::Continue() noexcept {
    timer_mutex_.Lock();
    if (state_ == kTimerState_Pause) {
        state_ = kTimerState_Execute;
    }
    timer_mutex_.Unlock();
}

Void ZTimer::TimerThreadFunc(ZTimer* _timer_ptr) noexcept {
    Bool finished = false;
    Int32 repeat_times_count;
    TimeType next_tick_time;
    TFunction<Void()> tick_func;

    //init
    _timer_ptr->timer_mutex_.Lock();

    repeat_times_count = _timer_ptr->repeat_times_ < 0 ? kTimerNeverEnd - 1 : 0;
    next_tick_time = TimeMs() + _timer_ptr->delay_start_time_;
    tick_func = std::move(_timer_ptr->temp_tick_func_);

    _timer_ptr->timer_mutex_.Unlock();

    do {
        //sleep
        TimeType sleep_time = next_tick_time - TimeMs();
        if (sleep_time > 0) {
            SleepMs(sleep_time);
        }

        _timer_ptr->timer_mutex_.Lock();

        //if finished all repeats
        if (repeat_times_count == _timer_ptr->repeat_times_) {
            _timer_ptr->state_ = kTimerState_Finished;
        }

        //if finished
        if (_timer_ptr->state_ == kTimerState_Finished) {
            //reset the timer state for a new start
            _timer_ptr->state_ = kTimerState_Idle;
            _timer_ptr->timer_mutex_.Unlock();
            finished = true;
            continue;
        }
        
        //if paused
        if (_timer_ptr->state_ == kTimerState_Pause) {
            _timer_ptr->timer_mutex_.Unlock();
            continue;
        }

        //if tick func changed
        if (_timer_ptr->temp_tick_func_) {
            tick_func = std::move(_timer_ptr->temp_tick_func_);
        }

        //update next tick time
        next_tick_time += _timer_ptr->interval_ms_;

        _timer_ptr->timer_mutex_.Unlock();

        // < 0 means never end
        if (repeat_times_count >= 0) {
            ++repeat_times_count;
        }

        //call tick func
        if (tick_func) {
            tick_func();
        }

    } while (!finished);
}

}//zengine