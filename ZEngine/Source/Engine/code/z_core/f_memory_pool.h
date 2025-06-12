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
#pragma once

#include "internal/z_drive.h"

namespace zengine {

namespace error_code {

enum FMemoryPoolErrorCode : ReturnType {
    kFMemoryPoolErrorCode_LinkError = kErrorCodeBase_FMemoryPool,
    kFMemoryPoolErrorCode_MemorySizeOutOfBound,
    kFMemoryPoolErrorCode_PoolTypeUnknown,
    kFMemoryPoolErrorCode_ApplyHeapMemoryFailed,
};

}//error_code

/*
    The namespace contains the function that controls the heap memory.
    It has it's own garbage collection.
*/
namespace memory_pool {

/*
    Applys memory by the size given and returns the pointer of the memory.
*/
CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType _size) noexcept;

/*
    Applys memory by the size given and returns the pointer of the memory.
    Returns the memory size of the memory block.
*/
CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType _size, MemoryType* _memory_size_ptr) noexcept;

/*
    Reapply the memory and copy the data to the new memory.
*/
CORE_DLLAPI NODISCARD Void* ReapplyMemory(Void* _old_memory_ptr, MemoryType _size) noexcept;

/*
    Reapply the memory and copy the data to the new memory.
*/
CORE_DLLAPI NODISCARD Void* ReapplyMemory(
    Void* _old_memory_ptr, 
    MemoryType _size, 
    MemoryType* _memory_size_ptr
) noexcept;

/*
    Check if the memory can be extended to the given size without changing the memory,
    if can then the memeory will extend and return true.
*/
CORE_DLLAPI NODISCARD Bool CheckMemory(Void* _memory_ptr, MemoryType _size) noexcept;

/*
    Check if the memory can be extended to the given size without changing the memory,
    if can then the memeory will extend and return true.
*/
CORE_DLLAPI NODISCARD Bool CheckMemory(
    Void* _memory_ptr, 
    MemoryType _size, 
    MemoryType* _memory_size_ptr
) noexcept;

/*
    Calculate the minimum size of the memory block that satisfies the memory requirement.
*/
CORE_DLLAPI NODISCARD MemoryType CalculateMemory(MemoryType _size) noexcept;

/*
   Release the memory by the memory pointer, it's a value parameter,
   so remember to set the pointer to null.
   nullptr is allowed to release.
*/
CORE_DLLAPI Void ReleaseMemory(Void* _memory_ptr) noexcept;

/*
    Applys memory for the smart pointers, size is 24.
*/
CORE_DLLAPI NODISCARD Void* ApplySmartPointerMemory() noexcept;

/*
   Release the memory by the memory pointer. Only can be used by smart pointers.
*/
CORE_DLLAPI Void ReleaseSmartPointerMemory(Void* _memory_ptr) noexcept;

#ifdef USE_MEMORY_POOL_TEST
/*
    Gets the memory pool current condition.
*/
CORE_DLLAPI NODISCARD decltype(auto) SmallMemoryListMemoryPoolMemoryBlockUsedNum() noexcept;
/*
    Gets the memory pool current condition.
*/
CORE_DLLAPI NODISCARD decltype(auto) SmartPointerListMemoryPoolMemoryBlockUsedNum() noexcept;
#endif //USE_MEMORY_POOL_TEST

}//memory_pool
}//zengine