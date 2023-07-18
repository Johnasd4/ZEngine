#define CORE_DLLFILE

#include "f_memory_pool.h"

#include "m_error_message.h"

#include "memory_pool/z_small_memory_block_list_memory_pool.h"

namespace zengine {
namespace memory_pool {

namespace internal {

using MemoryPoolBase = ZMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>;

using SmallMemoryBlock = ZSmallMemoryBlock<MEMORY_POOL_THREAD_SAFE>;
using SmallMemoryBlockListMemoryPool = ZSmallMemoryBlockListMemoryPool<MEMORY_POOL_THREAD_SAFE>;

}


CORE_DLLAPI NODISCARD Void* const ApplyMemory(const MemoryType size) noexcept {
    DEBUG(size < 0, "Negaive size not valid!");
    //small memory block
    if (size <= internal::SmallMemoryBlockListMemoryPool::memory_block_memory_max_size()) {
        return internal::SmallMemoryBlockListMemoryPool::ApplyMemory(size);
    }
    else {
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD Void* const ApplyMemory(const MemoryType size, MemoryType* const memory_size_ptr) noexcept {
    DEBUG(size < 0, "Negaive size not valid!");
    //small memory blocka
    if (size <= internal::SmallMemoryBlockListMemoryPool::memory_block_memory_max_size()){
        return internal::SmallMemoryBlockListMemoryPool::ApplyMemory(size, memory_size_ptr);
    }
    else{
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD const Bool CheckMemory(Void* const memory_ptr, const MemoryType size) noexcept {
    DEBUG(size < 0, "Negaive size not valid!");
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* const owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory block
    case MemoryPoolType::kZSmallMemoryBlockListMemoryPool:
        return internal::SmallMemoryBlockListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), size);
        break;
    //TODO(Johnasd4):Check memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI NODISCARD const Bool CheckMemory(Void* const memory_ptr, const MemoryType size,
                                             MemoryType* const memory_size_ptr) noexcept {
    DEBUG(size < 0, "Negaive size not valid!");
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* const owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory block
    case MemoryPoolType::kZSmallMemoryBlockListMemoryPool:
        return internal::SmallMemoryBlockListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), size, memory_size_ptr);
        break;
        //TODO(Johnasd4):Check memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI NODISCARD const MemoryType CalculateMemory(const MemoryType size) noexcept {
    DEBUG(size < 0, "Negaive size not valid!");
    //small memory blocka
    if (size <= internal::SmallMemoryBlockListMemoryPool::memory_block_memory_max_size()) {
        return internal::SmallMemoryBlockListMemoryPool::CalculateMemory(size);
    }
    else {
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI Void ReleaseMemory(Void* const memory_ptr) noexcept {
    if (memory_ptr == nullptr) {
        return;
    }
    //Gets the memory pool's pointer that owns the memory block.
    internal::MemoryPoolBase* const owner_memory_pool_ptr =
        reinterpret_cast<internal::SmallMemoryBlock*>(memory_ptr)[-1].owner_memory_pool_ptr;
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory block
    case MemoryPoolType::kZSmallMemoryBlockListMemoryPool:
        internal::SmallMemoryBlockListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), memory_ptr);
        break;
        //TODO(Johnasd4):Release memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
}

}//memory_pool
}//zengine
