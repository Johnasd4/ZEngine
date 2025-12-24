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

#include "../drive.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/allocators.h"
#include "rapidjson/prettywriter.h"

#include "../f_memory_pool.h"

namespace zengine {
namespace internal {
    
class JsonAllocatorP {
public:
    static constexpr Bool kNeedFree = true;

    FORCEINLINE Void* Malloc(SizeType _size) noexcept {
        if (_size == 0U) {
            return nullptr;
        }
        return memory_pool::ApplyThreadLocalMemory(static_cast<SizeType>(_size));
    }

    FORCEINLINE Void* Realloc(Void* _old_ptr, SizeType _old_size, SizeType _new_size) noexcept {
        if (_new_size == 0U) {
            memory_pool::ReleaseThreadLocalMemory(_old_ptr);
            return nullptr;
        }
        return memory_pool::ReapplyThreadLocalMemory(_old_ptr, static_cast<SizeType>(_new_size));
    }

    FORCEINLINE static Void Free(Void* _mem_ptr) noexcept {
        memory_pool::ReleaseThreadLocalMemory(_mem_ptr);
    }
};
using JsonAllocator = rapidjson::MemoryPoolAllocator<internal::JsonAllocatorP>;
using JsonDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, internal::JsonAllocator, internal::JsonAllocatorP>;
using JsonValue = rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>;

}//internal
}//zengine