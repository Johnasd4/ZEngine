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

#include <vector>

#include "t_allocator.h"
#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Vector container.
*/
template<typename _ObjectType>
class TVector : public ZObject {
public:
    using STDVector_ = std::vector<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDVector_::iterator;
    using ConstIterator_ = STDVector_::const_iterator;
    using ReverseIterator_ = STDVector_::reverse_iterator;
    using ConstReverseIterator_ = STDVector_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TVector() noexcept : SuperType_(), vec_() {}
    FORCEINLINE TVector(const TVector& _vec) noexcept : SuperType_(_vec), vec_(_vec.vec_) {}
    FORCEINLINE TVector(TVector&& _vec) noexcept 
        : SuperType_(std::forward<TVector>(_vec)), vec_(std::move(_vec.vec_)) {}

    FORCEINLINE TVector(SizeType _size) noexcept : SuperType_(), vec_(_size) {}
    FORCEINLINE TVector(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), vec_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TVector(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), vec_(_first, _last) {}
    FORCEINLINE TVector(InitializerList_ _init_list) noexcept : SuperType_(), vec_(_init_list) {}
 
    FORCEINLINE ~TVector() noexcept {}

    FORCEINLINE TVector& operator=(const TVector& _vec) noexcept { 
        SuperType_::operator=(_vec);
        vec_ = _vec.vec_;
        return *this;
    }
    FORCEINLINE TVector& operator=(TVector&& _vec) noexcept { 
        SuperType_::operator=(std::forward<TVector>(_vec));
        vec_ = std::move(_vec.vec_);
        return *this;
    }
    FORCEINLINE TVector& operator=(InitializerList_ _init_list) noexcept {
        vec_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        vec_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        vec_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        vec_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TVector& _vec) noexcept { return vec_ == _vec; }
    NODISCARD FORCEINLINE Bool operator!=(const TVector& _vec) noexcept { return vec_ != _vec; }

    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { return vec_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { return vec_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) noexcept { return vec_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { return vec_.at(_index); }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return vec_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return vec_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return vec_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return vec_.back(); }
    NODISCARD FORCEINLINE _ObjectType* DataPtr() noexcept { return vec_.data(); }
    NODISCARD FORCEINLINE const _ObjectType* DataPtr() const noexcept { return vec_.data(); }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return static_cast<SizeType>(vec_.size()); }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { return vec_.capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return vec_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return vec_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return vec_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return vec_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return vec_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return vec_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return vec_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return vec_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return vec_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return vec_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return vec_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return vec_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return vec_.crend(); }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { vec_.push_back(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { vec_.push_back(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopBack() noexcept { vec_.pop_back(); }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return vec_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return vec_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return vec_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return vec_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return vec_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return vec_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return vec_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { vec_.clear(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return vec_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return vec_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { vec_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { vec_.resize(_size, _val); }

    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { vec_.reserve(_capacity); }

    FORCEINLINE Void Swap(TVector& _vec) noexcept { vec_.swap(_vec); }

protected:
    using SuperType_ = ZObject;

private:
    STDVector_ vec_;
};

/*
    Vector container. Thread safe.
*/
template<typename _ObjectType>
class TVectorSafe : public ZObject {
public:
    using STDVector_ = std::vector<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDVector_::iterator;
    using ConstIterator_ = STDVector_::const_iterator;
    using ReverseIterator_ = STDVector_::reverse_iterator;
    using ConstReverseIterator_ = STDVector_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TVectorSafe() noexcept : SuperType_(), vec_(), mutex_() {}
    FORCEINLINE TVectorSafe(const TVectorSafe& _vec) noexcept : SuperType_(_vec), vec_(_vec.vec_), mutex_() {}
    FORCEINLINE TVectorSafe(TVectorSafe&& _vec) noexcept 
        : SuperType_(std::forward<TVectorSafe>(_vec)) 
    {
        vec_ = std::move(_vec.vec_);
        mutex_ = std::move(_vec.mutex_);
    }

    FORCEINLINE TVectorSafe(SizeType _size) noexcept : SuperType_(), vec_(_size), mutex_() {}
    FORCEINLINE TVectorSafe(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_(), vec_(_size, _val), mutex_() {}
    template <typename _InputIterator>
    FORCEINLINE TVectorSafe(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), vec_(_first, _last), mutex_() {}
    FORCEINLINE TVectorSafe(InitializerList_ _init_list) noexcept : SuperType_(), vec_(_init_list), mutex_() {}
 
    FORCEINLINE ~TVectorSafe() noexcept {}

    FORCEINLINE TVectorSafe& operator=(const TVectorSafe& _vec) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_vec.mutex_);
        SuperType_::operator=(_vec);
        vec_ = _vec.vec_;
        return *this;
    }
    FORCEINLINE TVectorSafe& operator=(TVectorSafe&& _vec) noexcept { 
        SuperType_::operator=(std::forward<TVectorSafe>(_vec));
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_vec.mutex_);
        vec_ = std::move(_vec.vec_);
        return *this;
    }
    FORCEINLINE TVectorSafe& operator=(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        vec_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        vec_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        TLockGuard lock_guard(mutex_);
        vec_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        vec_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TVectorSafe& _vec) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_vec.mutex_);
        return vec_ == _vec.vec_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TVectorSafe& _vec) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_vec.mutex_);
        return vec_ != _vec.vec_;
    }

    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_);
        return vec_[_index]; 
    }
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { 
        TLockGuard lock_guard(mutex_);
        return vec_[_index]; 
    }

    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.at(_index); 
    }
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.at(_index); 
    }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.front(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.front(); 
    }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.back(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.back(); 
    }
    NODISCARD FORCEINLINE _ObjectType* DataPtr() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.data(); 
    }
    NODISCARD FORCEINLINE const _ObjectType* DataPtr() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.data(); 
    }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return static_cast<SizeType>(vec_.size()); 
    }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.capacity(); 
    }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.empty(); 
    }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.begin(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.begin(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.cbegin(); 
    }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.rbegin(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.rbegin(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.crbegin(); 
    }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.end(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.end(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.cend(); 
    }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.rend(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.rend(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.crend(); 
    }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.push_back(_val); 
    }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.push_back(std::forward<_ObjectType>(_val)); 
    }
    FORCEINLINE Void PopBack() noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.pop_back(); 
    }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_);
        return vec_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { 
        TLockGuard lock_guard(mutex_); 
        return vec_.erase(_pos); 
    }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        TLockGuard lock_guard(mutex_);
        return vec_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.clear(); 
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return vec_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.resize(_size); 
    }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.resize(_size, _val); 
    }

    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.reserve(_capacity); 
    }

    FORCEINLINE Void Swap(TVectorSafe& _vec) noexcept { 
        TLockGuard lock_guard(mutex_); 
        vec_.swap(_vec.vec_);
    }

    /*
        Lock the container mutex. Used when operating iterators.
    */
    FORCEINLINE Void Lock() noexcept {
        mutex_.Lock();
    }

    /*
        Unlock the container mutex. Used when operating iterators.
    */
    FORCEINLINE Void Unlock() noexcept {
        mutex_.Unlock();
    }

protected:
    using SuperType_ = ZObject;

private:
    STDVector_ vec_;
    ZMutex mutex_;
};

}//zengine