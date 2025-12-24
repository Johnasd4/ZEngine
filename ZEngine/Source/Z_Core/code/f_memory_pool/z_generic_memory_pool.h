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

#include "z_mutex.h"

#include "z_system_memory_pool.h"

namespace zengine {
namespace memory_pool {

/**
 * @brief A generic memory pool class that manages system memory allocation and deallocation.
 * 
 * This class inherits from ZMemoryPool and provides mechanisms to allocate and release 
 * memory pages from the system. It maintains a linked list of memory pages and ensures 
 * thread safety via a mutex.
 */
class ZGenericMemoryPool {
public:
    /** @brief The header structure for a memory page managed by the pool. */
    struct ALIGN(16) MemoryPageHead_ {
        /** @brief Pointer to the memory pool that owns this page. */
        Void* memory_pool_ptr_;
        /** @brief Pointer to the next memory page in the linked list. */
        MemoryPageHead_* next_page_ptr_;
    };

public:
    /**
     * @brief Gets the thread-local instance of the generic memory pool.
     * @return ZGenericMemoryPool& Reference to the thread-local memory pool instance.
     */
    FORCEINLINE static ZGenericMemoryPool& ThreadLocalInstance() noexcept {
        static ZGenericMemoryPool instance;
        return instance;
    }

    /**
     * @brief Default constructor.
     *
     * Initializes the head memory page pointer to nullptr.
     */
    FORCEINLINE ZGenericMemoryPool() noexcept
        : head_memory_page_ptr_(nullptr)
    {}

    /**
     * @brief Destructor.
     *
     * Iterates through the linked list of memory pages and releases all allocated system memory.
     */
    FORCEINLINE ~ZGenericMemoryPool() noexcept
    {
        MemoryPageHead_* current_page_ptr = head_memory_page_ptr_;
        while (current_page_ptr != nullptr) {
            MemoryPageHead_* next_page_ptr = current_page_ptr->next_page_ptr_;
            ZSystemMemoryPool::Instance().FreeMemoryP(current_page_ptr);
            current_page_ptr = next_page_ptr;
        }
        head_memory_page_ptr_ = nullptr;
    }

    /**
     * @brief Applies for a specific number of memory pages from the system with 64KB alignment.
     * 
     * Allocates memory, initializes the page header, and inserts it into the head of the linked list.
     * 
     * @param _page_num The number of pages to allocate.
     * @return MemoryPageHead_* Pointer to the allocated memory page header, or nullptr if allocation fails.
     */
    NODISCARD MemoryPageHead_* Alloc64KBAlignmentMemoryAtomPageP(UInt32 _page_num) noexcept;

    /**
     * @brief Applies for a specific number of memory pages from the system with 2MB alignment.
     *
     * Allocates memory, initializes the page header, and inserts it into the head of the linked list.
     *
     * @param _page_num The number of pages to allocate.
     * @return MemoryPageHead_* Pointer to the allocated memory page header, or nullptr if allocation fails.
     */
    NODISCARD MemoryPageHead_* Alloc2MBAlignmentMemoryAtomPageP(UInt32 _page_num) noexcept;

private:
    ZGenericMemoryPool(const ZGenericMemoryPool&) = delete;
    ZGenericMemoryPool(ZGenericMemoryPool&&) = delete;
    ZGenericMemoryPool& operator=(const ZGenericMemoryPool&) = delete;
    ZGenericMemoryPool& operator=(ZGenericMemoryPool&&) = delete;

    /** @brief Pointer to the first memory page in the linked list. */
    MemoryPageHead_* head_memory_page_ptr_;
};

}//memory_pool
}//zengine