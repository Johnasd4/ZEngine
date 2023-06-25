#define CORE_DLLFILE

#include"f_memory_pool.h"

#include"memory_pool/z_small_memory_piece_list_memory_pool.h"

namespace zengine {
namespace memory_pool {

using SmallMemoryPieceListMemoryPool = ZSmallMemoryPieceListMemoryPool<MEMORY_POOL_THREAD_SAFE>;
using MemoryPoolBase = ZMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>;

CORE_DLLAPI NODISCARD const Address ApplyMemory(const MemoryType size) noexcept {
    //small memory piece
    if (size <= SmallMemoryPieceListMemoryPool::memory_piece_memory_max_size()){
        return SmallMemoryPieceListMemoryPool::ApplyMemory(size);
    }
    else{
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return nullptr;
}

CORE_DLLAPI NODISCARD const Address ApplyMemory(const MemoryType size, MemoryType* max_size_ptr) noexcept {
    //small memory piece
    if (size <= SmallMemoryPieceListMemoryPool::memory_piece_memory_max_size()) {
        return SmallMemoryPieceListMemoryPool::ApplyMemory(size, max_size_ptr);
    }
    else {
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return nullptr;
}



CORE_DLLAPI Void ReleaseMemory(const Address address) noexcept{
    MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<MemoryPoolBase**>(reinterpret_cast<AddressType>(address) - sizeof(Address));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory piece
    case MemoryPoolType::kZSmallMemoryPieceListMemoryPool:
        SmallMemoryPieceListMemoryPool::ReleaseMemory(
            address, static_cast<SmallMemoryPieceListMemoryPool*>(owner_memory_pool_ptr));
        break;
    //TODO(Johnasd4):Release memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
}

CORE_DLLAPI NODISCARD const Bool CheckMemory(const MemoryType size, const Address address) noexcept {
    MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<MemoryPoolBase**>(reinterpret_cast<AddressType>(address) - sizeof(Address));
    switch (owner_memory_pool_ptr->memory_pool_type())
    {
        //small memory piece
    case MemoryPoolType::kZSmallMemoryPieceListMemoryPool:
        return SmallMemoryPieceListMemoryPool::CheckMemory(
            size, static_cast<SmallMemoryPieceListMemoryPool*>(owner_memory_pool_ptr));
        break;
    //TODO(Johnasd4):Check memory to other memory pools.
    default:
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

}//memory_pool
}//zengine
