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
#include "drive/d_pch.h"

#include "z_timer.h"

#include "m_log.h"
#include "t_lock_guard.h"

namespace zengine {
namespace internal {

struct ZTimerData : public ZObject {
public:
    ZTimerData() noexcept
        : SuperType_()
        , delay_start_time_(0)
        , interval_ms_(ZTimer::kDefaultInterval)
        , repeat_times_(ZTimer::kTimerNeverEnd)
        , state_(ZTimer::TimerStateEnum_::kClosed)
        , temp_tick_func_()
        , timer_thread_()
        , timer_mutex_()
        , sleep_mutex_()
    {}

    TimeType delay_start_time_;
    TimeType interval_ms_;
    Int32 repeat_times_;
    ZTimer::TimerStateEnum_ state_;
    TFunction<Void()> temp_tick_func_;
    ZThread timer_thread_;
    ZMutex timer_mutex_;
    ZSemMutex sleep_mutex_;

protected:
    using SuperType_ = ZObject;
};

}//internal
}//zengine

namespace zengine {

ZTimer::ZTimer() noexcept 
    : SuperType_()
    , timer_data_ptr_()
{}

ZTimer::ZTimer(ZTimer&& _timer) noexcept 
    : SuperType_(std::forward<ZTimer>(_timer))
{
    timer_data_ptr_ = std::move(_timer.timer_data_ptr_);
}

ZTimer::~ZTimer() noexcept {
    if (!timer_data_ptr_) {
        return;
    }
    {
        TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
        timer_data_ptr_->state_ = TimerStateEnum_::kFinished;
    }
    if (timer_data_ptr_->timer_thread_.Joinable()) {
        timer_data_ptr_->timer_thread_.Join();
    }
}

ZTimer& ZTimer::operator=(ZTimer&& _timer) noexcept {
    SuperType_::operator=(std::forward<ZTimer>(_timer));
    if (timer_data_ptr_) {
        //finish current timer
        {
            TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
            if (timer_data_ptr_->state_ != TimerStateEnum_::kClosed) {
                Stop();
                WaitUntilFinished();
            }
        }
    }
    timer_data_ptr_ = std::move(_timer.timer_data_ptr_);
    return *this;
}

NODISCARD TimeType ZTimer::IntervalMs() noexcept { 
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    return timer_data_ptr_->interval_ms_;
}

Void ZTimer::SetIntervalMs(TimeType _interval_ms) noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    timer_data_ptr_->interval_ms_ = _interval_ms;
}

Void ZTimer::SetTickFunc(const TFunction<Void()>& _tick_func) noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    timer_data_ptr_->temp_tick_func_ = _tick_func;
}
Void ZTimer::SetTickFunc(TFunction<Void()>&& _tick_func) noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    timer_data_ptr_->temp_tick_func_ = std::forward<TFunction<Void()>>(_tick_func);
}

Void ZTimer::SetRepeatTimes(Int32 _repeat_times) noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    timer_data_ptr_->repeat_times_ = _repeat_times < 0 ? -1 : _repeat_times;
}

Void ZTimer::SetDelayStartTime(Int32 _delay_start_time) noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    timer_data_ptr_->delay_start_time_ = _delay_start_time;
}

NODISCARD Void ZTimer::WaitUntilFinished() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    if (timer_data_ptr_->timer_thread_.Joinable()) {
        timer_data_ptr_->timer_thread_.Join();
    }
}

NODISCARD Bool ZTimer::ReadyToStart() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    return timer_data_ptr_->state_ == TimerStateEnum_::kClosed;
}

NODISCARD ReturnType ZTimer::Start() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    ReturnType ret_val = kOK;
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);

    Z_CHECK(
        timer_data_ptr_->state_ != TimerStateEnum_::kClosed,
        error_code::kZTimerErrorCode_TimerStateError,
        L"Client state error! state: %d expect state: %d",
        timer_data_ptr_->state_, TimerStateEnum_::kClosed
    );

    timer_data_ptr_->state_ = TimerStateEnum_::kExecute;
    timer_data_ptr_->sleep_mutex_.Lock();
    timer_data_ptr_->timer_thread_ = ZThread(TimerThreadFuncP, *timer_data_ptr_);

    return ret_val;
}

Void ZTimer::Stop() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    if (timer_data_ptr_->state_ != TimerStateEnum_::kClosed) {
        timer_data_ptr_->sleep_mutex_.Unlock();
        timer_data_ptr_->state_ = TimerStateEnum_::kFinished;
    }
}

Void ZTimer::Pause() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    if (timer_data_ptr_->state_ == TimerStateEnum_::kExecute) {
        timer_data_ptr_->state_ = TimerStateEnum_::kPause;
    }
}

Void ZTimer::Continue() noexcept {
    if (!timer_data_ptr_) {
        timer_data_ptr_ = MakeUnique<internal::ZTimerData>();
    }
    TLockGuard lock_guard(timer_data_ptr_->timer_mutex_);
    if (timer_data_ptr_->state_ == TimerStateEnum_::kPause) {
        timer_data_ptr_->state_ = TimerStateEnum_::kExecute;
    }
}

Void ZTimer::TimerThreadFuncP(internal::ZTimerData& _timer_data) noexcept {
    Bool finished = false;
    Int32 repeat_times_count;
    TimeType next_tick_time;
    TFunction<Void()> tick_func;

    //init
    {
        TLockGuard lock_guard(_timer_data.timer_mutex_);
        repeat_times_count = _timer_data.repeat_times_ < 0 ? kTimerNeverEnd - 1 : 0;
        next_tick_time = TimeMs() + _timer_data.delay_start_time_;
        tick_func = std::move(_timer_data.temp_tick_func_);
    }

    do {
        //sleep
        TimeType sleep_time = next_tick_time - TimeMs();
        if (sleep_time > 0) {
            //end
            if (_timer_data.sleep_mutex_.TryLockFor(static_cast<UInt32>(sleep_time))) {
                _timer_data.sleep_mutex_.Unlock();
                break;
            }
        }

        //timer state update
        {
            TLockGuard lock_guard(_timer_data.timer_mutex_);
            //if finished all repeats
            if (repeat_times_count == _timer_data.repeat_times_) {
                _timer_data.state_ = TimerStateEnum_::kFinished;
            }

            //if finished
            if (_timer_data.state_ == TimerStateEnum_::kFinished) {
                //reset the timer state for a new start
                finished = true;
                continue;
            }

            //if paused
            if (_timer_data.state_ == TimerStateEnum_::kPause) {
                continue;
            }

            //if tick func changed
            if (_timer_data.temp_tick_func_) {
                tick_func = std::move(_timer_data.temp_tick_func_);
            }

            //update next tick time
            next_tick_time += _timer_data.interval_ms_;
        }

        // < 0 means never end
        if (repeat_times_count >= 0) {
            ++repeat_times_count;
        }

        //call tick func
        if (tick_func) {
            tick_func();
        }

    } while (!finished);
    
    //unlock sleep mutex
    _timer_data.sleep_mutex_.Unlock();

    //reset state
    {
        TLockGuard lock_guard(_timer_data.timer_mutex_);
        _timer_data.state_ = TimerStateEnum_::kClosed;
    }
}

}//zengine