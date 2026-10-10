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

#include <list>

#include "t_allocator.h"
#include "t_lock_guard.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief List container wrapper.
 *
 * This container is implemented as a doubly-linked list. It supports constant time insertion and
 * removal of elements from anywhere in the container. Fast random access is not supported.
 * It uses a custom allocator for memory management.
 *
 * @tparam _ObjectType The type of elements stored in the list.
 * @tparam _IsGlobal Specifies whether to use the global memory pool.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TList : public ZObject {
public:
    /** @brief Type definition for the base class. */
    using SuperType_ = ZObject;
    /** @brief Type definition for the underlying std::list. */
    using STDList_ = std::list<_ObjectType, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Type definition for the iterator. */
    using Iterator_ = STDList_::iterator;
    /** @brief Type definition for the const iterator. */
    using ConstIterator_ = STDList_::const_iterator;
    /** @brief Type definition for the reverse iterator. */
    using ReverseIterator_ = STDList_::reverse_iterator;
    /** @brief Type definition for the const reverse iterator. */
    using ConstReverseIterator_ = STDList_::const_reverse_iterator;
    /** @brief Type definition for the initializer list. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     *
     * Constructs an empty list.
     */
    FORCEINLINE TList() noexcept : SuperType_(), list_() {}

    /**
     * @brief Copy constructor.
     *
     * Constructs a list with a copy of each of the elements in another list.
     *
     * @param _list The list to copy from.
     */
    FORCEINLINE TList(const TList& _list) noexcept : SuperType_(_list), list_(_list.list_) {}

    /**
     * @brief Move constructor.
     *
     * Constructs a list that acquires the elements of another list.
     *
     * @param _list The list to move from.
     */
    FORCEINLINE TList(TList&& _list) noexcept 
        : SuperType_(std::move(_list))
        , list_(std::move(_list.list_)) 
    {}

    /**
     * @brief Size constructor.
     *
     * Constructs a list with n default-inserted elements.
     *
     * @param _size The number of elements to construct.
     */
    FORCEINLINE explicit TList(SizeType _size) noexcept : SuperType_(), list_(_size) {}

    /**
     * @brief Fill constructor.
     *
     * Constructs a list with n elements, each being a copy of val.
     *
     * @param _size The number of elements to construct.
     * @param _val The value to copy.
     */
    FORCEINLINE TList(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), list_(_size, _val) {}

    /**
     * @brief Range constructor.
     *
     * Constructs a list with as many elements as the range [_first, _last).
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _first Iterator to the first element in the range.
     * @param _last Iterator to the element following the last element in the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TList(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), list_(_first, _last) {}

    /**
     * @brief Initializer list constructor.
     *
     * Constructs a list with the contents of the initializer list.
     *
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TList(InitializerList_ _init_list) noexcept : SuperType_(), list_(_init_list) {}
 
    /**
     * @brief Destructor.
     *
     * Destroys the list and deallocates memory.
     */
    FORCEINLINE ~TList() noexcept = default;

    /**
     * @brief Copy assignment operator.
     *
     * Replaces the contents of the container with a copy of the contents of _list.
     *
     * @param _list The list to copy from.
     * @return Reference to this list.
     */
    FORCEINLINE TList& operator=(const TList& _list) noexcept { 
        SuperType_::operator=(_list);
        list_ = _list.list_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * Replaces the contents of the container with the contents of _list using move semantics.
     *
     * @param _list The list to move from.
     * @return Reference to this list.
     */
    FORCEINLINE TList& operator=(TList&& _list) noexcept { 
        SuperType_::operator=(std::move(_list));
        list_ = std::move(_list.list_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     *
     * Replaces the contents of the container with the contents of the initializer list.
     *
     * @param _init_list The initializer list to assign from.
     * @return Reference to this list.
     */
    FORCEINLINE TList& operator=(InitializerList_ _init_list) noexcept {
        list_ = _init_list;
        return *this;
    }

    /**
     * @brief Assigns new contents to the list, replacing its current contents.
     *
     * @param _size The new size of the list.
     * @param _val The value to fill the list with.
     * @return Void.
     */
    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return list_.assign(_size, _val);
    }

    /**
     * @brief Assigns new contents to the list from a range.
     *
     * @tparam InputIterator The type of the input iterator.
     * @param _first Iterator to the first element in the range.
     * @param _last Iterator to the element following the last element in the range.
     * @return Void.
     */
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator _first, InputIterator _last) noexcept {
        return list_.assign(_first, _last);
    }

    /**
     * @brief Assigns new contents to the list from an initializer list.
     *
     * @param _init_list The initializer list to assign from.
     * @return Void.
     */
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return list_.assign(_init_list);
    }

    /**
     * @brief Checks if the list is full.
     *
     * Since this is a linked list using dynamic allocation, it is technically never "full" unless
     * memory is exhausted.
     *
     * @return Always false in this implementation.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Gets the number of elements in the list.
     *
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(list_.size()); }

    /**
     * @brief Gets the maximum possible number of elements.
     *
     * @return The maximum size.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return kSizeTypeMax; }

    /**
     * @brief Checks if two lists are equal.
     *
     * @param _list The list to compare with.
     * @return True if the lists are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TList& _list) noexcept { 
        return list_ == _list.list_;
    }

    /**
     * @brief Checks if two lists are not equal.
     *
     * @param _list The list to compare with.
     * @return True if the lists are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TList& _list) noexcept { 
        return list_ != _list.list_;
    }

    /**
     * @brief Accesses the first element.
     *
     * @return Reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return list_.front(); }

    /**
     * @brief Accesses the first element (const).
     *
     * @return Const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return list_.front(); }

    /**
     * @brief Accesses the last element.
     *
     * @return Reference to the last element.
     */
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return list_.back(); }

    /**
     * @brief Accesses the last element (const).
     *
     * @return Const reference to the last element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return list_.back(); }

    /**
     * @brief Checks if the list is empty.
     *
     * @return True if the list is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return list_.empty(); }

    /**
     * @brief Returns an iterator to the beginning.
     *
     * @return Iterator to the beginning.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return list_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     *
     * @return Const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return list_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     *
     * @return Const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return list_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning.
     *
     * @return Reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return list_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     *
     * @return Const reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return list_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     *
     * @return Const reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return list_.crbegin(); }

    /**
     * @brief Returns an iterator to the end.
     *
     * @return Iterator to the end.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return list_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     *
     * @return Const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return list_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     *
     * @return Const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return list_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end.
     *
     * @return Reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return list_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     *
     * @return Const reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return list_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     *
     * @return Const reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return list_.crend(); }

    /**
     * @brief Constructs and inserts an element at the specified position.
     *
     * @tparam _ArgsType The types of arguments to forward to the constructor.
     * @param _pos Position before which the new element is to be inserted.
     * @param _args Arguments forwarded to construct the element.
     * @return Iterator pointing to the constructed element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return list_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Constructs and inserts an element at the beginning.
     *
     * @tparam _ArgsType The types of arguments to forward to the constructor.
     * @param _args Arguments forwarded to construct the element.
     * @return Reference to the constructed element.
     */
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceFront(_ArgsType&&... _args) noexcept {
        return list_.emplace_front(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Adds an element to the front of the list.
     *
     * @param _val The value to copy.
     * @return Void.
     */
    FORCEINLINE Void PushFront(const _ObjectType& _val) noexcept { list_.push_front(_val); }

    /**
     * @brief Adds an element to the front of the list (move).
     *
     * @param _val The value to move.
     * @return Void.
     */
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { list_.push_front(std::move(_val)); }

    /**
     * @brief Removes the first element.
     *
     * @return Void.
     */
    FORCEINLINE Void PopFront() noexcept { list_.pop_front(); }

    /**
     * @brief Constructs and inserts an element at the end.
     *
     * @tparam _ArgsType The types of arguments to forward to the constructor.
     * @param _args Arguments forwarded to construct the element.
     * @return Reference to the constructed element.
     */
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return list_.emplace_back(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Adds an element to the back of the list.
     *
     * @param _val The value to copy.
     * @return Void.
     */
    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { list_.push_back(_val); }

    /**
     * @brief Adds an element to the back of the list (move).
     *
     * @param _val The value to move.
     * @return Void.
     */
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { list_.push_back(std::move(_val)); }

    /**
     * @brief Removes the last element.
     *
     * @return Void.
     */
    FORCEINLINE Void PopBack() noexcept { list_.pop_back(); }

    /**
     * @brief Inserts a new element at the specified position.
     *
     * @param _pos Position before which the new element is inserted.
     * @param _val The value to copy.
     * @return Iterator pointing to the newly inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept {
        return list_.insert(_pos, _val);
    }

    /**
     * @brief Inserts a new element at the specified position (move).
     *
     * @param _pos Position before which the new element is inserted.
     * @param _val The value to move.
     * @return Iterator pointing to the newly inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return list_.insert(_pos, std::move(_val));
    }

    /**
     * @brief Inserts multiple copies of a value at the specified position.
     *
     * @param _pos Position before which the new elements are inserted.
     * @param _num The number of elements to insert.
     * @param _val The value to copy.
     * @return Iterator pointing to the first of the newly inserted elements.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return list_.insert(_pos, _num, _val);
    }

    /**
     * @brief Inserts elements from a range at the specified position.
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _pos Position before which the new elements are inserted.
     * @param _first Iterator to the first element in the range.
     * @param _last Iterator to the element following the last element in the range.
     * @return Iterator pointing to the first of the newly inserted elements.
     */
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return list_.insert(_pos, _first, _last);
    }

    /**
     * @brief Inserts elements from an initializer list at the specified position.
     *
     * @param _pos Position before which the new elements are inserted.
     * @param _init_list The initializer list to insert from.
     * @return Iterator pointing to the first of the newly inserted elements.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return list_.insert(_pos, _init_list);
    }

    /**
     * @brief Transfers all elements from one list to this list.
     *
     * @param _pos Position before which the elements are inserted.
     * @param _list The list to transfer from.
     * @return Void.
     */
    FORCEINLINE Void Splice(ConstIterator_ _pos, TList& _list) noexcept {
        list_.splice(_pos, _list.list_);
    }

    /**
     * @brief Transfers all elements from one list to this list (move).
     *
     * @param _pos Position before which the elements are inserted.
     * @param _list The list to transfer from.
     * @return Void.
     */
    FORCEINLINE Void Splice(ConstIterator_ _pos, TList&& _list) noexcept {
        list_.splice(_pos, std::move(_list.list_));
    }

    /**
     * @brief Transfers a single element from one list to this list.
     *
     * @param _pos Position before which the element is inserted.
     * @param _list The list to transfer from.
     * @param _start_pos Iterator to the element in _list to transfer.
     * @return Void.
     */
    FORCEINLINE Void Splice(ConstIterator_ _pos, TList& _list, ConstIterator_ _start_pos) noexcept {
        list_.splice(_pos, _list.list_, _start_pos);
    }

    /**
     * @brief Transfers a single element from one list to this list (move).
     *
     * @param _pos Position before which the element is inserted.
     * @param _list The list to transfer from.
     * @param _start_pos Iterator to the element in _list to transfer.
     * @return Void.
     */
    FORCEINLINE Void Splice(ConstIterator_ _pos, TList&& _list, ConstIterator_ _start_pos) noexcept {
        list_.splice(_pos, std::move(_list.list_), _start_pos);
    }

    /**
     * @brief Transfers a range of elements from one list to this list.
     *
     * @param _pos Position before which the elements are inserted.
     * @param _list The list to transfer from.
     * @param _first Iterator to the first element in the range.
     * @param _last Iterator to the element following the last element in the range.
     * @return Void.
     */
    FORCEINLINE Void Splice(
        ConstIterator_ _pos, TList& _list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        list_.splice(_pos, _list.list_, _first, _last);
    }

    /**
     * @brief Transfers a range of elements from one list to this list (move).
     *
     * @param _pos Position before which the elements are inserted.
     * @param _list The list to transfer from.
     * @param _first Iterator to the first element in the range.
     * @param _last Iterator to the element following the last element in the range.
     * @return Void.
     */
    FORCEINLINE Void Splice(
        ConstIterator_ _pos, TList&& _list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        list_.splice(_pos, std::move(_list.list_), _first, _last);
    }

    /**
     * @brief Removes an element at the specified position.
     *
     * @param _pos Iterator to the element to remove.
     * @return Iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return list_.erase(_pos); }

    /**
     * @brief Removes a range of elements.
     *
     * @param _first Iterator to the first element in the range to remove.
     * @param _last Iterator to the element following the last element in the range to remove.
     * @return Iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept {
        return list_.erase(_first, _last);
    }

    /**
     * @brief Removes all elements equal to the given value.
     *
     * @param _val The value of elements to remove.
     * @return Void.
     */
    FORCEINLINE Void Remove(const _ObjectType& _val) noexcept {
        list_.remove(_val);
    }

    /**
     * @brief Removes all elements that satisfy the predicate.
     *
     * @tparam _PredicateFunction The type of the predicate.
     * @param _func The predicate function.
     * @return Void.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void RemoveIf(_PredicateFunction _func) noexcept {
        list_.remove_if(_func);
    }

    /**
     * @brief Removes consecutive duplicate elements.
     *
     * @return Void.
     */
    FORCEINLINE Void Unique() noexcept {
        list_.unique();
    }

    /**
     * @brief Removes consecutive duplicate elements that satisfy the predicate.
     *
     * @tparam _PredicateFunction The type of the binary predicate.
     * @param _func The predicate function.
     * @return Void.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void UniqueIf(_PredicateFunction _func) noexcept {
        list_.unique(_func);
    }

    /**
     * @brief Merges two sorted lists.
     *
     * @param _list The sorted list to merge from.
     * @return Void.
     */
    FORCEINLINE Void Merge(TList& _list) {
        list_.merge(_list.list_);
    }

    /**
     * @brief Merges two sorted lists (move).
     *
     * @param _list The sorted list to merge from.
     * @return Void.
     */
    FORCEINLINE Void Merge(TList&& _list) {
        list_.merge(std::move(_list.list_));
    }

    /**
     * @brief Merges two sorted lists using a comparison function.
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _list The sorted list to merge from.
     * @param _func The comparison function.
     * @return Void.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TList& _list, _PredicateFunction _func) {
        list_.merge(_list.list_, _func);
    }

    /**
     * @brief Merges two sorted lists using a comparison function (move).
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _list The sorted list to merge from.
     * @param _func The comparison function.
     * @return Void.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TList&& _list, _PredicateFunction _func) {
        list_.merge(std::move(_list.list_), _func);
    }

    /**
     * @brief Sorts the elements in the list.
     *
     * @return Void.
     */
    FORCEINLINE Void Sort() noexcept {
        list_.sort();
    }

    /**
     * @brief Sorts the elements in the list using a comparison function.
     *
     * @tparam _PredicateFunction The type of the comparison function.
     * @param _func The comparison function.
     * @return Void.
     */
    template <typename _PredicateFunction>
    FORCEINLINE Void Sort(_PredicateFunction _func) noexcept {
        list_.sort(_func);
    }

    /**
     * @brief Clears the contents of the list.
     *
     * @return Void.
     */
    FORCEINLINE Void Clear() noexcept { list_.clear(); }

    /**
     * @brief Resizes the list to contain the specified number of elements.
     *
     * @param _size The new size.
     * @return Void.
     */
    FORCEINLINE Void Resize(SizeType _size) noexcept { list_.resize(_size); }

    /**
     * @brief Resizes the list to contain the specified number of elements.
     *
     * @param _size The new size.
     * @param _val The value to initialize new elements with.
     * @return Void.
     */
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { list_.resize(_size, _val); }

    /**
     * @brief Reverses the order of the elements in the list.
     *
     * @return Void.
     */
    FORCEINLINE Void Reverse() noexcept { list_.reverse(); }

    /**
     * @brief Swaps the contents of this list with another list.
     *
     * @param _list The list to swap with.
     * @return Void.
     */
    FORCEINLINE Void Swap(TList& _list) noexcept { list_.swap(_list.list_); }

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
    /** @brief The underlying standard list container. */
    STDList_ list_;
};

} // zengine