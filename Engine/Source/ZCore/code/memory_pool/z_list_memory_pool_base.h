#ifndef Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
#define Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_

#include"internal/drive.h"

#include"z_memory_pool_base.h"
#include"z_heap_memory_pool.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of the memory pool that use the structure of list pool.
    Waring: The program will crash if a nullptr is send in.
    Template Parameters:
    - MemoryPieceType: The memory piece type of the memory pool.
    - kMemoryPieceHeadOffset: The memory might not start at the start of the 
        memory piece, it's the offset to the real memory.
    - kIsThreadSafe: Thread safe or not.
*/
template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
class ZListMemoryPoolBase :public ZMemoryPoolBase<kIsThreadSafe> {
private:
    //The multipul factor that container auto extends based on the origin size.
    static constexpr Float32 kAutoExtendMulFactor = 0.2F;
    //The min number the container auto extends at least.
    static constexpr IndexType kAutoExtendMinNum = 1;
    //The max size applied one time when extending.
    static constexpr MemoryType kApplyHeapMemoryMaxSizePurTime = 4 * kMB;
    //The unit size of the applied memory.
    static constexpr MemoryType kApplyHeapMemoryUnitSize = 4 * kHeapMemoryUnitSize;

protected:
    using SuperType = ZMemoryPoolBase<kIsThreadSafe>;
    using MutexType = ZMemoryPoolThreadSafeBase<kIsThreadSafe>;
    
    struct Node{
        Node* next_node_ptr;
        MemoryPieceType memory_piece;
    };

#pragma warning(disable : 26495)
    FORCEINLINE ZListMemoryPoolBase() : SuperType() {}
#pragma warning(default : 26495)
    Void Initialize(const MemoryPoolType memory_pool_type, const MemoryType memory_piece_size,
                    const MemoryType memory_piece_memory_size, const IndexType capacity) noexcept;
    
    FORCEINLINE static constexpr MemoryType node_head_offset() { return kNodeHeadOffset; }
    NODISCARD FORCEINLINE const MemoryType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const MemoryType memory_piece_size() const { return memory_piece_size_; }
    NODISCARD FORCEINLINE const MemoryType memory_piece_memory_size() const { return memory_piece_memory_size_; }

    /*
        It returns the memory's pointer(not the memory piece).It's inlined
        cause it's only used a few times.
    */
    NODISCARD FORCEINLINE Void* const ApplyMemory();

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    FORCEINLINE Void ReleaseMemory(const Void* const memory_ptr);

private:
    //The total offset of the memory piece.
    static constexpr PointerType kNodeHeadOffset = sizeof(Node*) + kMemoryPieceHeadOffset;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    FORCEINLINE Void AutoExtendCapcity();

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    Void ExtendCapacity(const IndexType memory_piece_added_num) noexcept;

    //The size of the memory piece(include the usable memory size)
    MemoryType memory_piece_size_;
    //The size of the useable memory.
    MemoryType memory_piece_memory_size_;

    IndexType capacity_;
    Node* head_node_ptr_;
};

template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::Initialize(
        const MemoryPoolType memory_pool_type, const MemoryType memory_piece_size,
        const MemoryType memory_piece_memory_size, const IndexType capacity) noexcept {
    SuperType::Initialize(memory_pool_type);
    memory_piece_size_ = memory_piece_size;
    memory_piece_memory_size_ = memory_piece_memory_size;
    head_node_ptr_ = nullptr;
    capacity_ = capacity;
    ExtendCapacity(capacity);
}


template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
NODISCARD FORCEINLINE Void* const ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ApplyMemory() {
    MutexType::lock();
    if (head_node_ptr_ == nullptr) {
        AutoExtendCapcity();
    }
    Void* const memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(head_node_ptr_) + kNodeHeadOffset);
    head_node_ptr_ = head_node_ptr_->next_node_ptr;
    MutexType::unlock();
    return memory_ptr;
}

template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ReleaseMemory(
        const Void* const memory_ptr) {
    MutexType::lock();
    Node* const node_ptr = reinterpret_cast<Node*>(reinterpret_cast<PointerType>(memory_ptr) - kNodeHeadOffset);
    node_ptr->next_node_ptr = head_node_ptr_;
    head_node_ptr_ = node_ptr;
    MutexType::unlock();
}

template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::AutoExtendCapcity() {
    IndexType extend_num = static_cast<IndexType>(capacity_ * kAutoExtendMulFactor);
    if (extend_num < kAutoExtendMinNum) {
        extend_num = kAutoExtendMinNum;
    }
    ExtendCapacity(extend_num);
}

template<typename MemoryPieceType, PointerType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ExtendCapacity(
        const IndexType memory_piece_added_num) noexcept {
    if (memory_piece_added_num == 0) {
        return;
    }
    //Calculates the size that needs to apply. Rounds up to the unit size's multiple.
    MemoryType apply_heap_memory_size = memory_piece_added_num * memory_piece_size_;
    if (apply_heap_memory_size >= kApplyHeapMemoryMaxSizePurTime) {
        apply_heap_memory_size = kApplyHeapMemoryMaxSizePurTime;
    }
    else if ((apply_heap_memory_size & (kApplyHeapMemoryUnitSize - 1)) != 0) {
        apply_heap_memory_size = (apply_heap_memory_size & (~(kApplyHeapMemoryUnitSize - 1)))
            + kApplyHeapMemoryUnitSize;
    }
    //Apply heap memory.
    Void* const apply_memory_ptr = ZHeapMemoryPool<kIsThreadSafe>::ApplyMemory(apply_heap_memory_size);
    PointerType temp_memory_ptr = reinterpret_cast<PointerType>(apply_memory_ptr);
    SuperType* const this_memory_pool_ptr = static_cast<SuperType*>(this);
    //Recaculate the real memory piece num added. 
    IndexType apply_memory_piece_num = apply_heap_memory_size / memory_piece_size_; 
    capacity_ += apply_memory_piece_num;
    //Initialize the memory piece.
    for (IndexType count = 1; count < apply_memory_piece_num; count++) {
        //Initialize the memory piece.
        reinterpret_cast<Node*>(temp_memory_ptr)->memory_piece.Initialize(reinterpret_cast<Void*>(this));
        //Links the pieces into a list.
        reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr =
            reinterpret_cast<Node*>(temp_memory_ptr + memory_piece_size_);
        //Next memory piece start pointer.
        temp_memory_ptr += memory_piece_size_;
    }
    //Initialize the last memory piece.
    reinterpret_cast<Node*>(temp_memory_ptr)->memory_piece.Initialize(reinterpret_cast<Void*>(this));
    //Puts the pieces into the memory pool.
    reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr =
        const_cast<Node*>(head_node_ptr_);
    head_node_ptr_ = reinterpret_cast<Node*>(apply_memory_ptr);
}

}//memory_pool
}//zengine

#endif // !Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
