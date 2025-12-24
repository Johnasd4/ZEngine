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

#include "t_specialized_memory_pool.h"
#include "z_system_memory_pool.h"

namespace zengine {
namespace memory_pool {
namespace internal {

ZSpecializedMemoryPoolMemoryPageNodeMemoryPool::ZSpecializedMemoryPoolMemoryPageNodeMemoryPool() noexcept
    : head_memory_block_ptr_(nullptr)
    , unused_memory_block_ptr_(nullptr)
    , unused_memory_block_end_ptr_(nullptr)
{}

ZSpecializedMemoryPoolMemoryPageNodeMemoryPool::~ZSpecializedMemoryPoolMemoryPageNodeMemoryPool() noexcept {}

NOINLINE Void ZSpecializedMemoryPoolMemoryPageNodeMemoryPool::ExtendMemoryBlockP() noexcept {
    //allocate new memory page
    MemoryPageHead_* memory_page_head_ptr = reinterpret_cast<MemoryPageHead_*>(
        ZSystemMemoryPool::Instance().Alloc64KBAlignmentMemoryAtomPageP(kApplyMemoryAtomPageNum)
    );
    memory_page_head_ptr->next_memory_page_ptr_ = head_memory_page_ptr_;
    head_memory_page_ptr_ = memory_page_head_ptr;

    //initialize new memory page
    unused_memory_block_ptr_ = reinterpret_cast<Byte*>(memory_page_head_ptr) + kMemoryPageStartPointerOffset;
    unused_memory_block_end_ptr_ = reinterpret_cast<Byte*>(memory_page_head_ptr) + kMemoryPageSize;
}

}//internal
}//memory_pool
}//zengine