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
#ifndef Z_CORE_T_SET_H_
#define Z_CORE_T_SET_H_

#include "internal/z_drive.h"

#include <set>

#include "t_allocator.h"
#include "t_pair.h"
#include "z_object.h"

namespace zengine {

/*
    Set caintainer.
*/
template<typename _ObjectType, typename _CompareFunction = std::less<_ObjectType>>
class TSet : public ZObject {
public:
    using STDSet_ = std::set<_ObjectType, _CompareFunction, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDSet_::iterator;
    using ConstIterator_ = STDSet_::const_iterator;
    using ReverseIterator_ = STDSet_::reverse_iterator;
    using ConstReverseIterator_ = STDSet_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TSet() noexcept : SuperType_(), set_() {}
    FORCEINLINE TSet(const TSet& _set) noexcept : SuperType_(_set), set_(_set.set_) {}
    FORCEINLINE TSet(TSet&& _set) noexcept : SuperType_(std::forward<TSet>(_set)), set_(std::move(_set.set_)) {}

    template <typename _InputIterator>
    FORCEINLINE TSet(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), set_(_first, _last) {}
    FORCEINLINE TSet(InitializerList_ _init_list) noexcept : SuperType_(), set_(_init_list) {}
 
    FORCEINLINE ~TSet() noexcept {}

    FORCEINLINE TSet& operator=(const TSet& _set) noexcept { 
        SuperType_::operator=(_set);
        set_ = _set.set_;
        return *this;
    }
    FORCEINLINE TSet& operator=(TSet&& _set) noexcept { 
        SuperType_::operator=(std::forward<TSet>(_set));
        set_ = std::move(_set.set_);
        return *this;
    }
    FORCEINLINE TSet& operator=(InitializerList_ _init_list) noexcept {
        set_ = _init_list;
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TSet& _set) noexcept { return set_ == _set; }
    NODISCARD FORCEINLINE Bool operator!=(const TSet& _set) noexcept { return set_ != _set; }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return set_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return set_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return set_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return set_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(set_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return set_.empty(); }
    NODISCARD FORCEINLINE IndexType Count(const _ObjectType& _val) const noexcept { return set_.count(_val); }


    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return set_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return set_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return set_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return set_.crend(); }

    NODISCARD FORCEINLINE Iterator_ Find(const _ObjectType& _val) noexcept { return set_.find(_val); }
    NODISCARD FORCEINLINE ConstIterator_ Find(const _ObjectType& _val) const noexcept { return set_.find(_val); }
    NODISCARD FORCEINLINE Iterator_ LowerBound(const _ObjectType& _val) noexcept { return set_.lower_bound(_val); }
    NODISCARD FORCEINLINE ConstIterator_ LowerBound(const _ObjectType& _val) const noexcept {
        return set_.lower_bound(_val);
    }
    NODISCARD FORCEINLINE Iterator_ UpperBound(const _ObjectType& _val) noexcept { return set_.upper_bound(_val); }
    NODISCARD FORCEINLINE ConstIterator_ UpperBound(const _ObjectType& _val) const noexcept {
        return set_.upper_bound(_val);
    }
    NODISCARD TPair<Iterator_, Iterator_> EqualRange(const _ObjectType& _val) noexcept {
        auto temp_pair = set_.equal_range(_val);
        return TPair<Iterator_, Iterator_>(temp_pair.first, temp_pair.second);
    }
    NODISCARD TPair<ConstIterator_, ConstIterator_> EqualRange(const _ObjectType& _val) const noexcept {
        auto temp_pair = set_.equal_range(_val);
        return TPair<ConstIterator_, ConstIterator_>(temp_pair.first, temp_pair.second);
    }

    FORCEINLINE TPair<Iterator_, Bool> Insert(const _ObjectType& _val) noexcept {
        auto temp_pair = set_.insert(_val);
        return TPair<Iterator_, Bool>(temp_pair.first, temp_pair.second);
    }
    FORCEINLINE TPair<Iterator_, Bool> Insert(_ObjectType&& _val) noexcept {
        auto temp_pair = set_.insert(std::forward<_ObjectType>(_val));
        return TPair<Iterator_, Bool>(temp_pair.first, temp_pair.second);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, const _ObjectType& _val) noexcept {
        return set_.insert(_hint_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, _ObjectType&& _val) noexcept {
        return set_.insert(_hint_pos, std::forward<_ObjectType>(_val));
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return set_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return set_.insert(_pos, _init_list);
    }

    template <typename... _ArgsType>
    FORCEINLINE TPair<Iterator_, Bool> Emplace(_ArgsType&&... _args) noexcept {
        auto temp_pair = set_.emplace(std::forward<_ArgsType>(_args)...);
        return TPair<Iterator_, Bool>(temp_pair.first, temp_pair.second);
    }

    template <typename... _ArgsType>
    FORCEINLINE TPair<Iterator_, Bool> EmplaceHint(ConstIterator_ _pos,_ArgsType&&... _args) noexcept {
        auto temp_pair = set_.emplace_hint(_pos, std::forward<_ArgsType>(_args)...);
        return TPair<Iterator_, Bool>(temp_pair.first, temp_pair.second);
    }

    FORCEINLINE Bool Erase(const _ObjectType& _value) noexcept { return set_.erase(_value); }
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return set_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return set_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { set_.clear(); }

    FORCEINLINE Void Swap(TSet& _set) noexcept { set_.swap(_set); }

protected:
    using SuperType_ = ZObject;

private:
    STDSet_ set_;
};

/*
    Multipul Set caintainer.
*/
template<typename _ObjectType, typename _CompareFunction = std::less<_ObjectType>>
class TMultiset : public ZObject {
public:
    using STDSet_ = std::multiset<_ObjectType, _CompareFunction, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDSet_::iterator;
    using ConstIterator_ = STDSet_::const_iterator;
    using ReverseIterator_ = STDSet_::reverse_iterator;
    using ConstReverseIterator_ = STDSet_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TMultiset() noexcept : SuperType_(), set_() {}
    FORCEINLINE TMultiset(const TMultiset& _set) noexcept : SuperType_(_set), set_(_set.set_) {}
    FORCEINLINE TMultiset(TMultiset&& _set) noexcept 
        : SuperType_(std::forward<TMultiset>(_set)), set_(std::move(_set.set_)) {}

    template <typename _InputIterator>
    FORCEINLINE TMultiset(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), set_(_first, _last) {}
    FORCEINLINE TMultiset(InitializerList_ _init_list) noexcept : SuperType_(), set_(_init_list) {}
 
    FORCEINLINE ~TMultiset() noexcept {}

    FORCEINLINE TMultiset& operator=(const TMultiset& _set) noexcept { 
        SuperType_::operator=(_set);
        set_ = _set.set_;
        return *this;
    }
    FORCEINLINE TMultiset& operator=(TMultiset&& _set) noexcept { 
        SuperType_::operator=(std::forward<TMultiset>(_set));
        set_ = std::move(_set.set_);
        return *this;
    }
    FORCEINLINE TMultiset& operator=(InitializerList_ _init_list) noexcept {
        set_ = _init_list;
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TMultiset& _set) noexcept { return set_ == _set; }
    NODISCARD FORCEINLINE Bool operator!=(const TMultiset& _set) noexcept { return set_ != _set; }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return set_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return set_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return set_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return set_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(set_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return set_.empty(); }
    NODISCARD FORCEINLINE IndexType Count(const _ObjectType& _val) const noexcept { return set_.count(_val); }


    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return set_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return set_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return set_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return set_.crend(); }

    NODISCARD FORCEINLINE Iterator_ Find(const _ObjectType& _val) noexcept { return set_.find(_val); }
    NODISCARD FORCEINLINE ConstIterator_ Find(const _ObjectType& _val) const noexcept { return set_.find(_val); }
    NODISCARD FORCEINLINE Iterator_ LowerBound(const _ObjectType& _val) noexcept { return set_.lower_bound(_val); }
    NODISCARD FORCEINLINE ConstIterator_ LowerBound(const _ObjectType& _val) const noexcept {
        return set_.lower_bound(_val);
    }
    NODISCARD FORCEINLINE Iterator_ UpperBound(const _ObjectType& _val) noexcept { return set_.upper_bound(_val); }
    NODISCARD FORCEINLINE ConstIterator_ UpperBound(const _ObjectType& _val) const noexcept {
        return set_.upper_bound(_val);
    }
    NODISCARD TPair<Iterator_, Iterator_> EqualRange(const _ObjectType& _val) noexcept {
        auto temp_pair = set_.equal_range(_val);
        return TPair<Iterator_, Iterator_>(temp_pair.first, temp_pair.second);
    }
    NODISCARD TPair<ConstIterator_, ConstIterator_> EqualRange(const _ObjectType& _val) const noexcept {
        auto temp_pair = set_.equal_range(_val);
        return TPair<ConstIterator_, ConstIterator_>(temp_pair.first, temp_pair.second);
    }

    FORCEINLINE Iterator_ Insert(const _ObjectType& _val) noexcept {
        return set_.insert(_val);
    }
    FORCEINLINE Iterator_ Insert(_ObjectType&& _val) noexcept {
        return set_.insert(std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, const _ObjectType& _val) noexcept {
        return set_.insert(_hint_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _hint_pos, _ObjectType&& _val) noexcept {
        return set_.insert(_hint_pos, std::forward<_ObjectType>(_val));
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return set_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return set_.insert(_pos, _init_list);
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(_ArgsType&&... _args) noexcept {
        return set_.emplace(std::forward<_ArgsType>(_args)...);
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ EmplaceHint(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return set_.emplace_hint(_pos, std::forward<_ArgsType>(_args)...);
    }

    FORCEINLINE Bool Erase(const _ObjectType& _value) noexcept { return set_.erase(_value); }
    FORCEINLINE Iterator_ Erase(Iterator_ _pos) noexcept { return set_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return set_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { set_.clear(); }

    FORCEINLINE Void Swap(TMultiset& _set) noexcept { set_.swap(_set); }

protected:
    using SuperType_ = ZObject;

private:
    STDSet_ set_;
};

}//zengine

#endif // !Z_CORE_T_SET_H_