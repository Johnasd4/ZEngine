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
#ifndef Z_CORE_MEMORY_POOL_T_HEAP_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_T_HEAP_MEMORY_POOL_H_

#include "internal/z_drive.h"

#include "m_log.h"

#include "t_memory_pool_thread_safe_base.h"

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
class THeapMemoryPool : protected TMemoryPoolThreadSafeBase<kIsThreadSafe> {
public:
    NODISCARD static Void* ApplyMemory(MemoryType _size) noexcept;

protected:
    using MutexType_ = TMemoryPoolThreadSafeBase<kIsThreadSafe>;

private:
    struct HeapMemoryPtrArrayNode_ {
        static constexpr Int32 kHeapMemoryPtrNumPurNode = (kHeapMemoryUnitSize - sizeof(Void*)) / sizeof(Void*);

        Void* heap_memory_ptr_[kHeapMemoryPtrNumPurNode];
        HeapMemoryPtrArrayNode_* next_node_ptr_;
    };

    NODISCARD static THeapMemoryPool<kIsThreadSafe>& InstanceP() noexcept {
        static THeapMemoryPool<kIsThreadSafe> heap_memory_pool;;
        return heap_memory_pool;
    }

    THeapMemoryPool() noexcept 
        : current_node_ptr_(static_cast<HeapMemoryPtrArrayNode_*>(malloc(sizeof(HeapMemoryPtrArrayNode_))))
        , head_node_ptr_(current_node_ptr_)
        , current_node_heap_memory_ptr_num_(0) {}
    /*
        Release all the heap memory.
    */

    THeapMemoryPool(const THeapMemoryPool&) = delete;
    THeapMemoryPool(THeapMemoryPool&&) = delete;

    THeapMemoryPool& operator=(const THeapMemoryPool&) = delete;
    THeapMemoryPool& operator=(THeapMemoryPool&&) = delete;

    ~THeapMemoryPool() noexcept;

    HeapMemoryPtrArrayNode_* current_node_ptr_;
    HeapMemoryPtrArrayNode_* head_node_ptr_;
    Int32 current_node_heap_memory_ptr_num_;
};

template<Bool kIsThreadSafe>
NODISCARD Void* THeapMemoryPool<kIsThreadSafe>::ApplyMemory(MemoryType _size) noexcept {
    static THeapMemoryPool& memory_pool = InstanceP();
    Void* heap_memory_ptr = malloc(_size);
    if (heap_memory_ptr == nullptr) {
        Z_LOG_ERROR(
            error_code::kFMemoryPoolErrorCode_ApplyHeapMemoryFailed, 0, L"Apply heap memory failed! size: %d", _size);
        return heap_memory_ptr;
    }
    memory_pool.MutexType_::Lock();
    //applys new node when the memory runs out.
    if (memory_pool.current_node_heap_memory_ptr_num_ == HeapMemoryPtrArrayNode_::kHeapMemoryPtrNumPurNode) {
        memory_pool.current_node_heap_memory_ptr_num_ = 0;
        memory_pool.current_node_ptr_->next_node_ptr_ =
            static_cast<HeapMemoryPtrArrayNode_*>(malloc(sizeof(HeapMemoryPtrArrayNode_)));
        memory_pool.current_node_ptr_ = memory_pool.current_node_ptr_->next_node_ptr_;
    }
    memory_pool.current_node_ptr_->heap_memory_ptr_[memory_pool.current_node_heap_memory_ptr_num_++] = heap_memory_ptr;
    memory_pool.MutexType_::Unlock();
    return heap_memory_ptr;
}

template<Bool kIsThreadSafe>
THeapMemoryPool<kIsThreadSafe>::~THeapMemoryPool() noexcept {
    //Delete the filled nodes.
    HeapMemoryPtrArrayNode_* head_node_ptr = head_node_ptr_;
    HeapMemoryPtrArrayNode_* current_node_ptr = current_node_ptr_;
    IndexType current_node_heap_memory_ptr_num = current_node_heap_memory_ptr_num_;

    for (; head_node_ptr != current_node_ptr; ) {
        HeapMemoryPtrArrayNode_* delete_node = head_node_ptr;
        head_node_ptr = head_node_ptr->next_node_ptr_;
        //Delete the heap memory inside the node.
        for (IndexType index = 0; index < HeapMemoryPtrArrayNode_::kHeapMemoryPtrNumPurNode; ++index) {
            free(delete_node->heap_memory_ptr_[index]);
        }
        //Delete the node itself.
        free(delete_node);
    }
    //Delete the unfilled node.
    for (IndexType index = 0; index < current_node_heap_memory_ptr_num; ++index) {
        free(head_node_ptr->heap_memory_ptr_[index]);
    }
    free(head_node_ptr);
}

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_T_HEAP_MEMORY_POOL_H_
