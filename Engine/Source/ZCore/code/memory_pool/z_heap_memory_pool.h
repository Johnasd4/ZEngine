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
    NODISCARD static Void* ApplyMemory(MemoryType size) noexcept;

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
NODISCARD Void* ZHeapMemoryPool<kIsThreadSafe>::ApplyMemory(MemoryType size) noexcept {
    ZHeapMemoryPool& memory_pool = InstanceP();
    Void* heap_memory_ptr = malloc(size);
    memory_pool.MutexType::Lock();
    //applys new node when the memory runs out.
    if (memory_pool.current_node_heap_memory_ptr_num_ == HeapMemoryPtrArrayNode::kHeapMemoryPtrNumPurNode) {
        memory_pool.current_node_heap_memory_ptr_num_ = 0;
        memory_pool.current_node_ptr_->next_node_ptr = 
            static_cast<HeapMemoryPtrArrayNode*>(malloc(sizeof(HeapMemoryPtrArrayNode)));
        memory_pool.current_node_ptr_ = memory_pool.current_node_ptr_->next_node_ptr;
    }
    memory_pool.current_node_ptr_->heap_memory_ptr[memory_pool.current_node_heap_memory_ptr_num_++] = 
        heap_memory_ptr;
    memory_pool.MutexType::Unlock();
    return heap_memory_ptr;
}

#pragma warning(default : 6011)

#pragma warning(disable : 6001)

template<Bool kIsThreadSafe>
ZHeapMemoryPool<kIsThreadSafe>::~ZHeapMemoryPool() noexcept {
    //Delete the filled nodes.
    HeapMemoryPtrArrayNode* head_node_ptr = head_node_ptr_;
    HeapMemoryPtrArrayNode* current_node_ptr = current_node_ptr_;
    IndexType current_node_heap_memory_ptr_num = current_node_heap_memory_ptr_num_;
    HeapMemoryPtrArrayNode* delete_node = head_node_ptr;
    for (; head_node_ptr != current_node_ptr; ) {
        head_node_ptr = head_node_ptr->next_node_ptr;
        //Delete the heap memory inside the node.
        for (IndexType index = 0; index < HeapMemoryPtrArrayNode::kHeapMemoryPtrNumPurNode; ++index) {
            free(delete_node->heap_memory_ptr[index]);
        }
        //Delete the node itself.
        free(delete_node);
    }
    //Delete the unfilled node.
    for (IndexType index = 0; index < current_node_heap_memory_ptr_num; ++index) {
        free(delete_node->heap_memory_ptr[index]);
    }
    free(delete_node);
}

#pragma warning(disable : 6011)

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_HEAP_MEMORY_POOL_H_
