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
#ifndef Z_CORE_T_FIXED_QUEUE_H_
#define Z_CORE_T_FIXED_QUEUE_H_

#include "internal/drive.h"

#include <array>

#include "z_object.h"

namespace zengine {

/*
    Fixed queue caintainer, front points at the first object, back points at the last object.
*/
template<typename ObjectType, IndexType kCapacity>
class TFixedQueue : public ZObject {
public:
    using STDArray = std::array<ObjectType, kCapacity>;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE constexpr TFixedQueue() noexcept : SuperType(), 
        queue_(), front_index_(0), back_index_(kCapacity - 1), size_(0) {}
    FORCEINLINE constexpr TFixedQueue(const TFixedQueue& queue) noexcept : SuperType(), 
        queue_(queue.queue_), front_index_(queue.front_index_), back_index_(queue.back_index_), size_(queue.size_) {}
    FORCEINLINE constexpr TFixedQueue(TFixedQueue&& queue) noexcept : SuperType(), 
        queue_(std::move(queue.queue_)), front_index_(queue.front_index_), back_index_(queue.back_index_), 
        size_(queue.size_) {}
    FORCEINLINE TFixedQueue(InitializerList init_list) noexcept : SuperType(), 
        queue_(init_list), front_index_(0), back_index_(kCapacity - 1), size_(kCapacity) {}


    FORCEINLINE constexpr ~TFixedQueue() noexcept {}

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedQueue& queue) noexcept { return queue_ == queue.queue_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedQueue& queue) noexcept { return queue_ != queue.queue_; }
     
    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) noexcept { 
        return queue_[(front_index_ + index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const noexcept { 
        return queue_[(front_index_ + index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) noexcept { 
        return queue_.at((SizeType)((front_index_ + index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const noexcept { 
        return queue_.at((SizeType)((front_index_ + index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const noexcept { return queue_[front_index_]; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const noexcept { return queue_[back_index_]; }
    NODISCARD FORCEINLINE constexpr ObjectType* DataPtr() noexcept { return queue_.data(); }
    NODISCARD FORCEINLINE constexpr const ObjectType* DataPtr() const noexcept { return queue_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr IndexType Size() noexcept { return size_; }
    NODISCARD FORCEINLINE constexpr Bool Empty() noexcept { return size_ == 0; }

    constexpr Void Pop() noexcept { 
        --size_;
        front_index_ = (front_index_ + 1) % kCapacity;
    }  
    constexpr Void Push(const ObjectType& object) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        queue_[back_index_] = object;
        ++size_;
    }
    constexpr Void Push(ObjectType&& object) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        queue_[back_index_] = object;
        ++size_;
    }
    template<typename... ArgsType>
    constexpr Void Push(ArgsType&&... args) noexcept {
        back_index_ = (back_index_ + 1) % kCapacity;
        new(&queue_[back_index_]) ObjectType(std::forward<ArgsType>(args)...);
        ++size_;
    }

    constexpr Void Clear() noexcept { 
        size_ = 0;
        front_index_ = 0;
        back_index_ = kCapacity - 1;
    }
    constexpr Void Fill(const ObjectType& value) noexcept {
        queue_.fill(value); 
        front_index_ = 0;
        back_index_ = kCapacity - 1;
    }
    FORCEINLINE constexpr Void Swap(TFixedQueue& queue) noexcept { queue_.swap(queue); }

protected:
    using SuperType = ZObject;

private:
    STDArray queue_;
    IndexType front_index_;
    IndexType back_index_;
    IndexType size_;
};

}//zengine

#endif // !Z_CORE_T_FIXED_QUEUE_H_