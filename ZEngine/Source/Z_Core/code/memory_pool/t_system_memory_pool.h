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

#include "t_memory_pool_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct TSystemMemoryBlock : public TMemoryBlockBase {
    TMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void Initialize(Void* _pool_ptr) noexcept {
        owner_memory_pool_ptr = reinterpret_cast<TMemoryPoolBase<kIsThreadSafe>*>(_pool_ptr);
    }
};

/*
    The base type of the memory pool that use the structure of list pool.
    Template Parameters:
    - kIsThreadSafe: Thread safe or not.
*/
template<Bool kIsThreadSafe>
class TSystemMemoryPool : public TMemoryPoolBase<kIsThreadSafe> {
private:
    //The multipul factor that container auto extends based on the origin size.
    static constexpr PointerType kNodeHeadOffset = 8ULL;

public:
    using SuperType_ = TMemoryPoolBase<kIsThreadSafe>;
    using MutexType_ = TMemoryPoolThreadSafeBase<kIsThreadSafe>;
        
    FORCEINLINE static constexpr MemoryType NodeHeadOffset() noexcept { return kNodeHeadOffset; }

    NODISCARD static Void* ApplyMemory(const MemoryType _size) noexcept {
        static TSystemMemoryPool& memory_pool = InstanceP();
        SizeType size = _size + kNodeHeadOffset;
#ifdef USE_MEMORY_POOL_TEST
        memory_pool.memory_block_used_current_num_ += 1;
        memory_pool.momory_block_applyed_num_ += 1;
        if (memory_pool.memory_block_used_current_num_ >
            memory_pool.momory_block_peak_num_) {
            memory_pool.momory_block_peak_num_ = memory_pool.memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        TSystemMemoryBlock<kIsThreadSafe>* block_ptr = 
            reinterpret_cast<TSystemMemoryBlock<kIsThreadSafe>*>(malloc(size));
        if (block_ptr == nullptr) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_ApplyHeapMemoryFailed, 0,
                L"Apply heap memory failed! Exit program! size: %d", _size);
            exit(EXIT_FAILURE);
            return nullptr;
        }
        block_ptr->Initialize(&memory_pool);
        Void* memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(block_ptr) + kNodeHeadOffset);
        return memory_ptr;
    }

    NODISCARD static Void* RepplyMemory(Void* _memory_ptr, const MemoryType _size) noexcept {
        static TSystemMemoryPool& memory_pool = InstanceP();
        SizeType size = _size + kNodeHeadOffset;
        Void* block_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(_memory_ptr) - kNodeHeadOffset);
        block_ptr = realloc(block_ptr, size);
        if (block_ptr == nullptr) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_ApplyHeapMemoryFailed, 0,
                L"Apply heap memory failed! Exit program! size: %d", _size);
            exit(EXIT_FAILURE);
            return nullptr;
        }
        reinterpret_cast<TSystemMemoryBlock<kIsThreadSafe>*>(block_ptr)->Initialize(&memory_pool);
        Void* memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(block_ptr) + kNodeHeadOffset);
        return memory_ptr;
    }

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    NODISCARD FORCEINLINE static Bool CheckMemory(MemoryType _size) noexcept {
        return false;
    }

    NODISCARD FORCEINLINE static MemoryType CalculateMemory(MemoryType _size) noexcept {
        return _size;
    }

    static Void ReleaseMemory(Void* _memory_ptr) noexcept {
        static TSystemMemoryPool& memory_pool = InstanceP();
#ifdef USE_MEMORY_POOL_TEST
        memory_pool.memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
        Void* memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(_memory_ptr) - kNodeHeadOffset);
        free(memory_ptr);
    }

#pragma warning(disable : 26495)
    FORCEINLINE TSystemMemoryPool() : SuperType_() { InitializeP(); }
#pragma warning(default : 26495)

    ~TSystemMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
        ReturnType link_code = kOK;

        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print("\n***** system memory pool *****\n\n");
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }

        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print("applied times | used peak num | unused num\n");
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }

        link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print(
            "   %9d   |   %9d   |  %8d\n",
            momory_block_applyed_num_,
            momory_block_peak_num_,
            memory_block_used_current_num_);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::Print() link error!");
            return;
        }
#endif //USE_MEMORY_POOL_TEST        
    }

protected:
    using SuperType_ = TMemoryPoolBase<kIsThreadSafe>;

private:
    NODISCARD static TSystemMemoryPool& InstanceP() noexcept {
        static TSystemMemoryPool memory_pool;
        return memory_pool;
    }

    FORCEINLINE Void InitializeP() noexcept {
        SuperType_::InitializeP(MemoryPoolEnum::kMemoryPool_TSystemMemory);
    }

    TSystemMemoryPool(const TSystemMemoryPool&) = delete;
    TSystemMemoryPool(TSystemMemoryPool&&) = delete;

    TSystemMemoryPool& operator=(const TSystemMemoryPool&) = delete;
    TSystemMemoryPool& operator=(TSystemMemoryPool&&) = delete;

#ifdef USE_MEMORY_POOL_TEST
    Int32 memory_block_used_current_num_ = 0;
    Int32 momory_block_applyed_num_ = 0;
    Int32 momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

}//memory_pool
}//zengine