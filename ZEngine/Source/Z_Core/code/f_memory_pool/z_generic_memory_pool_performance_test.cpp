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
#if USE_MEMORY_POOL_PERFORMANCE_TEST

#include "z_generic_memory_pool_performance_test.h"

#include "m_log.h"
#include "z_file.h"

#include "z_generic_global_small_memory_pool.h"

namespace zengine {
namespace memory_pool {

ZGenericMemoryPoolPerformanceTest::ZGenericMemoryPoolPerformanceTest() noexcept
    : global_small_memory_pool_performance_data_()
    , global_medium_memory_pool_performance_data_()
    , global_large_memory_pool_performance_data_()
    , system_memory_pool_performance_data_()
    , thread_local_small_memory_pool_performance_data_()
    , thread_local_medium_memory_pool_performance_data_()
{}

ZGenericMemoryPoolPerformanceTest::~ZGenericMemoryPoolPerformanceTest() noexcept {
    ZFile file;
    ReturnType link_code = kOK;
    const ZSystemTime& system_time = ZSystemTime::GetProcessStartTimeInstance();

    TFixedString<kMaxFileDirLength> temp_str;
    SizeType temp_str_size;

    temp_str_size = temp_str.Assign(
        "{}/Memory_Performance_Generic.log",
        log::ZLog::CreateAndGetLogPath()
    );

    link_code = file.Open(
        temp_str.GetDataPtr(),
        ZFile::kWriteOnly | ZFile::kCreate | ZFile::kAppend
    );
    if (link_code != kOK) {
        return;
    }

    // global small memory pool performance
    // check whether initialized
    if (global_small_memory_pool_performance_data_[0].memory_block_size_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** global small memory pool *****\n"
            "    size    |  total num  | applied times | used peak num | unreleased num\n"
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
        for (SizeType index = 0; index < kSmallMemoryPoolNum; ++index) {
            GlobalMemoryPoolPerformanceData& performance_data = global_small_memory_pool_performance_data_[index];
            temp_str_size = temp_str.Assign(
                "  {:>8}  |  {:>9}  |   {:>9}   |   {:>9}   |  {:>8}\n",
                performance_data.memory_block_size_,
                performance_data.memory_block_total_num_,
                performance_data.memory_block_applied_num_,
                performance_data.memory_block_peak_num_,
                performance_data.memory_block_unreleased_num_
            );
            link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
            if (link_code != kOK) {
                return;
            }
        }
    }

    // global medium memory pool performance
    // check whether initialized
    if (global_medium_memory_pool_performance_data_[0].memory_block_size_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** global medium memory pool *****\n"
            "    size    |  total num  | applied times | used peak num | unreleased num\n"
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
        for (SizeType index = 0; index < kMediumMemoryPoolNum; ++index) {
            GlobalMemoryPoolPerformanceData& performance_data = global_medium_memory_pool_performance_data_[index];
            temp_str_size = temp_str.Assign(
                "  {:>8}  |  {:>9}  |   {:>9}   |   {:>9}   |  {:>8}\n",
                performance_data.memory_block_size_,
                performance_data.memory_block_total_num_,
                performance_data.memory_block_applied_num_,
                performance_data.memory_block_peak_num_,
                performance_data.memory_block_unreleased_num_
            );
            link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
            if (link_code != kOK) {
                return;
            }
        }
    }

    // global large memory pool performance
    // check whether initialized
    if (global_large_memory_pool_performance_data_[0].memory_block_size_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** global large memory pool *****\n"
            "    size    |  total num  | applied times | used peak num | unreleased num\n"
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
        for (SizeType index = 0; index < kLargeMemoryPoolNum; ++index) {
            GlobalMemoryPoolPerformanceData& performance_data = global_large_memory_pool_performance_data_[index];
            temp_str_size = temp_str.Assign(
                "  {:>8}  |  {:>9}  |   {:>9}   |   {:>9}   |  {:>8}\n",
                performance_data.memory_block_size_,
                performance_data.memory_block_total_num_,
                performance_data.memory_block_applied_num_,
                performance_data.memory_block_peak_num_,
                performance_data.memory_block_unreleased_num_
            );
            link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
            if (link_code != kOK) {
                return;
            }
        }
    }

    // system memory pool performance
    // check whether initialized
    if (system_memory_pool_performance_data_.memory_block_applied_num_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** system memory pool *****\n"
            "     applied times    |  used peak num | unreleased num\n"
            "  {:>18}  |  {:>12}  |  {:>12}\n"
            "     applied size     | used peak size | unreleased size\n"
            "  {:>18}  |  {:>12}  |  {:>12}\n",
            system_memory_pool_performance_data_.memory_block_applied_num_,
            system_memory_pool_performance_data_.memory_block_peak_num_,
            system_memory_pool_performance_data_.memory_block_unreleased_num_,
            system_memory_pool_performance_data_.memory_block_applied_size_,
            system_memory_pool_performance_data_.memory_block_peak_size_,
            system_memory_pool_performance_data_.memory_block_unreleased_size_
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
    }

    // thread local small memory pool performance
    // check whether initialized
    if (thread_local_small_memory_pool_performance_data_[0].memory_block_size_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** thread local small memory pool *****\n"
            "    size    | applied times | unreleased num\n"
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
        for (SizeType index = 0; index < kSmallMemoryPoolNum; ++index) {
            ThreadLocalMemoryPoolPerformanceData& performance_data =
                thread_local_small_memory_pool_performance_data_[index];
            temp_str_size = temp_str.Assign(
                "  {:>8}  |   {:>9}   |  {:>8}\n",
                performance_data.memory_block_size_,
                performance_data.memory_block_applied_num_,
                performance_data.memory_block_unreleased_num_
            );
            link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
            if (link_code != kOK) {
                return;
            }
        }
    }

    // thread local medium memory pool performance
    // check whether initialized
    if (thread_local_medium_memory_pool_performance_data_[0].memory_block_size_ != 0U) {
        temp_str_size = temp_str.Assign(
            "\n***** thread local medium memory pool *****\n"
            "    size    | applied times | unreleased num\n"
        );
        link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
        if (link_code != kOK) {
            return;
        }
        for (SizeType index = 0; index < kMediumMemoryPoolNum; ++index) {
            ThreadLocalMemoryPoolPerformanceData& performance_data =
                thread_local_medium_memory_pool_performance_data_[index];
            temp_str_size = temp_str.Assign(
                "  {:>8}  |   {:>9}   |  {:>8}\n",
                performance_data.memory_block_size_,
                performance_data.memory_block_applied_num_,
                performance_data.memory_block_unreleased_num_
            );
            link_code = file.Write(ZBuffer(temp_str.GetDataPtr(), temp_str_size));
            if (link_code != kOK) {
                return;
            }
        }
    }

    file.Close();  
}

} // memory_pool
} // zengine

#endif // USE_MEMORY_POOL_PERFORMANCE_TEST     