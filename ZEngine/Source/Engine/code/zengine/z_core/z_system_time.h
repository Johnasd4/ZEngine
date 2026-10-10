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
#pragma once

#include "drive.h"

#include <chrono>

#include "z_object.h"

namespace zengine {

/**
 * @brief The system time class.
 * @details Inherits from ZObject and manages time decomposition [year, month, day, hour, min, sec, ms, us, ns].
 */
class CORE_DLLAPI ZSystemTime : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

    /** @brief Constant for nanoseconds per microsecond. */
    static inline constexpr TimeType kNsPerUs = 1000;
    /** @brief Constant for microseconds per millisecond. */
    static inline constexpr TimeType kUsPerMs = 1000;
    /** @brief Constant for milliseconds per second. */
    static inline constexpr TimeType kMsPerSec = 1000;
    /** @brief Constant for seconds per minute. */
    static inline constexpr TimeType kSecPerMin = 60;
    /** @brief Constant for seconds per hour. */
    static inline constexpr TimeType kSecPerHour = 3600;
    /** @brief Constant for minutes per hour. */
    static inline constexpr TimeType kMinPerHour = 60;
    /** @brief Constant for hours per day. */
    static inline constexpr TimeType kHourPerDay = 24;

public:
    /**
     * @brief Gets the current system time. Use GetSteadyTimeNs() for measuring time intervals.
     * @details Returns the number of nanoseconds elapsed since 1970-01-01 00:00:00 UTC.
     * @return The current time as a TimeType (nanoseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetTimeNs() noexcept {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current system time. Use GetSteadyTimeUs() for measuring time intervals.
     * @details Returns the number of microseconds elapsed since 1970-01-01 00:00:00 UTC.
     * @return The current time as a TimeType (microseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetTimeUs() noexcept {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current system time. Use GetSteadyTimeMs() for measuring time intervals.
     * @details Returns the number of milliseconds elapsed since 1970-01-01 00:00:00 UTC.
     * @return The current time as a TimeType (milliseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetTimeMs() noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current system time. Use GetSteadyTimeSec() for measuring time intervals.
     * @details Returns the number of seconds elapsed since 1970-01-01 00:00:00 UTC.
     * @return The current time as a TimeType (seconds).
     */
    NODISCARD FORCEINLINE static TimeType GetTimeSec() noexcept {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current steady time.
     * @details Returns the number of nanoseconds elapsed since an unspecified starting point.
     * @return The current time as a TimeType (nanoseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetSteadyTimeNs() noexcept {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current steady time.
     * @details Returns the number of microseconds elapsed since an unspecified starting point.
     * @return The current time as a TimeType (microseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetSteadyTimeUs() noexcept {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current steady time.
     * @details Returns the number of milliseconds elapsed since an unspecified starting point.
     * @return The current time as a TimeType (milliseconds).
     */
    NODISCARD FORCEINLINE static TimeType GetSteadyTimeMs() noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the current steady time.
     * @details Returns the number of seconds elapsed since an unspecified starting point.
     * @return The current time as a TimeType (seconds).
     */
    NODISCARD FORCEINLINE static TimeType GetSteadyTimeSec() noexcept {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Gets the thread-local instance of the ZSystemTime.
     * @return A reference to the thread-local ZSystemTime instance.
     */
    static ZSystemTime& GetInstance() noexcept;

    /**
     * @brief Gets the static start time instance of the ZSystemTime.
     * @details This instance represents the time when the application started (or when the static instance was init).
     * @return A constant reference to the start time ZSystemTime instance.
     */
    static const ZSystemTime& GetProcessStartTimeInstance() noexcept;

    /**
     * @brief Default constructor.
     * @details Initializes the time object with the current system time.
     */
    ZSystemTime() noexcept;

    /**
     * @brief Parameterized constructor.
     * @details Initializes the time object with a specific raw time value.
     * @param _time_raw The raw time value (nanoseconds since epoch).
     */
    ZSystemTime(TimeType _time_raw) noexcept;

    /**
     * @brief Destructor.
     */
    ~ZSystemTime() noexcept = default;

    /**
     * @brief Gets the nanosecond component.
     * @return The nanosecond part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetNs() const noexcept { return ns_; }

    /**
     * @brief Gets the microsecond component.
     * @return The microsecond part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetUs() const noexcept { return us_; }

    /**
     * @brief Gets the millisecond component.
     * @return The millisecond part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetMs() const noexcept { return ms_; }

    /**
     * @brief Gets the second component.
     * @return The second part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetSec() const noexcept { return sec_; }

    /**
     * @brief Gets the minute component.
     * @return The minute part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetMin() const noexcept { return min_; }

    /**
     * @brief Gets the hour component.
     * @return The hour part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetHour() const noexcept { return hour_; }

    /**
     * @brief Gets the day component.
     * @return The day part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetDay() const noexcept { return day_; }

    /**
     * @brief Gets the month component.
     * @return The month part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetMonth() const noexcept { return month_; }

    /**
     * @brief Gets the year component.
     * @return The year part of the time.
     */
    NODISCARD FORCEINLINE TimeType GetYear() const noexcept { return year_; }

    /**
     * @brief Updates the time object using the specified raw time.
     * @details Performs a full calculation of all date and time components.
     * @param _time_raw The raw time to update to. Defaults to the current system time.
     */
    Void UpdateTime(TimeType _time_raw = GetTimeNs()) noexcept;

    /**
     * @brief Quickly updates the time object using the specified raw time.
     * @details Optimized for frequent calls. It only recalculates the date if the hour has changed (crossed midnight).
     * @param _time_raw The raw time to update to. Defaults to the current system time.
     */
    Void UpdateTimeFast(TimeType _time_raw = GetTimeNs()) noexcept;

private:
    /**
     * @brief Gets the region time offset in nanoseconds.
     * @return The offset from UTC to the local time zone in nanoseconds.
     */
    static TimeType GetRegionTimeOffsetNs() noexcept;

    /**
     * @brief Updates the time components from nanoseconds up to hours.
     * @details Calculates ns, us, ms, sec, min, and hour components, and returns the total remaining days.
     * @param _time_raw The raw time value in nanoseconds.
     * @return The total number of days derived from the raw time.
     */
    NODISCARD TimeType UpdateNsToHourReturnDay(TimeType _time_raw) noexcept;

    /**
     * @brief Updates the date components (year, month, day) based on the total days.
     * @param _day_time The total number of days since the epoch.
     */
    Void UpdateDateP(TimeType _day_time) noexcept;

private:
    /** @brief The nanosecond component of the time. */
    TimeType ns_;
    /** @brief The microsecond component of the time. */
    TimeType us_;
    /** @brief The millisecond component of the time. */
    TimeType ms_;
    /** @brief The second component of the time. */
    TimeType sec_;
    /** @brief The minute component of the time. */
    TimeType min_;
    /** @brief The hour component of the time. */
    TimeType hour_;
    /** @brief The day component of the time. */
    TimeType day_;
    /** @brief The month component of the time. */
    TimeType month_;
    /** @brief The year component of the time. */
    TimeType year_;
};

} // zengine