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

#include<ctime>

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
    static constexpr Int32 kTimeStringSize = sizeof("YYYY/MM/DD-HH:MM:SS");

public:
    static ZSystemTime& Instance() noexcept {
        static ZSystemTime system_time;
        return system_time;
    }

    ~ZSystemTime() noexcept {
        finished_ = true;
        if (time_thread_.Joinable()) {
            time_thread_.Join();
        }
    }

    /*
        The string length is 19, without '\0'.
    */
    Void GetTimeString(CChar* str) noexcept { 
        time_str_mutex_.Lock();
        memcpy(reinterpret_cast<Void*>(str), reinterpret_cast<Void*>(time_str_), ZSystemTime::kTimeStringSize - 1);
        time_str_mutex_.Unlock();
    }

protected:
    using SuperType = ZObject;

private:
    static Void SystemTimeThread() noexcept {
        ZSystemTime& system_time = ZSystemTime::Instance();
        while (!system_time.finished_) {
            //get current time
            time(&system_time.time_raw_);
            localtime_s(&system_time.time_, &system_time.time_raw_);
            
            //to string
            system_time.time_str_mutex_.Lock();
            sprintf(system_time.time_str_, "%04d/%02d/%02d-%02d:%02d:%02d",
                1900 + system_time.time_.tm_year, 1 + system_time.time_.tm_mon, system_time.time_.tm_mday, 
                8 + system_time.time_.tm_hour, system_time.time_.tm_min, system_time.time_.tm_sec);

            system_time.time_str_mutex_.Unlock();
            
            //sleep until next second
            Sleep(system_time.time_raw_ % 1000);
        }
    }

    ZSystemTime() noexcept {
        time(&time_raw_);
        localtime_s(&time_, &time_raw_);
        sprintf(time_str_, "%04d/%02d/%02d-%02d:%02d:%02d",
            1900 + time_.tm_year, 1 + time_.tm_mon, time_.tm_mday, 8 + time_.tm_hour, time_.tm_min, time_.tm_sec);
        finished_ = false;
        time_thread_ = ZThread(&ZSystemTime::SystemTimeThread);
    }

    time_t time_raw_;
    tm time_;
    CChar time_str_[kTimeStringSize];
    Bool finished_;
    ZThread time_thread_;
    ZMutex time_str_mutex_;
};

}//internal

/*
    The string length is 19, without '\0'. Format is "YYYY/MM/DD-HH:MM:SS".
*/
CORE_DLLAPI Void GetTimeString(CChar* str) noexcept {
    internal::ZSystemTime& system_time = internal::ZSystemTime::Instance();
    system_time.GetTimeString(str);
}

}//system_time
}//zengine
