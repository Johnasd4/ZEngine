/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creatiarrayommons.org/licenses/by-nc/4.0/

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
    Array container.
*/
template<typename _ObjectType>
class TArray : public ZObject<> {
public:
    using STDArray_ = std::vector<_ObjectType, TAllocator<_ObjectType>>;
    using Iterator_ = STDArray_::iterator;
    using ConstIterator_ = STDArray_::const_iterator;
    using ReverseIterator_ = STDArray_::reverse_iterator;
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TArray() noexcept : SuperType_(), array_() {}
    FORCEINLINE TArray(const TArray& _array) noexcept : SuperType_(_array), array_(_array.array_) {}
    FORCEINLINE TArray(TArray&& _array) noexcept 
        : SuperType_(std::forward<TArray>(_array)), array_(std::move(_array.array_)) {}

    FORCEINLINE TArray(SizeType _size) noexcept : SuperType_(), array_(_size) {}
    FORCEINLINE TArray(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), array_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TArray(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), array_(_first, _last) {}
    FORCEINLINE TArray(InitializerList_ _init_list) noexcept : SuperType_(), array_(_init_list) {}
 
    FORCEINLINE ~TArray() noexcept {}

    FORCEINLINE TArray& operator=(const TArray& _array) noexcept { 
        SuperType_::operator=(_array);
        array_ = _array.array_;
        return *this;
    }
    FORCEINLINE TArray& operator=(TArray&& _array) noexcept { 
        SuperType_::operator=(std::forward<TArray>(_array));
        array_ = std::move(_array.array_);
        return *this;
    }
    FORCEINLINE TArray& operator=(InitializerList_ _init_list) noexcept {
        array_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        array_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        array_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        array_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TArray& _array) noexcept { return array_ == _array; }
    NODISCARD FORCEINLINE Bool operator!=(const TArray& _array) noexcept { return array_ != _array; }

    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { return array_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { return array_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) noexcept { return array_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { return array_.at(_index); }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return array_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return array_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return array_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return array_.back(); }
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept { return array_.data(); }
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept { return array_.data(); }

    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return static_cast<SizeType>(array_.size()); }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return array_.capacity(); }
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return array_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return array_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return array_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return array_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return array_.crend(); }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { array_.push_back(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { array_.push_back(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopBack() noexcept { array_.pop_back(); }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return array_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return array_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return array_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return array_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return array_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return array_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return array_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { array_.clear(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return array_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return array_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { array_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { array_.resize(_size, _val); }

    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { array_.reserve(_capacity); }

    FORCEINLINE Void Swap(TArray& _array) noexcept { array_.swap(_array); }

protected:
    using SuperType_ = ZObject;

private:
    STDArray_ array_;
};

/*
    Array container. Thread safe.
*/
template<typename _ObjectType>
class TArraySafe : public ZObject<> {
public:
    using STDArray_ = std::vector<_ObjectType, TAllocator<_ObjectType>>;
    using Iterator_ = STDArray_::iterator;
    using ConstIterator_ = STDArray_::const_iterator;
    using ReverseIterator_ = STDArray_::reverse_iterator;
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TArraySafe() noexcept : SuperType_(), array_(), mutex_() {}
    FORCEINLINE TArraySafe(const TArraySafe& _array) noexcept 
        : SuperType_(_array), mutex_() 
    {
        TLockGuard lock_guard(_array.mutex_);
        array_ = _array.array_;
    }
    FORCEINLINE TArraySafe(TArraySafe&& _array) noexcept 
        : SuperType_(std::forward<TArraySafe>(_array)), mutex_()
    {
        TLockGuard lock_guard(_array.mutex_);
        array_ = std::move(_array.array_);
    }

    FORCEINLINE TArraySafe(SizeType _size) noexcept : SuperType_(), array_(_size), mutex_() {}
    FORCEINLINE TArraySafe(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_(), array_(_size, _val), mutex_() {}
    template <typename _InputIterator>
    FORCEINLINE TArraySafe(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), array_(_first, _last), mutex_() {}
    FORCEINLINE TArraySafe(InitializerList_ _init_list) noexcept : SuperType_(), array_(_init_list), mutex_() {}
 
    FORCEINLINE ~TArraySafe() noexcept {}

    FORCEINLINE TArraySafe& operator=(const TArraySafe& _array) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_array.mutex_);
        SuperType_::operator=(_array);
        array_ = _array.array_;
        return *this;
    }
    FORCEINLINE TArraySafe& operator=(TArraySafe&& _array) noexcept { 
        SuperType_::operator=(std::forward<TArraySafe>(_array));
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_array.mutex_);
        array_ = std::move(_array.array_);
        return *this;
    }
    FORCEINLINE TArraySafe& operator=(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        array_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        array_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        TLockGuard lock_guard(mutex_);
        array_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        array_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TArraySafe& _array) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_array.mutex_);
        return array_ == _array.array_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TArraySafe& _array) noexcept { 
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_array.mutex_);
        return array_ != _array.array_;
    }

    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_);
        return array_[_index]; 
    }
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { 
        TLockGuard lock_guard(mutex_);
        return array_[_index]; 
    }

    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.at(_index); 
    }
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.at(_index); 
    }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.front(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.front(); 
    }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.back(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.back(); 
    }
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.data(); 
    }
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.data(); 
    }

    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return static_cast<SizeType>(array_.size()); 
    }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.capacity(); 
    }
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.empty(); 
    }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.begin(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.begin(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.cbegin(); 
    }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.rbegin(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.rbegin(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.crbegin(); 
    }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.end(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.end(); 
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.cend(); 
    }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.rend(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.rend(); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.crend(); 
    }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.push_back(_val); 
    }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.push_back(std::forward<_ObjectType>(_val)); 
    }
    FORCEINLINE Void PopBack() noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.pop_back(); 
    }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_);
        return array_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { 
        TLockGuard lock_guard(mutex_); 
        return array_.erase(_pos); 
    }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        TLockGuard lock_guard(mutex_);
        return array_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.clear(); 
    }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return array_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.resize(_size); 
    }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.resize(_size, _val); 
    }

    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.reserve(_capacity); 
    }

    FORCEINLINE Void Swap(TArraySafe& _array) noexcept { 
        TLockGuard lock_guard(mutex_); 
        array_.swap(_array.array_);
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
    STDArray_ array_;
    ZMutex mutex_;
};

}//zengine