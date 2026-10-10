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

#include <deque>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief Double end queue container.
 * 
 * Implementation Principles:
 * 1. Core Structure: It encapsulates std::deque, implementing a dynamic array with segmented continuous memory.
 *    This allows O(1) time complexity for insertion and deletion at both the head and tail, avoiding massive
 *    memory migration during expansion compared to vector.
 * 
 * 2. Memory Management: Integrated with ZEngine's memory system via TAllocator.
 *    - If _IsGlobal is true, it uses the global memory pool, suitable for cross-thread or long-term storage.
 *    - If _IsGlobal is false, it may use specific strategies (like thread-local) to reduce lock contention and 
 *      overhead.
 * 
 * 3. Object System: Inherits from ZObject, making the container part of the engine's object hierarchy for potential
 *    features like serialization or unified management.
 * 
 * @tparam _ObjectType The type of elements stored in the deque.
 * @tparam _IsGlobal Indicates whether to use the global memory pool.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TDeque : public ZObject {
public:
    /** @brief The base class type. */
    using SuperType_ = ZObject;
    /** @brief The underlying standard deque type with the custom allocator. */
    using STDDeque_ = std::deque<_ObjectType, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Iterator type for the deque. */
    using Iterator_ = typename STDDeque_::iterator;
    /** @brief Constant iterator type for the deque. */
    using ConstIterator_ = typename STDDeque_::const_iterator;
    /** @brief Reverse iterator type for the deque. */
    using ReverseIterator_ = typename STDDeque_::reverse_iterator;
    /** @brief Constant reverse iterator type for the deque. */
    using ConstReverseIterator_ = typename STDDeque_::const_reverse_iterator;
    /** @brief Initializer list type for the deque. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor. Constructs an empty deque.
     */
    FORCEINLINE TDeque() noexcept : SuperType_(), deque_() {}

    /**
     * @brief Copy constructor.
     * @param _deque The deque to copy from.
     */
    FORCEINLINE TDeque(const TDeque& _deque) noexcept : SuperType_(_deque), deque_(_deque.deque_) {}

    /**
     * @brief Move constructor.
     * @param _deque The deque to move from.
     */
    FORCEINLINE TDeque(TDeque&& _deque) noexcept 
        : SuperType_(std::move(_deque))
        , deque_(std::move(_deque.deque_)) 
    {}

    /**
     * @brief Constructs a deque with a specific size.
     * @param _size The number of elements to initially create.
     */
    FORCEINLINE explicit TDeque(SizeType _size) noexcept : SuperType_(), deque_(_size) {}

    /**
     * @brief Constructs a deque with a specific size and fills it with a value.
     * @param _size The number of elements to create.
     * @param _value The value to fill the deque with.
     */
    FORCEINLINE TDeque(SizeType _size, const _ObjectType& _value) noexcept : SuperType_(), deque_(_size, _value) {}

    /**
     * @brief Range constructor. Constructs the deque with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The start of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TDeque(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), deque_(_first, _last) {}

    /**
     * @brief Initializer list constructor.
     * @param _init_list The initializer list to construct from.
     */
    FORCEINLINE TDeque(InitializerList_ _init_list) noexcept : SuperType_(), deque_(_init_list) {}
 
    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TDeque() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _deque The deque to copy from.
     * @return A reference to this deque.
     */
    FORCEINLINE TDeque& operator=(const TDeque& _deque) noexcept { 
        SuperType_::operator=(_deque);
        deque_ = _deque.deque_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _deque The deque to move from.
     * @return A reference to this deque.
     */
    FORCEINLINE TDeque& operator=(TDeque&& _deque) noexcept { 
        SuperType_::operator=(std::move(_deque));
        deque_ = std::move(_deque.deque_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * @param _init_list The initializer list to assign.
     * @return A reference to this deque.
     */
    FORCEINLINE TDeque& operator=(InitializerList_ _init_list) noexcept {
        deque_ = _init_list;
        return *this;
    }

    /**
     * @brief Assigns a new size and value to the deque.
     * @param _size The new size.
     * @param _value The value to fill with.
     * @return None.
     */
    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _value) noexcept {
        return deque_.assign(_size, _value);
    }

    /**
     * @brief Assigns a range to the deque.
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The start of the range.
     * @param _last The end of the range.
     * @return None.
     */
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return deque_.assign(_first, _last);
    }

    /**
     * @brief Assigns an initializer list to the deque.
     * @param _init_list The initializer list to assign.
     * @return None.
     */
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return deque_.assign(_init_list);
    }

    /**
     * @brief Checks if the deque is empty.
     * @return True if empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return deque_.empty(); }

    /**
     * @brief Checks if the deque is full.
     * @return Always returns false as deque is dynamic in size.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Gets the number of elements in the deque.
     * @return The size of the deque.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(deque_.size()); }

    /**
     * @brief Gets the capacity of the deque.
     * @return The maximum possible size of the deque (kSizeTypeMax).
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return kSizeTypeMax; }

    /**
     * @brief Checks if this deque is equal to another.
     * @param _deque The deque to compare with.
     * @return True if equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TDeque& _deque) noexcept { return deque_ == _deque.deque_; }

    /**
     * @brief Checks if this deque is not equal to another.
     * @param _deque The deque to compare with.
     * @return True if not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TDeque& _deque) noexcept { return deque_ != _deque.deque_; }

    /**
     * @brief Accesses the element at the specified index without bounds checking.
     * @param _index The index of the element to access.
     * @return A reference to the element.
     */
    NODISCARD FORCEINLINE _ObjectType& operator[](const SizeType _index) noexcept { return deque_[_index]; }

    /**
     * @brief Accesses the element at the specified index without bounds checking (const).
     * @param _index The index of the element to access.
     * @return A const reference to the element.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator[](const SizeType _index) const noexcept { return deque_[_index]; }

    /**
     * @brief Accesses the element at the specified index with bounds checking.
     * @param _index The index of the element to access.
     * @return A reference to the element.
     */
    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) { return deque_.at(_index); }

    /**
     * @brief Accesses the element at the specified index with bounds checking (const).
     * @param _index The index of the element to access.
     * @return A const reference to the element.
     */
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const { return deque_.at(_index); }

    /**
     * @brief Accesses the first element of the deque.
     * @return A reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return deque_.front(); }

    /**
     * @brief Accesses the first element of the deque (const).
     * @return A const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return deque_.front(); }

    /**
     * @brief Accesses the last element of the deque.
     * @return A reference to the last element.
     */
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return deque_.back(); }

    /**
     * @brief Accesses the last element of the deque (const).
     * @return A const reference to the last element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return deque_.back(); }

    /**
     * @brief Returns an iterator to the beginning.
     * @return An iterator to the beginning.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return deque_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return A const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return deque_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return A const iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return deque_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning.
     * @return A reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return deque_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return A const reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return deque_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return A const reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return deque_.crbegin(); }

    /**
     * @brief Returns an iterator to the end.
     * @return An iterator to the end.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return deque_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return A const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return deque_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return A const iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return deque_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end.
     * @return A reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return deque_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return A const reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return deque_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return A const reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return deque_.crend(); }

    /**
     * @brief Emplaces a new element at the specified position.
     * @tparam _ArgsType The types of arguments for constructing the element.
     * @param _pos The position to emplace at.
     * @param _args The arguments to forward to the constructor.
     * @return An iterator to the emplaced element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return deque_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Emplaces a new element at the front of the deque.
     * @tparam _ArgsType The types of arguments for constructing the element.
     * @param _args The arguments to forward to the constructor.
     * @return A reference to the emplaced element.
     */
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceFront(_ArgsType&&... _args) noexcept {
        return deque_.emplace_front(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Pushes a copy of a value to the front of the deque.
     * @param _value The value to push.
     * @return None.
     */
    FORCEINLINE Void PushFront(const _ObjectType& _value) noexcept { deque_.push_front(_value); }

    /**
     * @brief Pushes a value to the front of the deque via move.
     * @param _val The value to push.
     * @return None.
     */
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { deque_.push_front(std::move(_val)); }

    /**
     * @brief Removes the first element of the deque.
     * @return None.
     */
    FORCEINLINE Void PopFront() noexcept { deque_.pop_front(); }

    /**
     * @brief Emplaces a new element at the back of the deque.
     * @tparam _ArgsType The types of arguments for constructing the element.
     * @param _args The arguments to forward to the constructor.
     * @return A reference to the emplaced element.
     */
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return deque_.emplace_back(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Pushes a copy of a value to the back of the deque.
     * @param _val The value to push.
     * @return None.
     */
    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { deque_.push_back(_val); }

    /**
     * @brief Pushes a value to the back of the deque via move.
     * @param _val The value to push.
     * @return None.
     */
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { deque_.push_back(std::move(_val)); }

    /**
     * @brief Removes the last element of the deque.
     * @return None.
     */
    FORCEINLINE Void PopBack() noexcept { deque_.pop_back(); }

    /**
     * @brief Inserts a copy of a value at the specified position.
     * @param _pos The position to insert at.
     * @param _val The value to insert.
     * @return An iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return deque_.insert(_pos, _val);
    }

    /**
     * @brief Inserts a value at the specified position via move.
     * @param _pos The position to insert at.
     * @param _val The value to insert.
     * @return An iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return deque_.insert(_pos, std::move(_val));
    }

    /**
     * @brief Inserts multiple copies of a value at the specified position.
     * @param _pos The position to insert at.
     * @param _num The number of copies to insert.
     * @param _val The value to insert.
     * @return An iterator to the first inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return deque_.insert(_pos, _num, _val);
    }

    /**
     * @brief Inserts a range of elements at the specified position.
     * @tparam InputIterator The type of the input iterator.
     * @param _pos The position to insert at.
     * @param _first The start of the range.
     * @param _last The end of the range.
     * @return An iterator to the first inserted element.
     */
    template <typename InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InputIterator _first, InputIterator _last) noexcept {
        return deque_.insert(_pos, _first, _last);
    }

    /**
     * @brief Inserts an initializer list at the specified position.
     * @param _pos The position to insert at.
     * @param _init_list The initializer list to insert.
     * @return An iterator to the first inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return deque_.insert(_pos, _init_list);
    }

    /**
     * @brief Erases the element at the specified position.
     * @param _pos The position of the element to erase.
     * @return An iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return deque_.erase(_pos); }

    /**
     * @brief Erases a range of elements.
     * @param _first The start of the range to erase.
     * @param _last The end of the range to erase.
     * @return An iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return deque_.erase(_first, _last); 
    }

    /**
     * @brief Clears the deque, removing all elements.
     * @return None.
     */
    FORCEINLINE Void Clear() noexcept { deque_.clear(); }

    /**
     * @brief Resizes the deque.
     * @param _size The new size.
     * @return None.
     */
    FORCEINLINE Void Resize(SizeType _size) noexcept { deque_.resize(_size); }

    /**
     * @brief Resizes the deque and fills new elements with a value.
     * @param _size The new size.
     * @param _val The value to fill new elements with.
     * @return None.
     */
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { deque_.resize(_size, _val); }

    /**
     * @brief Swaps the contents of this deque with another.
     * @param _deque The deque to swap with.
     * @return None.
     */
    FORCEINLINE Void Swap(TDeque& _deque) noexcept { deque_.swap(_deque.deque_); }

public:
    /** @brief Returns an iterator to the beginning of the array. */
    NODISCARD FORCEINLINE Iterator_ begin() noexcept { return Begin(); }
    /** @brief Returns a const iterator to the beginning of the array. */
    NODISCARD FORCEINLINE ConstIterator_ begin() const noexcept { return Begin(); }
    /** @brief Returns an iterator to the end of the array. */
    NODISCARD FORCEINLINE Iterator_ end() noexcept { return End(); }
    /** @brief Returns a const iterator to the end of the array. */
    NODISCARD FORCEINLINE ConstIterator_ end() const noexcept { return End(); }

private:
    /** @brief The internal standard deque container. */
    STDDeque_ deque_;
};

} // zengine