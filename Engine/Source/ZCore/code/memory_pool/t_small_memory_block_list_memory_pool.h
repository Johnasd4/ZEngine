/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_

#include "internal/z_drive.h"

#include "f_console.h"
#include "t_array.h"
#include "t_lookup_table.h"

#include "t_list_memory_pool_base.h"
#include "t_memory_block_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct TSmallMemoryBlock : TMemoryBlockBase{
    TMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void InitializeP(Void* pool_ptr) noexcept {
        owner_memory_pool_ptr = reinterpret_cast<TMemoryPoolBase<kIsThreadSafe>*>(pool_ptr);
    }
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
class TSmallMemoryBlockListMemoryPool : 
    public TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), 
                               kIsThreadSafe> {
private:
    //The sizes of the memory blocks(includes the memory size).
    static constexpr IndexType kMemoryBlockTypeNum = 10;
    static constexpr MemoryType kMemoryBlockMinSize = 64;
    static constexpr MemoryType kMemoryBlockSizeMultGrowFactor = 2;

public:
    NODISCARD static TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& InstanceP() noexcept {
        static TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum> memory_pool_array(
            MemoryPoolArrayInitFunction);
        return memory_pool_array;
    }

    NODISCARD static Void* ApplyMemory(MemoryType size) noexcept;
    NODISCARD static Void* ApplyMemory(MemoryType size, MemoryType* memory_size_ptr) noexcept;

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    NODISCARD FORCEINLINE static Bool CheckMemory(TSmallMemoryBlockListMemoryPool* memory_pool_ptr, 
                                                  MemoryType size) noexcept {
        return memory_pool_ptr->SuperType::MemoryBlockMemorySize() >= size;
    }
    NODISCARD FORCEINLINE static Bool CheckMemory(TSmallMemoryBlockListMemoryPool* memory_pool_ptr, MemoryType size, 
                                                  MemoryType* memory_size_ptr) noexcept {
        return (*memory_size_ptr = memory_pool_ptr->SuperType::MemoryBlockMemorySize()) >= size;
    }

    NODISCARD FORCEINLINE static MemoryType CalculateMemory(MemoryType size) noexcept {
        static TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        IndexType size_index = (size + SuperType::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        return memory_pool_array[kMemorySize2MemoryPoolTable.At(size_index)].SuperType::MemoryBlockMemorySize();
    }

    static Void ReleaseMemory(TSmallMemoryBlockListMemoryPool* memory_pool_ptr, Void* memory_ptr) noexcept;

    NODISCARD static constexpr MemoryType MemoryBlockMemoryMaxSize() noexcept { return kMemoryBlockMemoryMaxSize; }
    NODISCARD static constexpr IndexType MemoryBlockTypeNum() noexcept { return kMemoryBlockTypeNum; }

    FORCEINLINE TSmallMemoryBlockListMemoryPool() : SuperType() {}
    ~TSmallMemoryBlockListMemoryPool() noexcept;
protected:
    using SuperType =
        TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe>;

private:
    static constexpr MemoryType kMemoryBlockHeadSize = SuperType::NodeHeadOffset();
    static constexpr TArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockSizeArray =
        TArray<MemoryType, kMemoryBlockTypeNum>([](TArray<MemoryType, kMemoryBlockTypeNum>* array_ptr) {
        (*array_ptr)[0] = kMemoryBlockMinSize;
            for (IndexType index = 1; index < array_ptr->Capacity(); ++index) {
                (*array_ptr)[index] = (*array_ptr)[index - 1] * kMemoryBlockSizeMultGrowFactor;
            }
        });
    static constexpr MemoryType kMemoryBlockMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1];
    static constexpr MemoryType kMemoryBlockMemoryMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1]
        - kMemoryBlockHeadSize;

    //The sizes of the memorys that can be uesd.
    static constexpr TArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockMemorySizeArray =
        TArray<MemoryType, kMemoryBlockTypeNum>([](TArray<MemoryType, kMemoryBlockTypeNum>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->Capacity(); ++index) {
                (*array_ptr)[index] = kMemoryBlockSizeArray[index] - SuperType::NodeHeadOffset();
            }
        });

    //The number of the blocks that the memory pool contains when created.
    static constexpr Int32 kMemoryBlockDefaultNum = 0;
    static constexpr TArray<IndexType, kMemoryBlockTypeNum> kMemoryBlockDefaultNumArray =
        TArray<IndexType, kMemoryBlockTypeNum>([](TArray<IndexType, kMemoryBlockTypeNum>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->Capacity(); ++index) {
                (*array_ptr)[index] = kMemoryBlockDefaultNum;
            }
        });

    //The lookup table that links the memory size to the memory pool index.
    //mamory alignment
    static constexpr IndexType kkMemorySize2MemoryPoolTableSize = kMemoryBlockMaxSize / kMemoryBlockMinSize;
    static constexpr TLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize> kMemorySize2MemoryPoolTable =
        TLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>(
            [](TLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>* table_ptr) {
                UInt8 current_pool_index = 0;
                IndexType current_pool_index_max_index = 1;
                for (IndexType index = 0; index < table_ptr->Size(); ++index) {
                    if (index < current_pool_index_max_index) {
                        (*table_ptr)[index] = current_pool_index;
                    }
                    else {
                        ++current_pool_index;
                        current_pool_index_max_index *= static_cast<IndexType>(kMemoryBlockSizeMultGrowFactor);
                        (*table_ptr)[index] = current_pool_index;
                    }
                }
            });

    static Void MemoryPoolArrayInitFunction(
        TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* array_ptr) noexcept;

    TSmallMemoryBlockListMemoryPool(const TSmallMemoryBlockListMemoryPool&) = delete;
    TSmallMemoryBlockListMemoryPool(TSmallMemoryBlockListMemoryPool&&) = delete;

    TSmallMemoryBlockListMemoryPool& operator=(const TSmallMemoryBlockListMemoryPool&) = delete;
    TSmallMemoryBlockListMemoryPool& operator=(TSmallMemoryBlockListMemoryPool&&) = delete;

    FORCEINLINE Void InitializeP(MemoryType memory_block_size, MemoryType memory_block_memory_size, 
                                 Int32 capacity) noexcept {
        SuperType::InitializeP(MemoryPoolEnum::kTSmallMemoryBlockListMemoryPool, memory_block_size,
                               memory_block_memory_size, capacity);
    }


#ifdef USE_MEMORY_POOL_TEST
    IndexType memory_block_used_current_num_ = 0;
    IndexType momory_block_applyed_num_ = 0;
    IndexType momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

template<Bool kIsThreadSafe>
NODISCARD Void* TSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size) noexcept {
    static TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
    IndexType size_index = (size + SuperType::NodeHeadOffset() - 1)/ kMemoryBlockMinSize;
    IndexType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
    memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
    if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
        memory_pool_array[memory_pool_index].momory_block_peak_num_) {
        memory_pool_array[memory_pool_index].momory_block_peak_num_ =
            memory_pool_array[memory_pool_index].memory_block_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    return memory_pool_array[memory_pool_index].SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
NODISCARD Void* TSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size, 
                                                                            MemoryType* memory_size_ptr) noexcept {
    static TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
    IndexType size_index = (size + SuperType::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
    IndexType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
    memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
    if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
        memory_pool_array[memory_pool_index].momory_block_peak_num_) {
        memory_pool_array[memory_pool_index].momory_block_peak_num_ =
            memory_pool_array[memory_pool_index].memory_block_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    (*memory_size_ptr) = memory_pool_array[memory_pool_index].SuperType::MemoryBlockMemorySize();
    return memory_pool_array[memory_pool_index].SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
Void TSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ReleaseMemory(TSmallMemoryBlockListMemoryPool* memory_pool_ptr, 
                                                                   Void* memory_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_ptr->memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
    memory_pool_ptr->SuperType::ReleaseMemory(memory_ptr);
}

template<Bool kIsThreadSafe>
Void TSmallMemoryBlockListMemoryPool<kIsThreadSafe>::MemoryPoolArrayInitFunction(
        TArray<TSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* array_ptr) noexcept {
    for (IndexType index = 0; index < array_ptr->Capacity(); ++index) {
        (*array_ptr)[index].InitializeP(kMemoryBlockSizeArray[index], kMemoryBlockMemorySizeArray[index],
                                       kMemoryBlockDefaultNumArray[index]);
    }
}

template<Bool kIsThreadSafe>
TSmallMemoryBlockListMemoryPool<kIsThreadSafe>::~TSmallMemoryBlockListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
    //The first pool realsed.
    if (SuperType::MemoryBlockSize() == kMemoryBlockMaxSize) {
        zengine::console::PrintMessage("\n\n***** small memory block pool *****\n\n");
        zengine::console::PrintMessage("    size    | usable size |  total num  | applied times | used peak num | unused num\n");
    }
    zengine::console::PrintMessage(
        "  %8u  |  %9u  |  %9d  |   %9d   |   %9d   |  %8d\n",
        SuperType::MemoryBlockSize(),
        SuperType::MemoryBlockMemorySize(),
        SuperType::Capacity(),
        momory_block_applyed_num_,
        momory_block_peak_num_,
        memory_block_used_current_num_);
#endif //USE_MEMORY_POOL_TEST        
}

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_
