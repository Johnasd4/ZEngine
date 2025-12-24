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

#include "z_generic_memory_pool.h"

namespace zengine {
namespace memory_pool {

NODISCARD ZGenericMemoryPool::MemoryPageHead_* ZGenericMemoryPool::Alloc64KBAlignmentMemoryAtomPageP(
    UInt32 _page_num
) noexcept {
    //apply memory
    Void* raw_mem_ptr = ZSystemMemoryPool::Instance().Alloc64KBAlignmentMemoryAtomPageP(_page_num);
    if (raw_mem_ptr == nullptr) {
        return nullptr;
    }

    //initialize memory page head
    MemoryPageHead_* mem_page_head_ptr = static_cast<MemoryPageHead_*>(raw_mem_ptr);
    mem_page_head_ptr->next_page_ptr_ = head_memory_page_ptr_;

    //insert to linked list head
    head_memory_page_ptr_ = mem_page_head_ptr;

    return mem_page_head_ptr;
}

NODISCARD ZGenericMemoryPool::MemoryPageHead_* ZGenericMemoryPool::Alloc2MBAlignmentMemoryAtomPageP(
    UInt32 _page_num
) noexcept {
    //apply memory
    Void* raw_mem_ptr = ZSystemMemoryPool::Instance().Alloc2MBAlignmentMemoryAtomPageP(_page_num);
    if (raw_mem_ptr == nullptr) {
        return nullptr;
    }

    //initialize memory page head
    MemoryPageHead_* mem_page_head_ptr = static_cast<MemoryPageHead_*>(raw_mem_ptr);
    mem_page_head_ptr->next_page_ptr_ = head_memory_page_ptr_;

    //insert to linked list head
    head_memory_page_ptr_ = mem_page_head_ptr;

    return mem_page_head_ptr;
}

}//memory_pool
}//zengine