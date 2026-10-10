/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "t_list.h"
#include "t_lock_guard.h"
#include "t_smart_pointer.h"
#include "t_fixed_memory.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {
namespace internal {

/**
 * @brief Base class for conditionally thread-safe pool implementations.
 * 
 * Implementation Principle:
 * Provides an empty shell for mutex operations when thread safety is disabled, allowing zero-overhead 
 * abstractions for non-thread-safe pool variants.
 *
 * @tparam _IsThreadSafe Boolean flag indicating whether thread safety is enabled.
 */
template<Bool _IsThreadSafe>
class CACHE_ALIGN TPoolBase : public ZObject {
public:
    /** @brief Base class alias. */
    using SuperType_ = ZObject;
    /** @brief Inherit base constructors. */
    using SuperType_::SuperType_;

protected:
    /**
     * @brief Locks the pool mutex. Does nothing for non-thread-safe pool.
     * @return Void
     */
    NODISCARD FORCEINLINE Void Lock() noexcept {}

    /**
     * @brief Unlocks the pool mutex. Does nothing for non-thread-safe pool.
     * @return Void
     */
    NODISCARD FORCEINLINE Void Unlock() noexcept {}

    /**
     * @brief Gets the internal mutex. Not available for non-thread-safe pool.
     * @return ZMutex& Reference to the internal mutex.
     */
    NODISCARD FORCEINLINE ZMutex& GetMutex() noexcept { 
        STATIC_ASSERT(sizeof(TPoolBase) == 0, "GetMutex() is not available for non-thread-safe pool.");
    }
};

/**
 * @brief Thread-safe specialization of the pool base class.
 * 
 * Implementation Principle:
 * Uses a real internal ZMutex to provide locking mechanisms for derived pools, ensuring thread safety.
 */
template<>
class CACHE_ALIGN TPoolBase<true> : public ZObject {
public:
    /** @brief Base class alias. */
    using SuperType_ = ZObject;
    /** @brief Inherit base constructors. */
    using SuperType_::SuperType_;

protected:
    /**
     * @brief Locks the pool mutex.
     * @return Void
     */
    NODISCARD FORCEINLINE Void Lock() noexcept { mutex_.Lock(); }

    /**
     * @brief Unlocks the pool mutex.
     * @return Void
     */
    NODISCARD FORCEINLINE Void Unlock() noexcept { mutex_.Unlock(); }

    /**
     * @brief Gets the internal mutex.
     * @return ZMutex& Reference to the internal mutex.
     */
    NODISCARD FORCEINLINE ZMutex& GetMutex() noexcept { return mutex_; }

private:
    /** @brief The internal mutex used for thread synchronization. */
    ZMutex mutex_;
};

/**
 * @brief A high-performance object pool container.
 *
 * Implementation Principle:
 * This pool manages memory by allocating large contiguous blocks (MemoryHead_) and slicing them into smaller
 * nodes (Node_). It maintains a singly-linked free-list (head_node_ptr_) for fast recycling of released objects.
 * When the free-list is empty, it uses a linear allocator via an uninitialized memory pointer (uninit_node_ptr_).
 * If the current block is exhausted, it automatically extends capacity by allocating a new memory block.
 * It uses an in-place storage (model_obj_data_) for a model object to avoid heap allocations when initializing.
 *
 * @tparam _ObjectType The type of object to be stored in the pool.
 * @tparam _NodeType The underlying node type wrapping the object.
 * @tparam _IsThreadSafe If true, enables thread-safe operations using an internal mutex.
 * @tparam _IsGlobal If true, uses the global memory pool for underlying block allocations.
 * @tparam _IfCallConstructorAndDestructor If true, explicitly calls constructors and destructors for objects.
 */
template<
    typename _ObjectType,
    typename _NodeType,
    Bool _IsThreadSafe = false,
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
class TPool : public internal::TPoolBase<_IsThreadSafe> {
public:
    /** @brief Base class alias. */
    using SuperType_ = internal::TPoolBase<_IsThreadSafe>;
    /** @brief Alias for the object type stored in the pool. */
    using NodeType_ = _NodeType;

private:
    /** @brief The minimum number of nodes to allocate when extending the pool. */
    static inline constexpr SizeType kExtendMinSize = 10ULL;
    /** @brief The factor used to calculate the number of nodes for auto-extension based on current size. */
    static inline constexpr SizeType kAutoExtendMultFactor = 1ULL;

    /**
     * @brief Header structure for each allocated memory block.
     * 
     * Implementation Principle:
     * Aligned for SIMD operations to ensure subsequent nodes are aligned if necessary. Acts as a linked list
     * of memory blocks to properly track and release all allocated segments when the pool is destroyed.
     */
    struct SIMD_ALIGN MemoryHead_ {
        /** @brief Pointer to the next memory block in the chain. */
        MemoryHead_* next_memory_ptr_;
        /** @brief Pointer to the end of the node array in this block. */
        NodeType_* node_end_ptr_;
    };

public:
    /**
     * @brief Default constructor. Initializes an empty pool.
     */
    TPool() noexcept
        : SuperType_()
        , head_node_ptr_(nullptr)
        , uninit_node_ptr_(nullptr)
        , uninit_node_end_ptr_(nullptr)
        , head_memory_ptr_(nullptr)
        , model_obj_data_()
        , has_model_(false)
        , size_(0ULL)
    {
    }
    TPool(const TPool&) = delete;
    TPool& operator=(const TPool&) = delete;

    /**
     * @brief Move constructor. Transfers ownership of resources from another pool.
     * @param _pool The pool to move from.
     */
    TPool(TPool&& _pool) noexcept
        : SuperType_()
    {
        _pool.SuperType_::Lock();
        MoveP(std::move(_pool));
        _pool.SuperType_::Unlock();
    }

    /**
     * @brief Constructor with arguments for the model object.
     * @tparam _ArgsType Types of the arguments to construct the model object.
     * @param args Arguments forwarded to the model object constructor.
     */
    template<typename... _ArgsType>
    TPool(_ArgsType&&... args) noexcept
        : SuperType_()
        , head_node_ptr_(nullptr)
        , uninit_node_ptr_(nullptr)
        , uninit_node_end_ptr_(nullptr)
        , head_memory_ptr_(nullptr)
        , has_model_(true)
        , size_(0ULL)
    {
        new(model_obj_data_.template GetDataPtr<Void>()) _ObjectType(std::forward<_ArgsType>(args)...);
    }

    /**
     * @brief Destructor. Releases all allocated memory blocks and destroys objects if required.
     */
    ~TPool() noexcept {
        DestroyP();
    }

    /**
     * @brief Move assignment operator. Transfers ownership of resources from another pool.
     * @param _pool The pool to move from.
     * @return TPool& Reference to this pool.
     */
    TPool& operator=(TPool&& _pool) noexcept {
        if (this == &_pool) {
            return *this;
        }
        if constexpr (_IsThreadSafe) {
            T2LockGuard lock_guard(SuperType_::GetMutex(), _pool.SuperType_::GetMutex());
            DestroyP();
            MoveP(std::move(_pool));
        }
        else {
            DestroyP();
            MoveP(std::move(_pool));
        }
        return *this;
    }

    /**
     * @brief Gets the current total capacity of the pool.
     * @return SizeType The number of nodes the pool can hold.
     */
    NODISCARD FORCEINLINE SizeType GetSize() noexcept {
        if constexpr (_IsThreadSafe) {
            TLockGuard lock_guard(SuperType_::GetMutex());
            return size_;
        }
        else {
            return size_;
        }
    }

    /**
     * @brief Sets or updates the model object used for initializing new nodes.
     * @tparam _ArgsType Types of the arguments to construct the model object.
     * @param args Arguments forwarded to the model object constructor.
     * @return Void
     */
    template<typename... _ArgsType>
    NODISCARD Void SetExtendObjectModel(_ArgsType&&... args) noexcept {
        SuperType_::Lock();
        if constexpr (_IfCallConstructorAndDestructor) {
            if (has_model_) {
                model_obj_data_.template GetDataPtr<_ObjectType>()->~_ObjectType();
            }
        }
        new(model_obj_data_.template GetDataPtr<Void>()) _ObjectType(std::forward<_ArgsType>(args)...);
        has_model_ = true;
        SuperType_::Unlock();
    }

    /**
     * @brief Allocates an object from the pool.
     * @return _ObjectType* Pointer to the allocated object.
     */
    NODISCARD _ObjectType* Apply() noexcept {
        SuperType_::Lock();
        if (head_node_ptr_ != nullptr) {
            _ObjectType* obj_ptr = &head_node_ptr_->obj_;
            head_node_ptr_ = head_node_ptr_->next_node_ptr_;
            SuperType_::Unlock();
            return obj_ptr;
        }
        else {
            if (uninit_node_ptr_ == uninit_node_end_ptr_) {
                ExtendPoolP();
            }
            // init the uninit node
            InitializeNodeP(*uninit_node_ptr_);

            // return the new node
            _ObjectType* obj_ptr = &uninit_node_ptr_->obj_;
            uninit_node_ptr_ += 1ULL;
            SuperType_::Unlock();
            return obj_ptr;
        }
    }

    /**
     * @brief Returns an object to the pool, adding it to the free list.
     * @param _obj_ptr Pointer to the object to release.
     * @return Void
     */
    NODISCARD Void Release(_ObjectType* _obj_ptr) noexcept {
        SuperType_::Lock();
        NodeType_* release_node_ptr = reinterpret_cast<NodeType_*>(_obj_ptr);
        release_node_ptr->next_node_ptr_ = head_node_ptr_;
        head_node_ptr_ = release_node_ptr;
        SuperType_::Unlock();
    }

    /**
     * @brief Releases a range of nodes back to the pool, adding them to the free list.
     * @param _start_node_ptr Pointer to the first node in the range.
     * @param _end_node_ptr Pointer to the last node in the range.
     * @return Void
     */
    NODISCARD Void Release(NodeType_* _start_node_ptr, NodeType_* _end_node_ptr) noexcept {
        SuperType_::Lock();

        _end_node_ptr->next_node_ptr_ = head_node_ptr_;
        head_node_ptr_ = _start_node_ptr;

        SuperType_::Unlock();
    }

    /**
     * @brief Prepares the pool to accommodate at least a specified number of new allocations.
     * @param _num The number of nodes to prepare.
     * @return Void
     */
    NODISCARD Void Prepare(SizeType _num) noexcept {
        SuperType_::Lock();
        SizeType uninit_num = uninit_node_end_ptr_ - uninit_node_ptr_;
        NodeType_* uninit_node_finish_ptr;
        SizeType need_extend_num;
        if (uninit_num >= _num) {
            uninit_node_finish_ptr = uninit_node_ptr_ + _num;
            need_extend_num = 0ULL;
        }
        else {
            uninit_node_finish_ptr = uninit_node_end_ptr_;
            need_extend_num = _num - uninit_num;
        }

        NodeType_* next_node_ptr = head_node_ptr_;

        if (uninit_num > 0ULL) {
            while (uninit_node_ptr_ != uninit_node_finish_ptr) {
                InitializeNodeP(*uninit_node_ptr_);
                uninit_node_ptr_->next_node_ptr_ = next_node_ptr;
                next_node_ptr = uninit_node_ptr_;
                uninit_node_ptr_ += 1ULL;
            }
        }

        if (need_extend_num > 0ULL) {
            ExtendPoolP(need_extend_num);
            uninit_node_finish_ptr = uninit_node_ptr_ + need_extend_num;
            while (uninit_node_ptr_ != uninit_node_finish_ptr) {
                InitializeNodeP(*uninit_node_ptr_);
                uninit_node_ptr_->next_node_ptr_ = next_node_ptr;
                next_node_ptr = uninit_node_ptr_;
                uninit_node_ptr_ += 1ULL;
            }
        }

        head_node_ptr_ = next_node_ptr;
        SuperType_::Unlock();
    }

    /**
     * @brief Manually extends the pool size.
     * @param _size The number of new nodes to allocate. If 0, uses auto-calculation.
     * @return Void
     */
    Void ExtendPool(SizeType _size = 0ULL) noexcept {
        SuperType_::Lock();
        // before extend, link all uninit nodes to head node
        NodeType_* next_node_ptr = head_node_ptr_;
        while (uninit_node_ptr_ != uninit_node_end_ptr_) {
            InitializeNodeP(*uninit_node_ptr_);
            uninit_node_ptr_->next_node_ptr_ = next_node_ptr;
            next_node_ptr = uninit_node_ptr_;
            uninit_node_ptr_ += 1ULL;
        }
        head_node_ptr_ = next_node_ptr;

        ExtendPoolP(_size);
        SuperType_::Unlock();
    }

private:
    /**
     * @brief Initializes a node using either the model object or default construction.
     * @param _node Reference to the node to be initialized.
     * @return Void
     */
    FORCEINLINE Void InitializeNodeP(NodeType_& _node) noexcept {
        // init the uninit node
        if (has_model_) {
            // make sure the compile won't fail when the _ObjectType has no copy constructor
            if constexpr (
                requires{
                    ::new(reinterpret_cast<Void*>(&_node.obj_))
                        _ObjectType(*model_obj_data_.template GetDataPtr<_ObjectType>());
                }
            ) {
                ::new(reinterpret_cast<Void*>(&_node.obj_))
                    _ObjectType(*model_obj_data_.template GetDataPtr<_ObjectType>());
            }
            else {
                ASSERT(
                    false,
                    "The _ObjectType must be copy constructible if a model object is provided."
                );
            }
        }
        else {
            if constexpr (_IfCallConstructorAndDestructor) {
                if constexpr (requires{ ::new(reinterpret_cast<Void*>(&_node.obj_)) _ObjectType(); }) {
                    ::new(reinterpret_cast<Void*>(&_node.obj_)) _ObjectType();
                }
                else {
                    ASSERT(
                        false,
                        "The _ObjectType must be default constructible if no model object is provided."
                    );
                }
            }
        }
    }

    /**
     * @brief Internal implementation to extend the pool by allocating a new memory block.
     * @param _num The number of nodes to allocate. If 0, uses the auto-extend factor.
     * @return Void
     */
    Void ExtendPoolP(SizeType _num = 0ULL) noexcept {
        SizeType extend_num = _num == 0ULL ? (size_ * kAutoExtendMultFactor) : _num;
        if (extend_num < kExtendMinSize) {
            extend_num = kExtendMinSize;
        }
        SizeType memory_size = sizeof(NodeType_) * extend_num + sizeof(MemoryHead_);
        MemoryHead_* memory_ptr = reinterpret_cast<MemoryHead_*>(memory_pool::ApplyMemory<_IsGlobal>(
            memory_size, &memory_size
        ));
        extend_num = (memory_size - sizeof(MemoryHead_)) / sizeof(NodeType_);
        uninit_node_ptr_ = reinterpret_cast<NodeType_*>(memory_ptr + 1ULL);
        uninit_node_end_ptr_ = uninit_node_ptr_ + extend_num;
        memory_ptr->next_memory_ptr_ = head_memory_ptr_;
        memory_ptr->node_end_ptr_ = uninit_node_end_ptr_;
        head_memory_ptr_ = memory_ptr;
        size_ += extend_num;
    }

    /**
     * @brief Internal function to destroy all objects and release all memory blocks.
     * @return Void
     */
    Void DestroyP() noexcept {
        if constexpr (_IfCallConstructorAndDestructor) {
            if (has_model_) {
                model_obj_data_.template GetDataPtr<_ObjectType>()->~_ObjectType();
                has_model_ = false;
            }
        }

        if (head_memory_ptr_ == nullptr) {
            return;
        }

        if constexpr (_IfCallConstructorAndDestructor) {
            NodeType_* node_ptr = reinterpret_cast<NodeType_*>(head_memory_ptr_ + 1ULL);
            while (node_ptr != uninit_node_ptr_) {
                node_ptr->obj_.~_ObjectType();
                node_ptr += 1ULL;
            }
        }
        MemoryHead_* release_memory_ptr = head_memory_ptr_;
        head_memory_ptr_ = head_memory_ptr_->next_memory_ptr_;
        memory_pool::ReleaseMemory<_IsGlobal>(release_memory_ptr);

        while (head_memory_ptr_ != nullptr) {
            if constexpr (_IfCallConstructorAndDestructor) {
                NodeType_* node_ptr = reinterpret_cast<NodeType_*>(head_memory_ptr_ + 1ULL);
                while (node_ptr != head_memory_ptr_->node_end_ptr_) {
                    node_ptr->obj_.~_ObjectType();
                    node_ptr += 1ULL;
                }
            }
            release_memory_ptr = head_memory_ptr_;
            head_memory_ptr_ = head_memory_ptr_->next_memory_ptr_;
            memory_pool::ReleaseMemory<_IsGlobal>(release_memory_ptr);
        }
    }

    /**
     * @brief Internal function to move resources from another pool to this one.
     * @param _pool The pool to move from.
     * @return Void
     */
    Void MoveP(TPool&& _pool) noexcept {
        // Copy model data from the source pool
        if (_pool.has_model_) {
            ::new(model_obj_data_.template GetDataPtr<Void>())
                _ObjectType(std::move(*_pool.model_obj_data_.template GetDataPtr<_ObjectType>()));
        }

        head_node_ptr_ = _pool.head_node_ptr_;
        uninit_node_ptr_ = _pool.uninit_node_ptr_;
        uninit_node_end_ptr_ = _pool.uninit_node_end_ptr_;
        head_memory_ptr_ = _pool.head_memory_ptr_;
        has_model_ = _pool.has_model_;
        size_ = _pool.size_;
        _pool.head_node_ptr_ = nullptr;
        _pool.uninit_node_ptr_ = nullptr;
        _pool.uninit_node_end_ptr_ = nullptr;
        _pool.head_memory_ptr_ = nullptr;
        _pool.has_model_ = false;
        _pool.size_ = 0ULL;
    }

    /** @brief Pointer to the first node in the free list (recycled nodes). */
    NodeType_* head_node_ptr_;
    /** @brief Pointer to the next available uninitialized node in the current block. */
    NodeType_* uninit_node_ptr_;
    /** @brief Pointer to the end of the current memory block (limit for uninit_node_ptr). */
    NodeType_* uninit_node_end_ptr_;
    /** @brief Pointer to the head of the memory block list (linked list of allocated blocks). */
    MemoryHead_* head_memory_ptr_;
    /** @brief In-place storage for the model object used to initialize new nodes. */
    TFixedMemory<sizeof(_ObjectType)> model_obj_data_;
    /** @brief Flag indicating whether a model object has been set. */
    Bool has_model_;
    /** @brief The total number of nodes currently managed by the pool (allocated capacity). */
    SizeType size_;
};

/**
 * @brief Node wrapper for objects stored in a singly linked free list.
 * 
 * Implementation Principle:
 * Used by the object pool to embed a singly linked list pointer alongside the allocated object.
 * 
 * @tparam _ObjectType The type of object being wrapped.
 */
template<typename _ObjectType>
struct TSinglyLinkedNode_ {
    /** @brief The actual object data. */
    _ObjectType obj_;
    /** @brief Pointer to the next node in the free list. */
    TSinglyLinkedNode_* next_node_ptr_;
};

/**
 * @brief Node wrapper for objects stored in a doubly linked free list.
 * 
 * Implementation Principle:
 * Used by the object pool for specialized containers (like doubly linked lists) to embed both next and 
 * previous pointers alongside the allocated object.
 * 
 * @tparam _ObjectType The type of object being wrapped.
 */
template<typename _ObjectType>
struct TDoublyLinkedNode_ {
    /** @brief The actual object data. */
    _ObjectType obj_;
    /** @brief Pointer to the next node in the free list. */
    TDoublyLinkedNode_* next_node_ptr_;
    /** @brief Pointer to the previous node in the free list. */
    TDoublyLinkedNode_* previous_node_ptr_;
};

} // internal
} // zengine
namespace zengine {

/**
 * @brief Alias for a pool implementation using singly linked nodes.
 * 
 * Implementation Principle:
 * Leverages TPool with TSinglyLinkedNode_ as the underlying node wrapper, optimized for space when 
 * reverse traversal is not required.
 * 
 * @tparam _ObjectType The type of object to be stored in the pool.
 * @tparam _IsThreadSafe If true, enables thread-safe operations using an internal mutex.
 * @tparam _IsGlobal If true, uses the global memory pool for underlying block allocations.
 * @tparam _IfCallConstructorAndDestructor If true, explicitly calls constructors and destructors for objects.
 */
template<
    typename _ObjectType, 
    Bool _IsThreadSafe = false,
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
using TSinglyLinkedPool = internal::TPool<
    _ObjectType, 
    internal::TSinglyLinkedNode_<_ObjectType>, 
    _IsThreadSafe, 
    _IsGlobal, 
    _IfCallConstructorAndDestructor
>;

/**
 * @brief Alias for a pool implementation using doubly linked nodes.
 * 
 * Implementation Principle:
 * Leverages TPool with TDoublyLinkedNode_ as the underlying node wrapper, designed for specialized containers 
 * needing reverse traversal capabilities.
 * 
 * @tparam _ObjectType The type of object to be stored in the pool.
 * @tparam _IsThreadSafe If true, enables thread-safe operations using an internal mutex.
 * @tparam _IsGlobal If true, uses the global memory pool for underlying block allocations.
 * @tparam _IfCallConstructorAndDestructor If true, explicitly calls constructors and destructors for objects.
 */
template<
    typename _ObjectType,
    Bool _IsThreadSafe = false,
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
using TDoublyLinkedPool = internal::TPool<
    _ObjectType,
    internal::TDoublyLinkedNode_<_ObjectType>,
    _IsThreadSafe,
    _IsGlobal,
    _IfCallConstructorAndDestructor
>;

} // zengine