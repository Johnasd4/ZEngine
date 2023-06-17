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
template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
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

    explicit FORCEINLINE ZListMemoryPoolBase() noexcept : SuperType() {}
    explicit ZListMemoryPoolBase(const MemoryPoolType memory_pool_type, const MemoryType memory_piece_size,
                                 const MemoryType memory_piece_memory_size, const IndexType capacity) noexcept;

    FORCEINLINE Void set_capacity(const IndexType capacity) { capacity_ = capacity; }
    FORCEINLINE Void set_head_node_ptr(Node* head_node_ptr) { head_node_ptr_ = head_node_ptr; }
    
    FORCEINLINE static constexpr MemoryType node_head_offset() { return kNodeHeadOffset; }
    FORCEINLINE const IndexType capacity() { return capacity_; }
    FORCEINLINE const MemoryType memory_piece_size() { return memory_piece_size_; }
    FORCEINLINE const MemoryType memory_piece_memory_size() { return memory_piece_memory_size_; }
    FORCEINLINE Node* head_node_ptr() { return head_node_ptr_; }

    /*
        It returns the memory's address(not the memory piece).It's inlined
        cause it's only used a few times.
    */
    FORCEINLINE const Address ApplyMemory();

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    FORCEINLINE Void ReleaseMemory(const Address memory_address);

private:
    //The total offset of the memory piece.
    static constexpr AddressType kNodeHeadOffset = sizeof(Node*) + kMemoryPieceHeadOffset;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    FORCEINLINE Void AutoExtend();

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    Void Extend(const IndexType memory_piece_added_num) noexcept;

    //The size of the memory piece(include the usable memory size)
    MemoryType memory_piece_size_;
    //The size of the useable memory.
    MemoryType memory_piece_memory_size_;

    IndexType capacity_;
    Node* head_node_ptr_;
};

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ZListMemoryPoolBase(
    const MemoryPoolType memory_pool_type, const MemoryType memory_piece_size,
    const MemoryType memory_piece_memory_size, const IndexType capacity) noexcept
        : SuperType(memory_pool_type)
        , memory_piece_size_(memory_piece_size)
        , memory_piece_memory_size_(memory_piece_memory_size)
        , head_node_ptr_(nullptr)
        , capacity_(capacity) {
    Extend(capacity);
}


template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
FORCEINLINE const Address ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ApplyMemory() {
    MutexType::lock();
    if (head_node_ptr_ == nullptr) {
        AutoExtend();
    }
    Address objectAddress = reinterpret_cast<Address>(reinterpret_cast<AddressType>(head_node_ptr_) + kNodeHeadOffset);
    head_node_ptr_ = head_node_ptr_->next_node_ptr;
    MutexType::unlock();
    return objectAddress;
}

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ReleaseMemory(
        const Address memory_address) {
    MutexType::lock();
    Node* nodeAddress = reinterpret_cast<Node*>(reinterpret_cast<AddressType>(memory_address) - kNodeHeadOffset);
    nodeAddress->next_node_ptr = head_node_ptr_;
    head_node_ptr_ = nodeAddress;
    MutexType::unlock();
}

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::AutoExtend() {
    IndexType extend_num = static_cast<IndexType>(capacity_ * kAutoExtendMulFactor);
    if (extend_num < kAutoExtendMinNum) {
        extend_num = kAutoExtendMinNum;
    }
    Extend(extend_num);
}

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::Extend(
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
    Address apply_memory_address = ZHeapMemoryPool<kIsThreadSafe>::Instance().ApplyMemory(apply_heap_memory_size);
    AddressType temp_memory_address = reinterpret_cast<AddressType>(apply_memory_address);
    SuperType* this_memory_pool_ptr = static_cast<SuperType*>(this);
    //Recaculate the real memory piece num added. 
    IndexType apply_memory_piece_num = apply_heap_memory_size / memory_piece_size_; 
    capacity_ += apply_memory_piece_num;
    //Initialize the memory piece.
    for (IndexType count = 1; count < apply_memory_piece_num; count++) {
        //Initialize the memory piece.
        reinterpret_cast<Node*>(temp_memory_address)->memory_piece.Init(reinterpret_cast<Address>(this));
        //Links the pieces into a list.
        reinterpret_cast<Node*>(temp_memory_address)->next_node_ptr =
            reinterpret_cast<Node*>(temp_memory_address + memory_piece_size_);
        //Next memory piece start address.
        temp_memory_address += memory_piece_size_;
    }
    //Initialize the last memory piece.
    reinterpret_cast<Node*>(temp_memory_address)->memory_piece.Init(reinterpret_cast<Address>(this));
    //Puts the pieces into the memory pool.
    reinterpret_cast<Node*>(temp_memory_address)->next_node_ptr =
        const_cast<Node*>(head_node_ptr_);
    head_node_ptr_ = reinterpret_cast<Node*>(apply_memory_address);
}

}//memory_pool
}//zengine

#endif // !Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
