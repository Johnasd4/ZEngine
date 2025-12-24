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

#include "drive.h"

#include "t_atom.h"
#include "t_function.h"
#include "t_smart_pointer.h"
#include "z_object.h"
#include "z_thread.h"

namespace zengine {
namespace error_code {
enum ZTimerErrorCodeEnum : ReturnType {
    kZTimerErrorCode_LinkError = kErrorCodeBase_ZTimer,
    kZTimerErrorCode_SystemOrLibraryError,
    kZTimerErrorCode_NullptrParam,
    kZTimerErrorCode_ParamOutOfRange,
    kZTimerErrorCode_TimerStateError
};
}//error_code
}//zengine

namespace zengine {
namespace internal {

struct ZTimerData;

}//internal
}//zengine

namespace zengine {

/*
    Timer class.
*/
class CORE_DLLAPI ZTimer : public ZObject<> {
public:
    //default 1 sec.
    static inline constexpr Int32 kDefaultInterval = 1000;
    static inline constexpr Int32 kTimerNeverEnd = -1;

    ZTimer() noexcept;
    ZTimer(ZTimer&& _timer) noexcept;
    ~ZTimer() noexcept;

    ZTimer& operator=(ZTimer&& _timer) noexcept;

    /*
        Returns the timer interval(ms).
    */
    NODISCARD TimeType IntervalMs() noexcept;

    /*
        Sets the time interval, default is 1000ms. 
        The frequency accuracy is 1ms, but the interval accuracy is +-16 ms.
        If the interval is set to 1ms, it might run 16 times in a role and sleep for 16 ms.
        WARNING: Does not effect the first tick.
    */
    Void SetIntervalMs(TimeType _interval_ms) noexcept;

    /*
        Sets the tick function.
    */
    Void SetTickFunc(const TFunction<Void()>& _tick_func) noexcept;
    /*
        Sets the tick function.
    */
    Void SetTickFunc(TFunction<Void()>&& _tick_func) noexcept;

    /*
        Sets the time the timer will repeat. Default always repeat.
    */
    Void SetRepeatTimes(Int32 _repeat_times) noexcept;

    /*
        Only effects before calling Start(). The accuracy is +- 16ms.
    */
    Void SetDelayStartTime(Int32 _delay_start_time) noexcept;

    /*
        Will suspend the current thread until the timer is finished.
    */
    Void WaitUntilFinished() noexcept;

    /*
        If timer can start.
    */
    NODISCARD Bool ReadyToStart() noexcept;

    /*
        Starts the timer. Will start a new thread.
    */
    NODISCARD ReturnType Start() noexcept;

    /*
        Ends the timer.
    */
    Void Stop() noexcept;

    /*
        Pause the timer. Only effect when the timer is started.
    */
    Void Pause() noexcept;

    /*
        Continue the timer. Only effect when the timer is paused.
    */
    Void Continue() noexcept;

protected:
    using SuperType_ = ZObject;
    friend struct internal::ZTimerData;

private:
    enum class TimerStateEnum_ : Int32 {
        kClosed,
        kExecute,
        kPause,
        kFinished
    };

    ZTimer(const ZTimer&) = delete;
    ZTimer& operator=(const ZTimer&) = delete;

    static Void TimerThreadFuncP(internal::ZTimerData& _timer_data) noexcept;

    TUniquePointer<internal::ZTimerData> timer_data_ptr_;
};

}//zengine