/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "internal/z_drive.h"

#include "t_list_memory_pool_base.h"
#include "t_memory_block_base.h"
#include "t_smart_pointer.h"

namespace zengine {
namespace memory_pool {

struct TSmartPtrBlock : public TMemoryBlockBase{
    FORCEINLINE Void Initialize(Void* _pool_ptr) noexcept {}
};

/*
    The memory pool for small memory. This singleton pattern like class, it has
    a fixed number of instance. All the memory pool will be generated when any
    of the memory pool is used. The instance is private.
    Waring: The program will crash if a nullptr is send in.
    Template Parameters:
    - kIsThreadSafe: Thread safe or not.
*/
template<Bool kIsThreadSafe>
class TSmartPointerListMemoryPool : public TListMemoryPoolBase<TSmartPtrBlock, 0, kIsThreadSafe> {
public:
    NODISCARD FORCEINLINE static Void* ApplyMemory() noexcept {
        static TSmartPointerListMemoryPool<kIsThreadSafe>& memory_pool = InstanceP();
#ifdef USE_MEMORY_POOL_TEST
        memory_pool.memory_block_used_current_num_ += 1;
        memory_pool.momory_block_applyed_num_ += 1;
        if (memory_pool.memory_block_used_current_num_ >
            memory_pool.momory_block_peak_num_) {
            memory_pool.momory_block_peak_num_ =
                memory_pool.memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        return memory_pool.SuperType_::ApplyMemory();
    }

    FORCEINLINE static Void ReleaseMemory(Void* _memory_ptr) noexcept {
        static TSmartPointerListMemoryPool<kIsThreadSafe>& memory_pool = InstanceP();
#ifdef USE_MEMORY_POOL_TEST
        memory_pool.memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
        memory_pool.SuperType_::ReleaseMemory(_memory_ptr);
    }

#ifdef USE_MEMORY_POOL_TEST
    NODISCARD static TPair<MemoryType, Int32*> MemoryBlockUsedNum() noexcept {
        static TSmartPointerListMemoryPool<kIsThreadSafe>& memory_pool = InstanceP();
        TPair<MemoryType, Int32*> used_memory;
        used_memory.first_ = memory_pool.SuperType_::MemoryBlockMemorySize();
        used_memory.second_ = &memory_pool.memory_block_used_current_num_;
        return used_memory;
    }
#endif //USE_MEMORY_POOL_TEST

    FORCEINLINE TSmartPointerListMemoryPool() : SuperType_() {
        SuperType_::InitializeP(
            MemoryPoolEnum::kMemoryPool_TSmartPointerList,
            kMemoryBlockSize,
            kMemoryBlockMemorySize,
            kMemoryBlockDefaultNum);
    }
    ~TSmartPointerListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
        ReturnType link_code = kOK;

        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print("\n***** smart pointer pool *****\n\n");
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }

        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print(
            "    size    | usable size |  total num  | applied times | used peak num | unused num\n"
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }
        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print(
            "  %8u  |  %9u  |  %9d  |   %9d   |   %9d   |  %8d\n",
            SuperType_::MemoryBlockSize(),
            SuperType_::MemoryBlockMemorySize(),
            SuperType_::Capacity(),
            momory_block_applyed_num_,
            momory_block_peak_num_,
            memory_block_used_current_num_);
        TMemoryPoolBase<kIsThreadSafe>::log_file_.Flush();
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }
#endif //USE_MEMORY_POOL_TEST        
    }

protected:
    using SuperType_ = TListMemoryPoolBase<TSmartPtrBlock, 0, kIsThreadSafe>;

private:
    static constexpr MemoryType kMemoryBlockHeadSize = SuperType_::NodeHeadOffset();
    static constexpr MemoryType kMemoryBlockMemorySize = sizeof(internal::TControlBlockP);
    static constexpr MemoryType kMemoryBlockSize = kMemoryBlockMemorySize + kMemoryBlockHeadSize;

    //The number of the blocks that the memory pool contains when created.
    static constexpr IndexType kMemoryBlockDefaultNum = 0;

    NODISCARD static TSmartPointerListMemoryPool<kIsThreadSafe>& InstanceP() noexcept {
        static TSmartPointerListMemoryPool<kIsThreadSafe> memory_pool_array;
        return memory_pool_array;
    }

    TSmartPointerListMemoryPool(const TSmartPointerListMemoryPool&) = delete;
    TSmartPointerListMemoryPool(TSmartPointerListMemoryPool&&) = delete;

    TSmartPointerListMemoryPool& operator=(const TSmartPointerListMemoryPool&) = delete;
    TSmartPointerListMemoryPool& operator=(TSmartPointerListMemoryPool&&) = delete;

#ifdef USE_MEMORY_POOL_TEST
    IndexType memory_block_used_current_num_ = 0;
    IndexType momory_block_applyed_num_ = 0;
    IndexType momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

}//memory_pool
}//zengine