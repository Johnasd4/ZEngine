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

#include "internal/z_drive.h"

#include <forward_list>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Forward List caintainer.
*/
template<typename _ObjectType>
class TForwardList : public ZObject {
public:
    using STDForwardList_ = std::forward_list<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDForwardList_::iterator;
    using ConstIterator_ = STDForwardList_::const_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TForwardList() noexcept : SuperType_(), forward_list_() {}
    FORCEINLINE TForwardList(const TForwardList& _forward_list) noexcept 
        : SuperType_(_forward_list), forward_list_(_forward_list.forward_list_) {}
    FORCEINLINE TForwardList(TForwardList&& _forward_list) noexcept 
        : SuperType_(std::forward<TForwardList>(_forward_list)), forward_list_(std::move(_forward_list.forward_list_)) {}

    FORCEINLINE TForwardList(SizeType _size) noexcept : SuperType_(), forward_list_(_size) {}
    FORCEINLINE TForwardList(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_(), forward_list_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TForwardList(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), forward_list_(_first, _last) {}
    FORCEINLINE TForwardList(InitializerList_ _init_list) noexcept : SuperType_(), forward_list_(_init_list) {}
 
    FORCEINLINE ~TForwardList() noexcept {}

    FORCEINLINE TForwardList& operator=(const TForwardList& _forward_list) noexcept { 
        forward_list_ = _forward_list.forward_list_;
        return *this;
    }
    FORCEINLINE TForwardList& operator=(TForwardList&& _forward_list) noexcept { 
        forward_list_ = std::move(_forward_list.forward_list_);
        return *this;
    }
    FORCEINLINE TForwardList& operator=(InitializerList_ _init_list) noexcept {
        forward_list_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return forward_list_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return forward_list_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return forward_list_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TForwardList& _forward_list) noexcept { 
        return forward_list_ == _forward_list.forward_list_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TForwardList& _forward_list) noexcept { 
        return forward_list_ != _forward_list.forward_list_;
    }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return forward_list_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return forward_list_.front(); }

    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return forward_list_.empty(); }

    NODISCARD FORCEINLINE Iterator_ BeforeBegin() noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE ConstIterator_ BeforeBegin() const noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBeforeBegin() const noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return forward_list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return forward_list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return forward_list_.cbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return forward_list_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return forward_list_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return forward_list_.cend(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, ArgsType&&... _args) noexcept {
        return forward_list_.emplace(_pos, std::forward<ArgsType>(_args)...);
    }

    template <typename... ArgsType>
    FORCEINLINE Iterator_ EmplaceFront(ArgsType&&... _args) noexcept {
        return forward_list_.emplace_front(std::forward<ArgsType>(_args)...);
    }
    FORCEINLINE Void PushFront(const _ObjectType& _val) noexcept { forward_list_.push_front(_val); }
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { 
        forward_list_.push_front(std::forward<_ObjectType>(_val)); 
    }
    FORCEINLINE Void PopFront() noexcept { forward_list_.pop_front(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ EmplaceAfter(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return forward_list_.emplace_after(_pos, std::forward<_ArgsType>(_args)...);
    }

    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, const _ObjectType& _val) noexcept {
        return forward_list_.insert_after(_pos, _val);
    }
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return forward_list_.insert_after(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return forward_list_.insert_after(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return forward_list_.insert_after(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ InsertAfter(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return forward_list_.insert_after(_pos, _init_list);
    }

    FORCEINLINE Iterator_ SpliceAfter(ConstIterator_ _pos, TForwardList& _forward_list) noexcept {
        return forward_list_.splice_after(_pos, _forward_list.forward_list_);
    }
    FORCEINLINE Iterator_ SpliceAfter(ConstIterator_ _pos, TForwardList&& _forward_list) noexcept {
        return forward_list_.splice_after(_pos, std::forward<_ObjectType>(_forward_list.forward_list_));
    }
    FORCEINLINE Iterator_ SpliceAfter(
        ConstIterator_ _pos, TForwardList& _forward_list, ConstIterator_ _start_pos
    ) noexcept {
        return forward_list_.splice_after(_pos, _forward_list.forward_list_, _start_pos);
    }
    FORCEINLINE Iterator_ SpliceAfter(
        ConstIterator_ _pos, TForwardList&& _forward_list, ConstIterator_ _start_pos
    ) noexcept {
        return forward_list_.splice_after(_pos, std::forward<_ObjectType>(_forward_list.forward_list_), _start_pos);
    }
    FORCEINLINE Iterator_ SpliceAfter(
        ConstIterator_ _pos, TForwardList& _forward_list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        return forward_list_.splice_after(_pos, _forward_list.forward_list_, _first, _last);
    }
    FORCEINLINE Iterator_ SpliceAfter(
        ConstIterator_ _pos, TForwardList&& _forward_list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        return forward_list_.splice_after(_pos, std::forward<_ObjectType>(_forward_list.forward_list_), _first, _last);
    }

    FORCEINLINE Iterator_ EraseAfter(ConstIterator_ _pos) noexcept { return forward_list_.erase_after(_pos); }
    FORCEINLINE Iterator_ EraseAfter(ConstIterator_ _first, ConstIterator_ _last) noexcept {
        return forward_list_.erase_after(_first, _last);
    }

    /*
        Remove all the objects same as the given value.
    */
    FORCEINLINE Void Remove(const _ObjectType& _val) noexcept {
        forward_list_.remove(_val);
    }
    /*
        Remove all the objects that comply with the fucntion.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void RemoveIf(_PredicateFunction _func) noexcept {
        forward_list_.remove_if(_func);
    }
    /*
        Make all the objects unique.
    */
    FORCEINLINE Void Unique() noexcept {
        forward_list_.unique();
    }
    /*
        Make all the objects unique that comply with the fucntion.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void UniqueIf(_PredicateFunction _func) noexcept {
        forward_list_.unique(_func);
    }

    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TForwardList& _forward_list) {
        forward_list_.unique(_forward_list.forward_list_);
    }
    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TForwardList&& _forward_list) {
        forward_list_.unique(std::move(_forward_list.forward_list_));
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TForwardList& _forward_list, _PredicateFunction _func) {
        forward_list_.unique(_forward_list.forward_list_, _func);
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TForwardList&& _forward_list, _PredicateFunction _func) {
        forward_list_.unique(std::move(_forward_list.forward_list_), _func);
    }

    FORCEINLINE Void Sort() noexcept {
        forward_list_.sort();
    }
    template <typename _PredicateFunction>
    FORCEINLINE Void Sort(_PredicateFunction _func) noexcept {
        forward_list_.sort(_func);
    }

    FORCEINLINE Void Clear() noexcept { forward_list_.clear(); }

    FORCEINLINE Void Resize(SizeType _size) noexcept { forward_list_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { forward_list_.resize(_size, _val); }

    FORCEINLINE Void Reverse() noexcept { forward_list_.reverse(); }

    FORCEINLINE Void Swap(TForwardList& _forward_list) noexcept { forward_list_.swap(_forward_list.forward_list_); }

protected:
    using SuperType_ = ZObject;

private:
    STDForwardList_ forward_list_;
};

}//zengine