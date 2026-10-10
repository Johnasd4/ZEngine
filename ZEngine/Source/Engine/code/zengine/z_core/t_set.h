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

#include <set>

#include "t_allocator.h"
#include "t_pair.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief Set container.
 * 
 * This class is a wrapper around std::set, providing a sorted associative container that contains unique keys.
 * It is typically implemented using a Red-Black Tree (self-balancing binary search tree), ensuring logarithmic
 * time complexity for insertion, deletion, and search operations.
 * 
 * @tparam _ObjectType The type of elements stored in the set.
 * @tparam _IsGlobal Specifies whether to use the global memory pool for allocation.
 * @tparam _CompareFunction The comparison function object type used to sort elements.
 */
template<
    typename _ObjectType, 
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL, 
    typename _CompareFunction = std::less<_ObjectType>
>
class TSet : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying std::set type. */
    using STDSet_ = std::set<_ObjectType, _CompareFunction, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Alias for the iterator type. */
    using Iterator_ = STDSet_::iterator;
    /** @brief Alias for the const iterator type. */
    using ConstIterator_ = STDSet_::const_iterator;
    /** @brief Alias for the reverse iterator type. */
    using ReverseIterator_ = STDSet_::reverse_iterator;
    /** @brief Alias for the const reverse iterator type. */
    using ConstReverseIterator_ = STDSet_::const_reverse_iterator;
    /** @brief Alias for the initializer list type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;
    /** @brief Alias for the return type of insert operations, containing an iterator and a boolean success flag. */
    using ReturnPairType_ = TPair<Iterator_, Bool>;

    /**
     * @brief Default constructor.
     * Constructs an empty set.
     */
    FORCEINLINE TSet() noexcept : SuperType_(), set_() {}

    /**
     * @brief Copy constructor.
     * @param _set The set to copy from.
     */
    FORCEINLINE TSet(const TSet& _set) noexcept : SuperType_(_set), set_(_set.set_) {}

    /**
     * @brief Move constructor.
     * @param _set The set to move from.
     */
    FORCEINLINE TSet(TSet&& _set) noexcept : SuperType_(std::move(_set)), set_(std::move(_set.set_)) {}

    /**
     * @brief Range constructor.
     * Constructs the set with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first Iterator to the first element.
     * @param _last Iterator to the element following the last element.
     */
    template <typename _InputIterator>
    FORCEINLINE TSet(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), set_(_first, _last) {}

    /**
     * @brief Initializer list constructor.
     * Constructs the set with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TSet(InitializerList_ _init_list) noexcept : SuperType_(), set_(_init_list) {}
 
    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TSet() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _set The set to copy from.
     * @return Reference to this set.
     */
    FORCEINLINE TSet& operator=(const TSet& _set) noexcept { 
        SuperType_::operator=(_set);
        set_ = _set.set_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _set The set to move from.
     * @return Reference to this set.
     */
    FORCEINLINE TSet& operator=(TSet&& _set) noexcept { 
        SuperType_::operator=(std::move(_set));
        set_ = std::move(_set.set_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * @param _init_list The initializer list to assign from.
     * @return Reference to this set.
     */
    FORCEINLINE TSet& operator=(InitializerList_ _init_list) noexcept {
        set_ = _init_list;
        return *this;
    }

    /**
     * @brief Checks if the set is empty.
     * @return True if the set is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return set_.empty(); }

    /**
     * @brief Checks if the set is full.
     * @return Always false for standard sets.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Gets the number of elements in the set.
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(set_.size()); }

    /**
     * @brief Gets the capacity of the set.
     * Since set is node-based, this returns the maximum possible size.
     * @return The maximum number of elements the set can hold.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return kSizeTypeMax; }

    /**
     * @brief Equality comparison operator.
     * @param _set The set to compare with.
     * @return True if the sets are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TSet& _set) noexcept { return set_ == _set; }

    /**
     * @brief Inequality comparison operator.
     * @param _set The set to compare with.
     * @return True if the sets are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TSet& _set) noexcept { return set_ != _set; }

    /**
     * @brief Counts elements with a specific value.
     * Since TSet contains unique elements, this returns 1 if found, 0 otherwise.
     * @param _val The value to count.
     * @return The number of elements matching the value.
     */
    NODISCARD FORCEINLINE SizeType Count(const _ObjectType& _val) const noexcept { return set_.count(_val); }

    /**
     * @brief Returns an iterator to the beginning.
     * @return Iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return set_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return Const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return set_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return Const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return set_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning.
     * @return Reverse iterator to the first element of the reversed set.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return set_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return Const reverse iterator to the first element of the reversed set.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return set_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return Const reverse iterator to the first element of the reversed set.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return set_.crbegin(); }

    /**
     * @brief Returns an iterator to the end.
     * @return Iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return set_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return Const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return set_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return Const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return set_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end.
     * @return Reverse iterator to the element following the last element of the reversed set.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return set_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return Const reverse iterator to the element following the last element of the reversed set.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return set_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return Const reverse iterator to the element following the last element of the reversed set.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return set_.crend(); }

    /**
     * @brief Finds an element with a specific value.
     * @param _val The value to search for.
     * @return Iterator to the element if found, otherwise End().
     */
    NODISCARD FORCEINLINE Iterator_ Find(const _ObjectType& _val) noexcept { return set_.find(_val); }

    /**
     * @brief Finds an element with a specific value (const).
     * @param _val The value to search for.
     * @return Const iterator to the element if found, otherwise End().
     */
    NODISCARD FORCEINLINE ConstIterator_ Find(const _ObjectType& _val) const noexcept { return set_.find(_val); }

    /**
     * @brief Returns an iterator to the first element not less than the given value.
     * @param _val The value to compare.
     * @return Iterator to the first element >= _val.
     */
    NODISCARD FORCEINLINE Iterator_ LowerBound(const _ObjectType& _val) noexcept { return set_.lower_bound(_val); }

    /**
     * @brief Returns a const iterator to the first element not less than the given value.
     * @param _val The value to compare.
     * @return Const iterator to the first element >= _val.
     */
    NODISCARD FORCEINLINE ConstIterator_ LowerBound(const _ObjectType& _val) const noexcept {
        return set_.lower_bound(_val);
    }

    /**
     * @brief Returns an iterator to the first element greater than the given value.
     * @param _val The value to compare.
     * @return Iterator to the first element > _val.
     */
    NODISCARD FORCEINLINE Iterator_ UpperBound(const _ObjectType& _val) noexcept { return set_.upper_bound(_val); }

    /**
     * @brief Returns a const iterator to the first element greater than the given value.
     * @param _val The value to compare.
     * @return Const iterator to the first element > _val.
     */
    NODISCARD FORCEINLINE ConstIterator_ UpperBound(const _ObjectType& _val) const noexcept {
        return set_.upper_bound(_val);
    }

    /**
     * @brief Returns a range of elements matching the specific value.
     * @param _val The value to search for.
     * @return A pair of iterators representing the range [lower_bound, upper_bound).
     */
    NODISCARD TPair<Iterator_, Iterator_> EqualRange(const _ObjectType& _val) noexcept {
        return TPair<Iterator_, Iterator_>(set_.equal_range(_val));
    }

    /**
     * @brief Returns a range of elements matching the specific value (const).
     * @param _val The value to search for.
     * @return A pair of const iterators representing the range [lower_bound, upper_bound).
     */
    NODISCARD TPair<ConstIterator_, ConstIterator_> EqualRange(const _ObjectType& _val) const noexcept {
        return TPair<ConstIterator_, ConstIterator_>(set_.equal_range(_val));
    }

    /**
     * @brief Inserts an element into the set.
     * @param _val The value to insert.
     * @return A pair containing an iterator to the element and a boolean indicating if insertion happened.
     */
    FORCEINLINE ReturnPairType_ Insert(const _ObjectType& _val) noexcept {
        return ReturnPairType_(set_.insert(_val));
    }

    /**
     * @brief Inserts an element into the set (move semantics).
     * @param _val The value to insert.
     * @return A pair containing an iterator to the element and a boolean indicating if insertion happened.
     */
    FORCEINLINE ReturnPairType_ Insert(_ObjectType&& _val) noexcept {
        return ReturnPairType_(set_.insert(std::move(_val)));
    }

    /**
     * @brief Inserts an element with a hint.
     * @param _hint_pos Iterator to the position used as a hint.
     * @param _val The value to insert.
     * @return Iterator to the inserted element or the existing element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, const _ObjectType& _val) noexcept {
        return set_.insert(_hint_pos, _val);
    }

    /**
     * @brief Inserts an element with a hint (move semantics).
     * @param _hint_pos Iterator to the position used as a hint.
     * @param _val The value to insert.
     * @return Iterator to the inserted element or the existing element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, _ObjectType&& _val) noexcept {
        return set_.insert(_hint_pos, std::move(_val));
    }

    /**
     * @brief Inserts elements from a range.
     * @tparam _InputIterator The type of the input iterator.
     * @param _first Iterator to the first element.
     * @param _last Iterator to the element following the last element.
     */
    template <typename _InputIterator>
    FORCEINLINE Void Insert(_InputIterator _first, _InputIterator _last) noexcept {
        set_.insert(_first, _last);
    }

    /**
     * @brief Inserts elements from an initializer list.
     * @param _init_list The initializer list to insert from.
     */
    FORCEINLINE Void Insert(InitializerList_ _init_list) noexcept {
        set_.insert(_init_list);
    }

    /**
     * @brief Constructs and inserts an element in-place.
     * @tparam _ArgsType The types of the arguments.
     * @param _args The arguments to construct the element.
     * @return A pair containing an iterator to the element and a boolean indicating if insertion happened.
     */
    template <typename... _ArgsType>
    FORCEINLINE ReturnPairType_ Emplace(_ArgsType&&... _args) noexcept {
        return ReturnPairType_(set_.emplace(std::forward<_ArgsType>(_args)...));
    }

    /**
     * @brief Constructs and inserts an element in-place with a hint.
     * @tparam _ArgsType The types of the arguments.
     * @param _pos Iterator to the position used as a hint.
     * @param _args The arguments to construct the element.
     * @return Iterator to the inserted element or the existing element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ EmplaceHint(ConstIterator_ _pos,_ArgsType&&... _args) noexcept {
        return set_.emplace_hint(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Removes elements with a specific value.
     * @param _value The value to remove.
     * @return The number of elements removed.
     */
    FORCEINLINE SizeType Erase(const _ObjectType& _value) noexcept { return set_.erase(_value); }

    /**
     * @brief Removes an element at a specific position.
     * @param _pos Iterator to the element to remove.
     * @return Iterator to the element following the removed element.
     */
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return set_.erase(_pos); }

    /**
     * @brief Removes elements in a range.
     * @param _first Iterator to the first element to remove.
     * @param _last Iterator to the element following the last element to remove.
     * @return Iterator to the element following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return set_.erase(_first, _last); 
    }

    /**
     * @brief Clears all elements from the set.
     */
    FORCEINLINE Void Clear() noexcept { set_.clear(); }

    /**
     * @brief Swaps the contents with another set.
     * @param _set The set to swap with.
     */
    FORCEINLINE Void Swap(TSet& _set) noexcept { set_.swap(_set.set_); }

private:
    /** @brief The underlying standard set container. */
    STDSet_ set_;
};

/**
 * @brief Multiple Set container.
 * 
 * This class is a wrapper around std::multiset, providing a sorted associative container that can contain duplicate keys.
 * It is typically implemented using a Red-Black Tree (self-balancing binary search tree), ensuring logarithmic
 * time complexity for insertion, deletion, and search operations.
 * 
 * @tparam _ObjectType The type of elements stored in the multiset.
 * @tparam _IsGlobal Specifies whether to use the global memory pool for allocation.
 * @tparam _CompareFunction The comparison function object type used to sort elements.
 */
template<
    typename _ObjectType,
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    typename _CompareFunction = std::less<_ObjectType>
>
class TMultipleSet : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying std::multiset type. */
    using STDSet_ = std::multiset<_ObjectType, _CompareFunction, TAllocator<_ObjectType, _IsGlobal>>;
    /** @brief Alias for the iterator type. */
    using Iterator_ = STDSet_::iterator;
    /** @brief Alias for the const iterator type. */
    using ConstIterator_ = STDSet_::const_iterator;
    /** @brief Alias for the reverse iterator type. */
    using ReverseIterator_ = STDSet_::reverse_iterator;
    /** @brief Alias for the const reverse iterator type. */
    using ConstReverseIterator_ = STDSet_::const_reverse_iterator;
    /** @brief Alias for the initializer list type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     * Constructs an empty multiset.
     */
    FORCEINLINE TMultipleSet() noexcept : SuperType_(), set_() {}

    /**
     * @brief Copy constructor.
     * @param _set The multiset to copy from.
     */
    FORCEINLINE TMultipleSet(const TMultipleSet& _set) noexcept : SuperType_(_set), set_(_set.set_) {}

    /**
     * @brief Move constructor.
     * @param _set The multiset to move from.
     */
    FORCEINLINE TMultipleSet(TMultipleSet&& _set) noexcept 
        : SuperType_(std::move(_set)), set_(std::move(_set.set_)) {}

    /**
     * @brief Range constructor.
     * Constructs the multiset with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first Iterator to the first element.
     * @param _last Iterator to the element following the last element.
     */
    template <typename _InputIterator>
    FORCEINLINE TMultipleSet(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), set_(_first, _last) {}

    /**
     * @brief Initializer list constructor.
     * Constructs the multiset with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TMultipleSet(InitializerList_ _init_list) noexcept : SuperType_(), set_(_init_list) {}
 
    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TMultipleSet() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _set The multiset to copy from.
     * @return Reference to this multiset.
     */
    FORCEINLINE TMultipleSet& operator=(const TMultipleSet& _set) noexcept { 
        SuperType_::operator=(_set);
        set_ = _set.set_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _set The multiset to move from.
     * @return Reference to this multiset.
     */
    FORCEINLINE TMultipleSet& operator=(TMultipleSet&& _set) noexcept { 
        SuperType_::operator=(std::move(_set));
        set_ = std::move(_set.set_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * @param _init_list The initializer list to assign from.
     * @return Reference to this multiset.
     */
    FORCEINLINE TMultipleSet& operator=(InitializerList_ _init_list) noexcept {
        set_ = _init_list;
        return *this;
    }

    /**
     * @brief Equality comparison operator.
     * @param _set The multiset to compare with.
     * @return True if the multisets are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TMultipleSet& _set) noexcept { return set_ == _set; }

    /**
     * @brief Inequality comparison operator.
     * @param _set The multiset to compare with.
     * @return True if the multisets are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TMultipleSet& _set) noexcept { return set_ != _set; }

    /**
     * @brief Gets the number of elements in the multiset.
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(set_.size()); }

    /**
     * @brief Gets the capacity of the multiset.
     * Since multiset is node-based, this returns the maximum possible size.
     * @return The maximum number of elements the multiset can hold.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return kSizeTypeMax; }

    /**
     * @brief Checks if the multiset is empty.
     * @return True if the multiset is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return set_.empty(); }

    /**
     * @brief Counts elements with a specific value.
     * @param _val The value to count.
     * @return The number of elements matching the value.
     */
    NODISCARD FORCEINLINE SizeType Count(const _ObjectType& _val) const noexcept { return set_.count(_val); }

    /**
     * @brief Returns an iterator to the beginning.
     * @return Iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return set_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return Const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return set_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning.
     * @return Const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return set_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning.
     * @return Reverse iterator to the first element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return set_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return Const reverse iterator to the first element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return set_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning.
     * @return Const reverse iterator to the first element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return set_.crbegin(); }

    /**
     * @brief Returns an iterator to the end.
     * @return Iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return set_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return Const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return set_.end(); }

    /**
     * @brief Returns a const iterator to the end.
     * @return Const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return set_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end.
     * @return Reverse iterator to the element following the last element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return set_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return Const reverse iterator to the element following the last element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return set_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end.
     * @return Const reverse iterator to the element following the last element of the reversed multiset.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return set_.crend(); }

    /**
     * @brief Finds an element with a specific value.
     * @param _val The value to search for.
     * @return Iterator to the element if found, otherwise End().
     */
    NODISCARD FORCEINLINE Iterator_ Find(const _ObjectType& _val) noexcept { return set_.find(_val); }

    /**
     * @brief Finds an element with a specific value (const).
     * @param _val The value to search for.
     * @return Const iterator to the element if found, otherwise End().
     */
    NODISCARD FORCEINLINE ConstIterator_ Find(const _ObjectType& _val) const noexcept { return set_.find(_val); }

    /**
     * @brief Returns an iterator to the first element not less than the given value.
     * @param _val The value to compare.
     * @return Iterator to the first element >= _val.
     */
    NODISCARD FORCEINLINE Iterator_ LowerBound(const _ObjectType& _val) noexcept { return set_.lower_bound(_val); }

    /**
     * @brief Returns a const iterator to the first element not less than the given value.
     * @param _val The value to compare.
     * @return Const iterator to the first element >= _val.
     */
    NODISCARD FORCEINLINE ConstIterator_ LowerBound(const _ObjectType& _val) const noexcept {
        return set_.lower_bound(_val);
    }

    /**
     * @brief Returns an iterator to the first element greater than the given value.
     * @param _val The value to compare.
     * @return Iterator to the first element > _val.
     */
    NODISCARD FORCEINLINE Iterator_ UpperBound(const _ObjectType& _val) noexcept { return set_.upper_bound(_val); }

    /**
     * @brief Returns a const iterator to the first element greater than the given value.
     * @param _val The value to compare.
     * @return Const iterator to the first element > _val.
     */
    NODISCARD FORCEINLINE ConstIterator_ UpperBound(const _ObjectType& _val) const noexcept {
        return set_.upper_bound(_val);
    }

    /**
     * @brief Returns a range of elements matching the specific value.
     * @param _val The value to search for.
     * @return A pair of iterators representing the range [lower_bound, upper_bound).
     */
    NODISCARD TPair<Iterator_, Iterator_> EqualRange(const _ObjectType& _val) noexcept {
        return TPair<Iterator_, Iterator_>(set_.equal_range(_val));
    }

    /**
     * @brief Returns a range of elements matching the specific value (const).
     * @param _val The value to search for.
     * @return A pair of const iterators representing the range [lower_bound, upper_bound).
     */
    NODISCARD TPair<ConstIterator_, ConstIterator_> EqualRange(const _ObjectType& _val) const noexcept {
        return TPair<ConstIterator_, ConstIterator_>(set_.equal_range(_val));
    }

    /**
     * @brief Inserts an element into the multiset.
     * @param _val The value to insert.
     * @return Iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(const _ObjectType& _val) noexcept {
        return set_.insert(_val);
    }

    /**
     * @brief Inserts an element into the multiset (move semantics).
     * @param _val The value to insert.
     * @return Iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(_ObjectType&& _val) noexcept {
        return set_.insert(std::move(_val));
    }

    /**
     * @brief Inserts an element with a hint.
     * @param _hint_pos Iterator to the position used as a hint.
     * @param _val The value to insert.
     * @return Iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, const _ObjectType& _val) noexcept {
        return set_.insert(_hint_pos, _val);
    }

    /**
     * @brief Inserts an element with a hint (move semantics).
     * @param _hint_pos Iterator to the position used as a hint.
     * @param _val The value to insert.
     * @return Iterator to the inserted element.
     */
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, _ObjectType&& _val) noexcept {
        return set_.insert(_hint_pos, std::move(_val));
    }

    /**
     * @brief Inserts elements from a range.
     * @tparam _InputIterator The type of the input iterator.
     * @param _first Iterator to the first element.
     * @param _last Iterator to the element following the last element.
     */
    template <typename _InputIterator>
    FORCEINLINE Void Insert(_InputIterator _first, _InputIterator _last) noexcept {
        set_.insert(_first, _last);
    }

    /**
     * @brief Inserts elements from an initializer list.
     * @param _init_list The initializer list to insert from.
     */
    FORCEINLINE Void Insert(InitializerList_ _init_list) noexcept {
        set_.insert(_init_list);
    }

    /**
     * @brief Constructs and inserts an element in-place.
     * @tparam _ArgsType The types of the arguments.
     * @param _args The arguments to construct the element.
     * @return Iterator to the inserted element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(_ArgsType&&... _args) noexcept {
        return set_.emplace(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Constructs and inserts an element in-place with a hint.
     * @tparam _ArgsType The types of the arguments.
     * @param _pos Iterator to the position used as a hint.
     * @param _args The arguments to construct the element.
     * @return Iterator to the inserted element.
     */
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ EmplaceHint(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return set_.emplace_hint(_pos, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Removes elements with a specific value.
     * @param _value The value to remove.
     * @return The number of elements removed.
     */
    FORCEINLINE SizeType Erase(const _ObjectType& _value) noexcept { return set_.erase(_value); }

    /**
     * @brief Removes an element at a specific position.
     * @param _pos Iterator to the element to remove.
     * @return Iterator to the element following the removed element.
     */
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return set_.erase(_pos); }

    /**
     * @brief Removes elements in a range.
     * @param _first Iterator to the first element to remove.
     * @param _last Iterator to the element following the last element to remove.
     * @return Iterator to the element following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return set_.erase(_first, _last); 
    }

    /**
     * @brief Clears all elements from the multiset.
     */
    FORCEINLINE Void Clear() noexcept { set_.clear(); }

    /**
     * @brief Swaps the contents with another multiset.
     * @param _set The multiset to swap with.
     */
    FORCEINLINE Void Swap(TMultipleSet& _set) noexcept { set_.swap(_set.set_); }

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
    /** @brief The underlying standard multiset container. */
    STDSet_ set_;
};

} // zengine