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

#include <unordered_map>

#include "t_allocator.h"
#include "t_pair.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A hash map container that wraps std::unordered_map and inherits from ZObject.
 *
 * This container is implemented using a hash table. It stores elements as key-value pairs, where the key is hashed
 * to determine the storage bucket. This allows for constant-time average complexity for lookups, insertions, and
 * deletions. It integrates with the ZEngine memory management system via TAllocator.
 *
 * @tparam _KeyType The type of the keys.
 * @tparam _ValueType The type of the mapped values.
 * @tparam _IsGlobal Specifies whether to use the global memory pool.
 * @tparam _Hasher The hash function object type.
 * @tparam _KeyEqual The key equality predicate type.
 */
template<
    typename _KeyType, typename _ValueType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    typename _Hasher = std::hash<_KeyType>, typename _KeyEqual = std::equal_to<_KeyType>
>
class THashMap : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Wrapper type for key-value pairs used in this map. */
    using PairType_ = TPair<_KeyType, _ValueType>;
    /** @brief The underlying standard unordered_map type. */
    using STDHashMap_ = std::unordered_map<
        _KeyType, _ValueType, _Hasher, _KeyEqual, TAllocator<std::pair<const _KeyType, _ValueType>, _IsGlobal>
    >;
    /** @brief Iterator type for the map. */
    using Iterator_ = STDHashMap_::iterator;
    /** @brief Return type for insert operations, containing an iterator and a boolean success flag. */
    using ReturnPairType_ = TPair<Iterator_, Bool>;
    /** @brief Constant iterator type for the map. */
    using ConstIterator_ = STDHashMap_::const_iterator;
    /** @brief Initializer list type for constructing the map. */
    using InitializerList_ = std::initializer_list<std::pair<const _KeyType, _ValueType>>;

    /**
     * @brief Default constructor.
     * Initializes an empty hash map.
     */
    FORCEINLINE THashMap() noexcept : SuperType_(), map_() {}

    /**
     * @brief Copy constructor.
     * @param _map The map to copy from.
     */
    FORCEINLINE THashMap(const THashMap& _map) noexcept : SuperType_(_map), map_(_map.map_) {}

    /**
     * @brief Move constructor.
     * @param _map The map to move from.
     */
    FORCEINLINE THashMap(THashMap&& _map) noexcept 
        : SuperType_(std::move(_map)), map_(std::move(_map.map_)) {}

    /**
     * @brief Constructs a hash map with a specified initial bucket count.
     * @param _size The initial number of buckets.
     */
    FORCEINLINE THashMap(SizeType _size) noexcept : SuperType_(), map_(_size) {}

    /**
     * @brief Range constructor.
     * Constructs the map with elements from the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE THashMap(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), map_(_first, _last) {}

    /**
     * @brief Range constructor with initial bucket count.
     * Constructs the map with elements from the range [_first, _last) and a minimum bucket count.
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     * @param _size The initial number of buckets.
     */
    template <typename _InputIterator>
    FORCEINLINE THashMap(_InputIterator _first, _InputIterator _last, SizeType _size) noexcept
        : SuperType_(), map_(_first, _last, _size) {}

    /**
     * @brief Initializer list constructor.
     * Constructs the map with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE THashMap(InitializerList_ _init_list) noexcept : SuperType_(), map_(_init_list) {}
 
    /** @brief Destructor. */
    FORCEINLINE ~THashMap() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _map The map to copy from.
     * @return A reference to this map.
     */
    FORCEINLINE THashMap& operator=(const THashMap& _map) noexcept { 
        SuperType_::operator=(_map);
        map_ = _map.map_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _map The map to move from.
     * @return A reference to this map.
     */
    FORCEINLINE THashMap& operator=(THashMap&& _map) noexcept { 
        SuperType_::operator=(std::move(_map));
        map_ = std::move(_map.map_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * @param _init_list The initializer list to assign from.
     * @return A reference to this map.
     */
    FORCEINLINE THashMap& operator=(InitializerList_ _init_list) noexcept {
        map_ = _init_list;
        return *this;
    }

    /**
     * @brief Checks if the map is empty.
     * @return True if the map is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return map_.empty(); }

    /**
     * @brief Checks if the map is full.
     * @return Always false for this container type.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return false; }

    /**
     * @brief Gets the number of elements in the map.
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(map_.size()); }

    /**
     * @brief Gets the number of buckets in the map.
     * @return The number of buckets.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return map_.bucket_count(); }

    /**
     * @brief Checks if two maps are equal.
     * @param _map The map to compare with.
     * @return True if the maps are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const THashMap& _map) noexcept { return map_ == _map.map_; }

    /**
     * @brief Checks if two maps are not equal.
     * @param _map The map to compare with.
     * @return True if the maps are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const THashMap& _map) noexcept { return map_ != _map.map_; }

    /**
     * @brief Accesses or inserts an element by key.
     * @param _key The key to look up.
     * @return A reference to the value associated with the key.
     */
    NODISCARD FORCEINLINE _ValueType& operator[](const _KeyType& _key) noexcept { return map_[_key]; }

    /**
     * @brief Accesses an element by key with bounds checking.
     * @param _key The key to look up.
     * @return A reference to the value associated with the key.
     */
    NODISCARD FORCEINLINE _ValueType& At(const _KeyType& _key) { return map_.at(_key); }

    /**
     * @brief Accesses an element by key with bounds checking (const).
     * @param _key The key to look up.
     * @return A const reference to the value associated with the key.
     */
    NODISCARD FORCEINLINE const _ValueType& At(const _KeyType& _key) const { return map_.at(_key); }

    /**
     * @brief Gets the current load factor.
     * @return The load factor (size / bucket_count).
     */
    NODISCARD FORCEINLINE Float32 LoadFactor() const noexcept { return map_.load_factor(); }

    /**
     * @brief Gets the maximum load factor.
     * @return The maximum load factor.
     */
    NODISCARD FORCEINLINE Float32 MaxLoadFactor() const noexcept { return map_.max_load_factor(); }

    /**
     * @brief Sets the maximum load factor.
     * @param _factor The new maximum load factor.
     */
    NODISCARD FORCEINLINE Void SetMaxLoadFactor(Float32 _factor) noexcept { map_.max_load_factor(_factor); }

    /**
     * @brief Gets an iterator to the beginning of the map.
     * @return An iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return map_.begin(); }

    /**
     * @brief Gets a const iterator to the beginning of the map.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return map_.begin(); }

    /**
     * @brief Gets a const iterator to the beginning of the map.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return map_.cbegin(); }

    /**
     * @brief Gets an iterator to the end of the map.
     * @return An iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return map_.end(); }

    /**
     * @brief Gets a const iterator to the end of the map.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return map_.end(); }

    /**
     * @brief Gets a const iterator to the end of the map.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return map_.cend(); }

    /**
     * @brief Inserts an element into the map (copy).
     * @param _val The pair to insert.
     * @return A pair containing an iterator to the inserted element and a boolean indicating success.
     */
    FORCEINLINE ReturnPairType_ Insert(const PairType_& _val) noexcept {
        return map_.insert(_val.STDPair());
    }

    /**
     * @brief Inserts an element into the map (move).
     * @param _val The pair to insert.
     * @return A pair containing an iterator to the inserted element and a boolean indicating success.
     */
    FORCEINLINE ReturnPairType_ Insert(PairType_&& _val) noexcept {
        return map_.insert(std::move(_val.STDPair()));
    }

    /**
     * @brief Inserts a range of elements into the map.
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE Void Insert(_InputIterator _first, _InputIterator _last) noexcept {
        map_.insert(_first, _last);
    }

    /**
     * @brief Inserts elements from an initializer list.
     * @param _init_list The initializer list to insert from.
     */
    FORCEINLINE Void Insert(InitializerList_ _init_list) noexcept {
        return map_.insert(_init_list);
    }

    /**
     * @brief Inserts an element or assigns to the current element if the key already exists (copy).
     * @param _key The key of the element.
     * @param _val The value of the element.
     * @return A pair containing an iterator to the element and a boolean indicating if insertion took place.
     */
    FORCEINLINE ReturnPairType_ InsertOrAssign(const _KeyType& _key, const _ValueType& _val) noexcept {
        return map_.insert_or_assign(_key, _val);
    }

    /**
     * @brief Inserts an element or assigns to the current element if the key already exists (move).
     * @param _key The key of the element.
     * @param _val The value of the element.
     * @return A pair containing an iterator to the element and a boolean indicating if insertion took place.
     */
    FORCEINLINE ReturnPairType_ InsertOrAssign(_KeyType&& _key, _ValueType&& _val) noexcept {
        return map_.insert_or_assign(std::forward<_KeyType>(_key), std::move(_val));
    }

    /**
     * @brief Constructs and inserts an element into the map (copy key).
     * @tparam _ArgsType The types of the arguments to construct the value.
     * @param _key The key of the element.
     * @param _args The arguments to construct the value.
     * @return A pair containing an iterator to the inserted element and a boolean indicating success.
     */
    template <typename... _ArgsType>
    FORCEINLINE ReturnPairType_ Emplace(const _KeyType& _key, _ArgsType&&... _args) noexcept {
        return map_.emplace(_key, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Constructs and inserts an element into the map (move key).
     * @tparam _ArgsType The types of the arguments to construct the value.
     * @param _key The key of the element.
     * @param _args The arguments to construct the value.
     * @return A pair containing an iterator to the inserted element and a boolean indicating success.
     */
    template <typename... _ArgsType>
    FORCEINLINE ReturnPairType_ Emplace(_KeyType&& _key, _ArgsType&&... _args) noexcept {
        return map_.emplace(std::forward<_KeyType>(_key), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Inserts an element if the key does not exist (copy key).
     * @tparam _ArgsType The types of the arguments to construct the value.
     * @param _key The key of the element.
     * @param _args The arguments to construct the value.
     * @return A pair containing an iterator to the element and a boolean indicating success.
     */
    template <typename... _ArgsType>
    FORCEINLINE ReturnPairType_ TryEmplace(const _KeyType& _key, _ArgsType&&... _args) noexcept {
        return map_.try_emplace(_key, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Inserts an element if the key does not exist (move key).
     * @tparam _ArgsType The types of the arguments to construct the value.
     * @param _key The key of the element.
     * @param _args The arguments to construct the value.
     * @return A pair containing an iterator to the element and a boolean indicating success.
     */
    template <typename... _ArgsType>
    FORCEINLINE ReturnPairType_ TryEmplace(_KeyType&& _key, _ArgsType&&... _args) noexcept {
        return map_.try_emplace(std::forward<_KeyType>(_key), std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Removes an element by key.
     * @param _key The key of the element to remove.
     */
    FORCEINLINE Void Erase(const _KeyType& _key) noexcept { map_.erase(_key); }

    /**
     * @brief Removes an element at a specified position.
     * @param _pos The iterator to the element to remove.
     * @return An iterator following the removed element.
     */
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return map_.erase(_pos); }

    /**
     * @brief Removes an element at a specified position (const iterator).
     * @param _pos The const iterator to the element to remove.
     * @return An iterator following the removed element.
     */
    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return map_.erase(_pos); }

    /**
     * @brief Removes elements in a range.
     * @param _first The beginning of the range to remove.
     * @param _last The end of the range to remove.
     * @return An iterator following the last removed element.
     */
    FORCEINLINE Iterator_ Erase(Iterator_ _first, Iterator_ _last) noexcept {
        return map_.erase(_first, _last);
    }

    /**
     * @brief Finds an element by key.
     * @param _key The key to find.
     * @return A const iterator to the found element, or End() if not found.
     */
    FORCEINLINE ConstIterator_ Find(const _KeyType& _key) const noexcept {
        return map_.find(_key);
    }

    /**
     * @brief Checks if the map contains a key.
     * @param _key The key to check.
     * @return True if the key exists, false otherwise.
     */
    FORCEINLINE Bool Contains(const _KeyType& _key) const noexcept {
        return map_.contains(_key);
    }

    /**
     * @brief Sets the number of buckets to at least the specified value.
     * @param _capacity The minimum number of buckets.
     */
    FORCEINLINE Void Rehash(SizeType _capacity) noexcept { map_.rehash(_capacity); }

    /**
     * @brief Reserves space for at least the specified number of elements.
     * @param _capacity The number of elements to reserve space for.
     */
    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { map_.reserve(_capacity); }

    /** @brief Clears all elements from the map. */
    FORCEINLINE Void Clear() noexcept { map_.clear(); }
    
    /**
     * @brief Swaps the contents of this map with another map.
     * @param _map The map to swap with.
     */
    FORCEINLINE Void Swap(THashMap& _map) noexcept { map_.swap(_map.map_); }

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
    /** @brief The underlying standard hash map storage. */
    STDHashMap_ map_;
};

} // zengine