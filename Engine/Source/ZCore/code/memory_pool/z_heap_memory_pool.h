#ifndef Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_

#include "internal/drive.h"

#include "z_memory_pool_thread_safe_base.h"

namespace zengine {
namespace memory_pool {

/*
    The heap memory controller. Apply heap memory from this class, it will 
    release the memory when destroyed.
    The controller applys a certain size of memory(default 4KB) to contain the
    memory pointer applyed by the user. It will extend a certain size of memory
    (default 4KB) when the origin memory is all used.
    Template Parameters:
    - kIsThreadSafe: Thread safe or not.
*/
template<Bool kIsThreadSafe>
class ZHeapMemoryPool :protected ZMemoryPoolThreadSafeBase<kIsThreadSafe> {
public:
    NODISCARD static Void* const ApplyMemory(const MemoryType size) noexcept;

protected:
    using MutexType = ZMemoryPoolThreadSafeBase<kIsThreadSafe>;

private:
    struct HeapMemoryPtrArrayNode {
        static constexpr Int32 kHeapMemoryPtrNumPurNode = (kHeapMemoryUnitSize - sizeof(Void*)) / sizeof(Void*);

        Void* heap_memory_ptr[kHeapMemoryPtrNumPurNode];
        HeapMemoryPtrArrayNode* next_node_ptr;
    };

    NODISCARD static ZHeapMemoryPool<kIsThreadSafe>& InstanceP() noexcept {
        static ZHeapMemoryPool<kIsThreadSafe> heap_memory_pool;;
        return heap_memory_pool;
    }

    ZHeapMemoryPool() noexcept 
        : current_node_ptr_(static_cast<HeapMemoryPtrArrayNode*>(malloc(sizeof(HeapMemoryPtrArrayNode))))
        , head_node_ptr_(current_node_ptr_)
        , current_node_heap_memory_ptr_num_(0) {}
    /*
        Release all the heap memory.
    */

    ZHeapMemoryPool(const ZHeapMemoryPool&) = delete;
    ZHeapMemoryPool(ZHeapMemoryPool&&) = delete;

    ZHeapMemoryPool& operator=(const ZHeapMemoryPool&) = delete;
    ZHeapMemoryPool& operator=(ZHeapMemoryPool&&) = delete;

    ~ZHeapMemoryPool() noexcept;

    HeapMemoryPtrArrayNode* current_node_ptr_;
    HeapMemoryPtrArrayNode* head_node_ptr_;
    Int32 current_node_heap_memory_ptr_num_;
};

#pragma warning(disable : 6011)

template<Bool kIsThreadSafe>
NODISCARD Void* const ZHeapMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size) noexcept {
    ZHeapMemoryPool& memory_pool = InstanceP();
    Void* const heap_memory_ptr = malloc(size);
    memory_pool.MutexType::lock();
    //applys new node when the memory runs out.
    if (memory_pool.current_node_heap_memory_ptr_num_ == HeapMemoryPtrArrayNode::kHeapMemoryPtrNumPurNode) {
        memory_pool.current_node_heap_memory_ptr_num_ = 0;
        memory_pool.current_node_ptr_->next_node_ptr = 
            static_cast<HeapMemoryPtrArrayNode*>(malloc(sizeof(HeapMemoryPtrArrayNode)));
        memory_pool.current_node_ptr_ = memory_pool.current_node_ptr_->next_node_ptr;
    }
    memory_pool.current_node_ptr_->heap_memory_ptr[memory_pool.current_node_heap_memory_ptr_num_++] = 
        heap_memory_ptr;
    memory_pool.MutexType::unlock();
    return heap_memory_ptr;
}

#pragma warning(default : 6011)

#pragma warning(disable : 6001)

template<Bool kIsThreadSafe>
ZHeapMemoryPool<kIsThreadSafe>::~ZHeapMemoryPool() noexcept {
    //Delete the filled nodes.
    while (head_node_ptr_ != current_node_ptr_) {
        HeapMemoryPtrArrayNode* const delete_node = head_node_ptr_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr;
        //Delete the heap memory inside the node.
        for (IndexType index = 0; index < HeapMemoryPtrArrayNode::kHeapMemoryPtrNumPurNode; ++index) {
            free(delete_node->heap_memory_ptr[index]);
        }
        //Delete the node itself.
        free(delete_node);
    }
    //Delete the unfilled node.
    for (IndexType index = 0; index < current_node_heap_memory_ptr_num_; ++index) {
        free(head_node_ptr_->heap_memory_ptr[index]);
    }
    free(head_node_ptr_);
}

#pragma warning(disable : 6011)

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_
