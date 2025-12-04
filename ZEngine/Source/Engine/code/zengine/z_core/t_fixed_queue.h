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

#include <array>

#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Fixed queue container, front points at the first object, back points at the last object.
*/
template<typename _ObjectType, SizeType kCapacity>
class TFixedQueue : public ZObject {
public:
    using STDFixedArray_ = std::array<_ObjectType, kCapacity>;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE constexpr TFixedQueue() noexcept 
        : SuperType_() , queue_(), front_index_(0ULL), back_index_(kCapacity - 1ULL), size_(0ULL) {}
    FORCEINLINE constexpr TFixedQueue(const TFixedQueue& _queue) noexcept : SuperType_(_queue) { CopyP(_queue); }
    FORCEINLINE constexpr TFixedQueue(TFixedQueue&& _queue) noexcept 
        : SuperType_(std::forward<TFixedQueue>(_queue)) 
    {
        MoveP(std::forward<TFixedQueue>(_queue));
    }

    FORCEINLINE constexpr ~TFixedQueue() noexcept {}

    FORCEINLINE TFixedQueue& operator=(const TFixedQueue& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        SuperType_::operator=(_queue);
        CopyP(_queue);
        return *this;
    }
    FORCEINLINE TFixedQueue& operator=(TFixedQueue&& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        SuperType_::operator=(std::forward<TFixedQueue>(_queue));
        MoveP(std::forward<TFixedQueue>(_queue));
        return *this;
    }

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedQueue& _queue) noexcept { 
        return queue_ == _queue.queue_; 
    }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedQueue& _queue) noexcept { 
        return queue_ != _queue.queue_; 
    }
     
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](SizeType _index) noexcept { 
        return queue_[(front_index_ + _index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](SizeType _index) const noexcept { 
        return queue_[(front_index_ + _index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& At(SizeType _index) noexcept { 
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(SizeType _index) const noexcept { 
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType* DataPtr() noexcept { return queue_.data(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType* DataPtr() const noexcept { return queue_.data(); }

    NODISCARD FORCEINLINE static constexpr SizeType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr SizeType Size() noexcept { return size_; }
    NODISCARD FORCEINLINE constexpr Bool Empty() noexcept { return size_ == 0ULL; }

    constexpr Void PopFront() noexcept { 
        if (size_ > 0ULL) {
            --size_;
            front_index_ = (front_index_ + 1ULL) % kCapacity;
        }
    }  
    constexpr Void PushBack(const _ObjectType& _obj) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        queue_[back_index_] = _obj;
        ++size_;
    }
    constexpr Void PushBack(_ObjectType&& _obj) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        queue_[back_index_] = std::forward<_ObjectType>(_obj);
        ++size_;
    }
    template<typename... _ArgsType>
    constexpr _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        new(&queue_[back_index_]) _ObjectType(std::forward<_ArgsType>(_args)...);
        ++size_;
        return queue_[back_index_];
    }

    constexpr Void Clear() noexcept { 
        size_ = 0ULL;
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    constexpr Void Fill(const _ObjectType& _value) noexcept {
        queue_.fill(_value); 
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    FORCEINLINE constexpr Void Swap(TFixedQueue& _queue) noexcept { queue_.swap(_queue); }

protected:
    using SuperType_ = ZObject;

private:
    FORCEINLINE Void CopyP(const TFixedQueue& _queue) noexcept {
        static_assert(
            std::is_trivially_copyable_v<_ObjectType>,
            "TFixedQueue::CopyP(): Type must be trivially copyable!"
        );
        Copy(&queue_, &_queue.queue_);
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
    }

    FORCEINLINE Void MoveP(TFixedQueue&& _queue) noexcept {
        Copy(&queue_, &_queue.queue_);
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
        _queue.front_index_ = 0ULL;
        _queue.back_index_ = kCapacity - 1ULL;
        _queue.size_ = 0ULL;
    }

    STDFixedArray_ queue_;
    SizeType front_index_;
    SizeType back_index_;
    SizeType size_;
};

/*
    Fixed queue container, front points at the first object, back points at the last object.
*/
template<typename _ObjectType, SizeType kCapacity, typename _MutexType = ZMutex>
class TFixedQueueSafe : public ZObject {
public:
    using STDFixedArray_ = std::array<_ObjectType, kCapacity>;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE constexpr TFixedQueueSafe() noexcept 
        : SuperType_() , queue_(), front_index_(0ULL), back_index_(kCapacity - 1ULL), size_(0ULL) {}
    FORCEINLINE constexpr TFixedQueueSafe(const TFixedQueueSafe& _queue) noexcept : SuperType_(_queue) 
    { 
        TLockGuard lock_guard(const_cast<TFixedQueueSafe*>(&_queue)->mutex_);
        CopyP(_queue); 
    }
    FORCEINLINE constexpr TFixedQueueSafe(TFixedQueueSafe&& _queue) noexcept 
        : SuperType_(std::forward<TFixedQueueSafe>(_queue)) 
    {
        TLockGuard lock_guard(const_cast<TFixedQueueSafe*>(&_queue)->mutex_);
        MoveP(std::forward<TFixedQueueSafe>(_queue));
    }

    FORCEINLINE constexpr ~TFixedQueueSafe() noexcept {}

    FORCEINLINE TFixedQueueSafe& operator=(const TFixedQueueSafe& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(const_cast<TFixedQueueSafe*>(&_queue)->mutex_);
        SuperType_::operator=(_queue);
        CopyP(_queue);
        return *this;
    }
    FORCEINLINE TFixedQueueSafe& operator=(TFixedQueueSafe&& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        TLockGuard lock_guard(mutex_);
        SuperType_::operator=(std::forward<TFixedQueueSafe>(_queue));
        MoveP(std::forward<TFixedQueueSafe>(_queue));
        return *this;
    }

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_ == _queue.queue_; 
    }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_ != _queue.queue_; 
    }
     
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_[(front_index_ + _index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](SizeType _index) const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_[(front_index_ + _index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& At(SizeType _index) noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(SizeType _index) const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_[front_index_]; 
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_[front_index_]; 
    }
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_[back_index_]; 
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_[back_index_]; 
    }
    NODISCARD FORCEINLINE constexpr _ObjectType* DataPtr() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_.data();
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType* DataPtr() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.data();
    }

    NODISCARD FORCEINLINE static constexpr SizeType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr SizeType Size() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return size_;
    }
    NODISCARD FORCEINLINE constexpr Bool Empty() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return size_ == 0;
    }

    constexpr Void PopFront() noexcept { 
        TLockGuard lock_guard(mutex_);
        if (size_ > 0ULL) {
            --size_;
            front_index_ = (front_index_ + 1ULL) % kCapacity;
        }
    }  
    constexpr Void PushBack(const _ObjectType& _obj) noexcept {
        TLockGuard lock_guard(mutex_);
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        queue_[back_index_] = _obj;
        ++size_;
    }
    constexpr Void PushBack(_ObjectType&& _obj) noexcept {
        TLockGuard lock_guard(mutex_);
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        queue_[back_index_] = std::forward<_ObjectType>(_obj);
        ++size_;
    }
    template<typename... _ArgsType>
    constexpr _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        new(&queue_[back_index_]) _ObjectType(std::forward<_ArgsType>(_args)...);
        ++size_;
        return queue_[back_index_];
    }

    constexpr Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_);
        size_ = 0ULL;
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    constexpr Void Fill(const _ObjectType& _value) noexcept {
        TLockGuard lock_guard(mutex_);
        queue_.fill(_value); 
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    FORCEINLINE constexpr Void Swap(TFixedQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.swap(_queue.queue_); 
    }

protected:
    using SuperType_ = ZObject;

private:
    FORCEINLINE Void CopyP(const TFixedQueueSafe& _queue) noexcept {
        static_assert(
            std::is_trivially_copyable_v<_ObjectType>,
            "TFixedQueueSafe::CopyP(): Type must be trivially copyable!"
        );
        Copy(&queue_, &_queue.queue_);
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
    }

    FORCEINLINE Void MoveP(TFixedQueueSafe&& _queue) noexcept {
        Copy(&queue_, &_queue.queue_);
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
        _queue.front_index_ = 0ULL;
        _queue.back_index_ = kCapacity - 1ULL;
        _queue.size_ = 0ULL;
    }

    STDFixedArray_ queue_;
    SizeType front_index_;
    SizeType back_index_;
    SizeType size_;
    _MutexType mutex_;
};

}//zengine