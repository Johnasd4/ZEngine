#ifndef Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_THREAD_SAFE_BASE_H_
#define Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_THREAD_SAFE_BASE_H_

#include"internal/drive.h"
#include"z_mutex.h"

namespace zengine{
namespace memory_pool {

/*
    Extend this class instead of using a mutex member. You can lock and unlock 
    it as the same as mutex. It will only work when kIsThreadSafe is true and
    do nothing when it's false.
    Template Parameters:
    - kIsThreadSafe: thread safe or not.  
*/
template<Bool kIsThreadSafe>
class ZMemoryPoolThreadSafeBase {
protected:
    FORCEINLINE const Void lock() { if constexpr (kIsThreadSafe) { mutex_.lock(); } }
    FORCEINLINE const Void unlock() { if constexpr (kIsThreadSafe) { mutex_.unlock(); } }

private:
    ZMutex mutex_;
};
template<>
class ZMemoryPoolThreadSafeBase<false>{};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_MEMORY_POOL_THREAD_SAFE_BASE_H_
