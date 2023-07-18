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
    NODISCARD FORCEINLINE const MemoryPoolType memory_pool_type() const { return memory_pool_type_; }

protected:
    FORCEINLINE ZMemoryPoolBase() {}

    FORCEINLINE Void Initialize(const MemoryPoolType memory_pool_type) { memory_pool_type_ = memory_pool_type; }

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
