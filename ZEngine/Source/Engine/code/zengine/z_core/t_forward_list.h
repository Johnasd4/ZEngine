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

#include <forward_list>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief Forward List container.
 *
 * This container is implemented as a singly linked list. Elements are stored in individually allocated memory nodes,
 * linked together by pointers. It provides constant time insertion and removal of elements at the front of the
 * container, but does not support random access or bidirectional iteration. It is more memory efficient than a
 * doubly linked list.
 *
 * @tparam _ObjectType The type of the elements to be stored in the list.
 * @tparam _IsGlobal Specifies whether to use the global memory pool for allocation.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TForwardList : public ZObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;
    /** @brief Type alias for the underlying std::forward_list. */
    using STDForwardList_ = std::forward_list<_ObjectType, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Type alias for the iterator. */
    using Iterator_ = STDForwardList_::iterator;
    /** @brief Type alias for the const iterator. */
    using ConstIterator_ = STDForwardList_::const_iterator;
    /** @brief Type alias for the initializer list. */
    using InitializerList_ = std::initializer_list<_ObjectType>;


    /**
     * @brief Default constructor.
     *
     * Constructs an empty forward list.
     */
    FORCEINLINE TForwardList() noexcept : SuperType_(), forward_list_() {}

    /**
     * @brief Copy constructor.
     *
     * Constructs the container with the copy of the contents of another container.
     *
     * @param _forward_list The container to copy from.
     */
    FORCEINLINE TForwardList(const TForwardList& _forward_list) noexcept 
        : SuperType_(_forward_list), forward_list_(_forward_list.forward_list_) {}

    /**
     * @brief Move constructor.
     *
     * Constructs the container with the contents of another container using move semantics.
     *
     * @param _forward_list The container to move from.
     */
    FORCEINLINE TForwardList(TForwardList&& _forward_list) noexcept 
        : SuperType_(std::move(_forward_list))
        , forward_list_(std::move(_forward_list.forward_list_)) 
    {}

    /**
     * @brief Size constructor.
     *
     * Constructs the container with count default-inserted instances of T.
     *
     * @param _size The number of elements to construct.
     */
    FORCEINLINE TForwardList(SizeType _size) noexcept : SuperType_(), forward_list_(_size) {}

    /**
     * @brief Fill constructor.
     *
     * Constructs the container with count copies of elements with value val.
     *
     * @param _size The number of elements to construct.
     * @param _val The value to initialize elements with.
     */
    FORCEINLINE TForwardList(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_(), forward_list_(_size, _val) {}

    /**
     * @brief Range constructor.
     *
     * Constructs the container with the contents of the range [first, last).
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The iterator to the beginning of the range.
     * @param _last The iterator to the end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TForwardList(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), forward_list_(_first, _last) {}

    /**
     * @brief Initializer list constructor.
     *
     * Constructs the container with the contents of the initializer list.
     *
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TForwardList(InitializerList_ _init_list) noexcept : SuperType_(), forward_list_(_init_list) {}
 
    /**
     * @brief Destructor.
     *
     * Destroys the container and its elements.
     */
    FORCEINLINE ~TForwardList() noexcept = default;

    /**
     * @brief Copy assignment operator.
     *
     * Replaces the contents with a copy of the contents of another container.
     *
     * @param _forward_list The container to copy from.
     * @return Reference to this container.
     */
    FORCEINLINE TForwardList& operator=(const TForwardList& _forward_list) noexcept { 
        forward_list_ = _forward_list.forward_list_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * Replaces the contents with those of another container using move semantics.
     *
     * @param _forward_list The container to move from.
     * @return Reference to this container.
     */
    FORCEINLINE TForwardList& operator=(TForwardList&& _forward_list) noexcept { 
        forward_list_ = std::move(_forward_list.forward_list_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     *
     * Replaces the contents with those of the initializer list.
     *
     * @param _init_list The initializer list to assign from.
     * @return Reference to this container.
     */
    FORCEINLINE TForwardList& operator=(InitializerList_ _init_list) noexcept {
        forward_list_ = _init_list;
        return *this;
    }

    /**
     * @brief Assigns new contents to the container, replacing current contents.
     *
     * @param _size The new size of the container.
     * @param _val The value to initialize elements with.
     */
    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        forward_list_.assign(_size, _val);
    }

    /**
     * @brief Assigns new contents to the container from a range.
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The iterator to the beginning of the range.
     * @param _last The iterator to the end of the range.
     */
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        forward_list_.assign(_first, _last);
    }

    /**
     * @brief Assigns new contents to the container from an initializer list.
     *
     * @param _init_list The initializer list to assign from.
     */
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        forward_list_.assign(_init_list);
    }

    /**
     * @brief Checks if the container has no elements.
     *
     * @return True if the container is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return forward_list_.empty(); }

    /**
     * @brief Checks if the container is full.
     *
     * Since this is a linked list, it is effectively never full until memory runs out.
     *
     * @return False always.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Gets the maximum possible number of elements the container can hold.
     *
     * @return The maximum number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return kSizeTypeMax; }

    /**
     * @brief Checks if the contents of two containers are equal.
     *
     * @param _forward_list The container to compare with.
     * @return True if the contents are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TForwardList& _forward_list) noexcept { 
        return forward_list_ == _forward_list.forward_list_;
    }

    /**
     * @brief Checks if the contents of two containers are not equal.
     *
     * @param _forward_list The container to compare with.
     * @return True if the contents are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TForwardList& _forward_list) noexcept { 
        return forward_list_ != _forward_list.forward_list_;
    }

    /**
     * @brief Accesses the first element.
     *
     * @return Reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return forward_list_.front(); }

    /**
     * @brief Accesses the first element (const).
     *
     * @return Const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return forward_list_.front(); }

    /**
     * @brief Returns an iterator to the position before the beginning of the list.
     *
     * @return Iterator to the position before the beginning.
     */
    NODISCARD FORCEINLINE Iterator_ BeforeBegin() noexcept { return forward_list_.before_begin(); }

    /**
     * @brief Returns a const iterator to the position before the beginning of the list.
     *
     * @return Const iterator to the position before the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ BeforeBegin() const noexcept { return forward_list_.before_begin(); }

    /**
     * @brief Returns a const iterator to the position before the beginning of the list.
     *
     * @return Const iterator to the position before the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBeforeBegin() const noexcept { return forward_list_.before_begin(); }

    /**
     * @brief Returns an iterator to the beginning.
     *
     * @return Iterator to the beginning.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return forward_list_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     *
     * @return Const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return forward_list_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     *
     * @return Const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return forward_list_.cbegin(); }

    /**
     * @brief Returns an iterator to the end.
     *
     * @return Iterator to the end.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return forward_list_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     *
     * @return Const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return forward_list_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     *
     * @return Const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return forward_list_.cend(); }

    /**
     * @brief Constructs an element in-place at the beginning.
     *
     * @tparam ArgsType The types of the arguments to forward to the constructor of the element.
     * @param _args The arguments to forward to the constructor.
     */
    template <typename... ArgsType>
    FORCEINLINE Void EmplaceFront(ArgsType&&... _args) noexcept {
        forward_list_.emplace_front(std::forward<ArgsType>(_args)...);
    }

    /**
     * @brief Inserts an element to the beginning.
     *
     * @param _val The value to insert.
     */
    FORCEINLINE Void PushFront(const _ObjectType& _val) noexcept { forward_list_.push_front(_val); }

    /**
     * @brief Inserts an element to the beginning using move semantics.
     *
     * @param _val The value to insert.
     */
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { 
        forward_list_.push_front(std::move(_val)); 
    }

    /**
     * @brief Removes the first element.
     */
    FORCEINLINE Void PopFront() noexcept { forward_list_.pop_front(); }

    /**
     * @brief Constructs an element in-place after the specified position.
     *
     * @tparam _ArgsType The types of the arguments to forward to the constructor.
     * @param _pos The position after which to insert.
     * @param _args The arguments to forward to the constructor.
     * @return Iterator to the newly inserted element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ EmplaceAfter(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return forward_list_.emplace_after(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Inserts an element after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _val The value to insert.
     * @return Iterator to the newly inserted element.
     */
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, const _ObjectType& _val) noexcept {
        return forward_list_.insert_after(_pos, _val);
    }

    /**
     * @brief Inserts an element after the specified position using move semantics.
     *
     * @param _pos The position after which to insert.
     * @param _val The value to insert.
     * @return Iterator to the newly inserted element.
     */
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return forward_list_.insert_after(_pos, std::move(_val));
    }

    /**
     * @brief Inserts multiple copies of a value after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _num The number of elements to insert.
     * @param _val The value to insert.
     * @return Iterator to the last inserted element.
     */
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return forward_list_.insert_after(_pos, _num, _val);
    }

    /**
     * @brief Inserts a range of elements after the specified position.
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _pos The position after which to insert.
     * @param _first The iterator to the beginning of the range.
     * @param _last The iterator to the end of the range.
     * @return Iterator to the last inserted element.
     */
    template <typename _InputIterator>
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return forward_list_.insert_after(_pos, _first, _last);
    }

    /**
     * @brief Inserts an initializer list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _init_list The initializer list to insert.
     * @return Iterator to the last inserted element.
     */
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return forward_list_.insert_after(_pos, _init_list);
    }

    /**
     * @brief Moves elements from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move elements from.
     */
    FORCEINLINE Void SpliceAfter(ConstIterator_ _pos, TForwardList& _forward_list) noexcept {
        forward_list_.splice_after(_pos, _forward_list.forward_list_);
    }

    /**
     * @brief Moves elements from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move elements from (rvalue).
     */
    FORCEINLINE Void SpliceAfter(ConstIterator_ _pos, TForwardList&& _forward_list) noexcept {
        forward_list_.splice_after(_pos, std::move(_forward_list.forward_list_));
    }

    /**
     * @brief Moves an element from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move the element from.
     * @param _start_pos The position of the element to move in the source list (iterates to the element after).
     */
    FORCEINLINE Void SpliceAfter(
        ConstIterator_ _pos, TForwardList& _forward_list, ConstIterator_ _start_pos
    ) noexcept {
        forward_list_.splice_after(_pos, _forward_list.forward_list_, _start_pos);
    }

    /**
     * @brief Moves an element from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move the element from (rvalue).
     * @param _start_pos The position of the element to move in the source list (iterates to the element after).
     */
    FORCEINLINE Void SpliceAfter(
        ConstIterator_ _pos, TForwardList&& _forward_list, ConstIterator_ _start_pos
    ) noexcept {
        forward_list_.splice_after(_pos, std::move(_forward_list.forward_list_), _start_pos);
    }

    /**
     * @brief Moves a range of elements from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move elements from.
     * @param _first The start of the range in the source list.
     * @param _last The end of the range in the source list.
     */
    FORCEINLINE Void SpliceAfter(
        ConstIterator_ _pos, TForwardList& _forward_list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        forward_list_.splice_after(_pos, _forward_list.forward_list_, _first, _last);
    }

    /**
     * @brief Moves a range of elements from another list after the specified position.
     *
     * @param _pos The position after which to insert.
     * @param _forward_list The list to move elements from (rvalue).
     * @param _first The start of the range in the source list.
     * @param _last The end of the range in the source list.
     */
    FORCEINLINE Void SpliceAfter(
        ConstIterator_ _pos, TForwardList&& _forward_list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        forward_list_.splice_after(_pos, std::move(_forward_list.forward_list_), _first, _last);
    }

    /**
     * @brief Erases the element after the specified position.
     *
     * @param _pos The position before the element to erase.
     * @return Iterator to the element following the erased element.
     */
    FORCEINLINE Iterator_ EraseAfter(ConstIterator_ _pos) noexcept { return forward_list_.erase_after(_pos); }

    /**
     * @brief Erases elements in the range (_first, _last).
     *
     * @param _first The position before the first element to erase.
     * @param _last The position after the last element to erase.
     * @return Iterator to the element following the erased range.
     */
    FORCEINLINE Iterator_ EraseAfter(ConstIterator_ _first, ConstIterator_ _last) noexcept {
        return forward_list_.erase_after(_first, _last);
    }

    /**
     * @brief Remove all the objects same as the given value.
     *
     * @param _val The value of elements to remove.
     */
    FORCEINLINE Void Remove(const _ObjectType& _val) noexcept {
        forward_list_.remove(_val);
    }

    /**
     * @brief Remove all the objects that comply with the fucntion.
     *
     * @tparam _PredicateFunction The type of the predicate function.
     * @param _func The predicate function.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void RemoveIf(_PredicateFunction _func) noexcept {
        forward_list_.remove_if(_func);
    }

    /**
     * @brief Make all the objects unique.
     *
     * Removes consecutive duplicate elements.
     */
    FORCEINLINE Void Unique() noexcept {
        forward_list_.unique();
    }

    /**
     * @brief Make all the objects unique that comply with the fucntion.
     *
     * @tparam _PredicateFunction The type of the predicate function.
     * @param _func The predicate function.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void UniqueIf(_PredicateFunction _func) noexcept {
        forward_list_.unique(_func);
    }

    /**
     * @brief Merge the two lists together sorted.
     *
     * @param _forward_list The list to merge from.
     */
    FORCEINLINE Void Merge(TForwardList& _forward_list) {
        forward_list_.merge(_forward_list.forward_list_);
    }

    /**
     * @brief Merge the two lists together sorted.
     *
     * @param _forward_list The list to merge from.
     */
    FORCEINLINE Void Merge(TForwardList&& _forward_list) {
        forward_list_.merge(std::move(_forward_list.forward_list_));
    }

    /**
     * @brief Merge the two lists together sorted using a comparison function.
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _forward_list The list to merge from.
     * @param _func The comparison function.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TForwardList& _forward_list, _PredicateFunction _func) {
        forward_list_.merge(_forward_list.forward_list_, _func);
    }

    /**
     * @brief Merge the two lists together sorted using a comparison function.
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _forward_list The list to merge from.
     * @param _func The comparison function.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TForwardList&& _forward_list, _PredicateFunction _func) {
        forward_list_.merge(std::move(_forward_list.forward_list_), _func);
    }

    /**
     * @brief Sorts the elements.
     */
    FORCEINLINE Void Sort() noexcept {
        forward_list_.sort();
    }

    /**
     * @brief Sorts the elements using a comparison function.
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _func The comparison function.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Sort(_PredicateFunction _func) noexcept {
        forward_list_.sort(_func);
    }

    /**
     * @brief Clears the contents.
     */
    FORCEINLINE Void Clear() noexcept { forward_list_.clear(); }

    /**
     * @brief Resizes the container.
     *
     * @param _size The new size.
     */
    FORCEINLINE Void Resize(SizeType _size) noexcept { forward_list_.resize(_size); }

    /**
     * @brief Resizes the container.
     *
     * @param _size The new size.
     * @param _val The value to initialize new elements with.
     */
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { forward_list_.resize(_size, _val); }

    /**
     * @brief Reverses the order of the elements.
     */
    FORCEINLINE Void Reverse() noexcept { forward_list_.reverse(); }

    /**
     * @brief Swaps the contents with another container.
     *
     * @param _forward_list The container to swap with.
     */
    FORCEINLINE Void Swap(TForwardList& _forward_list) noexcept { forward_list_.swap(_forward_list.forward_list_); }

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
    /** @brief The underlying standard forward list container. */
    STDForwardList_ forward_list_;
};

} // zengine