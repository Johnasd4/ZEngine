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

#include <deque>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Double end queue caintainer.
*/
template<typename _ObjectType>
class TDeque : public ZObject {
public:
    using STDDeque_ = std::deque<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDDeque_::iterator;
    using ConstIterator_ = STDDeque_::const_iterator;
    using ReverseIterator_ = STDDeque_::reverse_iterator;
    using ConstReverseIterator_ = STDDeque_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TDeque() noexcept : SuperType_(), deque_() {}
    FORCEINLINE TDeque(const TDeque& _deque) noexcept : SuperType_(_deque), deque_(_deque.deque_) {}
    FORCEINLINE TDeque(TDeque&& _deque) noexcept : SuperType_(std::forward<TDeque>(_deque)), deque_(std::move(_deque.deque_)) {}

    FORCEINLINE TDeque(SizeType _size) noexcept : SuperType_(), deque_(_size) {}
    FORCEINLINE TDeque(SizeType _size, const _ObjectType& _value) noexcept : SuperType_(), deque_(_size, _value) {}
    template <typename _InputIterator>
    FORCEINLINE TDeque(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), deque_(_first, _last) {}
    FORCEINLINE TDeque(InitializerList_ _init_list) noexcept : SuperType_(), deque_(_init_list) {}
 
    FORCEINLINE ~TDeque() noexcept {}

    FORCEINLINE TDeque& operator=(const TDeque& _deque) noexcept { 
        SuperType_::operator=(_deque);
        deque_ = _deque.deque_;
        return *this;
    }
    FORCEINLINE TDeque& operator=(TDeque&& _deque) noexcept { 
        SuperType_::operator=(std::forward<TDeque>(_deque));
        deque_= std::move(_deque.deque_);
        return *this;
    }
    FORCEINLINE TDeque& operator=(InitializerList_ _init_list) noexcept {
        deque_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _value) noexcept {
        return deque_.assign(_size, _value);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return deque_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return deque_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TDeque& _deque) noexcept { return deque_ == _deque.deque_; }
    NODISCARD FORCEINLINE Bool operator!=(const TDeque& _deque) noexcept { return deque_ != _deque.deque_; }

    NODISCARD FORCEINLINE _ObjectType& operator[](const SizeType _index) noexcept { return deque_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](const SizeType _index) const noexcept { return deque_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(IndexType _index) noexcept { return deque_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(IndexType _index) const noexcept { return deque_.at(_index); }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return deque_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return deque_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return deque_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return deque_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(deque_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return deque_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return deque_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return deque_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return deque_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return deque_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return deque_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return deque_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return deque_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return deque_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return deque_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return deque_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return deque_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return deque_.crend(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return deque_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }

    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceFront(_ArgsType&&... _args) noexcept {
        return deque_.emplace_front(std::forward<_ArgsType>(_args)...);
    }
    FORCEINLINE Void PushFront(const _ObjectType& _value) noexcept { deque_.push_front(_value); }
    FORCEINLINE Void PushFront(_ObjectType&& _val) noexcept { deque_.push_front(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopFront() noexcept { deque_.pop_front(); }

    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return deque_.emplace_back(std::forward<_ArgsType>(_args)...);
    }
    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { deque_.push_back(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { deque_.push_back(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopBack() noexcept { deque_.pop_back(); }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return deque_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return deque_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return deque_.insert(_pos, _num, _val);
    }
    template <typename InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InputIterator _first, InputIterator _last) noexcept {
        return deque_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return deque_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return deque_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return deque_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { deque_.clear(); }

    FORCEINLINE Void Resize(SizeType _size) noexcept { deque_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { deque_.resize(_size, _val); }

    FORCEINLINE Void Swap(TDeque& _deque) noexcept { deque_.swap(_deque.deque_); }

protected:
    using SuperType_ = ZObject;

private:
    STDDeque_ deque_;
};

}//zengine