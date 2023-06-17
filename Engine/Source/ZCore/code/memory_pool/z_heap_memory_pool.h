#ifndef Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_

#include"internal/drive.h"

#include"z_memory_pool_thread_safe_base.h"

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
    static ZHeapMemoryPool& Instance() noexcept {
        static ZHeapMemoryPool heap_memory_controller;
        return heap_memory_controller;
    }

    /*
        Use ApplyBigMemory to apply memory bigger then 2^32.
    */
    NODISCARD const Address ApplyMemory(const MemoryType size) noexcept;
    NODISCARD const Address ApplyBigMemory(const SizeType size) noexcept;

protected:
    using MutexType = ZMemoryPoolThreadSafeBase<kIsThreadSafe>;

private:
    struct AddressArrayNode {
        static constexpr Int32 kAddressNumPurNode = (kHeapMemoryUnitSize - sizeof(Address)) / sizeof(Address);

        Address address[kAddressNumPurNode];
        AddressArrayNode* next_node_ptr;
    };

    ZHeapMemoryPool() noexcept 
        : current_node_ptr_(static_cast<AddressArrayNode*>(malloc(sizeof(AddressArrayNode))))
        , head_node_ptr_(current_node_ptr_)
        , current_node_address_num_(0) {}
    /*
        Release all the heap memory.
    */
    ~ZHeapMemoryPool() noexcept;

    AddressArrayNode* current_node_ptr_;
    AddressArrayNode* head_node_ptr_;
    Int32 current_node_address_num_;
};

#pragma warning(disable : 6011)

template<Bool kIsThreadSafe>
NODISCARD const Address ZHeapMemoryPool<kIsThreadSafe>::ApplyMemory(const MemoryType size) noexcept {
    Address heap_memory_address = malloc(size);
    MutexType::lock();
    //applys new node when the memory runs out.
    if (current_node_address_num_ == AddressArrayNode::kAddressNumPurNode) {
        current_node_address_num_ = 0;
        current_node_ptr_->next_node_ptr = static_cast<AddressArrayNode*>(malloc(sizeof(AddressArrayNode)));
        current_node_ptr_ = current_node_ptr_->next_node_ptr;
    }
    current_node_ptr_->address[current_node_address_num_++] = heap_memory_address;
    MutexType::unlock();
    return heap_memory_address;
}

#pragma warning(default : 6011)

template<Bool kIsThreadSafe>
NODISCARD const Address ZHeapMemoryPool<kIsThreadSafe>::ApplyBigMemory(const SizeType size) noexcept {
    Address heap_memory_address = malloc(size);
    MutexType::lock();
    //applys new node when the memory runs out.
    if (current_node_address_num_ == AddressArrayNode::kAddressNumPurNode) {
        current_node_address_num_ = 0;
        current_node_ptr_->next_node_ptr = static_cast<AddressArrayNode*>(malloc(sizeof(AddressArrayNode)));
        current_node_ptr_ = current_node_ptr_->next_node_ptr;
    }
    current_node_ptr_->address[current_node_address_num_++] = heap_memory_address;
    MutexType::unlock();
    return heap_memory_address;
}

#pragma warning(disable : 6001)

template<Bool kIsThreadSafe>
ZHeapMemoryPool<kIsThreadSafe>::~ZHeapMemoryPool() noexcept {
    //Delete the filled nodes.
    while (head_node_ptr_ != current_node_ptr_) {
        AddressArrayNode* delete_node = head_node_ptr_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr;
        //Delete the heap memory inside the node.
        for (IndexType index = 0; index < AddressArrayNode::kAddressNumPurNode; ++index) {
            free(delete_node->address[index]);
        }
        //Delete the node itself.
        free(delete_node);
    }
    //Delete the unfilled node.
    for (IndexType index = 0; index < current_node_address_num_; ++index) {
        free(head_node_ptr_->address[index]);
    }
    free(head_node_ptr_);
}

#pragma warning(disable : 6011)

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_
