/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include <unordered_map>

#include "t_allocator.h"
#include "z_object.h"
#include "t_pair.h"

namespace zengine {

/*
    Unoerdered map, hash container.
*/
template<typename _KeyType, typename _ValueType>
class TUnorderedMap : public ZObject {
public:
    using ObjectType_ = std::pair<const _KeyType, _ValueType>;
    using STDUnorderedMap_ = std::unordered_map<
        _KeyType, _ValueType, std::hash<_KeyType>, std::equal_to<_KeyType>, 
        TContainerAllocator<ObjectType_>>;
    using Iterator_ = STDUnorderedMap_::iterator;
    using ConstIterator_ = STDUnorderedMap_::const_iterator;
    using InitializerList_ = std::initializer_list<std::pair<const _KeyType, _ValueType>>;

    FORCEINLINE TUnorderedMap() noexcept : SuperType_(), map_() {}
    FORCEINLINE TUnorderedMap(const TUnorderedMap& _map) noexcept : SuperType_(_map), map_(_map.map_) {}
    FORCEINLINE TUnorderedMap(TUnorderedMap&& _map) noexcept 
        : SuperType_(std::forward<TUnorderedMap>(_map)), map_(std::move(_map.map_)) {}

    FORCEINLINE TUnorderedMap(SizeType _size) noexcept : SuperType_(), map_(_size) {}
    template <typename _InputIterator>
    FORCEINLINE TUnorderedMap(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), map_(_first, _last) {}
    template <typename _InputIterator>
    FORCEINLINE TUnorderedMap(_InputIterator _first, _InputIterator _last, SizeType _size) noexcept
        : SuperType_(), map_(_first, _last, _size) {}
    FORCEINLINE TUnorderedMap(InitializerList_ _init_list) noexcept : SuperType_(), map_(_init_list) {}
 
    FORCEINLINE ~TUnorderedMap() noexcept {}

    FORCEINLINE TUnorderedMap& operator=(const TUnorderedMap& _map) noexcept { 
        SuperType_::operator=(_map);
        map_ = _map.map_;
        return *this;
    }
    FORCEINLINE TUnorderedMap& operator=(TUnorderedMap&& _map) noexcept { 
        SuperType_::operator=(std::forward<TUnorderedMap>(_map));
        map_ = std::move(_map.map_);
        return *this;
    }
    FORCEINLINE TUnorderedMap& operator=(InitializerList_ _init_list) noexcept {
        map_ = _init_list;
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TUnorderedMap& _map) noexcept { return map_ == _map.map_; }
    NODISCARD FORCEINLINE Bool operator!=(const TUnorderedMap& _map) noexcept { return map_ != _map.map_; }

    NODISCARD FORCEINLINE _ValueType& operator[](const _KeyType& _key) noexcept { return map_[_key]; }
    NODISCARD FORCEINLINE const _ValueType& operator[](const _KeyType& _key) const noexcept { return map_[_key]; }

    NODISCARD FORCEINLINE _ValueType& At(const _KeyType& _key) noexcept { return map_.at(_key); }
    NODISCARD FORCEINLINE const _ValueType& At(const _KeyType& _key) const noexcept { return map_.at(_key); }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return static_cast<SizeType>(map_.size()); }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { return map_.bucket_count(); }
    NODISCARD FORCEINLINE Float32 LoadFactor() const noexcept { return map_.load_factor(); }
    NODISCARD FORCEINLINE Float32 MaxLoadFactor() const noexcept { return map_.max_load_factor(); }
    NODISCARD FORCEINLINE Void SetMaxLoadFactor(Float32 _factor) noexcept { map_.max_load_factor(_factor); }

    NODISCARD FORCEINLINE Bool Empty() const noexcept { return map_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return map_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return map_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return map_.cbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return map_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return map_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return map_.cend(); }

    FORCEINLINE Iterator_ Insert(const ObjectType_& _val) noexcept { return map_.insert(_val).first; }
    FORCEINLINE Iterator_ Insert(ObjectType_&& _val) noexcept { 
        return map_.insert(std::forward<ObjectType_>(_val)).first; 
    }
    template <typename _InputIterator>
    FORCEINLINE Void Insert(_InputIterator _first, _InputIterator _last) noexcept {
        map_.insert(_first, _last);
    }
    FORCEINLINE Void Insert(InitializerList_ _init_list) noexcept {
        return map_.insert(_init_list);
    }

    FORCEINLINE Iterator_ InsertOrAssign(const _KeyType& _key, const _ValueType& _val) noexcept { 
        return map_.insert_or_assign(_key, _val).first;
    }
    FORCEINLINE Iterator_ InsertOrAssign(_KeyType&& _key, _ValueType&& _val) noexcept {
        return map_.insert_or_assign(std::forward<_KeyType>(_key), std::forward<_ValueType>(_val)).first;
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(const _KeyType& _key, _ArgsType&&... _args) noexcept {
        return map_.emplace(_key, std::forward<_ArgsType>(_args)...).first;
    }
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(_KeyType&& _key, _ArgsType&&... _args) noexcept {
        return map_.emplace(std::forward<_KeyType>(_key), std::forward<_ArgsType>(_args)...).first;
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ TryEmplace(const _KeyType& _key, _ArgsType&&... _args) noexcept {
        return map_.try_emplace(_key, std::forward<_ArgsType>(_args)...).first;
    }
    template <typename... _ArgsType>
    FORCEINLINE Iterator_ TryEmplace(_KeyType&& _key, _ArgsType&&... _args) noexcept {
        return map_.try_emplace(std::forward<_KeyType>(_key), std::forward<_ArgsType>(_args)...).first;
    }

    FORCEINLINE Void Erase(const _KeyType& _key) noexcept { map_.erase(_key); }
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return map_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return map_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(Iterator_ _first, Iterator_ _last) noexcept {
        return map_.erase(_first, _last);
    }

    FORCEINLINE ConstIterator_ Find(const _KeyType& _key) const noexcept {
        return map_.find(_key);
    }

    FORCEINLINE Bool Contains(const _KeyType& _key) const noexcept {
        return map_.contains(_key);
    }

    FORCEINLINE Void Rehash(SizeType _capacity) noexcept { map_.rehash(_capacity); }
    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { map_.reserve(_capacity); }

    FORCEINLINE Void Clear() noexcept { map_.clear(); }
    
    FORCEINLINE Void Swap(TUnorderedMap& _map) noexcept { map_.swap(_map.map_); }

protected:
    using SuperType_ = ZObject;

private:
    STDUnorderedMap_ map_;
};

}//zengine