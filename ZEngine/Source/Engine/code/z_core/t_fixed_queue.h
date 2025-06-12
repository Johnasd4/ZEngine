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

#include <array>

#include "z_object.h"

namespace zengine {

/*
    Fixed queue caintainer, front points at the first object, back points at the last object.
*/
template<typename _ObjectType, IndexType kCapacity>
class TFixedQueue : public ZObject {
public:
    using STDArray_ = std::array<_ObjectType, kCapacity>;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE constexpr TFixedQueue() noexcept 
        : SuperType_() , queue_(), front_index_(0), back_index_(kCapacity - 1), size_(0) {}
    FORCEINLINE constexpr TFixedQueue(const TFixedQueue& _queue) noexcept : SuperType_(_queue) { CopyP(_queue); }
    FORCEINLINE constexpr TFixedQueue(TFixedQueue&& _queue) noexcept : SuperType_(std::forward<TFixedQueue>(_queue)) {
        MoveP(std::forward<TFixedQueue>(_queue));
    }
    FORCEINLINE TFixedQueue(InitializerList_ _init_list) noexcept 
        : SuperType_(), queue_(_init_list), front_index_(0), back_index_(kCapacity - 1), size_(kCapacity) {}


    FORCEINLINE constexpr ~TFixedQueue() noexcept {}

    FORCEINLINE TFixedQueue& operator=(const TFixedQueue& _queue) noexcept {
        SuperType_::operator=(_queue);
        CopyP(_queue);
        return *this;
    }
    FORCEINLINE TFixedQueue& operator=(TFixedQueue&& _queue) noexcept {
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
     
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](IndexType _index) noexcept { 
        return queue_[(front_index_ + _index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](IndexType _index) const noexcept { 
        return queue_[(front_index_ + _index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& At(IndexType _index) noexcept { 
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(IndexType _index) const noexcept { 
        return queue_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType* DataPtr() noexcept { return queue_.data(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType* DataPtr() const noexcept { return queue_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr IndexType Size() noexcept { return size_; }
    NODISCARD FORCEINLINE constexpr Bool Empty() noexcept { return size_ == 0; }

    constexpr Void PopFront() noexcept { 
        --size_;
        front_index_ = (front_index_ + 1) % kCapacity;
    }  
    constexpr Void PushBack(const _ObjectType& _obj) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        queue_[back_index_] = _obj;
        ++size_;
    }
    constexpr Void PushBack(_ObjectType&& _obj) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        queue_[back_index_] = std::forward<_ObjectType>(_obj);
        ++size_;
    }
    template<typename... _ArgsType>
    constexpr _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        new(&queue_[back_index_]) _ObjectType(std::forward<_ArgsType>(_args)...);
        ++size_;
        return queue_[back_index_];
    }

    constexpr Void Clear() noexcept { 
        size_ = 0;
        front_index_ = 0;
        back_index_ = kCapacity - 1;
    }
    constexpr Void Fill(const _ObjectType& _value) noexcept {
        queue_.fill(_value); 
        front_index_ = 0;
        back_index_ = kCapacity - 1;
    }
    FORCEINLINE constexpr Void Swap(TFixedQueue& _queue) noexcept { queue_.swap(_queue); }

protected:
    using SuperType_ = ZObject;

private:
    FORCEINLINE Void CopyP(const TFixedQueue& _queue) noexcept {
        queue_ = _queue.queue_;
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
    }

    FORCEINLINE Void MoveP(TFixedQueue&& _queue) noexcept {
        queue_ = std::move(_queue.queue_);
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
        _queue.front_index_ = 0;
        _queue.back_index_ = kCapacity - 1;
        _queue.size_ = 0;
    }

    STDArray_ queue_;
    IndexType front_index_;
    IndexType back_index_;
    IndexType size_;
};

}//zengine