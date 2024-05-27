/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#pragma warning(disable : 26439)
#pragma warning(disable : 26495)

#include "internal/drive.h"

#include <ctime>

#include "z_mutex.h"
#include "z_object.h"
#include "z_thread.h"                 

namespace zengine {
namespace system_time {
namespace internal {

/*
    System time class, will start a new thread.
*/
class ZSystemTime : public ZObject {
private:
    static constexpr Int32 kTimeStringSize_YMDHMS = sizeof("YYYY/MM/DD-HH:MM:SS") - 1;

public:
    static ZSystemTime& Instance() noexcept {
        static ZSystemTime system_time;
        return system_time;
    }

    ~ZSystemTime() noexcept {
        time_thread_finished_ = true;
        if (time_thread_.Joinable()) {
            time_thread_.Join();
        }
    }

    /*
        The string length is 19, without '\0'.
    */
    Void GetTimeString_YMDHMS(CChar* str) noexcept {
        time_str_mutex_ymdhms_.Lock();
        memcpy(reinterpret_cast<Void*>(str), reinterpret_cast<Void*>(time_str_ymdhms_), ZSystemTime::kTimeStringSize_YMDHMS);
        time_str_mutex_ymdhms_.Unlock();
    }

protected:
    using SuperType = ZObject;

private:
    static Void SystemTimeThread() noexcept {
        ZSystemTime& system_time = ZSystemTime::Instance();
        while (!system_time.time_thread_finished_) {
            //get current time
            time_t temp_time_raw;
            time(&temp_time_raw);
            if (system_time.time_raw_ != temp_time_raw) {
                localtime_s(&system_time.time_, &system_time.time_raw_);
                //to string
                system_time.time_str_mutex_ymdhms_.Lock();
                sprintf(system_time.time_str_ymdhms_, "%04d/%02d/%02d-%02d:%02d:%02d",
                    1900 + system_time.time_.tm_year, 1 + system_time.time_.tm_mon, system_time.time_.tm_mday, 
                    8 + system_time.time_.tm_hour, system_time.time_.tm_min, system_time.time_.tm_sec);

                system_time.time_str_mutex_ymdhms_.Unlock();
            }
            Sleep(10);
        }
    }

    ZSystemTime() noexcept {
        time(&time_raw_);
        localtime_s(&time_, &time_raw_);
        sprintf(time_str_ymdhms_, "%04d/%02d/%02d-%02d:%02d:%02d",
            1900 + time_.tm_year, 1 + time_.tm_mon, time_.tm_mday, 8 + time_.tm_hour, time_.tm_min, time_.tm_sec);
        time_thread_finished_ = false;
        time_thread_ = ZThread(&ZSystemTime::SystemTimeThread);
    }

    time_t time_raw_;
    tm time_;
    CChar time_str_ymdhms_[kTimeStringSize_YMDHMS];
    ZMutex time_str_mutex_ymdhms_;
    Bool time_thread_finished_;
    ZThread time_thread_;
};

}//internal

/*
    The string length is 19, without '\0'. Format is "YYYY/MM/DD-HH:MM:SS".
*/
CORE_DLLAPI Void GetTimeString_YMDHMS(CChar* str) noexcept {
    internal::ZSystemTime& system_time = internal::ZSystemTime::Instance();
    system_time.GetTimeString_YMDHMS(str);
}

}//system_time
}//zengine
