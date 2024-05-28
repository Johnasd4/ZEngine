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
#ifndef Z_CORE_Z_SYSTEM_TIME_H_
#define Z_CORE_Z_SYSTEM_TIME_H_

#pragma warning(disable : 26439)

#include "internal/drive.h"

#include <ctime>

namespace zengine {

/*
    The time struct, contains [year, month, day, hour, min ,sec].
*/
class ZSystemTime {
public:
    CORE_DLLAPI ZSystemTime() noexcept;
    CORE_DLLAPI ~ZSystemTime() noexcept;

    NODISCARD FORCEINLINE Int32 Year() noexcept { return year_; }
    NODISCARD FORCEINLINE Int32 Month() noexcept { return month_; }
    NODISCARD FORCEINLINE Int32 Day() noexcept { return day_; }
    NODISCARD FORCEINLINE Int32 Hour() noexcept { return hour_; }
    NODISCARD FORCEINLINE Int32 Min() noexcept { return min_; }
    NODISCARD FORCEINLINE Int32 Sec() noexcept { return sec_; }

    /*
        Update system time, use UpdateTimeFast instead if updated frequently.
        Parameters:
        time_raw: use time(time_t) to get the raw value.
    */
    CORE_DLLAPI Void UpdateTime(Int64 time_raw = time(nullptr)) noexcept;
    /*
        Update system time, use UpdateTime instead if updated only a few times pur day.
        Parameters:
        time_raw: use time(time_t) to get the raw value.
    */
    CORE_DLLAPI Void UpdateTimeFast(Int64 time_raw = time(nullptr)) noexcept;

private:
    Int32 year_;
    Int32 month_;
    Int32 day_;
    Int32 hour_;
    Int32 min_;
    Int32 sec_;
};

}//zengine

#endif // !Z_CORE_Z_SYSTEM_TIME_H_
