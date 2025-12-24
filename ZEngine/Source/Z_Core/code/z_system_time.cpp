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

#include "z_system_time.h"

namespace zengine {
namespace internal {

/**
 * @brief Thread-safe cross-platform wrapper for getting local time.
 * @details Converts a raw time value to a structure representing the local time, handling platform differences between
 * Windows (localtime_s) and POSIX (localtime_r).
 * @param _time_raw Pointer to the stored time_t value.
 * @param _result Pointer to the tm structure where the result will be stored.
 */
FORCEINLINE Void LocalTimeSafe(const time_t* _time_raw, struct tm* _result) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(_result, _time_raw);
#else
    localtime_r(_time_raw, _result);
#endif
}

/**
 * @brief Thread-safe cross-platform wrapper for getting UTC time.
 * @details Converts a raw time value to a structure representing the Coordinated Universal Time (UTC), handling
 * platform differences between Windows (gmtime_s) and POSIX (gmtime_r).
 * @param _time_raw Pointer to the stored time_t value.
 * @param result Pointer to the tm structure where the result will be stored.
 */
FORCEINLINE Void UTCTimeSafe(const time_t* _time_raw, struct tm* result) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(result, _time_raw);
#else
    gmtime_r(_time_raw, result);
#endif
}

/**
 * @brief Computes the local time zone offset relative to UTC in nanoseconds.
 * @details Calculates the difference between the local time and UTC time for the current instant.
 * Used to adjust UTC timestamps to local time.
 * @return The offset in nanoseconds (positive for East of Prime Meridian, negative for West).
 */
FORCEINLINE static TimeType ComputeRegionTimeOffsetNs() noexcept {
    time_t raw_time = std::time(nullptr);

    struct tm local_tm = {};
    struct tm utc_tm = {};

    LocalTimeSafe(&raw_time, &local_tm);
    UTCTimeSafe(&raw_time, &utc_tm);

    time_t local_time = mktime(&local_tm);
    //set daylight saving time flag to 0
    utc_tm.tm_isdst = 0;
    time_t utc_time = mktime(&utc_tm);
    return 
        static_cast<TimeType>(difftime(local_time, utc_time)) * 
        (ZSystemTime::kMsPerSec * ZSystemTime::kUsPerMs * ZSystemTime::kNsPerUs);
}

}//internal
}//zengine

namespace zengine {

TimeType ZSystemTime::GetRegionTimeOffsetNs() noexcept {
    static TimeType region_time_offset = internal::ComputeRegionTimeOffsetNs();
    return region_time_offset;
}

ZSystemTime& ZSystemTime::Instance() noexcept {
    thread_local ZSystemTime system_time;
    return system_time;
}

const ZSystemTime& ZSystemTime::StartTimeInstance() noexcept {
    static ZSystemTime system_time;
    return system_time;
}

ZSystemTime::ZSystemTime() noexcept : SuperType_() { UpdateTime(); }
ZSystemTime::ZSystemTime(TimeType _time_raw) noexcept : SuperType_() { UpdateTime(_time_raw); }
ZSystemTime::~ZSystemTime() noexcept {}

Void ZSystemTime::UpdateTimeFast(TimeType _time_raw) noexcept {
    TimeType pre_hour = hour_;
    _time_raw += GetRegionTimeOffsetNs();
    _time_raw = UpdateNsToHourReturnDay(_time_raw);
    if (hour_ < pre_hour) {
        UpdateDateP(_time_raw);
    }
}

Void ZSystemTime::UpdateTime(TimeType _time_raw) noexcept {
    _time_raw += GetRegionTimeOffsetNs();
    _time_raw = UpdateNsToHourReturnDay(_time_raw);
    UpdateDateP(_time_raw);
}

NODISCARD TimeType ZSystemTime::UpdateNsToHourReturnDay(TimeType _time_raw) noexcept {
    ns_ = _time_raw % kNsPerUs;
    _time_raw /= kNsPerUs;
    us_ = _time_raw % kUsPerMs;
    _time_raw /= kUsPerMs;
    ms_ = _time_raw % kMsPerSec;
    _time_raw /= kMsPerSec;
    sec_ = _time_raw % kSecPerMin;
    _time_raw /= kSecPerMin;
    min_ = _time_raw % kMinPerHour;
    _time_raw /= kMinPerHour;
    hour_ = _time_raw % kHourPerDay;
    _time_raw /= kHourPerDay;
    return _time_raw;
}

Void ZSystemTime::UpdateDateP(
    TimeType _day_time
) noexcept {
    // 1. Adjust the day count by adding the offset to shift from Unix Epoch (days since 1970-01-01)
    //    to the civil calendar algorithm's reference point (days since 0000-03-01 proleptic Gregorian)
    // 719468 is the number of days from 0000-03-01 to 1970-01-01 in the proleptic Gregorian calendar
    const TimeType z = _day_time + 719468;

    const TimeType era = (z >= 0 ? z : z - 146096) / 146097;
    const TimeType doe = static_cast<TimeType>(z - era * 146097); // [0, 146096]
    const TimeType yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365; // [0, 399]
    const TimeType y = static_cast<TimeType>(yoe) + era * 400;
    const TimeType doy = doe - (365 * yoe + yoe / 4 - yoe / 100); // [0, 365]
    const TimeType mp = (5 * doy + 2) / 153; // [0, 11]

    day_ = doy - (153 * mp + 2) / 5 + 1; // [1, 31]
    month_ = mp + (mp < 10 ? 3 : -9); // [1, 12]
    year_ = y + (month_ <= 2);
}

CORE_DLLAPI NODISCARD TimeType Time() noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

}//zengine
