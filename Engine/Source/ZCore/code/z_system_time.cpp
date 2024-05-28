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

#include "z_system_time.h"

#include <ctime>         

namespace zengine {

static constexpr Int64 kSecPurMin = 60;
static constexpr Int64 kSecPurHour = 3600;
static constexpr Int64 kMinPurHour = 60;
static constexpr Int64 kHourPurDay = 24;
static constexpr Int64 kDayPurYear1 = 365;
static constexpr Int64 kDayPurYear4 = 1461;
static constexpr Int64 kDayPurYear100 = 36524;
static constexpr Int64 kDayPurYear400 = 146097;

static constexpr Int32 kNormalMonthDay[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static constexpr Int32 kSwissMonthDay[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static constexpr Int64 kStartTimeOffset = 62167219200LL;  //start at year 0 instead of 1970

namespace internal {

static Int64 CalculateRegionTimeOffset() noexcept {
    time_t raw_time = time(NULL);
    tm local_time = *localtime(&raw_time);
    tm utc_time = *gmtime(&raw_time);

    //Hour offsets.
    int hour_offset = local_time.tm_hour - utc_time.tm_hour;

    //Across day.
    if (local_time.tm_yday > utc_time.tm_yday) {
        hour_offset += kHourPurDay;
    }
    else if (local_time.tm_yday < utc_time.tm_yday) {
        hour_offset -= kHourPurDay;
    }
    
    //DST
    if (local_time.tm_isdst > 0) {
        hour_offset += 1;
    }
    return hour_offset * kSecPurHour;
}

}//internal

static Int64 GetTimeOffset() noexcept {
    static Int64 region_time_offset = internal::CalculateRegionTimeOffset() + kStartTimeOffset;
    return region_time_offset;
}

CORE_DLLAPI ZSystemTime::ZSystemTime() noexcept { UpdateTime(); }
CORE_DLLAPI ZSystemTime::~ZSystemTime() noexcept {}

CORE_DLLAPI Void ZSystemTime::UpdateTime(Int64 time_raw) noexcept {
    time_raw += GetTimeOffset();
    sec_ = (Int32)(time_raw % kSecPurMin);
    time_raw = time_raw / kSecPurMin;
    min_ = (Int32)(time_raw % kMinPurHour);
    time_raw = time_raw / kMinPurHour;
    hour_ = (Int32)(time_raw % kHourPurDay);
    time_raw = time_raw / kHourPurDay;

    Int64 year_400 = time_raw / kDayPurYear400;
    time_raw -= year_400 * kDayPurYear400;
    Int64 year_100 = time_raw / kDayPurYear100;
    time_raw -= year_100 * kDayPurYear100;
    Int64 year_4 = time_raw / kDayPurYear4;
    time_raw -= year_4 * kDayPurYear4;
    Int64 year_1 = time_raw / kDayPurYear1;
    time_raw -= year_1 * kDayPurYear1;
    year_ = (Int32)(year_400 * 400LL + year_100 * 100LL + year_4 * 4LL + year_1);

    Int32 month = 0;
    Int32 day = (Int32)time_raw;
    if ((year_1 == 0LL && year_4 != 0LL) || year_ % 400LL == 0) {
        while (day >= kSwissMonthDay[month]) {
            day -= kSwissMonthDay[month];
            ++month;
        }
    }
    else
    {
        while (day >= kNormalMonthDay[month]) {
            day -= kNormalMonthDay[month];
            ++month;
        }
    }
    day_ = day + 1;
    month_ = month + 1;
}

CORE_DLLAPI Void ZSystemTime::UpdateTimeFast(Int64 time_raw) noexcept {
    Int32 pre_hour = hour_;

    time_raw += GetTimeOffset();
    sec_ = (Int32)(time_raw % kSecPurMin);
    time_raw = time_raw / kSecPurMin;
    min_ = (Int32)(time_raw % kMinPurHour);
    time_raw = time_raw / kMinPurHour;
    hour_ = (Int32)(time_raw % kHourPurDay);

    if (hour_ < pre_hour) { //next day
        time_raw = time_raw / kHourPurDay;

        Int64 year_400 = time_raw / kDayPurYear400;
        time_raw -= year_400 * kDayPurYear400;
        Int64 year_100 = time_raw / kDayPurYear100;
        time_raw -= year_100 * kDayPurYear100;
        Int64 year_4 = time_raw / kDayPurYear4;
        time_raw -= year_4 * kDayPurYear4;
        Int64 year_1 = time_raw / kDayPurYear1;
        time_raw -= year_1 * kDayPurYear1;
        year_ += (Int32)(year_400 * 400L + year_100 * 100L + year_4 * 4L + year_1);

        Int32 month = 0;
        Int32 day = (Int32)time_raw;
        if ((year_1 == 0LL && year_4 != 0LL) || year_ % 400 == 0) {
            while (day >= kSwissMonthDay[month]) {
                day -= kSwissMonthDay[month];
                ++month;
            }
        }
        else
        {
            while (day >= kNormalMonthDay[month]) {
                day -= kNormalMonthDay[month];
                ++month;
            }
        }
        day_ = day + 1;
        month_ = month + 1;
    }
}

//namespace system_time {
//namespace internal {
//
///*
//    System time class, will start a new thread.
//*/
//class ZSystemTimeClock : public ZObject {
//private:
//    static constexpr Int32 kTimeStringSize_YMDHMS = sizeof("YYYY/MM/DD-HH:MM:SS") - 1;
//
//public:
//    static ZSystemTimeClock& Instance() noexcept {
//        static ZSystemTimeClock system_time;
//        return system_time;
//    }
//
//    ~ZSystemTimeClock() noexcept {
//        time_thread_finished_ = true;
//        if (time_thread_.Joinable()) {
//            time_thread_.Join();
//        }
//    }
//
//    /*
//        The string length is 19, without '\0'.
//    */
//    Void GetTimeString_YMDHMS(CChar* str) noexcept {
//        time_str_mutex_ymdhms_.Lock();
//        memcpy(reinterpret_cast<Void*>(str), reinterpret_cast<Void*>(time_str_ymdhms_), 
//               ZSystemTimeClock::kTimeStringSize_YMDHMS);
//        time_str_mutex_ymdhms_.Unlock();
//    }
//
//protected:
//    using SuperType = ZObject;
//
//private:
//    static Void SystemTimeThread() noexcept {
//        ZSystemTimeClock& system_time = ZSystemTimeClock::Instance();
//        while (!system_time.time_thread_finished_) {
//            //get current time
//            time_t temp_time_raw;
//            time(&temp_time_raw);
//            if (system_time.time_raw_ != temp_time_raw) {
//                localtime_s(&system_time.time_, &system_time.time_raw_);
//                //to string
//                system_time.time_str_mutex_ymdhms_.Lock();
//                sprintf(system_time.time_str_ymdhms_, "%04d/%02d/%02d-%02d:%02d:%02d",
//                    1900 + system_time.time_.tm_year, 1 + system_time.time_.tm_mon, system_time.time_.tm_mday, 
//                    8 + system_time.time_.tm_hour, system_time.time_.tm_min, system_time.time_.tm_sec);
//
//                system_time.time_str_mutex_ymdhms_.Unlock();
//            }
//            Sleep(10);
//        }
//    }
//
//    ZSystemTimeClock() noexcept {
//        time(&time_raw_);
//        localtime_s(&time_, &time_raw_);
//        sprintf(time_str_ymdhms_, "%04d/%02d/%02d-%02d:%02d:%02d",
//            1900 + time_.tm_year, 1 + time_.tm_mon, time_.tm_mday, 8 + time_.tm_hour, time_.tm_min, time_.tm_sec);
//        time_thread_finished_ = false;
//        time_thread_ = ZThread(&ZSystemTimeClock::SystemTimeThread);
//    }
//
//    time_t time_raw_;
//    tm time_;
//    CChar time_str_ymdhms_[kTimeStringSize_YMDHMS];
//    ZMutex time_str_mutex_ymdhms_;
//    Bool time_thread_finished_;
//    ZThread time_thread_;
//};
//
//}//internal
//
///*
//    The string length is 19, without '\0'. Format is "YYYY/MM/DD-HH:MM:SS".
//*/
//CORE_DLLAPI Void GetTimeString_YMDHMS(CChar* str) noexcept {
//    internal::ZSystemTimeClock& system_time = internal::ZSystemTimeClock::Instance();
//    system_time.GetTimeString_YMDHMS(str);
//}
//
//}//system_time

}//zengine
