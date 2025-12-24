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

#include "t_lock_guard.h"
#include "z_mutex.h"

#include "z_system_memory_pool.h"

namespace zengine {
namespace memory_pool {

/** @brief A structure representing a node in the memory page linked list, aligned to 16 bytes. */
struct ALIGN(16) ZMemoryPageNode {
    /** @brief Pointer to the underlying memory data. */
    Void* memory_data_ptr_;
    /** @brief Pointer to the next memory page node in the list. */
    ZMemoryPageNode* next_node_ptr_;
};

}//memory_pool
}//zengine

namespace zengine {
namespace memory_pool {
namespace internal {

/** @brief A specialized, thread-safe memory pool for managing ZMemoryPageNode objects. */
class CACHE_LINE_ALIGN ZSpecializedMemoryPoolMemoryPageNodeMemoryPool {
public:
    /** @brief Structure representing the head of a memory page. */
    struct ALIGN(16) MemoryPageHead_ {
        /** @brief Pointer to the next memory page. */
        MemoryPageHead_* next_memory_page_ptr_;
    };

private:
    /** @brief The size of the memory page based on alignment requirements. */
    static constexpr UInt32 kMemoryPageSize = kMemoryAtomPageSize;
    /** @brief The number of memory atom pages to apply for. */
    static constexpr UInt32 kApplyMemoryAtomPageNum = kMemoryPageSize / kMemoryAtomPageSize;
    /** @brief The size of a single memory block (size of ZMemoryPageNode). */
    static constexpr UInt32 kMemoryBlockSize = sizeof(ZMemoryPageNode);
    /** @brief Offset for the starting pointer of the memory page to ensure correct alignment. */
    static constexpr UInt32 kMemoryPageStartPointerOffset = 
        sizeof(MemoryPageHead_) + ((kMemoryPageSize - sizeof(MemoryPageHead_)) % sizeof(kMemoryBlockSize));

private:
    /** @brief Internal structure representing the header of a free memory block in the linked list. */
    struct MemoryBlockHead_ {
        /** @brief Pointer to the next free memory block in the list. */
        MemoryBlockHead_* next_memory_block_ptr_;
    };

public:
    /** @brief Retrieves the singleton instance of the memory pool. */
    FORCEINLINE static ZSpecializedMemoryPoolMemoryPageNodeMemoryPool& Instance() noexcept {
        static ZSpecializedMemoryPoolMemoryPageNodeMemoryPool instance;
        return instance;
    }

    /** @brief Constructs the specialized memory pool. */
    ZSpecializedMemoryPoolMemoryPageNodeMemoryPool() noexcept;

    /** @brief Destroys the specialized memory pool. */
    ~ZSpecializedMemoryPoolMemoryPageNodeMemoryPool() noexcept;

    /**
     * @brief Allocates a memory page node from the pool.
     * @return A pointer to the allocated ZMemoryPageNode.
     */
    NODISCARD FORCEINLINE ZMemoryPageNode* ApplyMemoryPageNode() noexcept {
        TLockGuard lock_guard(mutex_);
        ZMemoryPageNode* memory_page_node_ptr;
        if (head_memory_block_ptr_ != nullptr) {
            memory_page_node_ptr = reinterpret_cast<ZMemoryPageNode*>(head_memory_block_ptr_);
            head_memory_block_ptr_ = head_memory_block_ptr_->next_memory_block_ptr_;
        }
        else {
            // memory block all used, need to alloc from unused memory page
            if (unused_memory_block_ptr_ == unused_memory_block_end_ptr_) {
                ExtendMemoryBlockP();
            }
            memory_page_node_ptr = reinterpret_cast<ZMemoryPageNode*>(unused_memory_block_ptr_);
            unused_memory_block_ptr_ += kMemoryBlockSize;
        }
        return memory_page_node_ptr;
    }

    /**
     * @brief Releases a memory page node back to the pool.
     * @param _memory_ptr The pointer to the memory node to be released.
     */
    FORCEINLINE Void ReleaseMemoryPageNode(ZMemoryPageNode* _memory_ptr) noexcept {
        TLockGuard lock_guard(mutex_);
        MemoryBlockHead_* memory_block_ptr = reinterpret_cast<MemoryBlockHead_*>(_memory_ptr);
        memory_block_ptr->next_memory_block_ptr_ = head_memory_block_ptr_;
        head_memory_block_ptr_ = memory_block_ptr;

    }

private:
    ZSpecializedMemoryPoolMemoryPageNodeMemoryPool(const ZSpecializedMemoryPoolMemoryPageNodeMemoryPool&) = delete;
    ZSpecializedMemoryPoolMemoryPageNodeMemoryPool(ZSpecializedMemoryPoolMemoryPageNodeMemoryPool&&) = delete;
    ZSpecializedMemoryPoolMemoryPageNodeMemoryPool& operator=(const ZSpecializedMemoryPoolMemoryPageNodeMemoryPool&) = delete;
    ZSpecializedMemoryPoolMemoryPageNodeMemoryPool& operator=(ZSpecializedMemoryPoolMemoryPageNodeMemoryPool&&) = delete;

    /** @brief Internal function to prepare a new memory page when the current page is exhausted. */
    NOINLINE Void ExtendMemoryBlockP() noexcept;

    /** @brief Mutex for thread safety. */
    ZMutex mutex_;
    /** @brief Pointer to the head of the free memory block linked list. */
    MemoryBlockHead_* head_memory_block_ptr_;
    /** @brief Pointer to the current available position within the active memory page. */
    Byte* unused_memory_block_ptr_;
    /** @brief Pointer to the end of the current active memory page. */
    Byte* unused_memory_block_end_ptr_;
    /** @brief Pointer to the head of the memory page linked list. */
    MemoryPageHead_* head_memory_page_ptr_;
};

/** @brief Template class for providing thread-safe locking mechanisms. */
template<Bool _IfThreadSafe>
class TSpecializedMemoryPoolThreadSafe;

/** @brief Specialization of TSpecializedMemoryPoolThreadSafe for thread-safe environments. */
template<>
class TSpecializedMemoryPoolThreadSafe<true> {
public:
    /** @brief Constructs the thread-safe wrapper and initializes the mutex. */
    FORCEINLINE TSpecializedMemoryPoolThreadSafe() noexcept : mutex_() {}
    /** @brief Destroys the thread-safe wrapper. */
    FORCEINLINE ~TSpecializedMemoryPoolThreadSafe() noexcept {}

    /** @brief Locks the internal mutex. */
    FORCEINLINE Void Lock() noexcept { mutex_.Lock(); }
    /** @brief Unlocks the internal mutex. */
    FORCEINLINE Void Unlock() noexcept { mutex_.Unlock(); }

private:
    /** @brief The mutex used for synchronization. */
    ZMutex mutex_;
};

/** @brief Specialization of TSpecializedMemoryPoolThreadSafe for non-thread-safe environments. */
template<>
class TSpecializedMemoryPoolThreadSafe<false> {
public:
    /** @brief Constructs the non-thread-safe wrapper. */
    FORCEINLINE TSpecializedMemoryPoolThreadSafe() noexcept {}
    /** @brief Destroys the non-thread-safe wrapper. */
    FORCEINLINE ~TSpecializedMemoryPoolThreadSafe() noexcept {}

    /** @brief No-op lock function. */
    FORCEINLINE Void Lock() noexcept {}
    /** @brief No-op unlock function. */
    FORCEINLINE Void Unlock() noexcept {}
};

}//internal
}//memory_pool
}//zengine

namespace zengine {
namespace memory_pool {

/**
 * @brief A generic, high-performance specialized memory pool class.
 * @tparam _IfThreadSafe Indicates if the pool should be thread-safe.
 * @tparam _IfThreadLocal Indicates if the pool instance should be thread-local.
 * @tparam _MemoryBlockSize The size of each memory block managed by the pool.
 * @tparam _MemoryAtomPageMinNum The initial number of memory atom pages to allocate.
 * @tparam _MemoryAtomPageMaxNum The maximum number of memory atom pages to allocate.
 * @tparam _MemoryAtomPageExtendFactor The factor by which the allocation size grows.
 */
template<
    Bool _IfThreadSafe, UInt32 _MemoryBlockSize, 
    UInt32 _MemoryAtomPageMinNum, UInt32 _MemoryAtomPageMaxNum, UInt32 _MemoryAtomPageExtendFactor
>
class CACHE_LINE_ALIGN TSpecializedMemoryPool : public internal::TSpecializedMemoryPoolThreadSafe<_IfThreadSafe> {
private:
    static_assert(
        _MemoryAtomPageMinNum <= _MemoryAtomPageMaxNum,
        "TSpecializedMemoryPool: _MemoryAtomPageMinNum must be bigger than _MemoryAtomPageMaxNum!"
    );
    static_assert(
        _MemoryAtomPageMinNum * kMemoryAtomPageSize >= _MemoryBlockSize,
        "TSpecializedMemoryPool: _MemoryAtomPageMinNum is too small to hold a memory block!"
    );

    /** @brief Internal structure representing the header of a free memory block in the linked list. */
    struct MemoryBlockHead_ {
        /** @brief Pointer to the next free memory block in the list. */
        MemoryBlockHead_* next_memory_block_ptr_;
    };

public:
    /** @brief Constructs the specialized memory pool. */
#if USE_MEMORY_POOL_PERFORMANCE_TEST
    TSpecializedMemoryPool(ZStringView _memory_pool_name) noexcept
#else
    TSpecializedMemoryPool() noexcept
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
        : head_memory_block_ptr_(nullptr)
        , unused_memory_block_ptr_(nullptr)
        , unused_memory_block_end_ptr_(nullptr)
        , next_apply_memory_atom_page_num_(_MemoryAtomPageMinNum)
        , head_memory_page_node_ptr_(nullptr)
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        , memory_block_unreleased_num_(0U)
        , memory_block_applied_num_(0U)
        , memory_block_peak_num_(0U)
        , memory_block_total_num_(0U)
        , memory_pool_name_(_memory_pool_name)
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST
    {}

    /** @brief Destroys the memory pool and releases all allocated resources to the system. */
    ~TSpecializedMemoryPool() noexcept {
        internal::ZSpecializedMemoryPoolMemoryPageNodeMemoryPool& memory_page_node_pool =
            internal::ZSpecializedMemoryPoolMemoryPageNodeMemoryPool::Instance();
        ZSystemMemoryPool& system_memory_pool = ZSystemMemoryPool::Instance();
        while (head_memory_page_node_ptr_ != nullptr) {
            system_memory_pool.FreeMemoryP(head_memory_page_node_ptr_->memory_data_ptr_);
            memory_page_node_pool.ReleaseMemoryPageNode(head_memory_page_node_ptr_);
            head_memory_page_node_ptr_ = head_memory_page_node_ptr_->next_node_ptr_;
        }
    }

    /**
     * @brief Allocates a memory block from the pool.
     * @return A pointer to the allocated memory block.
     */
    NODISCARD FORCEINLINE Void* ApplyMemory() noexcept {
        TLockGuard lock_guard(*this);
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
            unused_memory_block_ptr_ += _MemoryBlockSize;
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
        TLockGuard lock_guard(*this);
        MemoryBlockHead_* memory_block_ptr = reinterpret_cast<MemoryBlockHead_*>(_memory_ptr);
        memory_block_ptr->next_memory_block_ptr_ = head_memory_block_ptr_;
        head_memory_block_ptr_ = memory_block_ptr;
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        memory_block_unreleased_num_ -= 1;
#endif//USE_MEMORY_POOL_PERFORMANCE_TEST
    }

private:
    TSpecializedMemoryPool(const TSpecializedMemoryPool&) = delete;
    TSpecializedMemoryPool(TSpecializedMemoryPool&&) = delete;
    TSpecializedMemoryPool& operator=(const TSpecializedMemoryPool&) = delete;
    TSpecializedMemoryPool& operator=(TSpecializedMemoryPool&&) = delete;

    /** @brief Internal function to extend the memory pool when current resources are exhausted. */
    NOINLINE Void ExtendMemoryBlockP() noexcept {
        //allocate new memory data from system
        Void* memory_data_ptr =
            ZSystemMemoryPool::Instance().Alloc64KBAlignmentMemoryAtomPageP(next_apply_memory_atom_page_num_);
        if (memory_data_ptr == nullptr) {
            ReturnType ret_val = error_code::kFMemoryPoolErrorCode_OutOfMemory;
            Z_LOG_ERROR(ret_val, 0, "Out of memory!");
            Exit(ret_val);
        }

        //calculate applied memory size
        SizeType apply_memory_size = 
            static_cast<SizeType>(next_apply_memory_atom_page_num_) * static_cast<SizeType>(kMemoryAtomPageSize);

        //update next apply memory page num
        if (next_apply_memory_atom_page_num_ < _MemoryAtomPageMaxNum) {
            next_apply_memory_atom_page_num_ *= _MemoryAtomPageExtendFactor;
            if (next_apply_memory_atom_page_num_ > _MemoryAtomPageMaxNum) {
                next_apply_memory_atom_page_num_ = _MemoryAtomPageMaxNum;
            }
        }

        //apply memory page head node from specialized memory pool
        ZMemoryPageNode* memory_page_node_ptr = 
            internal::ZSpecializedMemoryPoolMemoryPageNodeMemoryPool::Instance().ApplyMemoryPageNode();
        memory_page_node_ptr->memory_data_ptr_ = memory_data_ptr;
        memory_page_node_ptr->next_node_ptr_ = head_memory_page_node_ptr_;
        head_memory_page_node_ptr_ = memory_page_node_ptr;

        //calculate memory block num
        SizeType memory_block_num = apply_memory_size / static_cast<SizeType>(_MemoryBlockSize);

        //initialize new memory page
        unused_memory_block_ptr_ = static_cast<Byte*>(memory_data_ptr);
        unused_memory_block_end_ptr_ = 
            static_cast<Byte*>(memory_data_ptr) + memory_block_num * static_cast<SizeType>(_MemoryBlockSize);

#if USE_MEMORY_POOL_PERFORMANCE_TEST
        memory_block_total_num_ += static_cast<UInt32>(memory_block_num);
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
    }

    /** @brief Pointer to the head of the free memory block linked list. */
    MemoryBlockHead_* head_memory_block_ptr_;
    /** @brief Pointer to the current available position within the active memory page. */
    Byte* unused_memory_block_ptr_;
    /** @brief Pointer to the end of the current active memory page. */
    Byte* unused_memory_block_end_ptr_;
    /** @brief The number of memory atom pages to apply for in the next extension. */
    UInt32 next_apply_memory_atom_page_num_;
    /** @brief Pointer to the head of the memory page node list managed by this pool. */
    ZMemoryPageNode* head_memory_page_node_ptr_;
#if USE_MEMORY_POOL_PERFORMANCE_TEST
    /** @brief The number of memory blocks currently allocated but not yet released. */
    UInt32 memory_block_unreleased_num_;
    /** @brief The total number of memory block allocation requests made. */
    UInt32 memory_block_applied_num_;
    /** @brief The peak number of simultaneously allocated memory blocks. */
    UInt32 memory_block_peak_num_;
    /** @brief The total number of memory blocks managed by this pool. */
    UInt32 memory_block_total_num_;
    /** @brief The name of the memory pool for performance testing purposes. */
    ZStringView memory_pool_name_;
#endif // USE_MEMORY_POOL_PERFORMANCE_TEST
};

}//memory_pool
}//zengine