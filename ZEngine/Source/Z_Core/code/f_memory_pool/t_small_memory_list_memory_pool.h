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

#include "t_fixed_array.h"
#include "t_lookup_table.h"
#include "t_pair.h"

#include "t_list_memory_pool_base.h"
#include "t_memory_block_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct TSmallMemoryBlock : public TMemoryBlockBase{
    TMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void Initialize(Void* _pool_ptr) noexcept {
        owner_memory_pool_ptr = reinterpret_cast<TMemoryPoolBase<kIsThreadSafe>*>(_pool_ptr);
    }
};

/*
    The memory pool for small memory. This singleton pattern like class, it has
    a fixed number of instance. All the memory pool will be generated when any
    of the memory pool is used. The instance is private.
    Warning: The program will crash if a nullptr is send in.
    Template Parameters:
    - kIsThreadSafe: Thread safe or not.
*/
template<Bool kIsThreadSafe>
class TSmallMemoryListMemoryPool : public TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe> {
private:
    //The sizes of the memory blocks(includes the memory size).
    static inline constexpr SizeType kMemoryBlockTypeNum = 16;
    static inline constexpr SizeType kMemoryBlockMinSize = 32;
    static inline constexpr SizeType kMemoryBlockSizeMultGrowFactor = 2;

public:
    NODISCARD static Void* ApplyMemory(const SizeType _size) noexcept {
        static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        SizeType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        SizeType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
        memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
        memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
        if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
            memory_pool_array[memory_pool_index].momory_block_peak_num_) {
            memory_pool_array[memory_pool_index].momory_block_peak_num_ =
                memory_pool_array[memory_pool_index].memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        return memory_pool_array[memory_pool_index].SuperType_::ApplyMemory();
    }

    NODISCARD static Void* ApplyMemory(const SizeType _size, SizeType* _memory_size_ptr) noexcept {
        static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        SizeType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        SizeType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
        memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
        memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
        if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
            memory_pool_array[memory_pool_index].momory_block_peak_num_) {
            memory_pool_array[memory_pool_index].momory_block_peak_num_ =
                memory_pool_array[memory_pool_index].memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        (*_memory_size_ptr) = memory_pool_array[memory_pool_index].SuperType_::MemoryBlockMemorySize();
        return memory_pool_array[memory_pool_index].SuperType_::ApplyMemory();
    }

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    NODISCARD FORCEINLINE static Bool CheckMemory(
        TSmallMemoryListMemoryPool* _memory_pool_ptr, 
        SizeType _size
    ) noexcept {
        return _memory_pool_ptr->SuperType_::MemoryBlockMemorySize() >= _size;
    }
    NODISCARD FORCEINLINE static Bool CheckMemory(
        TSmallMemoryListMemoryPool* _memory_pool_ptr, 
        SizeType _size, 
        SizeType* _memory_size_ptr
    ) noexcept {
        return (*_memory_size_ptr = _memory_pool_ptr->SuperType_::MemoryBlockMemorySize()) >= _size;
    }

    NODISCARD FORCEINLINE static SizeType CalculateMemory(SizeType _size) noexcept {
        static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        SizeType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        return memory_pool_array[kMemorySize2MemoryPoolTable.At(size_index)].SuperType_::MemoryBlockMemorySize();
    }

    static Void ReleaseMemory(TSmallMemoryListMemoryPool* _memory_pool_ptr, Void* _memory_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
        _memory_pool_ptr->memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
        _memory_pool_ptr->SuperType_::ReleaseMemory(_memory_ptr);
    }

    NODISCARD static constexpr SizeType MemoryBlockMemoryMaxSize() noexcept { return kMemoryBlockMemoryMaxSize; }
    NODISCARD static constexpr SizeType MemoryBlockTypeNum() noexcept { return kMemoryBlockTypeNum; }

#ifdef USE_MEMORY_POOL_TEST
    NODISCARD static TFixedArray<TPair<SizeType, SizeType*>, kMemoryBlockTypeNum>& MemoryBlockUsedNum() noexcept {
        static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        static TFixedArray<TPair<SizeType, SizeType*>, kMemoryBlockTypeNum> used_memory_array;
        for (SizeType index = 0; index < memory_pool_array.Capacity(); ++index) {
            used_memory_array[index].first_ = memory_pool_array[index - 1].SuperType_::MemoryBlockMemorySize();
            used_memory_array[index].second_ = &memory_pool_array[index - 1].memory_block_used_current_num_;
        }
        return used_memory_array;
    }
#endif //USE_MEMORY_POOL_TEST

    FORCEINLINE TSmallMemoryListMemoryPool() : SuperType_() {}
    ~TSmallMemoryListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
        ReturnType link_code = kOK;

        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
        }

        //The first pool realsed.
        if (SuperType_::MemoryBlockSize() == kMemoryBlockMaxSize) {
            link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print("\n***** small memory pool *****\n\n");
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
            }

            link_code = TMemoryPoolBase<kIsThreadSafe>::log_file_.Print("    size    | usable size |  total num  | applied times | used peak num | unreleased num\n");
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
            }
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
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCode_LinkError, link_code, L"ZFile::OpenSafe() link error!");
        }
#endif //USE_MEMORY_POOL_TEST        
    }

protected:
    using SuperType_ = TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe>;

private:
    static inline constexpr SizeType kMemoryBlockHeadSize = SuperType_::NodeHeadOffset();
    static inline constexpr TFixedArray<SizeType, kMemoryBlockTypeNum> kMemoryBlockSizeArray =
        TFixedArray<SizeType, kMemoryBlockTypeNum>([](TFixedArray<SizeType, kMemoryBlockTypeNum>* _array_ptr) {
        (*_array_ptr)[0] = kMemoryBlockMinSize;
            for (SizeType index = 1; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = (*_array_ptr)[index - 1] * kMemoryBlockSizeMultGrowFactor;
            }
        });
    static inline constexpr SizeType kMemoryBlockMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1];
    static inline constexpr SizeType kMemoryBlockMemoryMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1]
        - kMemoryBlockHeadSize;

    //The sizes of the memorys that can be uesd.
    static inline constexpr TFixedArray<SizeType, kMemoryBlockTypeNum> kMemoryBlockMemorySizeArray =
        TFixedArray<SizeType, kMemoryBlockTypeNum>([](TFixedArray<SizeType, kMemoryBlockTypeNum>* _array_ptr) {
            for (SizeType index = 0; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = kMemoryBlockSizeArray[index] - SuperType_::NodeHeadOffset();
            }
        });

    //The number of the blocks that the memory pool contains when created.
    static inline constexpr SizeType kMemoryBlockDefaultNum = 0;
    static inline constexpr TFixedArray<SizeType, kMemoryBlockTypeNum> kMemoryBlockDefaultNumArray =
        TFixedArray<SizeType, kMemoryBlockTypeNum>([](TFixedArray<SizeType, kMemoryBlockTypeNum>* _array_ptr) {
            for (SizeType index = 0; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = kMemoryBlockDefaultNum;
            }
        });

    //The lookup table that links the memory size to the memory pool index.
    //mamory alignment
    static inline constexpr SizeType kMemorySize2MemoryPoolTableSize = kMemoryBlockMaxSize / kMemoryBlockMinSize;
    static inline constexpr TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize> kMemorySize2MemoryPoolTable =
        TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize>(
            [](TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize>* _table_ptr) {
                UInt8 current_pool_index = 0;
                SizeType current_pool_index_max_index = 1;
                for (SizeType index = 0; index < _table_ptr->Size(); ++index) {
                    if (index < current_pool_index_max_index) {
                        (*_table_ptr)[index] = current_pool_index;
                    }
                    else {
                        ++current_pool_index;
                        current_pool_index_max_index *= static_cast<SizeType>(kMemoryBlockSizeMultGrowFactor);
                        (*_table_ptr)[index] = current_pool_index;
                    }
                }
            });

    NODISCARD static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& InstanceP() noexcept {
        static TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum> memory_pool_array(
            [](TFixedArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* _array_ptr) {
                for (SizeType index = 0; index < _array_ptr->Capacity(); ++index) {
                    (*_array_ptr)[index].InitializeP(kMemoryBlockSizeArray[index], kMemoryBlockMemorySizeArray[index],
                        kMemoryBlockDefaultNumArray[index]);
                }
            });
        return memory_pool_array;
    }

    TSmallMemoryListMemoryPool(const TSmallMemoryListMemoryPool&) = delete;
    TSmallMemoryListMemoryPool(TSmallMemoryListMemoryPool&&) = delete;

    TSmallMemoryListMemoryPool& operator=(const TSmallMemoryListMemoryPool&) = delete;
    TSmallMemoryListMemoryPool& operator=(TSmallMemoryListMemoryPool&&) = delete;

    FORCEINLINE Void InitializeP(
        SizeType _memory_block_size, 
        SizeType _memory_block_memory_size, 
        SizeType _capacity
    ) noexcept {
        SuperType_::InitializeP(
            MemoryPoolEnum::kTSmallMemoryList, 
            _memory_block_size,
            _memory_block_memory_size, 
            _capacity);
    }

#ifdef USE_MEMORY_POOL_TEST
    SizeType memory_block_used_current_num_ = 0;
    SizeType momory_block_applyed_num_ = 0;
    SizeType momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

}//memory_pool
}//zengine