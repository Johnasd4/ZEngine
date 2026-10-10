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
#if USE_MEMORY_POOL_PERFORMANCE_TEST

#include "z_mutex.h"

#include "c_memory_pool.h"

namespace zengine {
namespace memory_pool {

/**
 * @brief A singleton class for testing and recording memory pool performance.
 *
 * This class collects statistics such as allocation counts, peak usage, and unreleased blocks
 * for global, thread-local, and system memory pools.
 */
class ZGenericMemoryPoolPerformanceTest {
public:
    /**
     * @brief Memory pool performance data for global memory pools.
     */
    struct GlobalMemoryPoolPerformanceData {
        /** @brief The size of a single memory block in bytes. */
        UInt32 memory_block_size_ = 0U;
        /** @brief The number of memory blocks currently allocated but not yet released. */
        UInt32 memory_block_unreleased_num_ = 0U;
        /** @brief The total number of memory block allocation requests made. */
        UInt32 memory_block_applied_num_ = 0U;
        /** @brief The peak number of simultaneously allocated memory blocks. */
        UInt32 memory_block_peak_num_ = 0U;
        /** @brief The total number of memory blocks managed by this pool. */
        UInt32 memory_block_total_num_ = 0U;
    };

    /**
     * @brief Memory pool performance data for thread local memory pools.
     *
     * Aligned to cache line size to prevent false sharing.
     */
    struct CACHE_ALIGN ThreadLocalMemoryPoolPerformanceData {
        /** @brief Mutex for synchronizing access to performance data. */
        ZMutex mutex_;
        /** @brief The size of a single memory block in bytes. */
        UInt32 memory_block_size_ = 0U;
        /** @brief The number of memory blocks currently allocated but not yet released. */
        UInt32 memory_block_unreleased_num_ = 0U;
        /** @brief The total number of memory block allocation requests made. */
        UInt32 memory_block_applied_num_ = 0U;
    };

    /**
     * @brief Memory pool performance data for system memory pool.
     */
    struct SystemMemoryPoolPerformanceData {
        /** @brief Number of blocks currently allocated. */
        UInt32 memory_block_unreleased_num_ = 0U;
        /** @brief Total number of allocations performed. */
        UInt32 memory_block_applied_num_ = 0U;
        /** @brief Peak number of concurrent blocks allocated. */
        UInt32 memory_block_peak_num_ = 0U;
        /** @brief Total size of blocks currently allocated. */
        SizeType memory_block_unreleased_size_ = 0ULL;
        /** @brief Total size of memory applied for since start. */
        SizeType memory_block_applied_size_ = 0ULL;
        /** @brief Peak size of concurrent memory allocated. */
        SizeType memory_block_peak_size_ = 0ULL;
    };

public:
    /**
     * @brief Gets the singleton instance of the performance test manager.
     * @return A reference to the ZGenericMemoryPoolPerformanceTest instance.
     */
    static ZGenericMemoryPoolPerformanceTest& GetInstance() noexcept {
        static ZGenericMemoryPoolPerformanceTest instance;
        return instance;
    }

private:
    /**
     * @brief Default constructor.
     *
     * Initializes all performance data structures to zero.
     */
    ZGenericMemoryPoolPerformanceTest() noexcept;

    /** @brief Deleted copy constructor. */
    ZGenericMemoryPoolPerformanceTest(const ZGenericMemoryPoolPerformanceTest&) = delete;

    /** @brief Deleted move constructor. */
    ZGenericMemoryPoolPerformanceTest(ZGenericMemoryPoolPerformanceTest&&) = delete;

    /**
     * @brief Destructor.
     *
     * Logs the collected performance data to a file upon destruction.
     */
    ~ZGenericMemoryPoolPerformanceTest() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZGenericMemoryPoolPerformanceTest& operator=(const ZGenericMemoryPoolPerformanceTest&) = delete;

    /** @brief Deleted move assignment operator. */
    ZGenericMemoryPoolPerformanceTest& operator=(ZGenericMemoryPoolPerformanceTest&&) = delete;

public:
    /** @brief Array of performance data for global small memory pools. */
    GlobalMemoryPoolPerformanceData global_small_memory_pool_performance_data_[kSmallMemoryPoolNum];
    /** @brief Array of performance data for global medium memory pools. */
    GlobalMemoryPoolPerformanceData global_medium_memory_pool_performance_data_[kMediumMemoryPoolNum];
    /** @brief Array of performance data for global large memory pools. */
    GlobalMemoryPoolPerformanceData global_large_memory_pool_performance_data_[kLargeMemoryPoolNum];
    /** @brief Performance data for the system memory pool. */
    SystemMemoryPoolPerformanceData system_memory_pool_performance_data_;
    /** @brief Array of performance data for thread-local small memory pools. */
    ThreadLocalMemoryPoolPerformanceData thread_local_small_memory_pool_performance_data_[kSmallMemoryPoolNum];
    /** @brief Array of performance data for thread-local medium memory pools. */
    ThreadLocalMemoryPoolPerformanceData thread_local_medium_memory_pool_performance_data_[kMediumMemoryPoolNum];
};

} // memory_pool
} // zengine

#endif // USE_MEMORY_POOL_PERFORMANCE_TEST