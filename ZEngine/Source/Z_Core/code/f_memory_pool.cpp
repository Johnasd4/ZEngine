/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "f_memory_pool.h"

#include "m_log.h"

#include "memory_pool/t_small_memory_list_memory_pool.h"
#include "memory_pool/t_smart_pointer_list_memory_pool.h"
#include "memory_pool/t_system_memory_pool.h"

namespace zengine {
namespace memory_pool {

namespace internal {

using MemoryPoolBase = TMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>;

using SmallMemoryBlock = TSmallMemoryBlock<MEMORY_POOL_THREAD_SAFE>;
using SystemMemoryBlock = TSystemMemoryBlock<MEMORY_POOL_THREAD_SAFE>;
using SmallMemoryListMemoryPool = TSmallMemoryListMemoryPool<MEMORY_POOL_THREAD_SAFE>;
using SmartPointerListMemoryPool = TSmartPointerListMemoryPool<MEMORY_POOL_THREAD_SAFE>;
using SystemMemoryPool = TSystemMemoryPool<MEMORY_POOL_THREAD_SAFE>;

}

CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType _size) noexcept {
    //small memory block
    if (_size <= internal::SmallMemoryListMemoryPool::MemoryBlockMemoryMaxSize()) {
        return internal::SmallMemoryListMemoryPool::ApplyMemory(_size);
    }
    //system memory pool
    else {
        return internal::SystemMemoryPool::ApplyMemory(_size);
    }
}

CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType _size, MemoryType* _memory_size_ptr) noexcept {
    //small memory block
    if (_size <= internal::SmallMemoryListMemoryPool::MemoryBlockMemoryMaxSize()){
        return internal::SmallMemoryListMemoryPool::ApplyMemory(_size, _memory_size_ptr);
    }
    //system memory pool
    else{
        *_memory_size_ptr = _size;
        return internal::SystemMemoryPool::ApplyMemory(_size);
    }
}

CORE_DLLAPI NODISCARD Void* ReapplyMemory(Void* _old_memory_ptr, MemoryType _size) noexcept {
    if (_old_memory_ptr == nullptr) {
        return ApplyMemory(_size);
    }
    //Gets the memory pool's pointer that owns the memory block.
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(_old_memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
    //small memory block
    case MemoryPoolEnum::kMemoryPool_TSmallMemoryList:
    {
        if (CheckMemory(_old_memory_ptr, _size)) {
            return _old_memory_ptr;
        }
        Void* new_memory_ptr = ApplyMemory(_size);
        memcpy(
            new_memory_ptr, _old_memory_ptr, 
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr)->MemoryBlockMemorySize());

        internal::SmallMemoryListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr), _old_memory_ptr);

        return new_memory_ptr;
        break;
    }
    //system memory pool
    default:
        return internal::SystemMemoryPool::RepplyMemory(_old_memory_ptr, _size);
    }
}

CORE_DLLAPI NODISCARD Void* ReapplyMemory(
    Void* _old_memory_ptr,
    MemoryType _size,
    MemoryType* _memory_size_ptr
) noexcept {
    //Gets the memory pool's pointer that owns the memory block.
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(_old_memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
    //small memory block
    case MemoryPoolEnum::kMemoryPool_TSmallMemoryList:
    {
        if (CheckMemory(_old_memory_ptr, _size, _memory_size_ptr)) {
            return _old_memory_ptr;
        }
        Void* new_memory_ptr = ApplyMemory(_size, _memory_size_ptr);

        memcpy(
            new_memory_ptr, _old_memory_ptr,
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr)->MemoryBlockMemorySize());

        internal::SmallMemoryListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr), _old_memory_ptr);

        return new_memory_ptr;
        break;
    }
    //system memory pool
    default:
        *_memory_size_ptr = _size;
        return internal::SystemMemoryPool::RepplyMemory(_old_memory_ptr, _size);
    }
}

CORE_DLLAPI NODISCARD Bool CheckMemory(Void* _memory_ptr, MemoryType _size) noexcept {
    if (_memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(_memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
    //small memory block
    case MemoryPoolEnum::kMemoryPool_TSmallMemoryList:
        return internal::SmallMemoryListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr), _size);
        break;
    //system memory pool
    default:
        return internal::SystemMemoryPool::CheckMemory(_size);
    }
}

CORE_DLLAPI NODISCARD Bool CheckMemory(Void* _memory_ptr, MemoryType _size, MemoryType* _memory_size_ptr) noexcept {
    if (_memory_ptr == nullptr) {
        return false;
    }
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(_memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
    //small memory block
    case MemoryPoolEnum::kMemoryPool_TSmallMemoryList:
        return internal::SmallMemoryListMemoryPool::CheckMemory(
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr), _size, _memory_size_ptr);
        break;
    //system memory pool
    default:
        *_memory_size_ptr = 0U;
        return internal::SystemMemoryPool::CheckMemory(_size);
    }
}

CORE_DLLAPI NODISCARD MemoryType CalculateMemory(MemoryType _size) noexcept {
    //small memory block
    if (_size <= internal::SmallMemoryListMemoryPool::MemoryBlockMemoryMaxSize()) {
        return internal::SmallMemoryListMemoryPool::CalculateMemory(_size);
    }
    //system memory pool
    else {
        return internal::SystemMemoryPool::CalculateMemory(_size);
    }
}

CORE_DLLAPI Void ReleaseMemory(Void* _memory_ptr) noexcept {
    if (_memory_ptr == nullptr) {
        return;
    }
    //Gets the memory pool's pointer that owns the memory block.
    internal::MemoryPoolBase* owner_memory_pool_ptr =
        *reinterpret_cast<internal::MemoryPoolBase**>(reinterpret_cast<PointerType>(_memory_ptr) - sizeof(Void*));
    switch (owner_memory_pool_ptr->PoolType())
    {
    //small memory block
    case MemoryPoolEnum::kMemoryPool_TSmallMemoryList:
        internal::SmallMemoryListMemoryPool::ReleaseMemory(
            static_cast<internal::SmallMemoryListMemoryPool*>(owner_memory_pool_ptr), _memory_ptr);
        break;
    //system memory pool
    default:
        internal::SystemMemoryPool::ReleaseMemory(_memory_ptr);
        break;
    }
}

CORE_DLLAPI NODISCARD Void* ApplySmartPointerMemory() noexcept {
    return internal::SmartPointerListMemoryPool::ApplyMemory();
}

CORE_DLLAPI Void ReleaseSmartPointerMemory(Void* _memory_ptr) noexcept {
    internal::SmartPointerListMemoryPool::ReleaseMemory(_memory_ptr);
}

#ifdef USE_MEMORY_POOL_TEST
CORE_DLLAPI NODISCARD decltype(auto) SmallMemoryListMemoryPoolMemoryBlockUsedNum() noexcept {
    return internal::SmallMemoryListMemoryPool::MemoryBlockUsedNum();
}
CORE_DLLAPI NODISCARD decltype(auto) SmartPointerListMemoryPoolMemoryBlockUsedNum() noexcept {
    return internal::SmartPointerListMemoryPool::MemoryBlockUsedNum();
}
#endif //USE_MEMORY_POOL_TEST

}//memory_pool
}//zengine
