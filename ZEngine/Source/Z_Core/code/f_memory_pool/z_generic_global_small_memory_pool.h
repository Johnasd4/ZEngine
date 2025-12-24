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

/**
 * @brief A thread-safe generic memory pool implementation optimized for small memory blocks.
 *
 * This pool uses global synchronization but leverages thread-local storage for fetching pages
 * to reduce contention during page extension.
 */
class CACHE_LINE_ALIGN ZGenericGlobalSmallMemoryPool {
private:
    /** @brief The size of the memory page based on alignment requirements. */
    static constexpr UInt32 kMemoryPageSize = kMemoryPageAlignmentSize;
    /** @brief The minimum size of system memory to apply for. */
    static constexpr UInt32 kApplySystemMemoryMinSize = 1U * kMB;
    /** @brief The maximum size of system memory to apply for. */
    static constexpr UInt32 kApplySystemMemoryMaxSize = 4U * kMB;
    /** @brief The minimum number of pages to apply for from the system. */
    static constexpr UInt32 kApplySystemMemoryPageMinNum = kApplySystemMemoryMinSize / kMemoryAtomPageSize;
    /** @brief The maximum number of pages to apply for from the system. */
    static constexpr UInt32 kApplySystemMemoryPageMaxNum = kApplySystemMemoryMaxSize / kMemoryAtomPageSize;
    /** @brief The mutt factor of the num of pages to apply for from the system. */
    static constexpr UInt32 kApplySystemMemoryPageMultFactor = 2U;

public:
    /**
     * @brief Extended memory page header structure containing validation data.
     */
    struct ALIGN(16) MemoryPageHeadExtend_ {
        /** @brief The base memory page header. */
        ZGenericMemoryPool::MemoryPageHead_ page_head_;
    };

private:
    /**
     * @brief Internal structure representing the header of a free memory block in the linked list.
     */
    struct MemoryBlockHead_ {
        /** @brief Pointer to the next free memory block in the list. */
        MemoryBlockHead_* next_memory_block_ptr_;
    };

    /**
     * @brief Structure holding thread-local data regarding current memory page allocation.
     */
    struct CACHE_LINE_ALIGN MemoryPageData_ {
        ZMutex mutex_;
        /** @brief Pointer to the current active memory page. */
        Byte* page_ptr_ = nullptr;
        /** @brief Pointer to the end of the current active memory page. */
        Byte* page_end_ptr_ = nullptr;
        /** @brief The number of page atoms to apply for next. */
        UInt32 next_apply_memory_atom_page_num_ = kApplySystemMemoryPageMinNum;
    };

public:
    /**
     * @brief Constructs the memory pool with default values.
     */
    FORCEINLINE ZGenericGlobalSmallMemoryPool() noexcept
        : head_memory_block_ptr_(nullptr)
        , unused_memory_block_ptr_(nullptr)
        , unused_memory_block_end_ptr_(nullptr)
        , memory_block_size_(0U)
        , memory_page_start_ptr_offset_(0U)
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        , memory_block_unreleased_num_(0U)
        , memory_block_applied_num_(0U)
        , memory_block_peak_num_(0U)
        , memory_block_total_num_(0U)
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST
    {}

    /**
     * @brief Destroys the memory pool and releases resources.
     */
    FORCEINLINE ~ZGenericGlobalSmallMemoryPool() noexcept {}

    /**
     * @brief Initializes the memory pool with a specific memory block size.
     * @param _memory_block_size The size of each memory block to be managed by this pool.
     */
    FORCEINLINE Void Initialize(UInt32 _memory_block_size) noexcept {
        memory_block_size_ = _memory_block_size;
        memory_page_start_ptr_offset_ =
            static_cast<UInt32>(sizeof(MemoryPageHeadExtend_)) + 
            ((kMemoryPageSize - static_cast<UInt32>(sizeof(MemoryPageHeadExtend_))) % memory_block_size_);
    }

    /**
     * @brief Gets the size of the memory blocks managed by this pool.
     * @return UInt32 The size of a single memory block in bytes.
     */
    FORCEINLINE UInt32 GetMemoryBlockSize() noexcept {
        return memory_block_size_;
    }

    /**
     * @brief Allocates a memory block from the pool.
     * @return Void* A pointer to the allocated memory block.
     */
    NODISCARD FORCEINLINE Void* ApplyMemory() noexcept {
        TLockGuard lock_guard(mutex_);
        Void* memory_ptr;
        if (head_memory_block_ptr_ != nullptr) {
            memory_ptr = reinterpret_cast<Void*>(head_memory_block_ptr_);
            head_memory_block_ptr_ = head_memory_block_ptr_->next_memory_block_ptr_;
        }
        else {
            // memory block all used, need to alloc from unused memory page
            if (unused_memory_block_ptr_ == unused_memory_block_end_ptr_) {
                ExtendMemoryBlockP();
            }
            memory_ptr = unused_memory_block_ptr_;
            unused_memory_block_ptr_ += memory_block_size_;
        }

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        memory_block_unreleased_num_ += 1;
        memory_block_applied_num_ += 1;
        if (memory_block_unreleased_num_ > memory_block_peak_num_) {
            memory_block_peak_num_ = memory_block_unreleased_num_;
        }
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST
        return memory_ptr;
    }

    /**
     * @brief Releases a memory block back to the pool.
     * @param _memory_ptr The pointer to the memory block to be released.
     */
    FORCEINLINE Void ReleaseMemory(Void* _memory_ptr) noexcept {
        TLockGuard lock_guard(mutex_);
        MemoryBlockHead_* memory_block_ptr = reinterpret_cast<MemoryBlockHead_*>(_memory_ptr);
        memory_block_ptr->next_memory_block_ptr_ = head_memory_block_ptr_;
        head_memory_block_ptr_ = memory_block_ptr;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        memory_block_unreleased_num_ -= 1;
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST
    }

protected:
    /** @brief Alias for the base class type. */
    using SuperType = ZGenericMemoryPool;

private:
    ZGenericGlobalSmallMemoryPool(const ZGenericGlobalSmallMemoryPool&) = delete;
    ZGenericGlobalSmallMemoryPool(ZGenericGlobalSmallMemoryPool&&) = delete;
    ZGenericGlobalSmallMemoryPool& operator=(const ZGenericGlobalSmallMemoryPool&) = delete;
    ZGenericGlobalSmallMemoryPool& operator=(ZGenericGlobalSmallMemoryPool&&) = delete;

    /**
     * @brief Retrieves the thread-local instance of the memory page data.
     * @return MemoryPageData_& Reference to the thread-local memory page data.
     */
    FORCEINLINE static MemoryPageData_& MemoryPageDataInstanceP() noexcept {
        static MemoryPageData_ memory_page_data;
        return memory_page_data;
    }

    /**
     * @brief Internal function to request new memory pages from the system when the local cache is exhausted.
     */
    NOINLINE static Void ExtendMemoryPageP() noexcept;

    /**
     * @brief Internal function to prepare a new memory page when the current page is exhausted.
     */
    NOINLINE Void ExtendMemoryBlockP() noexcept;

    /** @brief Mutex for apply and release memory. */
    ZMutex mutex_;
    /** @brief Pointer to the head of the free memory block linked list. */
    MemoryBlockHead_* head_memory_block_ptr_;
    /** @brief Pointer to the current available position within the active memory page. */
    Byte* unused_memory_block_ptr_;
    /** @brief Pointer to the end of the current active memory page. */
    Byte* unused_memory_block_end_ptr_;
    /** @brief The size of a single memory block in bytes. */
    UInt32 memory_block_size_;
    /** @brief The offset to the start of usable memory in a memory page. */
    UInt32 memory_page_start_ptr_offset_;
#if USE_MEMORY_POOL_PERFORMANCE_TEST
    /** @brief The number of memory blocks currently allocated but not yet released. */
    UInt32 memory_block_unreleased_num_;
    /** @brief The total number of memory block allocation requests made. */
    UInt32 memory_block_applied_num_;
    /** @brief The peak number of simultaneously allocated memory blocks. */
    UInt32 memory_block_peak_num_;
    /** @brief The total number of memory blocks managed by this pool. */
    UInt32 memory_block_total_num_;
#endif // USE_MEMORY_POOL_PERFORMANCE_TEST
};

/**
 * @brief A singleton group managing multiple small memory pools for different block sizes.
 */
class CACHE_LINE_ALIGN ZGenericGlobalSmallMemoryPoolGroup {
private:
    /** @brief The increment size for the mapping table index. */
    static constexpr UInt32 kTableSizePerIndex = kSmallMemoryPoolMemoryBlockSizeMin;
    /** @brief The total size of the mapping table. */
    static constexpr UInt32 kTableSize = kSmallMemoryPoolMemoryBlockSizeMax / kTableSizePerIndex;
    /** @brief A lookup table mapping requested sizes to the appropriate pool index. */
    static constexpr TFixedArray<UInt8, kTableSize> kSizeToPoolTable =
        TFixedArray<UInt8, kTableSize>{[](TFixedArray<UInt8, kTableSize>* _array_ptr) {
            SizeType table_index = 0ULL;
            for (UInt8 pool_index = 0; pool_index < kSmallMemoryPoolNum; ++pool_index) {
                SizeType next_pool_index = kSmallMemoryPoolMemoryBlockSizeArray[pool_index] / kTableSizePerIndex;
                for (; table_index < next_pool_index; ++table_index) {
                    (*_array_ptr)[table_index] = pool_index;
                }
            }
        }};

public:
    /** @brief The maximum size of a memory block that can be managed by this group. */
    static constexpr UInt32 kMemoryBlockSizeMax = kSmallMemoryPoolMemoryBlockSizeMax;

    /**
     * @brief Retrieves the singleton instance of the memory pool group.
     * @return ZGenericGlobalSmallMemoryPoolGroup& Reference to the singleton instance.
     */
    FORCEINLINE static ZGenericGlobalSmallMemoryPoolGroup& Instance() noexcept {
        static ZGenericGlobalSmallMemoryPoolGroup instance;
        return instance;
    }

    /**
     * @brief Calculates the actual memory block size for a requested size.
     * @param _memory_size The requested size of memory.
     * @return SizeType The actual size of the memory block that will be allocated.
     */
    NODISCARD FORCEINLINE SizeType CalculateMemory(SizeType _memory_size) noexcept {
        SizeType pool_index = kSizeToPoolTable[(_memory_size - 1ULL) / kTableSizePerIndex];
        return pool_array_[pool_index].GetMemoryBlockSize();
    }

    /**
     * @brief Allocates memory of the specified size.
     * @param _memory_size The requested size of memory.
     * @return Void* A pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE Void* ApplyMemory(SizeType _memory_size) noexcept {
        SizeType pool_index = kSizeToPoolTable[(_memory_size - 1ULL) / kTableSizePerIndex];
        return pool_array_[pool_index].ApplyMemory();
    }

    /**
     * @brief Allocates memory and retrieves the actual allocated block size.
     * @param _memory_size The requested size of memory.
     * @param _apply_size_ptr A pointer to store the actual size of the allocated memory block.
     * @return Void* A pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE Void* ApplyMemory(SizeType _memory_size, SizeType* _apply_size_ptr) noexcept {
        SizeType pool_index = kSizeToPoolTable[(_memory_size - 1ULL) / kTableSizePerIndex];
        *_apply_size_ptr = pool_array_[pool_index].GetMemoryBlockSize();
        return pool_array_[pool_index].ApplyMemory();
    }

private:
    /**
     * @brief Constructs the memory pool group and initializes the individual pools.
     */
    ZGenericGlobalSmallMemoryPoolGroup() noexcept;

    /**
     * @brief Destroys the memory pool group.
     */
    ~ZGenericGlobalSmallMemoryPoolGroup() noexcept;

    /** @brief The array of small memory pools, each managing a specific block size. */
    TFixedArray<ZGenericGlobalSmallMemoryPool, kSmallMemoryPoolNum> pool_array_;
};

}//memory_pool
}//zengine