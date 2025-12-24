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

#include "t_fixed_array.h"
#include "t_lock_guard.h"

#include "c_memory_pool.h"
#include "z_generic_memory_pool.h"

namespace zengine {

namespace memory_pool {

/** @brief A global memory pool manager for large memory blocks (typically > 2MB) directly allocated from the system. */
class CACHE_LINE_ALIGN ZGenericSystemMemoryPool {
public:
    /** @brief Header structure pre-pended to every large memory block for tracking purposes. */
    struct CACHE_LINE_ALIGN MemoryBlockHead_ {
        /** @brief Padding to ensure cache line alignment. */
        Byte padding_[CACHE_LINE_SIZE - sizeof(ZGenericMemoryPool::MemoryPageHead_)];
        /** @brief Pointer back to the owning pool (useful for debugging or generic release). */
        ZGenericSystemMemoryPool* memory_pool_ptr_;
        /** @brief The total size of the allocated block (including this header). */
        SizeType memory_block_size_;
    };

public:
    /** @brief Retrieves the global singleton instance of the system memory pool. */
    FORCEINLINE static ZGenericSystemMemoryPool& Instance() noexcept {
        static ZGenericSystemMemoryPool instance;
        return instance;
    }

    /** @brief Constructs the system memory pool. */
    ZGenericSystemMemoryPool() noexcept;

    /** @brief Destroys the pool. Note: Does not automatically free leaked memory to avoid shutdown order issues. */
    ~ZGenericSystemMemoryPool() noexcept;

    /**
     * @brief Calculates the actual memory block size for a requested size.
     * @param _memory_size The requested size of memory.
     * @return The actual size of the memory block that will be allocated.
     */
    NODISCARD FORCEINLINE SizeType CalculateMemory(SizeType _memory_size) noexcept {
        //apply memory from system (Size + Header)
        UInt32 apply_page_num = static_cast<UInt32>(
            ((_memory_size + sizeof(MemoryBlockHead_) + static_cast<SizeType>(kMemoryAtomPageMask))) /
            kMemoryAtomPageSize
        );
        return static_cast<SizeType>(apply_page_num) * static_cast<SizeType>(kMemoryAtomPageSize);
    }

    /** @brief Allocates a large memory block directly from the system. */
    NODISCARD Void* ApplyMemory(SizeType _size) noexcept {
        //apply memory from system (Size + Header)
        UInt32 apply_page_num = static_cast<UInt32>(
            ((_size + sizeof(MemoryBlockHead_) + static_cast<SizeType>(kMemoryAtomPageMask))) /
            static_cast<SizeType>(kMemoryAtomPageSize)
        );
        SizeType apply_size = static_cast<SizeType>(apply_page_num) * static_cast<SizeType>(kMemoryAtomPageSize);
        Void* raw_memory_ptr = ZSystemMemoryPool::Instance().Alloc2MBAlignmentMemoryAtomPageP(apply_page_num);
        if (raw_memory_ptr == nullptr) {
            return nullptr;
        }
        //initialize memory page head
        MemoryBlockHead_* memory_block_head_ptr = static_cast<MemoryBlockHead_*>(raw_memory_ptr);
        memory_block_head_ptr->memory_pool_ptr_ = this;
        memory_block_head_ptr->memory_block_size_ = apply_size;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        {
            TLockGuard<ZMutex> lock_guard(memory_performance_mutex_);
            memory_block_unreleased_num_ += 1U;
            memory_block_unreleased_size_ += apply_size;
            memory_block_applied_num_ += 1U;
            memory_block_applied_size_ += apply_size;
            if (memory_block_peak_num_ < memory_block_unreleased_num_) {
                memory_block_peak_num_ = memory_block_unreleased_num_;
            }
            if (memory_block_peak_size_ < memory_block_unreleased_size_) {
                memory_block_peak_size_ = memory_block_unreleased_size_;
            }
        }
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST

        // Return the pointer after the header
        return memory_block_head_ptr + 1ULL;
    }

    /** @brief Allocates a large memory block directly from the system. */
    NODISCARD Void* ApplyMemory(SizeType _size, SizeType* _memory_size_ptr) noexcept {
        //apply memory from system (Size + Header)
        UInt32 apply_page_num = static_cast<UInt32>(
            ((_size + sizeof(MemoryBlockHead_) + static_cast<SizeType>(kMemoryAtomPageMask))) /
            static_cast<SizeType>(kMemoryAtomPageSize)
        );
        SizeType apply_size = static_cast<SizeType>(apply_page_num) * static_cast<SizeType>(kMemoryAtomPageSize);
        Void* raw_memory_ptr = ZSystemMemoryPool::Instance().Alloc2MBAlignmentMemoryAtomPageP(apply_page_num);
        if (raw_memory_ptr == nullptr) {
            return nullptr;
        }
        //initialize memory page head
        MemoryBlockHead_* memory_block_head_ptr = static_cast<MemoryBlockHead_*>(raw_memory_ptr);
        memory_block_head_ptr->memory_pool_ptr_ = this;
        memory_block_head_ptr->memory_block_size_ = apply_size;
        *_memory_size_ptr = apply_size - sizeof(MemoryBlockHead_);

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        {
            TLockGuard<ZMutex> lock_guard(memory_performance_mutex_);
            memory_block_unreleased_num_ += 1U;
            memory_block_unreleased_size_ += apply_size;
            memory_block_applied_num_ += 1U;
            memory_block_applied_size_ += apply_size;
            if (memory_block_peak_num_ < memory_block_unreleased_num_) {
                memory_block_peak_num_ = memory_block_unreleased_num_;
            }
            if (memory_block_peak_size_ < memory_block_unreleased_size_) {
                memory_block_peak_size_ = memory_block_unreleased_size_;
            }
        }
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST

        // Return the pointer after the header
        return memory_block_head_ptr + 1ULL;
    }

    /** @brief Releases a large memory block back to the system. */
    FORCEINLINE Void ReleaseMemory(Void* _memory_ptr) noexcept {
        MemoryBlockHead_* memory_block_head_ptr = static_cast<MemoryBlockHead_*>(_memory_ptr) - 1ULL;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        {
            TLockGuard<ZMutex> lock_guard(memory_performance_mutex_);
            memory_block_unreleased_num_ -= 1U;
            memory_block_unreleased_size_ -= memory_block_head_ptr->memory_block_size_;
        }
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST

        ZSystemMemoryPool::Instance().FreeMemoryP(memory_block_head_ptr);
    }

private:
    ZGenericSystemMemoryPool(const ZGenericSystemMemoryPool&) = delete;
    ZGenericSystemMemoryPool(ZGenericSystemMemoryPool&&) = delete;
    ZGenericSystemMemoryPool& operator=(const ZGenericSystemMemoryPool&) = delete;
    ZGenericSystemMemoryPool& operator=(ZGenericSystemMemoryPool&&) = delete;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
    /** @brief Mutex to protect performance statistics. */
    ZMutex memory_performance_mutex_;
    /** @brief Number of blocks currently allocated. */
    UInt32 memory_block_unreleased_num_;
    /** @brief Total number of allocations performed. */
    UInt32 memory_block_applied_num_;
    /** @brief Peak number of concurrent blocks allocated. */
    UInt32 memory_block_peak_num_;
    /** @brief Total size of blocks currently allocated. */
    SizeType memory_block_unreleased_size_;
    /** @brief Total size of memory applied for since start. */
    SizeType memory_block_applied_size_;
    /** @brief Peak size of concurrent memory allocated. */
    SizeType memory_block_peak_size_;
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
};

}//memory_pool

}//zengine