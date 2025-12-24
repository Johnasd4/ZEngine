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
 * @brief A thread-safe generic memory pool implementation optimized for medium-sized memory blocks (2KB to 64KB).
 */
class CACHE_LINE_ALIGN ZGenericGlobalMediumMemoryPool {
private:
    /** @brief Enumeration defining the memory block type strategies. */
    enum class MemoryBlockTypeEnum_ : UInt32 {
        /** @brief Strategy using 1.5x size. */
        k1_5,
        /** @brief Strategy using 2x size. */
        k2
    };

    /** @brief The size of the memory page for 1.5x strategy. */
    static constexpr UInt32 kMemoryPageSize1_5 = kMediumMemoryPoolMemoryBlockWithHeadSizeMax_1_5;
    /** @brief The size of the memory page for 2x strategy. */
    static constexpr UInt32 kMemoryPageSize2 = kMediumMemoryPoolMemoryBlockWithHeadSizeMax_2;
    /** @brief The number of atom pages per memory page for 1.5x strategy. */
    static constexpr UInt32 kMemoryAtomPagePerMemoryPageNum1_5 = kMemoryPageSize1_5 / kMemoryAtomPageSize;
    /** @brief The number of atom pages per memory page for 2x strategy. */
    static constexpr UInt32 kMemoryAtomPagePerMemoryPageNum2 = kMemoryPageSize2 / kMemoryAtomPageSize;
    /** @brief The minimum number of pages to apply for from the system. */
    static constexpr UInt32 kApplySystemMemoryPageMinNum = 32U;
    /** @brief The maximum number of pages to apply for from the system. */
    static constexpr UInt32 kApplySystemMemoryPageMaxNum = 128U;
    /** @brief The minimum number of atom pages to apply for from the system for 1.5x strategy. */
    static constexpr UInt32 kApplySystemMemoryAtomPageMinNum1_5 =
        kApplySystemMemoryPageMinNum * kMemoryPageSize1_5 / kMemoryAtomPageSize;
    /** @brief The maximum number of atom pages to apply for from the system for 1.5x strategy. */
    static constexpr UInt32 kApplySystemMemoryAtomPageMaxNum1_5 =
        kApplySystemMemoryPageMaxNum * kMemoryPageSize1_5 / kMemoryAtomPageSize;
    /** @brief The minimum number of atom pages to apply for from the system for 2x strategy. */
    static constexpr UInt32 kApplySystemMemoryAtomPageMinNum2 =
        kApplySystemMemoryPageMinNum * kMemoryPageSize2 / kMemoryAtomPageSize;
    /** @brief The maximum number of atom pages to apply for from the system for 2x strategy. */
    static constexpr UInt32 kApplySystemMemoryAtomPageMaxNum2 =
        kApplySystemMemoryPageMaxNum * kMemoryPageSize2 / kMemoryAtomPageSize;
    /** @brief The extension factor for applying system memory pages. */
    static constexpr UInt32 kApplySystemMemoryPageExtendFactor = 2U;
    /** @brief The minimum number of memory blocks to apply for. */
    static constexpr UInt32 kApplyMemoryBlockMinNum = 4U;
    /** @brief The maximum number of memory blocks to apply for. */
    static constexpr UInt32 kApplyMemoryBlockMaxNum = 64U;
    /** @brief The extension factor for applying memory blocks. */
    static constexpr UInt32 kApplyMemoryBlockExtendFactor = 4U;

public:
    /** @brief The size of the header structure prepended to each memory page. */
    static constexpr UInt32 kMemoryBlockHeadSize = static_cast<UInt32>(sizeof(ZGenericMemoryPool::MemoryPageHead_));

    /** @brief Internal structure representing the header of a free memory block in the linked list. */
    struct MemoryBlockHead_ {
        /** @brief Pointer to the next free memory block in the list. */
        MemoryBlockHead_* next_memory_block_ptr_;
    };

    /** @brief Structure holding thread-local data regarding current memory page allocation. */
    struct CACHE_LINE_ALIGN MemoryPageData_ {
        ZMutex mutex_;
        /** @brief Pointer to the current active memory page. */
        Byte* page_ptr_;
        /** @brief Pointer to the end of the current active memory page. */
        Byte* page_end_ptr_;
        /** @brief The current number of atom pages in the memory page. */
        UInt32 memory_atom_page_num_;
        /** @brief The number of atom pages to apply for next. */
        UInt32 next_apply_memory_atom_page_num_;
        /** @brief The maximum number of atom pages to apply for next. */
        UInt32 next_apply_memory_atom_page_max_num_;
    };

    /** @brief Structure holding a group of thread-local data for different block types. */
    struct MemoryPageDataGroup_ {
        /** @brief Memory page data for the 1.5x strategy. */
        MemoryPageData_ memory_page_data_1_5_ = {
            ZMutex(), nullptr, nullptr, 0U, kApplySystemMemoryAtomPageMinNum1_5, kApplySystemMemoryAtomPageMaxNum1_5
        };
        /** @brief Memory page data for the 2x strategy. */
        MemoryPageData_ memory_page_data_2_ = {
            ZMutex(), nullptr, nullptr, 0U, kApplySystemMemoryAtomPageMinNum2, kApplySystemMemoryAtomPageMaxNum2
        };
    };

public:
    /**
     * @brief Constructs the medium memory pool.
     * @param _memory_block_with_head_size The total size of a memory block including its header overhead.
     */
    FORCEINLINE ZGenericGlobalMediumMemoryPool() noexcept
        : head_memory_block_ptr_(nullptr)
        , unused_memory_block_ptr_(nullptr)
        , unused_memory_block_end_ptr_(nullptr)
        , memory_block_size_(0U)
        , memory_block_with_head_size_(0U)
        , block_type_()
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
    FORCEINLINE ~ZGenericGlobalMediumMemoryPool() noexcept {}

    /**
     * @brief Initializes the memory pool parameters based on the provided block size.
     * @param _memory_block_with_head_size The total size of a memory block including its header overhead.
     */
    FORCEINLINE Void Initialize(UInt32 _memory_block_with_head_size) noexcept {
        memory_block_size_ = _memory_block_with_head_size - kMemoryBlockHeadSize;
        memory_block_with_head_size_ = _memory_block_with_head_size;
        if(memory_block_size_ % (2 * kKB) != 0U) {
            block_type_ = MemoryBlockTypeEnum_::k1_5;
            next_apply_memory_atom_page_num_ =
                (kApplyMemoryBlockMinNum * _memory_block_with_head_size + kMemoryPageSize1_5 - 1) /
                kMemoryPageSize1_5 * kMemoryAtomPagePerMemoryPageNum1_5;
            next_apply_memory_atom_page_max_num_ =
                (kApplyMemoryBlockMaxNum * _memory_block_with_head_size + kMemoryPageSize1_5 - 1) /
                kMemoryPageSize1_5 * kMemoryAtomPagePerMemoryPageNum1_5;
        }
        else {
            block_type_ = MemoryBlockTypeEnum_::k2;
            next_apply_memory_atom_page_num_ =
                (kApplyMemoryBlockMinNum * _memory_block_with_head_size + kMemoryPageSize2 - 1) /
                kMemoryPageSize2 * kMemoryAtomPagePerMemoryPageNum2;
            next_apply_memory_atom_page_max_num_ =
                (kApplyMemoryBlockMaxNum * _memory_block_with_head_size + kMemoryPageSize2 - 1) /
                kMemoryPageSize2 * kMemoryAtomPagePerMemoryPageNum2;
        }
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
     * @return Void* A pointer to the allocated memory block, or nullptr if allocation fails.
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
            reinterpret_cast<ZGenericMemoryPool::MemoryPageHead_*>(unused_memory_block_ptr_)->memory_pool_ptr_ = this;
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

private:
    ZGenericGlobalMediumMemoryPool(const ZGenericGlobalMediumMemoryPool&) = delete;
    ZGenericGlobalMediumMemoryPool(ZGenericGlobalMediumMemoryPool&&) = delete;
    ZGenericGlobalMediumMemoryPool& operator=(const ZGenericGlobalMediumMemoryPool&) = delete;
    ZGenericGlobalMediumMemoryPool& operator=(ZGenericGlobalMediumMemoryPool&&) = delete;

    /**
     * @brief Retrieves the thread-local instance of the memory page data.
     * @return MemoryPageDataGroup_& Reference to the thread-local memory page data group.
     */
    FORCEINLINE static MemoryPageDataGroup_& MemoryPageDataGroupInstanceP() noexcept {
        static MemoryPageDataGroup_ memory_page_data_group;
        return memory_page_data_group;
    }

    /**
     * @brief Internal function to request new memory pages from the system when the local cache is exhausted.
     * @param _memory_page_data_ptr Pointer to the thread-local memory page data.
     */
    NOINLINE static Void ExtendMemoryPageP(MemoryPageData_* _memory_page_data_ptr) noexcept;

    /**
     * @brief Internal function to prepare a new memory block when the current block is exhausted.
     */
    NOINLINE Void ExtendMemoryBlockP() noexcept;

    /** @brief Mutex to protect the remote release list. */
    CACHE_LINE_ALIGN ZMutex mutex_;
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
    /** @brief The memory block type strategy used by this pool. */
    MemoryBlockTypeEnum_ block_type_;
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
 * @brief A singleton group managing multiple medium memory pools for different block sizes.
 */
class CACHE_LINE_ALIGN ZGenericGlobalMediumMemoryPoolGroup {
private:
    /** @brief The increment size for the mapping table index. */
    static constexpr UInt32 kTableSizePerIndex = std::invoke([]() {
        UInt32 ans = kMediumMemoryPoolMemoryBlockWithHeadSizeArray[0];
        for (SizeType index = 0ULL; index < kMediumMemoryPoolNum; ++index) {
            UInt32 current_size = kMediumMemoryPoolMemoryBlockWithHeadSizeArray[index];
            if ((current_size & (current_size - 1)) != 0) {
                return current_size & (~current_size + 1);
            }
        }
        return ans;
        });
    /** @brief The total size of the mapping table. */
    static constexpr UInt32 kTableSize = kMediumMemoryPoolMemoryBlockWithHeadSizeMax / kTableSizePerIndex;
    /** @brief A lookup table mapping requested sizes to the appropriate pool index. */
    static constexpr TFixedArray<UInt8, kTableSize> kSizeToPoolTable =
        TFixedArray<UInt8, kTableSize>{[](TFixedArray<UInt8, kTableSize>* _array_ptr) {
            SizeType table_index = 0ULL;
            for (UInt8 pool_index = 0; pool_index < kMediumMemoryPoolNum; ++pool_index) {
                SizeType next_pool_index =
                    kMediumMemoryPoolMemoryBlockWithHeadSizeArray[pool_index] / kTableSizePerIndex;
                for (; table_index < next_pool_index; ++table_index) {
                    (*_array_ptr)[table_index] = pool_index;
                }
            }
        }};

public:
    /** @brief The maximum usable memory block size supported by this pool group. */
    static constexpr UInt32 kMemoryBlockSizeMax =
        kMediumMemoryPoolMemoryBlockWithHeadSizeMax - ZGenericGlobalMediumMemoryPool::kMemoryBlockHeadSize;

public:
    /**
     * @brief Retrieves the instance of the memory pool group.
     * @return ZGenericGlobalMediumMemoryPoolGroup& A reference to the singleton instance.
     */
    FORCEINLINE static ZGenericGlobalMediumMemoryPoolGroup& Instance() noexcept {
        static ZGenericGlobalMediumMemoryPoolGroup instance;
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
                (_memory_size + ZGenericGlobalMediumMemoryPool::kMemoryBlockHeadSize - 1ULL) / kTableSizePerIndex
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
                (_memory_size + ZGenericGlobalMediumMemoryPool::kMemoryBlockHeadSize - 1ULL) / kTableSizePerIndex
            ];
        *_apply_size_ptr = pool_array_[pool_index].GetMemoryBlockSize();
        return pool_array_[pool_index].ApplyMemory();
    }

private:
    /**
     * @brief Constructs the memory pool group and initializes the individual pools.
     */
    ZGenericGlobalMediumMemoryPoolGroup() noexcept;

    /**
     * @brief Destroys the memory pool group.
     */
    ~ZGenericGlobalMediumMemoryPoolGroup() noexcept;

    /** @brief The array of medium memory pools, each managing a specific block size. */
    TFixedArray<ZGenericGlobalMediumMemoryPool, kMediumMemoryPoolNum> pool_array_;
};

}//memory_pool

}//zengine