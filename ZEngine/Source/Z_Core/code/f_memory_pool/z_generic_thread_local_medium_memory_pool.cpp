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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_generic_thread_local_medium_memory_pool.h"

#include "m_log.h"
#include "t_lock_guard.h"

#include "z_generic_memory_pool_performance_test.h"

namespace zengine {
namespace memory_pool {

NOINLINE Void ZGenericThreadLocalMediumMemoryPool::ExtendMemoryBlockP() noexcept {
    // get memory page data
    MemoryPageData_* memory_page_data_ptr;
    if (block_type_ == MemoryBlockTypeEnum_::k1_5) {
        memory_page_data_ptr = &MemoryPageDataGroupInstanceP().memory_page_data_1_5_;
    }
    else {
        memory_page_data_ptr = &MemoryPageDataGroupInstanceP().memory_page_data_2_;
    }

    // memory page all used, need to alloc from system
    if (memory_page_data_ptr->memory_atom_page_num_ == 0U) {
        ExtendMemoryPageP(memory_page_data_ptr);
    }

    UInt32 apply_page_num;
    if (next_apply_memory_atom_page_num_ < memory_page_data_ptr->memory_atom_page_num_) {
        apply_page_num = next_apply_memory_atom_page_num_;
        if (next_apply_memory_atom_page_num_ < next_apply_memory_atom_page_max_num_) {
            next_apply_memory_atom_page_num_ = next_apply_memory_atom_page_num_ * kApplyMemoryBlockExtendFactor;
            if (next_apply_memory_atom_page_num_ > next_apply_memory_atom_page_max_num_) {
                next_apply_memory_atom_page_num_ = next_apply_memory_atom_page_max_num_;
            }
        }
    }
    else {
        apply_page_num = memory_page_data_ptr->memory_atom_page_num_;
    }
    memory_page_data_ptr->memory_atom_page_num_ -= apply_page_num;

    // initialize new memory page
    UInt32 apply_memory_size = apply_page_num * kMemoryAtomPageSize;
    unused_memory_block_ptr_ = reinterpret_cast<Byte*>(memory_page_data_ptr->page_ptr_);
    unused_memory_block_end_ptr_ = unused_memory_block_ptr_ + apply_memory_size;
     
    // move page pointer
    memory_page_data_ptr->page_ptr_ += apply_memory_size;
}

NOINLINE Void ZGenericThreadLocalMediumMemoryPool::ExtendMemoryPageP(MemoryPageData_* _memory_page_data_ptr) noexcept {
    UInt32 apply_memory_atom_page_num = _memory_page_data_ptr->next_apply_memory_atom_page_num_;
    Byte* page_head_ptr = reinterpret_cast<Byte*>(
        ZGenericMemoryPool::GetThreadLocalInstance().Alloc64KBAlignmentMemoryAtomPageP(
            apply_memory_atom_page_num
        )
    );
    if (page_head_ptr == nullptr) {
        ReturnType ret_val = error_code::kFMemoryPoolErrorCode_OutOfMemory;
        Z_LOG_ERROR(ret_val, 0, "Out of memory!");
        Exit(ret_val);
    }
    _memory_page_data_ptr->memory_atom_page_num_ = apply_memory_atom_page_num;
    _memory_page_data_ptr->page_ptr_ = page_head_ptr;
    _memory_page_data_ptr->page_end_ptr_ =
        page_head_ptr + _memory_page_data_ptr->next_apply_memory_atom_page_num_ * kMemoryAtomPageSize;
    if (
        _memory_page_data_ptr->next_apply_memory_atom_page_num_ < 
        _memory_page_data_ptr->next_apply_memory_atom_page_max_num_
    ) {
        _memory_page_data_ptr->next_apply_memory_atom_page_num_ *= kApplySystemMemoryPageExtendFactor;
        if (
            _memory_page_data_ptr->next_apply_memory_atom_page_num_ > 
            _memory_page_data_ptr->next_apply_memory_atom_page_max_num_
        ) {
            _memory_page_data_ptr->next_apply_memory_atom_page_num_ = 
                _memory_page_data_ptr->next_apply_memory_atom_page_max_num_;
        }
    }
}

ZGenericThreadLocalMediumMemoryPoolGroup::ZGenericThreadLocalMediumMemoryPoolGroup() noexcept
    : pool_array_()
{
    for (UInt32 index = 0; index < kMediumMemoryPoolNum; ++index) {
        pool_array_[index].Initialize(kMediumMemoryPoolMemoryBlockWithHeadSizeArray[index]);
    }
}

ZGenericThreadLocalMediumMemoryPoolGroup::~ZGenericThreadLocalMediumMemoryPoolGroup() noexcept {
#if USE_MEMORY_POOL_PERFORMANCE_TEST
    ZGenericMemoryPoolPerformanceTest& performance_test_instance = ZGenericMemoryPoolPerformanceTest::GetInstance();
    for (SizeType index = 0; index < kMediumMemoryPoolNum; ++index) {
        ZGenericThreadLocalMediumMemoryPool& memory_pool = pool_array_[index];
        ZGenericMemoryPoolPerformanceTest::ThreadLocalMemoryPoolPerformanceData& performance_data =
            performance_test_instance.thread_local_medium_memory_pool_performance_data_[index];
        TLockGuard lock_guard(performance_data.mutex_);
        performance_data.memory_block_size_ = memory_pool.memory_block_size_;
        performance_data.memory_block_unreleased_num_ += memory_pool.memory_block_unreleased_num_;
        performance_data.memory_block_applied_num_ += memory_pool.memory_block_applied_num_;
    }
#endif // USE_MEMORY_POOL_PERFORMANCE_TEST 
}

} // memory_pool
} // zengine