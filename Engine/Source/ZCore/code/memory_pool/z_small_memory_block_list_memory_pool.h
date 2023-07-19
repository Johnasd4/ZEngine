#ifndef Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_

#include "internal/drive.h"

#include "f_console_output.h"
#include "z_array.h"
#include "z_lookup_table.h"

#include "z_list_memory_pool_base.h"
#include "z_memory_block_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct ZSmallMemoryBlock : ZMemoryBlockBase{
    ZMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void Initialize(Void* pool_ptr) {
        owner_memory_pool_ptr = reinterpret_cast<ZMemoryPoolBase<kIsThreadSafe>*>(pool_ptr);
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
class ZSmallMemoryBlockListMemoryPool : 
    public ZListMemoryPoolBase<ZSmallMemoryBlock<kIsThreadSafe>, sizeof(ZSmallMemoryBlock<kIsThreadSafe>), 
                               kIsThreadSafe> {
private:
    //The sizes of the memory blocks(includes the memory size).
    static constexpr IndexType kMemoryBlockTypeNum = 9;
    static constexpr MemoryType kMemoryBlockMinSize = 128;
    static constexpr MemoryType kMemoryBlockSizeMulGrowFactor = 2;

public:
    NODISCARD static ZArray<ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& Instance() noexcept {
        static ZArray<ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum> memory_pool_array(
            MemoryPoolArrayInitFunction);
        return memory_pool_array;
    }

    NODISCARD static Void* const ApplyMemory(const MemoryType size) noexcept;
    NODISCARD static Void* const ApplyMemory(const MemoryType size, MemoryType* const memory_size_ptr) noexcept;

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    NODISCARD FORCEINLINE static const Bool CheckMemory(ZSmallMemoryBlockListMemoryPool* const memory_pool_ptr,
                                                        const MemoryType size);
    NODISCARD FORCEINLINE static const Bool CheckMemory(ZSmallMemoryBlockListMemoryPool* const memory_pool_ptr,
                                                        const MemoryType size, MemoryType* const memory_size_ptr);

    NODISCARD FORCEINLINE static const MemoryType CalculateMemory(const MemoryType size) noexcept;

    static Void ReleaseMemory(ZSmallMemoryBlockListMemoryPool* memory_pool_ptr, Void* const memory_ptr) noexcept;

    NODISCARD static constexpr MemoryType memory_block_memory_max_size() { return kMemoryBlockMemoryMaxSize; }
    NODISCARD static constexpr IndexType memory_block_type_num() { return kMemoryBlockTypeNum; }

protected:
    using SuperType =
        ZListMemoryPoolBase<ZSmallMemoryBlock<kIsThreadSafe>, sizeof(ZSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe>;

private:
    friend class ZArray<ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>;

    static constexpr MemoryType kMemoryBlockHeadSize = SuperType::node_head_offset();
    static constexpr ZArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockSizeArray =
        ZArray<MemoryType, kMemoryBlockTypeNum>([](ZArray<MemoryType, kMemoryBlockTypeNum>* array_ptr) {
        (*array_ptr)[0] = kMemoryBlockMinSize;
            for (IndexType index = 1; index < array_ptr->size(); ++index) {
                (*array_ptr)[index] = (*array_ptr)[index - 1] * kMemoryBlockSizeMulGrowFactor;
            }
        });
    static constexpr MemoryType kMemoryBlockMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1];
    static constexpr MemoryType kMemoryBlockMemoryMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1]
        - kMemoryBlockHeadSize;

    //The sizes of the memorys that can be uesd.
    static constexpr ZArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockMemorySizeArray =
        ZArray<MemoryType, kMemoryBlockTypeNum>([](ZArray<MemoryType, kMemoryBlockTypeNum>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->size(); ++index) {
                (*array_ptr)[index] = kMemoryBlockSizeArray[index] - SuperType::node_head_offset();
            }
        });

    //The number of the blocks that the memory pool contains when created.
    static constexpr Int32 kMemoryBlockDefaultNum = 0;
    static constexpr ZArray<IndexType, kMemoryBlockTypeNum> kMemoryBlockDefaultNumArray =
        ZArray<IndexType, kMemoryBlockTypeNum>([](ZArray<IndexType, kMemoryBlockTypeNum>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->size(); ++index) {
                (*array_ptr)[index] = kMemoryBlockDefaultNum;
            }
        });

    //The lookup table that links the memory size to the memory pool index.
    //mamory alignment
    static constexpr IndexType kkMemorySize2MemoryPoolTableSize = kMemoryBlockMaxSize / kMemoryBlockMinSize;
    static constexpr ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize> kMemorySize2MemoryPoolTable =
        ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>(
            [](ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>* array_ptr) {
                UInt8 current_pool_index = 0;
                IndexType current_pool_index_max_index = 1;
                for (IndexType index = 0; index < array_ptr->size(); ++index) {
                    if (index < current_pool_index_max_index) {
                        (*array_ptr)[index] = current_pool_index;
                    }
                    else {
                        ++current_pool_index;
                        current_pool_index_max_index *= static_cast<IndexType>(kMemoryBlockSizeMulGrowFactor);
                        (*array_ptr)[index] = current_pool_index;
                    }
                }
            });

    static Void MemoryPoolArrayInitFunction(
        ZArray<ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* array_ptr) noexcept;

    FORCEINLINE ZSmallMemoryBlockListMemoryPool() : SuperType() {}

    ZSmallMemoryBlockListMemoryPool(const ZSmallMemoryBlockListMemoryPool&) = delete;
    ZSmallMemoryBlockListMemoryPool(ZSmallMemoryBlockListMemoryPool&&) = delete;

    ZSmallMemoryBlockListMemoryPool& operator=(const ZSmallMemoryBlockListMemoryPool&) = delete;
    ZSmallMemoryBlockListMemoryPool& operator=(ZSmallMemoryBlockListMemoryPool&&) = delete;

    ~ZSmallMemoryBlockListMemoryPool() noexcept;

    FORCEINLINE Void Initialize(const MemoryType memory_block_size, const MemoryType memory_block_memory_size, 
                                const Int32 capacity) noexcept;


#ifdef USE_MEMORY_POOL_TEST
    IndexType memory_block_used_current_num_ = 0;
    IndexType momory_block_applyed_num_ = 0;
    IndexType momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

template<Bool kIsThreadSafe>
NODISCARD Void* const ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size) noexcept {
    IndexType size_index = (size + SuperType::node_head_offset() - 1)/ kMemoryBlockMinSize;
#ifdef USE_MEMORY_POOL_TEST
    Instance()[kMemorySize2MemoryPoolTable.At(size_index)].memory_block_used_current_num_ += 1;
    Instance()[kMemorySize2MemoryPoolTable.At(size_index)].momory_block_applyed_num_ += 1;
    if (Instance()[kMemorySize2MemoryPoolTable.At(size_index)].memory_block_used_current_num_ >
        Instance()[kMemorySize2MemoryPoolTable.At(size_index)].momory_block_peak_num_) {
        Instance()[kMemorySize2MemoryPoolTable.At(size_index)].momory_block_peak_num_ =
            Instance()[kMemorySize2MemoryPoolTable.At(size_index)].memory_block_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    return Instance()[kMemorySize2MemoryPoolTable.At(size_index)].SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
NODISCARD Void* const ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ApplyMemory(
        const MemoryType size, MemoryType* const memory_size_ptr) noexcept {
    IndexType size_index = (size + SuperType::node_head_offset() - 1) / kMemoryBlockMinSize;
#ifdef USE_MEMORY_POOL_TEST
    Instance()[kMemorySize2MemoryPoolTable[size_index]].memory_block_used_current_num_ += 1;
    Instance()[kMemorySize2MemoryPoolTable[size_index]].momory_block_applyed_num_ += 1;
    if (Instance()[kMemorySize2MemoryPoolTable[size_index]].memory_block_used_current_num_ >
        Instance()[kMemorySize2MemoryPoolTable[size_index]].momory_block_peak_num_) {
        Instance()[kMemorySize2MemoryPoolTable[size_index]].momory_block_peak_num_ =
            Instance()[kMemorySize2MemoryPoolTable[size_index]].memory_block_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    (*memory_size_ptr) = Instance()[kMemorySize2MemoryPoolTable.At(size_index)].SuperType::memory_block_memory_size();
    return Instance()[kMemorySize2MemoryPoolTable.At(size_index)].SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
NODISCARD FORCEINLINE const Bool ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::CheckMemory(
        ZSmallMemoryBlockListMemoryPool* const memory_pool_ptr, const MemoryType size) {
    return memory_pool_ptr->SuperType::memory_block_memory_size() >= size;
}
template<Bool kIsThreadSafe>
NODISCARD FORCEINLINE const Bool ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::CheckMemory(
        ZSmallMemoryBlockListMemoryPool* const memory_pool_ptr, const MemoryType size, MemoryType* const 
        memory_size_ptr) {
    return (*memory_size_ptr = memory_pool_ptr->SuperType::memory_block_memory_size()) >= size;
}

template<Bool kIsThreadSafe>
NODISCARD FORCEINLINE const MemoryType ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::CalculateMemory(
        const MemoryType size) noexcept {
    IndexType size_index = (size + SuperType::node_head_offset() - 1) / kMemoryBlockMinSize;
    return Instance()[kMemorySize2MemoryPoolTable.At(size_index)].SuperType::memory_block_memory_size();
}

template<Bool kIsThreadSafe>
Void ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::ReleaseMemory(
    ZSmallMemoryBlockListMemoryPool* const memory_pool_ptr, Void* const memory_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_ptr->memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
    memory_pool_ptr->SuperType::ReleaseMemory(memory_ptr);
}

template<Bool kIsThreadSafe>
Void ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::MemoryPoolArrayInitFunction(
    ZArray<ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* array_ptr) noexcept {
    for (IndexType index = 0; index < array_ptr->size(); ++index) {
        (*array_ptr)[index].Initialize(kMemoryBlockSizeArray[index], kMemoryBlockMemorySizeArray[index],
                                       kMemoryBlockDefaultNumArray[index]);
    }
}

template<Bool kIsThreadSafe>
ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::~ZSmallMemoryBlockListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
    //The first pool realsed.
    if (SuperType::memory_block_size() == kMemoryBlockMaxSize) {
        zengine::console::Print(
            zengine::console::ConsoleOutputTextColour::kConsoleTextColourLightGreen,
            zengine::console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
            "\n\n***** small memory block pool *****\n\n");
        zengine::console::Print(
            zengine::console::ConsoleOutputTextColour::kConsoleTextColourLightGreen,
            zengine::console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
            "    size    | usable size |  total num  | applied times | used peak num | unused num\n");
    }
    zengine::console::Print(
        zengine::console::ConsoleOutputTextColour::kConsoleTextColourLightYellow,
        zengine::console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "  %8u  |  %9u  |  %9d  |   %9d   |   %9d   |  %8d\n",
        SuperType::memory_block_size(),
        SuperType::memory_block_memory_size(),
        SuperType::capacity(),
        momory_block_applyed_num_,
        momory_block_peak_num_,
        memory_block_used_current_num_);
#endif //USE_MEMORY_POOL_TEST        
}

template<Bool kIsThreadSafe>
FORCEINLINE Void ZSmallMemoryBlockListMemoryPool<kIsThreadSafe>::Initialize(const MemoryType memory_block_size, 
                                                                            const MemoryType memory_block_memory_size, 
                                                                            const Int32 capacity) noexcept {
    SuperType::Initialize(MemoryPoolType::kZSmallMemoryBlockListMemoryPool, memory_block_size,
                          memory_block_memory_size, capacity);
}


}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_BLOCK_LIST_MEMORY_POOL_H_
