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
#ifndef Z_MEMORY_POOL_INTERNAL_T_LIST_MEMORY_POOL_BASE_H_
#define Z_MEMORY_POOL_INTERNAL_T_LIST_MEMORY_POOL_BASE_H_

#include "internal/z_drive.h"

#include "t_memory_pool_base.h"
#include "t_heap_memory_pool.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of the memory pool that use the structure of list pool.
    Waring: The program will crash if a nullptr is send in.
    Template Parameters:
    - MemoryBlockType: The memory block type of the memory pool.
    - kMemoryBlockHeadOffset: The memory might not start at the start of the 
        memory block, it's the offset to the real memory.
    - kIsThreadSafe: Thread safe or not.
*/
template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
class TListMemoryPoolBase :public TMemoryPoolBase<kIsThreadSafe> {
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
    using SuperType_ = TMemoryPoolBase<kIsThreadSafe>;
    using MutexType_ = TMemoryPoolThreadSafeBase<kIsThreadSafe>;
    
    struct Node{
        Node* next_node_ptr_;
        _MemoryBlockType memory_block_;
    };

#pragma warning(disable : 26495)
    FORCEINLINE TListMemoryPoolBase() noexcept : SuperType_() {}
#pragma warning(default : 26495)
    Void InitializeP(
        MemoryPoolEnum _memory_pool_type, 
        MemoryType _memory_block_size,
        MemoryType _memory_block_memory_size,
        IndexType _capacity
    ) noexcept;
    
    FORCEINLINE static constexpr MemoryType NodeHeadOffset() noexcept { return kNodeHeadOffset; }
    NODISCARD FORCEINLINE MemoryType Capacity() const noexcept { return capacity_; }
    NODISCARD FORCEINLINE MemoryType MemoryBlockSize() const noexcept { return memory_block_size_; }
    NODISCARD FORCEINLINE MemoryType MemoryBlockMemorySize() const noexcept { return memory_block_memory_size_; }

    /*
        It returns the memory's pointer(not the memory block).It's inlined
        cause it's only used a few times.
    */
    NODISCARD FORCEINLINE Void* ApplyMemory() noexcept;

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    FORCEINLINE Void ReleaseMemory(Void* _memory_ptr) noexcept;

private:
    //The total offset of the memory block.
    static constexpr PointerType kNodeHeadOffset = sizeof(Node*) + kMemoryBlockHeadOffset;

    TListMemoryPoolBase(const TListMemoryPoolBase&) = delete;
    TListMemoryPoolBase(TListMemoryPoolBase&&) = delete;

    TListMemoryPoolBase& operator=(const TListMemoryPoolBase&) = delete;
    TListMemoryPoolBase& operator=(TListMemoryPoolBase&&) = delete;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    FORCEINLINE Void AutoExtendCapcityP() noexcept;

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    Void ExtendCapacityP(IndexType _memory_block_added_num) noexcept;

    //The size of the memory block(include the usable memory size)
    MemoryType memory_block_size_;
    //The size of the useable memory.
    MemoryType memory_block_memory_size_;

    IndexType capacity_;
    Node* head_node_ptr_;
};

template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
Void TListMemoryPoolBase<_MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::InitializeP(
    MemoryPoolEnum _memory_pool_type, 
    MemoryType _memory_block_size,
    MemoryType _memory_block_memory_size, 
    IndexType _capacity
) noexcept {
    SuperType_::InitializeP(_memory_pool_type);
    memory_block_size_ = _memory_block_size;
    memory_block_memory_size_ = _memory_block_memory_size;
    head_node_ptr_ = nullptr;
    capacity_ = _capacity;
    ExtendCapacityP(_capacity);
}


template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
NODISCARD FORCEINLINE Void* TListMemoryPoolBase<_MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ApplyMemory(
) noexcept {
    MutexType_::Lock();
    if (head_node_ptr_ == nullptr) {
        AutoExtendCapcityP();
    }
    Void* memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(head_node_ptr_) + kNodeHeadOffset);
    head_node_ptr_ = head_node_ptr_->next_node_ptr_;
    MutexType_::Unlock();
    return memory_ptr;
}

template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void TListMemoryPoolBase<_MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ReleaseMemory(
    Void* _memory_ptr
) noexcept {
    MutexType_::Lock();
    Node* node_ptr = reinterpret_cast<Node*>(reinterpret_cast<PointerType>(_memory_ptr) - kNodeHeadOffset);
    node_ptr->next_node_ptr_ = head_node_ptr_;
    head_node_ptr_ = node_ptr;
    MutexType_::Unlock();
}

template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void TListMemoryPoolBase<_MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::AutoExtendCapcityP(
) noexcept {
    IndexType extend_num = static_cast<IndexType>(capacity_ * kAutoExtendMulFactor);
    if (extend_num < kAutoExtendMinNum) {
        extend_num = kAutoExtendMinNum;
    }
    ExtendCapacityP(extend_num);
}

template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
Void TListMemoryPoolBase<_MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ExtendCapacityP(
    IndexType _memory_block_added_num
) noexcept {
    if (_memory_block_added_num == 0) {
        return;
    }
    MemoryType memory_block_size = memory_block_size_;
    //Calculates the size that needs to apply. Rounds up to the unit size's multiple.
    MemoryType apply_heap_memory_size = _memory_block_added_num * memory_block_size;
    if (apply_heap_memory_size >= kApplyHeapMemoryMaxSizePurTime) {
        apply_heap_memory_size = kApplyHeapMemoryMaxSizePurTime;
    }
    else if ((apply_heap_memory_size & (kApplyHeapMemoryUnitSize - 1)) != 0) {
        apply_heap_memory_size = (apply_heap_memory_size & (~(kApplyHeapMemoryUnitSize - 1)))
            + kApplyHeapMemoryUnitSize;
    }
    //Apply heap memory.
    Void* apply_memory_ptr = THeapMemoryPool<kIsThreadSafe>::ApplyMemory(apply_heap_memory_size);
    PointerType temp_memory_ptr = reinterpret_cast<PointerType>(apply_memory_ptr);
    SuperType_* this_memory_pool_ptr = static_cast<SuperType_*>(this);
    //Recaculate the real memory block num added. 
    IndexType apply_memory_block_num = apply_heap_memory_size / memory_block_size;
    capacity_ += apply_memory_block_num;
    //Initialize the memory block.
    for (IndexType count = 1; count < apply_memory_block_num; count++) {
        //Initialize the memory block.
        reinterpret_cast<Node*>(temp_memory_ptr)->memory_block_.InitializeP(reinterpret_cast<Void*>(this));
        //Links the blocks into a list.
        reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr_ =
            reinterpret_cast<Node*>(temp_memory_ptr + memory_block_size);
        //Next memory block start pointer.
        temp_memory_ptr += memory_block_size;
    }
    //Initialize the last memory block.
    reinterpret_cast<Node*>(temp_memory_ptr)->memory_block_.InitializeP(reinterpret_cast<Void*>(this));
    //Puts the blocks into the memory pool.
    reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr_ =
        const_cast<Node*>(head_node_ptr_);
    head_node_ptr_ = reinterpret_cast<Node*>(apply_memory_ptr);
}

}//memory_pool
}//zengine

#endif // !Z_MEMORY_POOL_INTERNAL_T_LIST_MEMORY_POOL_BASE_H_
