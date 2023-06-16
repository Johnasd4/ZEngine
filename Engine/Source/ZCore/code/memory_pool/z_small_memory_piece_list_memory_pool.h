#ifndef Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_

#include"internal/drive.h"

#include"f_console.h"
#include"z_fixed_array.h"

#include"z_list_memory_pool_base.h"
#include"z_memory_piece_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct ZSmallMemoryPiece : ZMemoryPieceBase{
    ZMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void Init(const Address pool_ptr) { 
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
class ZSmallMemoryPieceListMemoryPool : public ZListMemoryPoolBase<ZSmallMemoryPiece<kIsThreadSafe>, 
                                                             sizeof(ZSmallMemoryPiece<kIsThreadSafe>), kIsThreadSafe> {
public:
    static const Address ApplyMemory(const MemoryType size) noexcept;

    static Void ReleaseMemory(const Address address, ZSmallMemoryPieceListMemoryPool* memory_pool_ptr) noexcept;

    /*
        Checks if the memory can extend without moving to a new Address, If can
        then it will auto extend and return true.
    */
    FORCEINLINE static const Bool CheckMemory(const MemoryType size, ZSmallMemoryPieceListMemoryPool* memory_pool_ptr) {
        return memory_pool_ptr->memory_piece_memory_size() >= size;
    }

    FORCEINLINE static constexpr MemoryType memory_piece_memory_max_size() { return kMemoryPieceMemoryMaxSize; }

protected:
    using SuperType = 
        ZListMemoryPoolBase<ZSmallMemoryPiece<kIsThreadSafe>, sizeof(ZSmallMemoryPiece<kIsThreadSafe>), kIsThreadSafe>;

private:
   static constexpr MemoryType kMemoryPieceHeadSize = SuperType::node_head_offset();

    //The sizes of the memory pieces(includes the memory size).
    static constexpr IndexType kMemoryPieceTypeNum = 20;
    static constexpr MemoryType kMemoryPieceMinSize = 64;
    static constexpr MemoryType kMemoryPieceSizeMulGrowFactor = 2;
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

    //The number of the pieces that the memory pool contains when created.
    static constexpr Int32 kMemoryPieceDefaultNum = 0;
    static constexpr ZFixedArray<IndexType, kMemoryPieceTypeNum> KMemoryPieceDefaultNumArray =
        ZFixedArray<IndexType, kMemoryPieceTypeNum>([](ZFixedArray<IndexType, kMemoryPieceTypeNum>* array_ptr) {
            for (IndexType index = 0; index < ZFixedArray<Int32, kMemoryPieceTypeNum>::size(); ++index) {
                (*array_ptr)(index) = kMemoryPieceDefaultNum;
            }
        });



    /*
        Returns the first(smallest memeory pool) instance.
    */
    static ZSmallMemoryPieceListMemoryPool& Instance() noexcept {
        static ZSmallMemoryPieceListMemoryPool& memory_pool = CreateInstance();
        return memory_pool;
    }
    /*
        Used to create all the memory pool instance.
    */
    template<IndexType kInstanceCount = 0>
    static ZSmallMemoryPieceListMemoryPool& CreateInstance() noexcept;

    explicit ZSmallMemoryPieceListMemoryPool(const MemoryType memory_piece_size, const IndexType capacity) noexcept;
    ~ZSmallMemoryPieceListMemoryPool() noexcept;

#ifdef USE_MEMORY_POOL_TEST
    mutable IndexType memory_piece_used_current_num_ = 0;
    mutable IndexType momory_piece_applyed_num_ = 0;
    mutable IndexType momory_piece_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
    ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>* next_memory_pool_ptr_;


};

template<Bool kIsThreadSafe>
const Address ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size) noexcept {
    while (size > memory_pool_ptr->kMemoryPieceMemorySize) {
        memory_pool_ptr = memory_pool_ptr->next_memory_pool_ptr_;
    }
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_ptr->memory_piece_used_current_num_ += 1;
    memory_pool_ptr->momory_piece_applyed_num_ += 1;
    if (memory_pool_ptr->memory_piece_used_current_num_ > memory_pool_ptr->momory_piece_peak_num_) {
        memory_pool_ptr->momory_piece_peak_num_ = memory_pool_ptr->memory_piece_used_current_num_;
    }
#endif //USE_MEMORY_POOL_TEST
    return memory_pool_ptr->SuperType::ApplyMemory();
}

template<Bool kIsThreadSafe>
Void ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ReleaseMemory(const Address address, 
                                                             ZSmallMemoryPieceListMemoryPool* memory_pool_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
    memory_pool_ptr->memory_piece_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
    memory_pool_ptr->SuperType::ReleaseMemory(address);
}

/*
    Generates all the memory pools at once. 
*/
template<Bool kIsThreadSafe>
template<IndexType kInstanceCount>
ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>& ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::CreateInstance() noexcept {
    static ZSmallMemoryPieceListMemoryPool memory_pool(kMemoryPieceSizeArray(kInstanceCount), 
                                                 KMemoryPieceDefaultNumArray(kInstanceCount));
    //Generates new memory pool until all memory pools are generated.
    if constexpr (kInstanceCount != kMemoryPieceTypeNum - 1) {
        memory_pool.next_memory_pool_ptr_ = &CreateInstance<kInstanceCount + 1>();
    }
    else{
        memory_pool.next_memory_pool_ptr_ = nullptr;
    }
    return memory_pool;
}

template<Bool kIsThreadSafe>
ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::ZSmallMemoryPieceListMemoryPool(const MemoryType memory_piece_size,
                                                                    const Int32 capacity) noexcept 
        : SuperType(MemoryPoolType::kZSmallMemoryPieceListMemoryPool)
        , kMemoryPieceMemorySize(memory_piece_size - kMemoryPieceHeadSize)
        , kMemoryPieceSize(memory_piece_size) {
    Extend(capacity);
}

template<Bool kIsThreadSafe>
ZSmallMemoryPieceListMemoryPool<kIsThreadSafe>::~ZSmallMemoryPieceListMemoryPool() noexcept {
#ifdef USE_MEMORY_POOL_TEST

    //The first pool realsed.
    if (kMemoryPieceSize == kMemoryPieceMaxSize) {
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
        kMemoryPieceSize,
        kMemoryPieceMemorySize,
        SuperType::capacity(),
        momory_piece_applyed_num_,
        momory_piece_peak_num_,
        memory_piece_used_current_num_);
#endif //USE_MEMORY_POOL_TEST        
}



}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_SMALL_MEMORY_PIECE_LIST_MEMORY_POOL_H_
