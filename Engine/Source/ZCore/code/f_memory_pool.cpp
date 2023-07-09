#define CORE_DLLFILE

#include"f_memory_pool.h"

#include"memory_pool/z_small_memory_piece_list_memory_pool.h"

namespace zengine {
namespace memory_pool {

namespace internal {

using SmallMemoryPieceListMemoryPool = ZSmallMemoryPieceListMemoryPool<MEMORY_POOL_THREAD_SAFE>;
using MemoryPoolBase = ZMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>;

}


CORE_DLLAPI NODISCARD Void* ApplyMemory(const MemoryType size) noexcept {
    //small memory piece
    if (size <= internal::SmallMemoryPieceListMemoryPool::memory_piece_memory_max_size()) {
        return internal::SmallMemoryPieceListMemoryPool::ApplyMemory(size);
    }
    else {
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD Void* ApplyMemory(const MemoryType size, MemoryType* memory_size_ptr) noexcept {
    //small memory piecea
    if (size <= internal::SmallMemoryPieceListMemoryPool::memory_piece_memory_max_size()){
        return internal::SmallMemoryPieceListMemoryPool::ApplyMemory(size, memory_size_ptr);
    }
    else{
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD const Bool CheckMemory(const Void* memory_ptr, const MemoryType size) noexcept {
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory piece
    case MemoryPoolType::kZSmallMemoryPieceListMemoryPool:
        return internal::SmallMemoryPieceListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryPieceListMemoryPool*>(owner_memory_pool_ptr), size);
        break;
    //TODO(Johnasd4):Check memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI NODISCARD const Bool CheckMemory(const Void* memory_ptr, const MemoryType size,
                                             MemoryType* memory_size_ptr) noexcept {
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory piece
    case MemoryPoolType::kZSmallMemoryPieceListMemoryPool:
        return internal::SmallMemoryPieceListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryPieceListMemoryPool*>(owner_memory_pool_ptr), size, memory_size_ptr);
        break;
        //TODO(Johnasd4):Check memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI Void ReleaseMemory(const Void* memory_ptr) noexcept {
    if (memory_ptr == nullptr) {
        return;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory piece
    case MemoryPoolType::kZSmallMemoryPieceListMemoryPool:
        internal::SmallMemoryPieceListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryPieceListMemoryPool*>(owner_memory_pool_ptr), memory_ptr);
        break;
        //TODO(Johnasd4):Release memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
}

}//memory_pool
}//zengine
