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

#include "t_pool.h"

namespace zengine {

/**
 * @brief A list container wrapper for objects allocated from a memory pool.
 * 
 * Implementation Principle:
 * Uses an intrusive-like architecture where the list exclusively manages the linking pointers 
 * (previous_node_ptr_, next_node_ptr_) of nodes allocated and managed by an external pool.
 * This ensures zero-allocation operations during node insertions and deletions, providing maximum 
 * performance for scenarios that require massive and frequent object creation/destruction.
 * 
 * @tparam _ObjectType The type of the object stored in the list.
 * @tparam _PoolType The pool type that allocates and manages the nodes.
 */
template<typename _ObjectType, typename _PoolType>
class TPoolList : public ZObject {
private:
    /** @brief Base class alias. */
    using SuperType_ = ZObject;
    /** @brief Node type extracted from the pool type. */
    using NodeType_ = typename _PoolType::NodeType_;

public:
    /**
     * @brief Iterator for traversing the TPoolList.
     * 
     * Implementation Principle:
     * Wraps a raw node pointer and provides standard iteration capabilities by moving through 
     * the node's internal pointers.
     */
    class Iterator_ {
    public:
        /** tag indicating the iterator supports bidirectional operations. */
        using iterator_concept = std::bidirectional_iterator_tag;
        /** tag indicating the iterator supports bidirectional operations. */
        using iterator_category = std::bidirectional_iterator_tag;
        /** The type of the values iterated over. */
        using value_type = _ObjectType;
        /** Type used to represent the distance between two iterators. */
        using difference_type = std::ptrdiff_t;
        /** Const pointer to the value type. */
        using pointer = const _ObjectType*;
        /** Const reference to the value type. */
        using reference = const _ObjectType&;

    public:
        /**
         * @brief Constructor initializing the iterator with a node pointer.
         * @param _node_ptr The node to point to.
         */
        FORCEINLINE Iterator_(NodeType_* _node_ptr) noexcept : node_ptr_(_node_ptr) {}

        /**
         * @brief Copy constructor.
         * @param _iterator The iterator to copy from.
         */
        FORCEINLINE Iterator_(const Iterator_& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {}

        /**
         * @brief Move constructor.
         * @param _iterator The iterator to move from.
         */
        FORCEINLINE Iterator_(Iterator_&& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {
            _iterator.node_ptr_ = nullptr;
        }

        /**
         * @brief Equality comparison operator with a node pointer.
         * @param _node_ptr The node pointer to compare.
         * @return Bool True if equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator==(const NodeType_* _node_ptr) const noexcept {
            return node_ptr_ == _node_ptr;
        }

        /**
         * @brief Inequality comparison operator with a node pointer.
         * @param _node_ptr The node pointer to compare.
         * @return Bool True if not equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator!=(const NodeType_* _node_ptr) const noexcept {
            return node_ptr_ != _node_ptr;
        }

        /**
         * @brief Equality comparison operator with another iterator.
         * @param _iterator The iterator to compare.
         * @return Bool True if equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator==(const Iterator_& _iterator) const noexcept {
            return node_ptr_ == _iterator.node_ptr_;
        }

        /**
         * @brief Inequality comparison operator with another iterator.
         * @param _iterator The iterator to compare.
         * @return Bool True if not equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator!=(const Iterator_& _iterator) const noexcept {
            return node_ptr_ != _iterator.node_ptr_;
        }

        /**
         * @brief Assignment operator.
         * @param _iterator The iterator to assign from.
         * @return Iterator_& Reference to this iterator.
         */
        FORCEINLINE Iterator_& operator=(const Iterator_& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            return *this;
        }

        /**
         * @brief Pre-increment operator. Moves to the next node.
         * @return Iterator_& Reference to this iterator.
         */
        FORCEINLINE Iterator_& operator++() noexcept {
            node_ptr_ = node_ptr_->next_node_ptr_;
            return *this;
        }

        /**
         * @brief Post-increment operator. Moves to the next node and returns the previous state.
         * @return Iterator_ The iterator state before incrementing.
         */
        FORCEINLINE Iterator_ operator++(int) noexcept {
            Iterator_ temp = *this;
            node_ptr_ = node_ptr_->next_node_ptr_;
            return temp;
        }

        /**
         * @brief Pre-decrement operator. Moves to the previous node.
         * @return Iterator_& Reference to this iterator.
         */
        FORCEINLINE Iterator_& operator--() noexcept {
            node_ptr_ = node_ptr_->previous_node_ptr_;
            return *this;
        }

        /**
         * @brief Post-decrement operator. Moves to the previous node and returns the previous state.
         * @return Iterator_ The iterator state before decrementing.
         */
        FORCEINLINE Iterator_ operator--(int) noexcept {
            Iterator_ temp = *this;
            node_ptr_ = node_ptr_->previous_node_ptr_;
            return temp;
        }

        /**
         * @brief Dereference operator.
         * @return _ObjectType& Reference to the object.
         */
        NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return node_ptr_->obj_; }

        /**
         * @brief Const dereference operator.
         * @return const _ObjectType& Const reference to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return node_ptr_->obj_; }

        /**
         * @brief Member access operator.
         * @return _ObjectType* Pointer to the object.
         */
        NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return &node_ptr_->obj_; }

        /**
         * @brief Const member access operator.
         * @return const _ObjectType* Const pointer to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return &node_ptr_->obj_; }

        /**
         * @brief Gets the raw pointer to the object.
         * @return _ObjectType* Pointer to the object.
         */
        NODISCARD FORCEINLINE _ObjectType* GetPtr() noexcept { return &node_ptr_->obj_; }

        /**
         * @brief Gets the const raw pointer to the object.
         * @return const _ObjectType* Const pointer to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return &node_ptr_->obj_; }

        /** @brief Friend declaration to allow ConstIterator_ to access node_ptr_. */
        friend class ConstIterator_;

    private:
        /** @brief Pointer to the current node in the iteration. */
        NodeType_* node_ptr_;
    };

    /**
     * @brief Const iterator for traversing the TPoolList safely.
     * 
     * Implementation Principle:
     * Holds a const pointer to the underlying node to guarantee that neither the node links nor 
     * the object data can be modified through this iterator.
     */
    class ConstIterator_ {
    public:
        /** tag indicating the iterator supports bidirectional operations. */
        using iterator_concept = std::bidirectional_iterator_tag;
        /** tag indicating the iterator supports bidirectional operations. */
        using iterator_category = std::bidirectional_iterator_tag;
        /** The type of the values iterated over. */
        using value_type = _ObjectType;
        /** Type used to represent the distance between two iterators. */
        using difference_type = std::ptrdiff_t;
        /** Const pointer to the value type. */
        using pointer = const _ObjectType*;
        /** Const reference to the value type. */
        using reference = const _ObjectType&;

    public:
        /**
         * @brief Constructor initializing the iterator with a node pointer.
         * @param _node_ptr The node to point to.
         */
        FORCEINLINE ConstIterator_(const NodeType_* _node_ptr) noexcept : node_ptr_(_node_ptr) {}

        /**
         * @brief Copy constructor.
         * @param _iterator The iterator to copy from.
         */
        FORCEINLINE ConstIterator_(const ConstIterator_& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {}

        /**
         * @brief Constructor from a non-const iterator.
         * @param _iterator The non-const iterator to copy from.
         */
        FORCEINLINE ConstIterator_(const Iterator_& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {}

        /**
         * @brief Move constructor.
         * @param _iterator The iterator to move from.
         */
        FORCEINLINE ConstIterator_(ConstIterator_&& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {
            _iterator.node_ptr_ = nullptr;
        }

        /**
         * @brief Equality comparison operator with a node pointer.
         * @param _node_ptr The node pointer to compare.
         * @return Bool True if equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator==(const NodeType_* _node_ptr) const noexcept {
            return node_ptr_ == _node_ptr;
        }

        /**
         * @brief Inequality comparison operator with a node pointer.
         * @param _node_ptr The node pointer to compare.
         * @return Bool True if not equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator!=(const NodeType_* _node_ptr) const noexcept {
            return node_ptr_ != _node_ptr;
        }

        /**
         * @brief Equality comparison operator with another const iterator.
         * @param _iterator The iterator to compare.
         * @return Bool True if equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator==(const ConstIterator_& _iterator) const noexcept {
            return node_ptr_ == _iterator.node_ptr_;
        }

        /**
         * @brief Inequality comparison operator with another const iterator.
         * @param _iterator The iterator to compare.
         * @return Bool True if not equal, false otherwise.
         */
        NODISCARD FORCEINLINE Bool operator!=(const ConstIterator_& _iterator) const noexcept {
            return node_ptr_ != _iterator.node_ptr_;
        }

        /**
         * @brief Assignment operator.
         * @param _iterator The const iterator to assign from.
         * @return ConstIterator_& Reference to this iterator.
         */
        FORCEINLINE ConstIterator_& operator=(const ConstIterator_& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            return *this;
        }

        /**
         * @brief Pre-increment operator. Moves to the next node.
         * @return ConstIterator_& Reference to this iterator.
         */
        FORCEINLINE ConstIterator_& operator++() noexcept {
            node_ptr_ = node_ptr_->next_node_ptr_;
            return *this;
        }

        /**
         * @brief Post-increment operator. Moves to the next node and returns the previous state.
         * @return ConstIterator_ The iterator state before incrementing.
         */
        FORCEINLINE ConstIterator_ operator++(int) noexcept {
            ConstIterator_ temp = *this;
            node_ptr_ = node_ptr_->next_node_ptr_;
            return temp;
        }

        /**
         * @brief Pre-decrement operator. Moves to the previous node.
         * @return ConstIterator_& Reference to this iterator.
         */
        FORCEINLINE ConstIterator_& operator--() noexcept {
            node_ptr_ = node_ptr_->previous_node_ptr_;
            return *this;
        }

        /**
         * @brief Post-decrement operator. Moves to the previous node and returns the previous state.
         * @return ConstIterator_ The iterator state before decrementing.
         */
        FORCEINLINE ConstIterator_ operator--(int) noexcept {
            ConstIterator_ temp = *this;
            node_ptr_ = node_ptr_->previous_node_ptr_;
            return temp;
        }

        /**
         * @brief Const dereference operator.
         * @return const _ObjectType& Const reference to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return node_ptr_->obj_; }

        /**
         * @brief Const member access operator.
         * @return const _ObjectType* Const pointer to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return &node_ptr_->obj_; }

        /**
         * @brief Gets the const raw pointer to the object.
         * @return const _ObjectType* Const pointer to the object.
         */
        NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return &node_ptr_->obj_; }

    private:
        /** @brief Pointer to the current node in the iteration. */
        const NodeType_* node_ptr_;
    };

public:
    /**
     * @brief Constructor associating the list with a memory pool.
     * @param _pool_ptr Pointer to the pool that manages the memory for this list's nodes.
     */
    FORCEINLINE TPoolList(_PoolType* _pool_ptr) noexcept
        : SuperType_()
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , size_(0ULL)
        , pool_ptr_(_pool_ptr)
    {}

    /**
     * @brief Move constructor.
     * @param _list The list to move from.
     */
    FORCEINLINE TPoolList(TPoolList&& _list) noexcept
        : SuperType_(std::move(_list))
    {
        MoveP(std::move(_list));
    }

    /**
     * @brief Destructor. Clears the list and releases nodes back to the pool.
     */
    FORCEINLINE ~TPoolList() noexcept {
        Clear();
    }

    /**
     * @brief Move assignment operator.
     * @param _list The list to move from.
     * @return TPoolList& Reference to this list.
     */
    TPoolList& operator=(TPoolList&& _list) noexcept {
        if (this == &_list) {
            return *this;
        }
        SuperType_::operator=(std::move(_list));
        Clear();
        MoveP(std::move(_list));
        return *this;
    }

    /**
     * @brief Gets the number of elements in the list.
     * @return SizeType The size of the list.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept {
        return size_;
    }

    /**
     * @brief Checks if the list is empty.
     * @return Bool True if empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return size_ == 0ULL; }

    /**
     * @brief Checks if the list is full (always false for this implementation).
     * @return Bool Always false.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Accesses the first element in the list.
     * @return _ObjectType& Reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        return head_node_ptr_->obj_;
    }

    /**
     * @brief Accesses the first element in the list (const).
     * @return const _ObjectType& Const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        return head_node_ptr_->obj_;
    }

    /**
     * @brief Accesses the last element in the list.
     * @return _ObjectType& Reference to the last element.
     */
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        return end_node_ptr_->obj_;
    }

    /**
     * @brief Accesses the last element in the list (const).
     * @return const _ObjectType& Const reference to the last element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        return end_node_ptr_->obj_;
    }

    /**
     * @brief Returns an iterator to the beginning of the list.
     * @return Iterator_ Iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return Iterator_(head_node_ptr_); }

    /**
     * @brief Returns a const iterator to the beginning of the list.
     * @return ConstIterator_ Const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return ConstIterator_(head_node_ptr_); }

    /**
     * @brief Returns an iterator to the end of the list (past the last element).
     * @return Iterator_ Iterator to the end.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return Iterator_(nullptr); }

    /**
     * @brief Returns a const iterator to the end of the list.
     * @return ConstIterator_ Const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return ConstIterator_(nullptr); }

    /**
     * @brief Pushes an object to the front of the list.
     * @param _obj_ptr Pointer to the object (must be allocated from the associated pool).
     * @return Void
     */
    Void PushFront(_ObjectType* _obj_ptr) noexcept {
        NodeType_* node_ptr = reinterpret_cast<NodeType_*>(_obj_ptr);
        node_ptr->next_node_ptr_ = head_node_ptr_;
        node_ptr->previous_node_ptr_ = nullptr;
        // first node
        if (head_node_ptr_ == nullptr) {
            head_node_ptr_ = end_node_ptr_ = node_ptr;
        }
        else {
            head_node_ptr_->previous_node_ptr_ = node_ptr;
            head_node_ptr_ = node_ptr;
        }
        ++size_;
    }

    /**
     * @brief Pushes an object to the back of the list.
     * @param _obj_ptr Pointer to the object (must be allocated from the associated pool).
     * @return Void
     */
    Void PushBack(_ObjectType* _obj_ptr) noexcept {
        NodeType_* node_ptr = reinterpret_cast<NodeType_*>(_obj_ptr);
        node_ptr->next_node_ptr_ = nullptr;
        node_ptr->previous_node_ptr_ = end_node_ptr_;
        // first node
        if (head_node_ptr_ == nullptr) {
            head_node_ptr_ = end_node_ptr_ = node_ptr;
        }
        else {
            end_node_ptr_->next_node_ptr_ = node_ptr;
            end_node_ptr_ = node_ptr;
        }
        ++size_;
    }

    /**
     * @brief Removes the first element from the list and releases it to the pool.
     * @return Void
     */
    Void PopFront() noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        _ObjectType* release_obj_ptr = &head_node_ptr_->obj_;
        if (head_node_ptr_ != end_node_ptr_) {
            head_node_ptr_ = head_node_ptr_->next_node_ptr_;
            head_node_ptr_->previous_node_ptr_ = nullptr;
        }
        else {
            head_node_ptr_ = end_node_ptr_ = nullptr;
        }
        --size_;
        pool_ptr_->Release(release_obj_ptr);
    }

    /**
     * @brief Removes the last element from the list and releases it to the pool.
     * @return Void
     */
    Void PopBack() noexcept {
        ASSERT(size_ > 0ULL, "TPoolList: List is empty!");
        _ObjectType* release_obj_ptr = &end_node_ptr_->obj_;
        if (head_node_ptr_ != end_node_ptr_) {
            end_node_ptr_ = end_node_ptr_->previous_node_ptr_;
            end_node_ptr_->next_node_ptr_ = nullptr;
        }
        else {
            head_node_ptr_ = end_node_ptr_ = nullptr;
        }
        --size_;
        pool_ptr_->Release(release_obj_ptr);
    }

    /**
     * @brief Removes a specific element from the list and releases it to the pool.
     * @param _obj_ptr Pointer to the object to erase.
     * @return Void
     */
    Void Erase(_ObjectType* _obj_ptr) noexcept {
        ASSERT(_obj_ptr != nullptr, "TPoolList: Invalid object pointer!");
        NodeType_* node_ptr = reinterpret_cast<NodeType_*>(_obj_ptr);
        if (head_node_ptr_ == end_node_ptr_) {
            head_node_ptr_ = end_node_ptr_ = nullptr;
        }
        else if (node_ptr == head_node_ptr_) {
            head_node_ptr_ = head_node_ptr_->next_node_ptr_;
            head_node_ptr_->previous_node_ptr_ = nullptr;
        }
        else if (node_ptr == end_node_ptr_) {
            end_node_ptr_ = end_node_ptr_->previous_node_ptr_;
            end_node_ptr_->next_node_ptr_ = nullptr;
        }
        else {
            node_ptr->previous_node_ptr_->next_node_ptr_ = node_ptr->next_node_ptr_;
            node_ptr->next_node_ptr_->previous_node_ptr_ = node_ptr->previous_node_ptr_;
        }
        --size_;
        pool_ptr_->Release(_obj_ptr);
    }

    /**
     * @brief Clears the list by releasing all elements back to the pool.
     * @return Void
     */
    Void Clear() noexcept {
        if(size_ > 0ULL) {
            pool_ptr_->Release(head_node_ptr_, end_node_ptr_);
            head_node_ptr_ = end_node_ptr_ = nullptr;
            size_ = 0ULL;
        }
    }

private:
    /**
     * @brief Internal function to move resources from another pool list to this one.
     * @param _pool_list The pool list to move from.
     * @return Void
     */
    Void MoveP(TPoolList&& _pool_list) noexcept {
        head_node_ptr_ = _pool_list.head_node_ptr_;
        end_node_ptr_ = _pool_list.end_node_ptr_;
        size_ = _pool_list.size_;
        pool_ptr_ = _pool_list.pool_ptr_;
        _pool_list.head_node_ptr_ = nullptr;
        _pool_list.end_node_ptr_ = nullptr;
        _pool_list.size_ = 0ULL;
    }

public:
    /** @brief Alias for the Begin() function, used for stl compatibility. */
    NODISCARD FORCEINLINE Iterator_ begin() noexcept { return Begin(); }
    /** @brief Alias for the Begin() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ begin() const noexcept { return Begin(); }
    /** @brief Alias for the End() function, used for stl compatibility. */
    NODISCARD FORCEINLINE Iterator_ end() noexcept { return End(); }
    /** @brief Alias for the End() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ end() const noexcept { return End(); }

private:
    /** @brief Pointer to the first node in the list. */
    NodeType_* head_node_ptr_;
    /** @brief Pointer to the last node in the list. */
    NodeType_* end_node_ptr_;
    /** @brief The number of elements in the list. */
    SizeType size_;
    /** @brief Pointer to the pool that manages the memory for these nodes. */
    _PoolType* pool_ptr_;
};

} // zengine