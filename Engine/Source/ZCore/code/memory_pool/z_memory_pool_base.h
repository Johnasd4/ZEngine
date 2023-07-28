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
#ifndef Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_
#define Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_

#include "internal/drive.h"

#include "z_memory_pool_thread_safe_base.h"

namespace zengine {
namespace memory_pool {

enum MemoryPoolType : IndexType {
    kZSmallMemoryBlockListMemoryPool = 1
};

/*
    The base type of all memory pool. The member kPoolType is to find out which
    kind of pool it is.
    Template Parameters:
    - kIsThreadSafe: thread safe or not.
*/
template<Bool kIsThreadSafe>
class ZMemoryPoolBase :public ZMemoryPoolThreadSafeBase<kIsThreadSafe> {
public:
    NODISCARD FORCEINLINE MemoryPoolType memory_pool_type() const { return memory_pool_type_; }

protected:
    FORCEINLINE ZMemoryPoolBase() {}

    FORCEINLINE Void InitializeP(MemoryPoolType memory_pool_type) { memory_pool_type_ = memory_pool_type; }

private:
    ZMemoryPoolBase(const ZMemoryPoolBase&) = delete;
    ZMemoryPoolBase(ZMemoryPoolBase&&) = delete;

    ZMemoryPoolBase& operator=(const ZMemoryPoolBase&) = delete;
    ZMemoryPoolBase& operator=(ZMemoryPoolBase&&) = delete;

    MemoryPoolType memory_pool_type_;
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_
