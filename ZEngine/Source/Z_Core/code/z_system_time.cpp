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

#include "z_system_time.h"

namespace zengine {

static constexpr TimeType kSecPurMin = 60;
static constexpr TimeType kSecPurHour = 3600;
static constexpr TimeType kMinPurHour = 60;
static constexpr TimeType kHourPurDay = 24;
static constexpr TimeType kDayPurYear1 = 365;
static constexpr TimeType kDayPurYear4 = 1461;
static constexpr TimeType kDayPurYear100 = 36524;
static constexpr TimeType kDayPurYear400 = 146097;

static constexpr Int32 kNormalMonthDay[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static constexpr Int32 kSwissMonthDay[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static constexpr TimeType kStartTimeOffset = 62167219200LL;  //start at year 0 instead of 1970

namespace internal {

static TimeType CalculateRegionTimeOffset() noexcept {
    time_t raw_time = time(nullptr);
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

static TimeType GetTimeOffset() noexcept {
    static TimeType region_time_offset = internal::CalculateRegionTimeOffset() + kStartTimeOffset;
    return region_time_offset;
}

ZSystemTime& ZSystemTime::Instance() noexcept {
    static ZSystemTime system_time;
    return system_time;
}

const ZSystemTime& ZSystemTime::StartTimeInstance() noexcept {
    static ZSystemTime system_time;
    return system_time;
}

ZSystemTime::ZSystemTime() noexcept : SuperType_() { UpdateTime(); }
ZSystemTime::ZSystemTime(TimeType _time_raw) noexcept : SuperType_() { UpdateTime(_time_raw); }
ZSystemTime::~ZSystemTime() noexcept {}

Void ZSystemTime::UpdateTime(TimeType _time_raw) noexcept {
    _time_raw += GetTimeOffset();
    sec_ = (Int32)(_time_raw % kSecPurMin);
    _time_raw = _time_raw / kSecPurMin;
    min_ = (Int32)(_time_raw % kMinPurHour);
    _time_raw = _time_raw / kMinPurHour;
    hour_ = (Int32)(_time_raw % kHourPurDay);
    _time_raw = _time_raw / kHourPurDay;

    TimeType year_400 = _time_raw / kDayPurYear400;
    _time_raw -= year_400 * kDayPurYear400;
    TimeType year_100 = _time_raw / kDayPurYear100;
    _time_raw -= year_100 * kDayPurYear100;
    TimeType year_4 = _time_raw / kDayPurYear4;
    _time_raw -= year_4 * kDayPurYear4;
    TimeType year_1 = _time_raw / kDayPurYear1;
    _time_raw -= year_1 * kDayPurYear1;
    year_ = (Int32)(year_400 * 400LL + year_100 * 100LL + year_4 * 4LL + year_1);

    Int32 month = 0;
    Int32 day = (Int32)_time_raw;
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

Void ZSystemTime::UpdateTimeFast(TimeType _time_raw) noexcept {
    Int32 pre_hour = hour_;

    _time_raw += GetTimeOffset();
    sec_ = (Int32)(_time_raw % kSecPurMin);
    _time_raw = _time_raw / kSecPurMin;
    min_ = (Int32)(_time_raw % kMinPurHour);
    _time_raw = _time_raw / kMinPurHour;
    hour_ = (Int32)(_time_raw % kHourPurDay);

    if (hour_ < pre_hour) { //next day
        _time_raw = _time_raw / kHourPurDay;

        TimeType year_400 = _time_raw / kDayPurYear400;
        _time_raw -= year_400 * kDayPurYear400;
        TimeType year_100 = _time_raw / kDayPurYear100;
        _time_raw -= year_100 * kDayPurYear100;
        TimeType year_4 = _time_raw / kDayPurYear4;
        _time_raw -= year_4 * kDayPurYear4;
        TimeType year_1 = _time_raw / kDayPurYear1;
        _time_raw -= year_1 * kDayPurYear1;
        year_ += (Int32)(year_400 * 400L + year_100 * 100L + year_4 * 4L + year_1);

        Int32 month = 0;
        Int32 day = (Int32)_time_raw;
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

}//zengine
