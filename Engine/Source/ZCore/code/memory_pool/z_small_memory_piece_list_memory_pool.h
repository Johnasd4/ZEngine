#ifndef Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_

#include "internal/drive.h"

#include "f_console.h"
#include "z_fixed_array.h"
#include "z_lookup_table.h"

#include "z_list_memory_pool_base.h"
#include "z_memory_piece_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct ZSmallMemoryPiece : ZMemoryPieceBase{
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
class ZSmallMemoryPieceListMemoryPool : 
    public ZListMemoryPoolBase<ZSmallMemoryPiece<kIsThreadSafe>, sizeof(ZSmallMemoryPiece<kIsThreadSafe>), 
                               kIsThreadSafe> {
private:
    //The sizes of the memory pieces(includes the memory size).
    static constexpr IndexType kMemoryPieceTypeNum = 10;
    static constexpr MemoryType kMemoryPieceMinSize = 64;
    static constexpr MemoryType kMemoryPieceSizeMulGrowFactor = 2;

public:
    NODISCARD static Void* ApplyMemory(const MemoryType size) noexcept;
    NODISCARD static Void* ApplyMemory(const MemoryType size, MemoryType* memory_size_ptr) noexcept;

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    FORCEINLINE static const Bool CheckMemory(ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const MemoryType size);
    FORCEINLINE static const Bool CheckMemory(ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const MemoryType size,
                                              MemoryType* memory_size_ptr);

    static Void ReleaseMemory(ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const Void* memory_ptr) noexcept;

    FORCEINLINE static constexpr MemoryType memory_piece_memory_max_size() { return kMemoryPieceMemoryMaxSize; }
    FORCEINLINE static constexpr IndexType memory_piece_type_num() { return kMemoryPieceTypeNum; }

protected:
    using SuperType = 
        ZListMemoryPoolBase<ZSmallMemoryPiece<kIsThreadSafe>, sizeof(ZSmallMemoryPiece<kIsThreadSafe>), kIsThreadSafe>;

private:
    friend class ZFixedArray<ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>, kMemoryPieceTypeNum>;

    static constexpr MemoryType kMemoryPieceHeadSize = SuperType::node_head_offset();
    static constexpr ZFixedArray<MemoryType, kMemoryPieceTypeNum> kMemoryPieceSizeArray =
        ZFixedArray<MemoryType, kMemoryPieceTypeNum>([](ZFixedArray<MemoryType, kMemoryPieceTypeNum>* array_ptr) {
            (*array_ptr)(0) = kMemoryPieceMinSize;
            for (IndexType index = 1; index < ZFixedArray<Int32, kMemoryPieceTypeNum>::size(); ++index) {
                (*array_ptr)(index) = (*array_ptr)(index - 1) * kMemoryPieceSizeMulGrowFactor;
            }
        });
    static constexpr MemoryType kMemoryPieceMaxSize = kMemoryPieceSizeArray(kMemoryPieceTypeNum - 1);
    static constexpr MemoryType kMemoryPieceMemoryMaxSize = kMemoryPieceSizeArray(kMemoryPieceTypeNum - 1)
        - kMemoryPieceHeadSize;

    //The sizes of the memorys that can be uesd.
    static constexpr ZFixedArray<MemoryType, kMemoryPieceTypeNum> kMemoryPieceMemorySizeArray =
        ZFixedArray<MemoryType, kMemoryPieceTypeNum>([](ZFixedArray<MemoryType, kMemoryPieceTypeNum>* array_ptr) {
            for (IndexType index = 0; index < ZFixedArray<Int32, kMemoryPieceTypeNum>::size(); ++index) {
                (*array_ptr)(index) = kMemoryPieceSizeArray(index) - SuperType::node_head_offset();
            }
        });

    //The number of the pieces that the memory pool contains when created.
    static constexpr Int32 kMemoryPieceDefaultNum = 0;
    static constexpr ZFixedArray<IndexType, kMemoryPieceTypeNum> kMemoryPieceDefaultNumArray =
        ZFixedArray<IndexType, kMemoryPieceTypeNum>([](ZFixedArray<IndexType, kMemoryPieceTypeNum>* array_ptr) {
            for (IndexType index = 0; index < ZFixedArray<Int32, kMemoryPieceTypeNum>::size(); ++index) {
                (*array_ptr)(index) = kMemoryPieceDefaultNum;
            }
        });

    //The lookup table that links the memory size to the memory pool index.
    //mamory alignment
    static constexpr IndexType kkMemorySize2MemoryPoolTableSize = kMemoryPieceMemoryMaxSize + 1;
    static constexpr ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize> kMemorySize2MemoryPoolTable =
        ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>(
            [](ZLookupTable<UInt8, kkMemorySize2MemoryPoolTableSize>* array_ptr) {
                UInt8 current_pool_index = 0;
                for (IndexType index = 0; index < array_ptr->size(); ++index) {
                    if (index <= static_cast<IndexType>(kMemoryPieceMemorySizeArray(current_pool_index))) {
                        (*array_ptr)(index) = current_pool_index;
                    }
                    else {
                        ++current_pool_index;
                        (*array_ptr)(index) = current_pool_index;
                    }
                }
            });

    static Void MemoryPoolArrayInitFunction(
        ZFixedArray<ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>, kMemoryPieceTypeNum>* array_ptr) noexcept;

    FORCEINLINE ZSmallMemoryPieceListMemoryPool() : SuperType() {}
    ~ZSmallMemoryPieceListMemoryPool() noexcept;

    FORCEINLINE Void Initialize(const MemoryType memory_piece_size, const MemoryType memory_piece_memory_size, 
                                const Int32 capacity) noexcept;


#ifdef USE_MEMORY_POOL_TEST
    IndexType memory_piece_used_current_num_ = 0;
    IndexType momory_piece_applyed_num_ = 0;
    IndexType momory_piece_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

template<Bool kIsThreadSafe>
NODISCARD Void* ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ApplyMemory(
        const MemoryType size) noexcept {
    static ZFixedArray<ZSmallMemoryPieceListMemoryPool<kIsThreadSafe> , kMemoryPieceTypeNum> memory_pool_array(
        MemoryPoolArrayInitFunction);

#ifdef USE_MEMORY_POOL_TEST
    memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_ += 1;
    memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_applyed_num_ += 1;
    if (memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_ > 
        memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_peak_num_) {
        memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_peak_num_ = 
            memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    return memory_pool_array(kMemorySize2MemoryPoolTable.At(size)).SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
NODISCARD Void* ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ApplyMemory(
        const MemoryType size, MemoryType* memory_size_ptr) noexcept {
    static ZFixedArray<ZSmallMemoryPieceListMemoryPool<kIsThreadSafe> , kMemoryPieceTypeNum> memory_pool_array(
        MemoryPoolArrayInitFunction);

#ifdef USE_MEMORY_POOL_TEST
    memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_ += 1;
    memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_applyed_num_ += 1;
    if (memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_ > 
        memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_peak_num_) {
        memory_pool_array(kMemorySize2MemoryPoolTable(size)).momory_piece_peak_num_ = 
            memory_pool_array(kMemorySize2MemoryPoolTable(size)).memory_piece_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    (*memory_size_ptr) = memory_pool_array(kMemorySize2MemoryPoolTable.At(size)).SuperType::memory_piece_memory_size();
    return memory_pool_array(kMemorySize2MemoryPoolTable.At(size)).SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
FORCEINLINE const Bool ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::CheckMemory(
        ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const MemoryType size) {
    return memory_pool_ptr->SuperType::memory_piece_memory_size() >= size;
}
template<Bool kIsThreadSafe>
FORCEINLINE const Bool ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::CheckMemory(
        ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const MemoryType size, MemoryType* memory_size_ptr) {
    return (*memory_size_ptr = memory_pool_ptr->SuperType::memory_piece_memory_size()) >= size;
}

template<Bool kIsThreadSafe>
Void ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ReleaseMemory(
    ZSmallMemoryPieceListMemoryPool* memory_pool_ptr, const Void* memory_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_ptr->memory_piece_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
    memory_pool_ptr->SuperType::ReleaseMemory(memory_ptr);
}

template<Bool kIsThreadSafe>
Void ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::MemoryPoolArrayInitFunction(
    ZFixedArray<ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>, kMemoryPieceTypeNum>* array_ptr) noexcept {
    for (IndexType index = 0; index < array_ptr->size(); ++index) {
        (*array_ptr)(index).Initialize(kMemoryPieceSizeArray(index), kMemoryPieceMemorySizeArray(index),
                                       kMemoryPieceDefaultNumArray(index));
    }
}

template<Bool kIsThreadSafe>
ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::~ZSmallMemoryPieceListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
    //The first pool realsed.
    if (SuperType::memory_piece_size() == kMemoryPieceMaxSize) {
        zengine::console::Print(
            zengine::console::ConsoleTextColour::kConsoleTextColourLightGreen,
            zengine::console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,
            "\n\n***** small memory piece pool *****\n\n");
        zengine::console::Print(
            zengine::console::ConsoleTextColour::kConsoleTextColourLightGreen,
            zengine::console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,
            "    size    | usable size |  total num  | applied times | used peak num | unused num\n");
    }
    zengine::console::Print(
        zengine::console::ConsoleTextColour::kConsoleTextColourLightYellow,
        zengine::console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "  %8u  |  %9u  |  %9d  |   %9d   |   %9d   |  %8d\n",
        SuperType::memory_piece_size(),
        SuperType::memory_piece_memory_size(),
        SuperType::capacity(),
        momory_piece_applyed_num_,
        momory_piece_peak_num_,
        memory_piece_used_current_num_);
#endif //USE_MEMORY_POOL_TEST        
}

template<Bool kIsThreadSafe>
FORCEINLINE Void ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::Initialize(const MemoryType memory_piece_size, 
                                                                            const MemoryType memory_piece_memory_size, 
                                                                            const Int32 capacity) noexcept {
    SuperType::Initialize(MemoryPoolType::kZSmallMemoryPieceListMemoryPool, memory_piece_size,
                          memory_piece_memory_size, capacity);
}


}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_
