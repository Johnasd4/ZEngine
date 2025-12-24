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

#include "m_log.h"
#include "z_generic_thread_local_small_memory_pool.h"

namespace zengine {
namespace memory_pool {

NOINLINE Void ZGenericThreadLocalSmallMemoryPool::ExtendMemoryBlockP() noexcept {
    MemoryPageData_& memory_page_data = MemoryPageDataInstanceP();

    //memory page all used, need to alloc from system
    if (memory_page_data.page_ptr_ == memory_page_data.page_end_ptr_) {
        ExtendMemoryPageP();
    }

    //allocate new memory page
    MemoryPageHeadExtend_* page_ptr = reinterpret_cast<MemoryPageHeadExtend_*>(
        memory_page_data.page_ptr_
    );
    memory_page_data.page_ptr_ += kMemoryPageSize;

    //initialize new memory page
    page_ptr->page_head_.memory_pool_ptr_ = this;
    unused_memory_block_ptr_ = reinterpret_cast<Byte*>(page_ptr) + memory_page_start_ptr_offset_;
    unused_memory_block_end_ptr_ = reinterpret_cast<Byte*>(page_ptr) + kMemoryPageSize;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
    memory_block_total_num_ += 
        (kMemoryPageSize - static_cast<UInt32>(sizeof(MemoryPageHeadExtend_))) / memory_block_size_;
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
}

NOINLINE Void ZGenericThreadLocalSmallMemoryPool::ExtendMemoryPageP() noexcept {
    MemoryPageData_& memory_page_data = MemoryPageDataInstanceP();
    Byte* page_head_ptr = reinterpret_cast<Byte*>(
        ZGenericMemoryPool::ThreadLocalInstance().Alloc64KBAlignmentMemoryAtomPageP(
            memory_page_data.next_apply_memory_atom_page_num_
        )
    );
    if(page_head_ptr == nullptr) {
        ReturnType ret_val = error_code::kFMemoryPoolErrorCode_OutOfMemory;
        Z_LOG_ERROR(ret_val, 0, "Out of memory!");
        Exit(ret_val);
    }
    memory_page_data.page_ptr_ = page_head_ptr;
    memory_page_data.page_end_ptr_ = 
        page_head_ptr + memory_page_data.next_apply_memory_atom_page_num_ * kMemoryAtomPageSize;
    if (memory_page_data.next_apply_memory_atom_page_num_ < kApplySystemMemoryPageMaxNum) {
        memory_page_data.next_apply_memory_atom_page_num_ *= kApplySystemMemoryPageMultFactor;
        if (memory_page_data.next_apply_memory_atom_page_num_ > kApplySystemMemoryPageMaxNum) {
            memory_page_data.next_apply_memory_atom_page_num_ = kApplySystemMemoryPageMaxNum;
        }
    }
}

ZGenericThreadLocalSmallMemoryPoolGroup::ZGenericThreadLocalSmallMemoryPoolGroup() noexcept
    : pool_array_()
{
    for (SizeType index = 0; index < kSmallMemoryPoolNum; ++index) {
        pool_array_[index].Initialize(kSmallMemoryPoolMemoryBlockSizeArray[index]);
    }
}

ZGenericThreadLocalSmallMemoryPoolGroup::~ZGenericThreadLocalSmallMemoryPoolGroup() noexcept {}

}//memory_pool
}//zengine