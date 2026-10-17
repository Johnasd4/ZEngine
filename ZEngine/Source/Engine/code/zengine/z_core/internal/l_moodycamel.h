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

#define MOODYCAMEL_EXCEPTIONS_ENABLED 0
#include "moodycamel/concurrentqueue.h"
#include "moodycamel/readerwriterqueue.h"

#include "../f_memory_pool.h"

namespace zengine {
namespace internal {

/**
 * @brief Custom traits for the moodycamel concurrent queue to use ZEngine's memory pool.
 *
 * This struct overrides the default memory allocation and deallocation methods
 * of the moodycamel concurrent queue to utilize ZEngine's memory pool for better
 * performance and memory management.
 */
struct ZEngineQueueTraits : public moodycamel::ConcurrentQueueDefaultTraits {
    static inline Void* malloc(SizeType _size) {
        return memory_pool::ApplyMemory(_size);
    }

    static inline Void free(Void* _ptr) {
        return memory_pool::ReleaseMemory(_ptr);
    }
};

struct ZEngineQueueTraits : public moodycamel::ConcurrentQueueDefaultTraits {
    static inline Void* malloc(SizeType _size) {
        return memory_pool::ApplyMemory(_size);
    }

    static inline Void free(Void* _ptr) {
        return memory_pool::ReleaseMemory(_ptr);
    }
};

} // internal
} // zengine