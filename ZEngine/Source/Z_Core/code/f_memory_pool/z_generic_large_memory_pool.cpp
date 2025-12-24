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
#include "z_generic_large_memory_pool.h"

namespace zengine {
namespace memory_pool {

NOINLINE Void ZGenericLargeMemoryPool::ExtendMemoryBlockP() noexcept {
    Byte* page_head_ptr = reinterpret_cast<Byte*>(
        Alloc2MBAlignmentMemoryAtomPageP(next_apply_memory_atom_page_num_)
    );
    if (page_head_ptr == nullptr) {
        ReturnType ret_val = error_code::kFMemoryPoolErrorCode_OutOfMemory;
        Z_LOG_ERROR(ret_val, 0, "Out of memory!");
        Exit(ret_val);
    }

    //initialize new memory page
    UInt32 apply_memory_size = next_apply_memory_atom_page_num_ * kMemoryAtomPageSize;
    unused_memory_block_ptr_ = page_head_ptr;
    unused_memory_block_end_ptr_ = unused_memory_block_ptr_ + apply_memory_size;

#if USE_MEMORY_POOL_PERFORMANCE_TEST
    memory_block_total_num_ += apply_memory_size / memory_block_with_head_size_;
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST

    if (next_apply_memory_atom_page_num_ < next_apply_memory_atom_page_max_num_) {
        next_apply_memory_atom_page_num_ = next_apply_memory_atom_page_num_ * kApplyMemoryBlockExtendFactor;
        if (next_apply_memory_atom_page_num_ > next_apply_memory_atom_page_max_num_) {
            next_apply_memory_atom_page_num_ = next_apply_memory_atom_page_max_num_;
        }
    }
}

ZGenericLargeMemoryPoolGroup::ZGenericLargeMemoryPoolGroup() noexcept
    : pool_array_()
{
    for (UInt32 index = 0; index < kLargeMemoryPoolNum; ++index) {
        pool_array_[index].Initialize(kLargeMemoryPoolMemoryBlockWithHeadSizeArray[index]);
    }
}

ZGenericLargeMemoryPoolGroup::~ZGenericLargeMemoryPoolGroup() noexcept {}

}//memory_pool
}//zengine