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
    Fixed queue caintainer.
*/
template<typename ObjectType, IndexType kCapacity>
class TFixedQueue : public ZObject {
public:
    using STDArray = std::array<ObjectType, kCapacity>;
    using Iterator = STDArray::iterator;
    using ConstIterator = STDArray::const_iterator;
    using ReverseIterator = STDArray::reverse_iterator;
    using ConstReverseIterator = STDArray::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE constexpr TFixedQueue() noexcept : SuperType(), queue_() {}
    FORCEINLINE constexpr TFixedQueue(const TFixedQueue& queue) noexcept : SuperType(), queue_(queue.queue_) {}
    FORCEINLINE constexpr TFixedQueue(TFixedQueue&& queue) noexcept : SuperType(), queue_(std::move(queue.queue_)) {}
    FORCEINLINE TFixedQueue(InitializerList init_list) noexcept : SuperType(), queue_(init_list) {}

    FORCEINLINE constexpr ~TFixedQueue() noexcept {}

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedQueue& queue) noexcept { return queue_ == queue.queue_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedQueue& queue) noexcept { return queue_ != queue.queue_; }
     
    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) noexcept { return queue_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const noexcept { return queue_[index]; }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) noexcept { return queue_.at(index); }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const noexcept { return queue_.at(index); }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() noexcept { return queue_.front(); }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const noexcept { return queue_.front(); }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() noexcept { return queue_.back(); }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const noexcept { return queue_.back(); }
    NODISCARD FORCEINLINE constexpr ObjectType* DataPtr() noexcept { return queue_.data(); }
    NODISCARD FORCEINLINE constexpr const ObjectType* DataPtr() const noexcept { return queue_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Size() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return queue_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return queue_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return queue_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return queue_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return queue_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return queue_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return queue_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return queue_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return queue_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return queue_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return queue_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return queue_.crend(); }

    FORCEINLINE Void Fill(const ObjectType& value) noexcept { queue_.fill(value); }
    FORCEINLINE Void Swap(TFixedQueue& queue) noexcept { queue_.swap(queue); }

protected:
    using SuperType = ZObject;

private:
    IndexType front_index_;
    IndexType back_index_;
    STDArray queue_;
};

}//zengine

#endif // !Z_CORE_T_FIXED_QUEUE_H_