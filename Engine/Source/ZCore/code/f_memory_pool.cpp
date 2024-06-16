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
#define CORE_DLLFILE

#include "f_memory_pool.h"
#include "m_log.h"

#include "memory_pool/t_small_memory_block_list_memory_pool.h"

namespace zengine {
namespace memory_pool {

namespace internal {

using MemoryPoolBase = TMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>;

using SmallMemoryBlock = TSmallMemoryBlock<MEMORY_POOL_THREAD_SAFE>;
using SmallMemoryBlockListMemoryPool = TSmallMemoryBlockListMemoryPool<MEMORY_POOL_THREAD_SAFE>;

}


CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType size) noexcept {
    //small memory block
    if (size <= internal::SmallMemoryBlockListMemoryPool::MemoryBlockMemoryMaxSize()) {
        return internal::SmallMemoryBlockListMemoryPool::ApplyMemory(size);
    }
    else {
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeMemorySizeOutOfBound, 0, 
                    "Memory size out of bound! size: %d", size);
        //TODO(Johnasd4):Apply memory from other memory pools.

        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType size, MemoryType* memory_size_ptr) noexcept {
    //small memory blocka
    if (size <= internal::SmallMemoryBlockListMemoryPool::MemoryBlockMemoryMaxSize()){
        return internal::SmallMemoryBlockListMemoryPool::ApplyMemory(size, memory_size_ptr);
    }
    else{
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeMemorySizeOutOfBound, 0, 
                    "Memory size out of bound! size: %d", size);
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI NODISCARD Bool CheckMemory(Void* memory_ptr, MemoryType size) noexcept {
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
        //small memory block
    case MemoryPoolEnum::kTSmallMemoryBlockListMemoryPool:
        return internal::SmallMemoryBlockListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), size);
        break;
    //TODO(Johnasd4):Check memory to other memory pools.
    default:
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeMemorySizeOutOfBound, 0, 
                    "Memory size out of bound! size: %d", size);
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI NODISCARD Bool CheckMemory(Void* memory_ptr, MemoryType size, MemoryType* memory_size_ptr) noexcept {
    if (memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
        //small memory block
    case MemoryPoolEnum::kTSmallMemoryBlockListMemoryPool:
        return internal::SmallMemoryBlockListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), size, memory_size_ptr);
        break;
        //TODO(Johnasd4):Check memory to other memory pools.
    default:
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeMemorySizeOutOfBound, 0, 
                    "Memory size out of bound! size: %d", size);
        exit(EXIT_FAILURE);
        break;
    }
    return false;
}

CORE_DLLAPI NODISCARD MemoryType CalculateMemory(MemoryType size) noexcept {
    //small memory blocka
    if (size <= internal::SmallMemoryBlockListMemoryPool::MemoryBlockMemoryMaxSize()) {
        return internal::SmallMemoryBlockListMemoryPool::CalculateMemory(size);
    }
    else {
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeMemorySizeOutOfBound, 0, 
                    "Memory size out of bound! size: %d", size);
        //TODO(Johnasd4):Apply memory from other memory pools.
        exit(EXIT_FAILURE);
    }
    return 0;
}

CORE_DLLAPI Void ReleaseMemory(Void* memory_ptr) noexcept {
    if (memory_ptr == nullptr) {
        return;
    }
    //Gets the memory pool's pointer that owns the memory block.
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        reinterpret_cast<internal::SmallMemoryBlock*>(memory_ptr)[-1].owner_memory_pool_ptr;
    switch (owner_memory_pool_ptr->PoolType())
    {
        //small memory block
    case MemoryPoolEnum::kTSmallMemoryBlockListMemoryPool:
        internal::SmallMemoryBlockListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryBlockListMemoryPool*>(owner_memory_pool_ptr), memory_ptr);
        break;
        //TODO(Johnasd4):Release memory to other memory pools.
    default:
        Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodePoolTypeUnknown, 0,
                    "Memory pool type unknown! size: %d", owner_memory_pool_ptr->PoolType());
        exit(EXIT_FAILURE);
        break;
    }
}

}//memory_pool
}//zengine
