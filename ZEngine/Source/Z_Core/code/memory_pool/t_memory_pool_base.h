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
#include "z_string.h"

#include "t_memory_pool_thread_safe_base.h"

namespace zengine {
namespace memory_pool {

enum MemoryPoolEnum : IndexType {
    kMemoryPool_TSmallMemoryList,
    kWindowState_TSmartPointerList
};

/*
    The base type of all memory pool. The member kPoolType is to find out which
    kind of pool it is.
    Template Parameters:
    - kIsThreadSafe: thread safe or not.
*/
template<Bool kIsThreadSafe>
class TMemoryPoolBase : public TMemoryPoolThreadSafeBase<kIsThreadSafe> {
public:
    static constexpr IndexType kMaxLogLength = 4096;

    NODISCARD FORCEINLINE MemoryPoolEnum PoolType() const noexcept { return pool_type_; }

protected:
    FORCEINLINE TMemoryPoolBase() noexcept : pool_type_() {}

    ~TMemoryPoolBase() noexcept {}

    FORCEINLINE Void InitializeP(MemoryPoolEnum _pool_type) noexcept { pool_type_ = _pool_type; }

#ifdef USE_MEMORY_POOL_TEST
    inline static ZFile& log_file_ = []() -> ZFile& {
        static ZFile file;
        TWFixedString<ZFile::kFileNameLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.SetString(
            L"%ls\\%04d%02d%02d%02d%02d%02d_memory.log", log::ZLog::CreateAndGetLogPath(),
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec());
        ReturnType link_code = file.Open(file_dir.String(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
        }
        return file;
    }();
#endif //USE_MEMORY_POOL_TEST

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
