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

#include "m_log/z_log/z_trace_log.h"

#include "c_memory_pool.h"
#include "t_specialized_memory_pool.h"

namespace zengine {
namespace memory_pool {

/**
 * @brief A thread-safe specialized memory pool implementation optimized for trace log objects.
 *
 * This pool specializes the TSpecializedMemoryPool for the specific size of zengine::log::ZTraceLog.
 */
class CACHE_LINE_ALIGN ZSpecializedTraceLogMemoryPool : public TSpecializedMemoryPool<
    true, sizeof(zengine::log::ZTraceLog),
    1U, 1U,
    1U
> {
public:
    /** @brief The name of the memory pool, used for identification and logging. */
    static constexpr ZStringView kMemoryPoolName = "Trace Log Memory Pool";

    /**
     * @brief Gets the singleton instance of the memory pool.
     * @return Reference to the singleton instance of ZSpecializedTraceLogMemoryPool.
     */
    FORCEINLINE static ZSpecializedTraceLogMemoryPool& Instance() noexcept {
        static ZSpecializedTraceLogMemoryPool instance;
        return instance;
    }

    /** @brief Default constructor that initializes the memory pool with specific performance settings if enabled. */
    FORCEINLINE ZSpecializedTraceLogMemoryPool() noexcept
#if USE_MEMORY_POOL_PERFORMANCE_TEST
        : SuperType_(kMemoryPoolName)
#else
        : SuperType_()
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST
    {}

    /** @brief Destroys the memory pool and releases resources. */
    FORCEINLINE ~ZSpecializedTraceLogMemoryPool() noexcept {}

protected:
    /** @brief Type alias for the base specialized memory pool class. */
    using SuperType_ = TSpecializedMemoryPool<
        true, sizeof(zengine::log::ZTraceLog),
        1U, 1U,
        1U
    >;
};

}//memory_pool
}//zengine