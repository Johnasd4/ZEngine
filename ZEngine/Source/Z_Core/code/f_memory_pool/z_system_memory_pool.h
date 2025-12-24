/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "c_memory_pool.h"

namespace zengine {
namespace memory_pool {
namespace internal {

/**
 * @brief Allocates physical memory pages directly from the operating system.
 * @param _size The size of memory to allocate in bytes.
 * @param _alignment_param The alignment exponent parameter (e.g., 21 for 2MB alignment, 16 for 64KB alignment).
 * @return Void* A pointer to the allocated memory block, or nullptr if allocation fails.
 */
NODISCARD FORCEINLINE Void* AllocP(UInt32 _size, UInt32 _alignment_param) {
    // Convert exponent to bytes:
    // If _alignment_param is 21, alignment_bytes becomes 2MB (2097152).
    // If _alignment_param is 16, alignment_bytes becomes 64KB (65536).
    SizeType alignment_bytes = 1ULL << _alignment_param;

#ifdef _WIN32
    // Windows requires MEM_ADDRESS_REQUIREMENTS struct to specify alignment.
    MEM_ADDRESS_REQUIREMENTS address_reqs = { 0 };
    address_reqs.Alignment = alignment_bytes;
    address_reqs.LowestStartingAddress = NULL;
    address_reqs.HighestEndingAddress = NULL;

    MEM_EXTENDED_PARAMETER param = { 0 };
    param.Type = MemExtendedParameterAddressRequirements;
    param.Pointer = &address_reqs; // Pointer to the struct

    return VirtualAlloc2(
        NULL,           // process handle
        NULL,           // base address
        _size,          // size
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE,
        &param,         // extend parameter pointer
        1               // parameter count
    );
#else
    Void* mem_ptr = nullptr;
    // posix_memalign expects alignment in bytes.
    if (posix_memalign(&mem_ptr, alignment_bytes, _size) != 0) {
        return nullptr;
    }
    return mem_ptr;
#endif
}

/**
 * @brief Frees physical memory pages previously allocated by internal allocators.
 * @param _memory_ptr Pointer to the memory block to free.
 */
FORCEINLINE Void FreeP(Void* _memory_ptr) {
#ifdef _WIN32
    VirtualFree(_memory_ptr, 0, MEM_RELEASE);
#else
    free(_memory_ptr);
#endif
}

}//internal
}//memory_pool
}//zengine

namespace zengine {
namespace memory_pool {

class ZSystemMemoryPool {
public:
    FORCEINLINE static ZSystemMemoryPool& Instance() noexcept {
        static ZSystemMemoryPool instance;
        return instance;
    }

    /**
     * @brief Allocates a specific number of standard memory pages.
     * @param _page_num The number of pages to allocate.
     * @return Void* A pointer to the allocated memory block, or nullptr if allocation fails.
     */
    NODISCARD FORCEINLINE Void* Alloc64KBAlignmentMemoryAtomPageP(UInt32 _page_num) {
        return internal::AllocP(_page_num * kMemoryAtomPageSize, kMemoryPageAlignmentParam);
    }

    /**
     * @brief Allocates a specific number of large memory pages, automatically aligning to the large page size.
     * @param _page_num The number of large pages to allocate.
     * @return Void* A pointer to the allocated memory block, or nullptr if allocation fails.
     */
    NODISCARD FORCEINLINE Void* Alloc2MBAlignmentMemoryAtomPageP(UInt32 _page_num) {
        //align size to the next multiple of the alignment
        return internal::AllocP(_page_num * kMemoryAtomPageSize, kLargeMemoryPageAlignmentParam);
    }

    /**
     * @brief Frees a memory block allocated by AllocMemoryPageP or AllocLargeMemoryPageP.
     * @param _memory_ptr Pointer to the memory block to free.
     */
    FORCEINLINE Void FreeMemoryP(Void* _memory_ptr) {
        return internal::FreeP(_memory_ptr);
    }

private:
    ~ZSystemMemoryPool() noexcept;
    ZSystemMemoryPool() noexcept;
};

}//memory_pool
}//zengine