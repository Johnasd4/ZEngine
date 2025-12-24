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
 * @brief A thread-safe generic memory pool implementation optimized for large-sized memory blocks.
 */
class CACHE_LINE_ALIGN ZGenericLargeMemoryPool : public ZGenericMemoryPool {
private:
    /** @brief The size of a memory page (256KB). */
    static constexpr UInt32 kMemoryPageSize = 256U * kKB;
    /** @brief The number of atom pages per memory page. */
    static constexpr UInt32 kMemoryAtomPagePerMemoryPageNum = kMemoryPageSize / kMemoryAtomPageSize;
    /** @brief The minimum number of memory blocks to apply for. */
    static constexpr UInt32 kApplyMemoryBlockMinNum = 2U;
    /** @brief The maximum number of memory blocks to apply for. */
    static constexpr UInt32 kApplyMemoryBlockMaxNum = 16U;
    /** @brief The extension factor for applying memory blocks. */
    static constexpr UInt32 kApplyMemoryBlockExtendFactor = 2U;

    /** @brief Extended memory page header structure containing validation data. */
    struct CACHE_LINE_ALIGN MemoryPageHeadExtend_ {
        /** @brief Padding to ensure cache line alignment. */
        Byte padding_[CACHE_LINE_SIZE - sizeof(ZGenericMemoryPool::MemoryPageHead_)];
        /** @brief The base memory page header. */
        ZGenericMemoryPool::MemoryPageHead_ page_head_;
    };

public:
    /** @brief The size of the header structure prepended to each memory page. */
    static constexpr UInt32 kMemoryBlockHeadSize = sizeof(MemoryPageHeadExtend_);

    /** @brief Internal structure representing the header of a free memory block in the linked list. */
    struct MemoryBlockHead_ {
        /** @brief Pointer to the next free memory block in the list. */
        MemoryBlockHead_* next_memory_block_ptr_;
    };

public:
    /**
     * @brief Constructs the large memory pool.
     * @param _memory_block_with_head_size The total size of a memory block including its header overhead.
     */
    FORCEINLINE ZGenericLargeMemoryPool() noexcept
        : SuperType_()
        , head_memory_block_ptr_(nullptr)
        , unused_memory_block_ptr_(nullptr)
        , unused_memory_block_end_ptr_(nullptr)
        , memory_block_size_(0U)
        , memory_block_with_head_size_(0U)
        , next_apply_memory_atom_page_num_(0U)
        , next_apply_memory_atom_page_max_num_(0U)
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        , memory_block_unreleased_num_(0U)
        , momory_block_applyed_num_(0U)
        , momory_block_peak_num_(0U)
        , memory_block_total_num_(0U)
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
    {}

    /**
     * @brief Destroys the memory pool and releases resources.
     */
    FORCEINLINE ~ZGenericLargeMemoryPool() noexcept {}

    /**
     * @brief Initializes the memory pool parameters based on the provided block size.
     * @param _memory_block_with_head_size The total size of a memory block including its header overhead.
     */
    FORCEINLINE Void Initialize(UInt32 _memory_block_with_head_size) noexcept {
        memory_block_size_ = _memory_block_with_head_size - kMemoryBlockHeadSize;
        memory_block_with_head_size_ = _memory_block_with_head_size;
        next_apply_memory_atom_page_num_ = 
        (kApplyMemoryBlockMinNum * _memory_block_with_head_size + kMemoryPageSize - 1) /
            kMemoryPageSize * kMemoryAtomPagePerMemoryPageNum;
        next_apply_memory_atom_page_max_num_ =
            (kApplyMemoryBlockMaxNum * _memory_block_with_head_size + kMemoryPageSize - 1) /
            kMemoryPageSize * kMemoryAtomPagePerMemoryPageNum;
    }

    /**
     * @brief Gets the size of the usable memory block (excluding the header).
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
            if (unused_memory_block_ptr_ == unused_memory_block_end_ptr_) {
                ExtendMemoryBlockP();
            }
            reinterpret_cast<MemoryPageHeadExtend_*>(unused_memory_block_ptr_)->page_head_.memory_pool_ptr_ = this;
            memory_ptr = unused_memory_block_ptr_ + kMemoryBlockHeadSize;
            unused_memory_block_ptr_ += memory_block_with_head_size_;
        }
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        memory_block_unreleased_num_ += 1;
        momory_block_applyed_num_ += 1;
        if (memory_block_unreleased_num_ > momory_block_peak_num_) {
            momory_block_peak_num_ = memory_block_unreleased_num_;
        }
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
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
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
    }

protected:
    using SuperType_ = ZGenericMemoryPool;

private:
    ZGenericLargeMemoryPool(const ZGenericLargeMemoryPool&) = delete;
    ZGenericLargeMemoryPool(ZGenericLargeMemoryPool&&) = delete;
    ZGenericLargeMemoryPool& operator=(const ZGenericLargeMemoryPool&) = delete;
    ZGenericLargeMemoryPool& operator=(ZGenericLargeMemoryPool&&) = delete;

    /**
     * @brief Internal function to prepare a new memory page when the current page is exhausted.
     */
    NOINLINE Void ExtendMemoryBlockP() noexcept;

    /** @brief Mutex to ensure thread safety during allocation and deallocation. */
    ZMutex mutex_;
    /** @brief Pointer to the head of the free memory block linked list. */
    MemoryBlockHead_* head_memory_block_ptr_;
    /** @brief Pointer to the current available position within the active memory page. */
    Byte* unused_memory_block_ptr_;
    /** @brief Pointer to the end of the current active memory page. */
    Byte* unused_memory_block_end_ptr_;
    /** @brief The size of the usable data part of a memory block. */
    UInt32 memory_block_size_;
    /** @brief The total size of a memory block including internal headers. */
    UInt32 memory_block_with_head_size_;
    /** @brief The number of pages to request in the next system allocation. */
    UInt32 next_apply_memory_atom_page_num_;
    /** @brief The max number of pages to request in the next system allocation. */
    UInt32 next_apply_memory_atom_page_max_num_;
#if USE_MEMORY_POOL_PERFORMANCE_TEST
    /** @brief The number of memory blocks currently allocated but not yet released. */
    UInt32 memory_block_unreleased_num_;
    /** @brief The total number of memory block allocation requests made. */
    UInt32 momory_block_applyed_num_;
    /** @brief The peak number of simultaneously allocated memory blocks. */
    UInt32 momory_block_peak_num_;
    /** @brief The total number of memory blocks managed by this pool. */
    UInt32 memory_block_total_num_;
#endif // USE_MEMORY_POOL_PERFORMANCE_TEST
};

/**
 * @brief A singleton group managing multiple large memory pools for different block sizes.
 */
class CACHE_LINE_ALIGN ZGenericLargeMemoryPoolGroup {
private:
    /** @brief The increment size for the mapping table index. */
    static constexpr UInt32 kTableSizePerIndex = std::invoke([]() {
        UInt32 ans = kLargeMemoryPoolMemoryBlockWithHeadSizeArray[0];
        for(SizeType index = 0ULL; index < kLargeMemoryPoolNum; ++index) {
            UInt32 current_size = kLargeMemoryPoolMemoryBlockWithHeadSizeArray[index];
            if ((current_size & (current_size - 1)) != 0) {
                return current_size & (~current_size + 1);
            }
        }
        return ans;
    });
    /** @brief The total size of the mapping table. */
    static constexpr UInt32 kTableSize = kLargeMemoryPoolMemoryBlockWithHeadSizeMax / kTableSizePerIndex;
    /** @brief A lookup table mapping requested sizes to the appropriate pool index. */
    static constexpr TFixedArray<UInt8, kTableSize> kSizeToPoolTable =
        TFixedArray<UInt8, kTableSize>{[](TFixedArray<UInt8, kTableSize>* _array_ptr) {
            SizeType table_index = 0ULL;
            for (UInt8 pool_index = 0; pool_index < kLargeMemoryPoolNum; ++pool_index) {
                SizeType next_pool_index =
                    kLargeMemoryPoolMemoryBlockWithHeadSizeArray[pool_index] / kTableSizePerIndex;
                for (; table_index < next_pool_index; ++table_index) {
                    (*_array_ptr)[table_index] = pool_index;
                }
            }
        }};

public:
    /** @brief The maximum usable memory block size supported by this pool group. */
    static constexpr UInt32 kMemoryBlockSizeMax =
        kLargeMemoryPoolMemoryBlockWithHeadSizeMax - ZGenericLargeMemoryPool::kMemoryBlockHeadSize;

public:
    /**
     * @brief Retrieves the instance of the memory pool group.
     * @return ZGenericLargeMemoryPoolGroup& A reference to the singleton instance.
     */
    FORCEINLINE static ZGenericLargeMemoryPoolGroup& Instance() noexcept {
        static ZGenericLargeMemoryPoolGroup instance;
        return instance;
    }

    /**
     * @brief Calculates the actual memory block size for a requested size.
     * @param _memory_size The requested size of memory.
     * @return The actual size of the memory block that will be allocated.
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
        SizeType pool_index =
            kSizeToPoolTable[
                (_memory_size + ZGenericLargeMemoryPool::kMemoryBlockHeadSize - 1ULL) / kTableSizePerIndex
            ];
        return pool_array_[pool_index].ApplyMemory();
    }

    /**
     * @brief Allocates memory and retrieves the actual allocated block size.
     * @param _memory_size The requested size of memory.
     * @param _apply_size_ptr A pointer to store the actual size of the allocated memory block.
     * @return Void* A pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE Void* ApplyMemory(SizeType _memory_size, SizeType* _apply_size_ptr) noexcept {
        SizeType pool_index = 
            kSizeToPoolTable[
                (_memory_size + ZGenericLargeMemoryPool::kMemoryBlockHeadSize - 1ULL) / kTableSizePerIndex
            ];
        *_apply_size_ptr = pool_array_[pool_index].GetMemoryBlockSize();
        return pool_array_[pool_index].ApplyMemory();
    }

private:
    /**
     * @brief Constructs the memory pool group and initializes the individual pools.
     */
    ZGenericLargeMemoryPoolGroup() noexcept;

    /**
     * @brief Destroys the memory pool group.
     */
    ~ZGenericLargeMemoryPoolGroup() noexcept;

    /** @brief The array of large memory pools, each managing a specific block size. */
    TFixedArray<ZGenericLargeMemoryPool, kLargeMemoryPoolNum> pool_array_;
};

}//memory_pool
}//zengine