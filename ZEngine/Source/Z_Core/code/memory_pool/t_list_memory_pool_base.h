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

#include "t_memory_pool_base.h"
#include "t_heap_memory_pool.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of the memory pool that use the structure of list pool.
    Warning: The program will crash if a nullptr is send in.
    Template Parameters:
    - MemoryBlockType: The memory block type of the memory pool.
    - kMemoryBlockHeadOffset: The memory might not start at the start of the 
        memory block, it's the offset to the real memory.
    - kIsThreadSafe: Thread safe or not.
*/
template<typename _MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
class TListMemoryPoolBase : public TMemoryPoolBase<kIsThreadSafe> {
private:
    //The multiple factor that container auto extends based on the origin size.
    static inline constexpr Float32 kAutoExtendMulFactor = 0.2F;
    //The min number the container auto extends at least.
    static inline constexpr SizeType kAutoExtendMinNum = 1;
    //The max size applied one time when extending.
    static inline constexpr SizeType kApplyHeapMemoryMaxSizePurTime = 4 * kMB;
    //The unit size of the applied memory.
    static inline constexpr SizeType kApplyHeapMemoryUnitSize = 4 * kHeapMemoryUnitSize;

public:
    NODISCARD FORCEINLINE SizeType MemoryBlockMemorySize() const noexcept { return memory_block_memory_size_; }

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
        SizeType _memory_block_size,
        SizeType _memory_block_memory_size,
        SizeType _capacity
    ) noexcept {
        SuperType_::InitializeP(_memory_pool_type);
        memory_block_size_ = _memory_block_size;
        memory_block_memory_size_ = _memory_block_memory_size;
        head_node_ptr_ = nullptr;
        capacity_ = _capacity;
        ExtendCapacityP(_capacity);
    }
    
    FORCEINLINE static constexpr SizeType NodeHeadOffset() noexcept { return kNodeHeadOffset; }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { return capacity_; }
    NODISCARD FORCEINLINE SizeType MemoryBlockSize() const noexcept { return memory_block_size_; }

    /*
        It returns the memory's pointer(not the memory block).It's inlined
        cause it's only used a few times.
    */
    NODISCARD FORCEINLINE Void* ApplyMemory() noexcept {
        MutexType_::Lock();
        if (head_node_ptr_ == nullptr) {
            AutoExtendCapcityP();
        }
        Void* memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(head_node_ptr_) + kNodeHeadOffset);
        head_node_ptr_ = head_node_ptr_->next_node_ptr_;
        MutexType_::Unlock();
        return memory_ptr;
    }

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    FORCEINLINE Void ReleaseMemory(Void* _memory_ptr) noexcept {
        MutexType_::Lock();
        Node* node_ptr = reinterpret_cast<Node*>(reinterpret_cast<PointerType>(_memory_ptr) - kNodeHeadOffset);
        node_ptr->next_node_ptr_ = head_node_ptr_;
        head_node_ptr_ = node_ptr;
        MutexType_::Unlock();
    }

private:
    //The total offset of the memory block.
    static inline constexpr PointerType kNodeHeadOffset = sizeof(Node*) + kMemoryBlockHeadOffset;

    TListMemoryPoolBase(const TListMemoryPoolBase&) = delete;
    TListMemoryPoolBase(TListMemoryPoolBase&&) = delete;

    TListMemoryPoolBase& operator=(const TListMemoryPoolBase&) = delete;
    TListMemoryPoolBase& operator=(TListMemoryPoolBase&&) = delete;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    FORCEINLINE Void AutoExtendCapcityP() noexcept {
        SizeType extend_num = static_cast<SizeType>(capacity_ * kAutoExtendMulFactor);
        if (extend_num < kAutoExtendMinNum) {
            extend_num = kAutoExtendMinNum;
        }
        ExtendCapacityP(extend_num);
    }

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    Void ExtendCapacityP(SizeType _memory_block_added_num) noexcept {
        if (_memory_block_added_num == 0) {
            return;
        }
        SizeType memory_block_size = memory_block_size_;
        //Calculates the size that needs to apply. Rounds up to the unit size's multiple.
        SizeType apply_heap_memory_size = _memory_block_added_num * memory_block_size;
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
        SizeType apply_memory_block_num = apply_heap_memory_size / memory_block_size;
        capacity_ += apply_memory_block_num;
        //Initialize the memory block.
        for (SizeType count = 1; count < apply_memory_block_num; count++) {
            //Initialize the memory block.
            reinterpret_cast<Node*>(temp_memory_ptr)->memory_block_.Initialize(reinterpret_cast<Void*>(this));
            //Links the blocks into a list.
            reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr_ =
                reinterpret_cast<Node*>(temp_memory_ptr + memory_block_size);
            //Next memory block start pointer.
            temp_memory_ptr += memory_block_size;
        }
        //Initialize the last memory block.
        reinterpret_cast<Node*>(temp_memory_ptr)->memory_block_.Initialize(reinterpret_cast<Void*>(this));
        //Puts the blocks into the memory pool.
        reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr_ = const_cast<Node*>(head_node_ptr_);
        head_node_ptr_ = reinterpret_cast<Node*>(apply_memory_ptr);
    }

    //The size of the memory block(include the usable memory size)
    SizeType memory_block_size_;
    //The size of the useable memory.
    SizeType memory_block_memory_size_;

    SizeType capacity_;
    Node* head_node_ptr_;
};

}//memory_pool
}//zengine