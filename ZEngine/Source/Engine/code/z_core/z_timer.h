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
#pragma once

#include "internal/z_drive.h"

#include "t_function.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_thread.h"

namespace zengine {
namespace error_code {
enum ZTimerErrorCode : ReturnType {
    kZTimerErrorCode_LinkError = kErrorCodeBase_ZTimer,
    kZTimerErrorCode_SystemError,
    kZTimerErrorCode_NullptrParam,
    kZTimerErrorCode_ParamOutOfRange,
    kZTimerErrorCode_TimerStateError
};
}//error_code
}//zengine

namespace zengine {

/*
    Timer class.
*/
class CORE_DLLAPI ZTimer : public ZObject {
public:
    //default 1 sec.
    static constexpr Int32 kDefaultInterval = 1000;
    static constexpr Int32 kTimerNeverEnd = -1;

    ZTimer() noexcept;
    ~ZTimer() noexcept;

    /*
        Returns the timer interval(ms).
    */
    NODISCARD TimeType IntervalMs() noexcept;

    /*
        Sets the time interval, default is 1000ms. 
        The frequency accuracy is 1ms, but the interval accuracy is ¡À16 ms.
        If the interval is set to 1ms, it might run 16 times in a role and sleep for 16 ms.
    */
    NODISCARD Void SetIntervalMs(TimeType _interval_ms) noexcept;

    /*
        Sets the tick function.
    */
    NODISCARD Void SetTickFunc(const TFunction<Void()>& _tick_func) noexcept;
    /*
        Sets the tick function.
    */
    NODISCARD Void SetTickFunc(TFunction<Void()>&& _tick_func) noexcept;

    /*
        Sets the time the timer will repeat. Default always repeat.
    */
    NODISCARD Void SetRepeatTimes(Int32 _repeat_times) noexcept;

    /*
        Only effects before calling Start(). The accuracy is +- 16ms.
    */
    NODISCARD Void SetDelayStartTime(Int32 _delay_start_time) noexcept;

    /*
        Will suspend the current thread until the timer is finished.
    */
    NODISCARD Void WaitUntilFinished() noexcept;

    /*
        Starts the timer. Will start a new thread.
    */
    NODISCARD Bool ReadyToStart() noexcept;

    /*
        Starts the timer. Will start a new thread.
    */
    NODISCARD ReturnType Start() noexcept;

    /*
        Ends the timer.
    */
    NODISCARD Void End() noexcept;

    /*
        Pause the timer. Only effect when the timer is started.
    */
    NODISCARD Void Pause() noexcept;

    /*
        Continue the timer. Only effect when the timer is paused.
    */
    NODISCARD Void Continue() noexcept;


protected:
    using SuperType_ = ZObject;

private:
    enum TimerState_ {
        kTimerState_Idle,
        kTimerState_Execute,
        kTimerState_Pause,
        kTimerState_Finished
    };

    ZTimer(const ZTimer&) = delete;
    ZTimer(ZTimer&&) = delete;
    ZTimer& operator=(const ZTimer&) = delete;
    ZTimer& operator=(ZTimer&&) = delete;

    static Void TimerThreadFunc(ZTimer* _timer_ptr) noexcept;

    TimeType delay_start_time_;
    TimeType interval_ms_;
    Int32 repeat_times_;
    TimerState_ state_;
    TFunction<Void()> temp_tick_func_;
    ZThread timer_thread_;
    ZMutex timer_mutex_;
};

}//zengine