#ifndef Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_
#define Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_

#include"internal/drive.h"

#include"z_memory_pool_thread_safe_base.h"

namespace zengine {
namespace memory_pool {

enum MemoryPoolType : IndexType {
    kZSmallMemoryPieceListMemoryPool = 1
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
    FORCEINLINE const MemoryPoolType memory_pool_type() const { return kMemoryPoolType; }

protected:
    static constexpr MemoryPoolType RegisterMemoryPoolType() { 
        return ZMemoryPoolBase<kIsThreadSafe>::kMemoryPoolTypeCounter.next();
    }

    FORCEINLINE explicit ZMemoryPoolBase(const MemoryPoolType memory_pool_type) :kMemoryPoolType(memory_pool_type) {}

private:
    const MemoryPoolType kMemoryPoolType;
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_BASE_H_
