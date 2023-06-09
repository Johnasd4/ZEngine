#ifndef Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
#define Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_

#include"internal/drive.h"

#include"z_memory_pool_base.h"

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
class ZListMemoryPoolBase :public ZMemoryPoolBase<kIsThreadSafe>{
protected:
    using SuperType = ZMemoryPoolBase<kIsThreadSafe>;
    using MutexType = ZMemoryPoolThreadSafeBase<kIsThreadSafe>;
    
    struct Node{
        Node* next_node_ptr;
        MemoryPieceType memory_piece;
    };

    //The total offset of the memory piece.
    static constexpr AddressType kNodeHeadOffset = sizeof(Node*) + kMemoryPieceHeadOffset;

    __forceinline explicit ZListMemoryPoolBase(const MemoryPoolType memory_pool_type)
        : SuperType(memory_pool_type), head_node_ptr_(nullptr), capacity_(0U) {}

    __forceinline Void set_capacity(const IndexType capacity) { capacity_ = capacity; }
    __forceinline Void set_head_node_ptr(Node* head_node_ptr) { head_node_ptr_ = head_node_ptr; }

    __forceinline const IndexType capacity() { return capacity_; }
    __forceinline Node* head_node_ptr() { return head_node_ptr_; }

    /*
        It returns the memory's address(not the memory piece).It's inlined
        cause it's only used a few times.
    */
    __forceinline const Address ApplyMemory();

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    __forceinline Void ReleaseMemory(const Address memory_address);

private:
    //The multipul factor that container auto extends based on the origin size.
    static constexpr Float32 kAutoExtendMulFactor = 0.2F;
    //The min number the container auto extends at least.
    static constexpr IndexType kAutoExtendMinNum = 1;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    __forceinline const Void AutoExtend();

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    virtual Void Extend(const IndexType memory_piece_added_num) = 0;

    //池内object总数
    IndexType capacity_;
    //链表头
    Node* head_node_ptr_;
};

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
__forceinline const Address ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ApplyMemory() {
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
__forceinline Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::ReleaseMemory(
        const Address memory_address) {
    MutexType::lock();
    Node* nodeAddress = reinterpret_cast<Node*>(reinterpret_cast<AddressType>(memory_address) - kNodeHeadOffset);
    nodeAddress->next_node_ptr = head_node_ptr_;
    head_node_ptr_ = nodeAddress;
    MutexType::unlock();
}

template<typename MemoryPieceType, AddressType kMemoryPieceHeadOffset, Bool kIsThreadSafe>
__forceinline const Void ZListMemoryPoolBase<MemoryPieceType, kMemoryPieceHeadOffset, kIsThreadSafe>::AutoExtend() {
    IndexType extend_num = static_cast<IndexType>(capacity_ * kAutoExtendMulFactor);
    if (extend_num < kAutoExtendMinNum) {
        extend_num = kAutoExtendMinNum;
    }
    Extend(extend_num);
}

}//memory_pool
}//zengine

#endif // !Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
