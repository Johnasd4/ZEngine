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

#include <list>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    List container.
*/
template<typename _ObjectType>
class TList : public ZObject {
public:
    using STDList_ = std::list<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDList_::iterator;
    using ConstIterator_ = STDList_::const_iterator;
    using ReverseIterator_ = STDList_::reverse_iterator;
    using ConstReverseIterator_ = STDList_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TList() noexcept : SuperType_(), list_() {}
    FORCEINLINE TList(const TList& _list) noexcept : SuperType_(_list), list_(_list.list_) {}
    FORCEINLINE TList(TList&& _list) noexcept : SuperType_(std::forward<TList>(_list)), list_(std::move(_list.list_)) {}

    FORCEINLINE TList(SizeType _size) noexcept : SuperType_(), list_(_size) {}
    FORCEINLINE TList(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), list_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TList(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), list_(_first, _last) {}
    FORCEINLINE TList(InitializerList_ _init_list) noexcept : SuperType_(), list_(_init_list) {}
 
    FORCEINLINE ~TList() noexcept {}

    FORCEINLINE TList& operator=(const TList& _list) noexcept { 
        SuperType_::operator=(_list);
        list_ = _list.list_;
        return *this;
    }
    FORCEINLINE TList& operator=(TList&& _list) noexcept { 
        SuperType_::operator=(std::forward<TList>(_list));
        list_ = std::move(_list.list_);
        return *this;
    }
    FORCEINLINE TList& operator=(InitializerList_ _init_list) noexcept {
        list_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return list_.assign(_size, _val);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator _first, InputIterator _last) noexcept {
        return list_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return list_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TList& _list) noexcept { 
        return list_ == _list.list_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TList& _list) noexcept { 
        return list_ != _list.list_;
    }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return list_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return list_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return list_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return list_.back(); }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return static_cast<SizeType>(list_.size()); }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { return kSizeTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return list_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return list_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return list_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return list_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return list_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return list_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return list_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return list_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return list_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return list_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return list_.crend(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return list_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }

    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceFront(_ArgsType&&... _args) noexcept {
        return list_.emplace_front(std::forward<_ArgsType>(_args)...);
    }
    FORCEINLINE Void PushFront(const _ObjectType& _val) noexcept { list_.push_front(_val); }
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { list_.push_front(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopFront() noexcept { list_.pop_front(); }

    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return list_.emplace_back(std::forward<_ArgsType>(_args)...);
    }
    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { list_.push_back(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { list_.push_back(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopBack() noexcept { list_.pop_back(); }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept {
        return list_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return list_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return list_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return list_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return list_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Splice(ConstIterator_ _pos, TList& _list) noexcept {
        return list_.splice(_pos, _list.list_);
    }
    FORCEINLINE Iterator_ Splice(ConstIterator_ _pos, TList&& _list) noexcept {
        return list_.splice(_pos, std::move(_list.list_));
    }
    FORCEINLINE Iterator_ Splice(ConstIterator_ _pos, TList& _list, ConstIterator_ _start_pos) noexcept {
        return list_.splice(_pos, _list.list_, _start_pos);
    }
    FORCEINLINE Iterator_ Splice(ConstIterator_ _pos, TList&& _list, ConstIterator_ _start_pos) noexcept {
        return list_.splice(_pos, std::move(_list.list_), _start_pos);
    }
    FORCEINLINE Iterator_ Splice(
        ConstIterator_ _pos, TList& _list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        return list_.splice(_pos, _list.list_, _first, _last);
    }
    FORCEINLINE Iterator_ Splice(
        ConstIterator_ _pos, TList&& _list, ConstIterator_ _first, ConstIterator_ _last
    ) noexcept {
        return list_.splice(_pos, std::move(_list.list_), _first, _last);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return list_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept {
        return list_.erase(_first, _last);
    }

    /*
        Remove all the objects same as the given value.
    */
    FORCEINLINE Void Remove(const _ObjectType& _val) noexcept {
        list_.remove(_val);
    }
    /*
        Remove all the objects that comply with the fucntion.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void RemoveIf(_PredicateFunction _func) noexcept {
        list_.remove_if(_func);
    }
    /*
        Make all the objects unique.
    */
    FORCEINLINE Void Unique() noexcept {
        list_.unique();
    }
    /*
        Make all the objects unique that comply with the fucntion.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void UniqueIf(_PredicateFunction _func) noexcept {
        list_.unique(_func);
    }

    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TList& _list) {
        list_.unique(_list);
    }
    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TList&& _list) {
        list_.unique(std::move(_list.list_));
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TList& _list, _PredicateFunction _func) {
        list_.unique(_list, _func);
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename _PredicateFunction>
    FORCEINLINE Void Merge(TList&& _list, _PredicateFunction _func) {
        list_.unique(std::move(_list.list_), _func);
    }

    FORCEINLINE Void Sort() noexcept {
        list_.sort();
    }
    template <typename _PredicateFunction>
    FORCEINLINE Void Sort(_PredicateFunction _func) noexcept {
        list_.sort(_func);
    }

    FORCEINLINE Void Clear() noexcept { list_.clear(); }

    FORCEINLINE Void Resize(SizeType _size) noexcept { list_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { list_.resize(_size, _val); }

    FORCEINLINE Void Reverse() noexcept { list_.reverse(); }

    FORCEINLINE Void Swap(TList& _list) noexcept { list_.swap(_list); }

protected:
    using SuperType_ = ZObject;

private:
    STDList_ list_;
};

}//zengine