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

#include <vector>

#include "t_allocator.h"
#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief Array container.
 * @details A wrapper around std::vector that integrates with the engine's object model and memory allocator.
 * @tparam _ObjectType The type of the elements stored in the array.
 * @tparam _IsGlobal Specifies whether to use the global memory pool.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TArray : public ZObject {
public:
    /** @brief The base class type. */
    using SuperType_ = ZObject;
    /** @brief The underlying standard vector type. */
    using STDArray_ = std::vector<_ObjectType, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Iterator for the array. */
    using Iterator_ = STDArray_::iterator;
    /** @brief Constant iterator for the array. */
    using ConstIterator_ = STDArray_::const_iterator;
    /** @brief Reverse iterator for the array. */
    using ReverseIterator_ = STDArray_::reverse_iterator;
    /** @brief Constant reverse iterator for the array. */
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    /** @brief Initializer list type for the array. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

public:
    /** @brief Default constructor. Constructs an empty array. */
    FORCEINLINE TArray() noexcept : SuperType_(), array_() {}

    /**
     * @brief Copy constructor.
     * @param _array The array to copy from.
     */
    FORCEINLINE TArray(const TArray& _array) noexcept : SuperType_(_array), array_(_array.array_) {}

    /**
     * @brief Move constructor.
     * @param _array The array to move from.
     */
    FORCEINLINE TArray(TArray&& _array) noexcept 
        : SuperType_(std::move(_array)), array_(std::move(_array.array_)) {}

    /**
     * @brief Constructs an array with count default-inserted instances of T.
     * @param _size The number of elements to insert.
     */
    FORCEINLINE explicit TArray(SizeType _size) noexcept : SuperType_(), array_(_size) {}

    /**
     * @brief Constructs an array with count copies of elements with value val.
     * @param _size The number of elements to insert.
     * @param _val The value to initialize the elements with.
     */
    FORCEINLINE TArray(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), array_(_size, _val) {}

    /**
     * @brief Constructs an array with the contents of the range [first, last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The iterator to the first element in the range.
     * @param _last The iterator to the element following the last element in the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TArray(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), array_(_first, _last) {}

    /**
     * @brief Constructs an array with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TArray(InitializerList_ _init_list) noexcept : SuperType_(), array_(_init_list) {}
 
    /** @brief Destructor. */
    FORCEINLINE ~TArray() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _array The array to copy from.
     * @return A reference to this array.
     */
    FORCEINLINE TArray& operator=(const TArray& _array) noexcept { 
        SuperType_::operator=(_array);
        array_ = _array.array_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _array The array to move from.
     * @return A reference to this array.
     */
    FORCEINLINE TArray& operator=(TArray&& _array) noexcept { 
        SuperType_::operator=(std::move(_array));
        array_ = std::move(_array.array_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * @param _init_list The initializer list to assign from.
     * @return A reference to this array.
     */
    FORCEINLINE TArray& operator=(InitializerList_ _init_list) noexcept {
        array_ = _init_list;
        return *this;
    }

    /**
     * @brief Replaces the contents with count copies of value val.
     * @param _size The new size of the array.
     * @param _val The value to initialize elements with.
     */
    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        array_.assign(_size, _val);
    }

    /**
     * @brief Replaces the contents with copies of those in the range [first, last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The iterator to the first element in the range.
     * @param _last The iterator to the element following the last element in the range.
     */
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        array_.assign(_first, _last);
    }

    /**
     * @brief Replaces the contents with the elements from the initializer list.
     * @param _init_list The initializer list to assign from.
     */
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        array_.assign(_init_list);
    }

    /**
     * @brief Checks if the contents of two arrays are equal.
     * @param _array The array to compare with.
     * @return True if the contents are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TArray& _array) noexcept { return this == &_array; }

    /**
     * @brief Checks if the contents of two arrays are not equal.
     * @param _array The array to compare with.
     * @return True if the contents are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TArray& _array) noexcept { return this != &_array; }

    /**
     * @brief Checks if the array has no elements.
     * @return True if the array is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return array_.empty(); }

    /**
     * @brief Checks if the array is full.
     * @return Always returns false as array is dynamic in size.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Returns a pointer to the underlying array data.
     * @return A pointer to the underlying array data.
     */
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept { return array_.data(); }

    /**
     * @brief Returns a pointer to the underlying array data (const).
     * @return A const pointer to the underlying array data.
     */
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept { return array_.data(); }

    /**
     * @brief Accesses the specified element.
     * @param _index The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { return array_[_index]; }

    /**
     * @brief Accesses the specified element (const).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { return array_[_index]; }

    /**
     * @brief Accesses the specified element with bounds checking.
     * @param _index The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) { return array_.at(_index); }

    /**
     * @brief Accesses the specified element with bounds checking (const).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const { return array_.at(_index); }

    /**
     * @brief Accesses the first element.
     * @return A reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return array_.front(); }

    /**
     * @brief Accesses the first element (const).
     * @return A const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return array_.front(); }

    /**
     * @brief Accesses the last element.
     * @return A reference to the last element.
     */
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return array_.back(); }

    /**
     * @brief Accesses the last element (const).
     * @return A const reference to the last element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return array_.back(); }

    /**
     * @brief Returns the number of elements in the array.
     * @return The number of elements in the array.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(array_.size()); }

    /**
     * @brief Returns the number of elements that the array has currently allocated space for.
     * @return The capacity of the array.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return array_.capacity(); }

    /**
     * @brief Returns an iterator to the first element.
     * @return An iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return array_.begin(); }

    /**
     * @brief Returns a const iterator to the first element.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return array_.begin(); }

    /**
     * @brief Returns a const iterator to the first element.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return array_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the first element of the reversed array.
     * @return A reverse iterator to the first element of the reversed array.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return array_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the first element of the reversed array.
     * @return A const reverse iterator to the first element of the reversed array.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return array_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the first element of the reversed array.
     * @return A const reverse iterator to the first element of the reversed array.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return array_.crbegin(); }

    /**
     * @brief Returns an iterator to the element following the last element.
     * @return An iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return array_.end(); }

    /**
     * @brief Returns a const iterator to the element following the last element.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return array_.end(); }

    /**
     * @brief Returns a const iterator to the element following the last element.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return array_.cend(); }

    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed array.
     * @return A reverse iterator to the element following the last element of the reversed array.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return array_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed array.
     * @return A const reverse iterator to the element following the last element of the reversed array.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return array_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed array.
     * @return A const reverse iterator to the element following the last element of the reversed array.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return array_.crend(); }

    /**
     * @brief Adds an element to the end.
     * @param _val The value to append.
     */
    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { array_.push_back(_val); }

    /**
     * @brief Adds an element to the end (move).
     * @param _val The value to append.
     */
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { array_.push_back(std::move(_val)); }

    /** @brief Removes the last element. */
    FORCEINLINE Void PopBack() noexcept { array_.pop_back(); }

    /**
     * @brief Inserts elements at the specified location in the container.
     * @param _pos Iterator before which the content will be inserted.
     * @param _val Element to insert.
     * @return Iterator pointing to the inserted value.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return array_.insert(_pos, _val);
    }

    /**
     * @brief Inserts elements at the specified location in the container (move).
     * @param _pos Iterator before which the content will be inserted.
     * @param _val Element to insert.
     * @return Iterator pointing to the inserted value.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return array_.insert(_pos, std::move(_val));
    }

    /**
     * @brief Inserts count copies of elements at the specified location in the container.
     * @param _pos Iterator before which the content will be inserted.
     * @param _num The number of elements to insert.
     * @param _val Element to insert.
     * @return Iterator pointing to the first element inserted, or _pos if count is 0.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return array_.insert(_pos, _num, _val);
    }

    /**
     * @brief Inserts elements from range [first, last) at the specified location.
     * @tparam _InputIterator The type of the input iterator.
     * @param _pos Iterator before which the content will be inserted.
     * @param _first The iterator to the first element in the range.
     * @param _last The iterator to the element following the last element in the range.
     * @return Iterator pointing to the first element inserted, or _pos if first==last.
     */
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return array_.insert(_pos, _first, _last);
    }

    /**
     * @brief Inserts elements from initializer list at the specified location.
     * @param _pos Iterator before which the content will be inserted.
     * @param _init_list The initializer list to insert from.
     * @return Iterator pointing to the first element inserted, or _pos if the list is empty.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return array_.insert(_pos, _init_list);
    }

    /**
     * @brief Removes the element at pos.
     * @param _pos Iterator to the element to remove.
     * @return Iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return array_.erase(_pos); }

    /**
     * @brief Removes the elements in the range [first, last).
     * @param _first Iterator to the first element to remove.
     * @param _last Iterator following the last element to remove.
     * @return Iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return array_.erase(_first, _last); 
    }

    /**
     * @brief Removes the first occurrence of the specified value from the array.
     * @param _val The value to remove.
     * @return True if the value was found and removed, false otherwise.
     */
    FORCEINLINE Bool EraseFirst(const _ObjectType& _val) noexcept {
        for (auto& iter: array_) {
            if (iter == _val) {
                array_.erase(iter);
                return true;
            }
        }
        return false;
    }

    /**
	 * @brief Removes the element at the specified position without preserving order.
     * @param _pos Iterator to the element to remove.
     * @return True if the element was found and removed, false otherwise.
     */
    template <typename _InputIterator>
    FORCEINLINE Bool EraseUnordered(_InputIterator _pos) noexcept {
        if (array_.empty() || _pos < array_.begin() || _pos >= array_.end()) {
            return false;
        }

        auto last_iter = array_.end() - 1;
        if (_pos != last_iter) {
            *_pos = std::move(*last_iter);
        }

        array_.pop_back();

        return true;
    }

    /**
	 * @brief Removes the first occurrence of the specified value from the array without preserving order.
     * @param _val The value to remove.
     * @return True if the value was found and removed, false otherwise.
     */
    FORCEINLINE Bool EraseFirstUnordered(const _ObjectType& _val) noexcept {
        for (SizeType index = 0; index < array_.size(); ++index) {
            if (array_[index] == _val) {
				array_[index] = std::move(array_.back());
                array_.pop_back();
                return true;
            }
        }
        return false;
    }

    /** 
     * @brief Clears the contents. 
     */
    FORCEINLINE Void Clear() noexcept { array_.clear(); }

    /**
     * @brief Constructs and inserts an element at the specified position.
     * @tparam _ArgsType The types of the arguments.
     * @param _pos Iterator before which the new element will be constructed.
     * @param _args Arguments to forward to the constructor of the element.
     * @return Iterator pointing to the constructed element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return array_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Appends a new element to the end of the container.
     * @tparam _ArgsType The types of the arguments.
     * @param _args Arguments to forward to the constructor of the element.
     * @return A reference to the inserted element.
     */
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return array_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    /**
     * @brief Resizes the container to contain count elements.
     * @param _size The new size of the container.
     */
    FORCEINLINE Void Resize(SizeType _size) noexcept { array_.resize(_size); }

    /**
     * @brief Resizes the container to contain count elements, copies val if elements are added.
     * @param _size The new size of the container.
     * @param _val The value to initialize new elements with.
     */
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { array_.resize(_size, _val); }

    /**
     * @brief Requests that the array capacity be at least enough to contain capacity elements.
     * @param _capacity The new capacity of the array.
     */
    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { array_.reserve(_capacity); }

    /**
     * @brief Exchanges the contents of the container with those of other.
     * @param _array The array to swap with.
     */
    FORCEINLINE Void Swap(TArray& _array) noexcept { array_.swap(_array.array_); }

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
    /** @brief The internal standard vector storage. */
    STDArray_ array_;
};

} // zengine