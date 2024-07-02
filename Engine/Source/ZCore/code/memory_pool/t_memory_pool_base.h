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
#ifndef Z_CORE_MEMORY_POOL_T_MEMORY_POOL_BASE_H_
#define Z_CORE_MEMORY_POOL_T_MEMORY_POOL_BASE_H_

#include "internal/z_drive.h"

#include "f_memory_pool.h"
#include "m_log.h"
#include "z_file.h"

#include "t_memory_pool_thread_safe_base.h"

namespace zengine {
namespace memory_pool {

enum MemoryPoolEnum : IndexType {
    kTSmallMemoryListMemoryPool,
    kTSmartPointerListMemoryPool
};

/*
    The base type of all memory pool. The member kPoolType is to find out which
    kind of pool it is.
    Template Parameters:
    - kIsThreadSafe: thread safe or not.
*/
template<Bool kIsThreadSafe>
class TMemoryPoolBase :public TMemoryPoolThreadSafeBase<kIsThreadSafe> {
public:
    static constexpr IndexType kMaxLogLength = 4096;

    NODISCARD FORCEINLINE MemoryPoolEnum PoolType() const noexcept { return pool_type_; }

protected:
    virtual Void OutputLogString(TFixedString<kMaxLogLength> _str) noexcept {
        _str[0] = '\0';
    }

    FORCEINLINE TMemoryPoolBase() noexcept : pool_type_() {}

    ~TMemoryPoolBase() noexcept {
        static ZFile& file = []() -> ZFile& {
            static ZFile file;
            ReturnType link_code = kOK;
            TWFixedString<ZFile::kFileNameLength> file_str;
            ZSystemTime system_time;

            file_str.SetString(
                L"%ls%04d%02d%02d%02d%02d%02d_memory.log", log::ZLog::kPathTString,
                system_time.Year(), system_time.Month(), system_time.Day(),
                system_time.Hour(), system_time.Min(), system_time.Sec());
            link_code = file.OpenSafe(log::ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendT);
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, "ZFile::OpenSafe() link error!");
            }
            return file;
        }();
        static TFixedString<kMaxLogLength> log_str;

        file.Print(log_str.DataPtr());
    }

    FORCEINLINE Void InitializeP(MemoryPoolEnum _pool_type) noexcept { pool_type_ = _pool_type; }

private:
    TMemoryPoolBase(const TMemoryPoolBase&) = delete;
    TMemoryPoolBase(TMemoryPoolBase&&) = delete;

    TMemoryPoolBase& operator=(const TMemoryPoolBase&) = delete;
    TMemoryPoolBase& operator=(TMemoryPoolBase&&) = delete;

    MemoryPoolEnum pool_type_;
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_T_MEMORY_POOL_BASE_H_
