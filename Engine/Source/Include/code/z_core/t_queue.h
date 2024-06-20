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
#ifndef Z_CORE_T_QUEUE_H_
#define Z_CORE_T_QUEUE_H_

#include "internal/z_drive.h"

#include "t_allocator.h"
#include "t_deque.h"
#include "z_object.h"

namespace zengine {

/*
    Queue caintainer.
*/
template<typename ObjectType, typename ContainerType = TDeque<ObjectType>>
class TQueue : public ZObject {
public:
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TQueue() noexcept : SuperType(), queue_() {}
    FORCEINLINE TQueue(const TQueue& queue) noexcept : SuperType(), queue_(queue.queue_) {}
    FORCEINLINE TQueue(TQueue&& queue) noexcept : SuperType(), queue_(std::move(queue.queue_)) {}

    FORCEINLINE TQueue(SizeType size) noexcept : SuperType(), queue_(size) {}
    FORCEINLINE TQueue(SizeType size, const ObjectType& value) noexcept : SuperType(), queue_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TQueue(InputIterator first, InputIterator last) noexcept : SuperType(), queue_(first, last) {}
    FORCEINLINE TQueue(InitializerList init_list) noexcept : SuperType(), queue_(init_list) {}
 
    FORCEINLINE ~TQueue() noexcept {}

    FORCEINLINE TQueue& operator=(const TQueue& queue) noexcept { 
        queue_.operator=(queue.queue_);
        return *this;
    }
    FORCEINLINE TQueue& operator=(TQueue&& queue) noexcept { 
        queue_.operator=(std::move(queue.queue_));
        return *this;
    }
    FORCEINLINE TQueue& operator=(InitializerList init_list) noexcept {
        queue_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return queue_.Assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return queue_.Assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return queue_.Assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TQueue& queue) noexcept { return queue_ == queue; }
    NODISCARD FORCEINLINE Bool operator!=(const TQueue& queue) noexcept { return queue_ != queue; }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return queue_.Front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return queue_.Front(); }
    NODISCARD FORCEINLINE ObjectType& Back() noexcept { return queue_.Back(); }
    NODISCARD FORCEINLINE const ObjectType& Back() const noexcept { return queue_.Back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return queue_.Size(); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return queue_.Capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return queue_.Empty(); }

    FORCEINLINE Void Pop() noexcept { queue_.PopFront(); }

    FORCEINLINE Void Push(const ObjectType& value) noexcept { queue_.PushBack(value); }
    FORCEINLINE Void Push(ObjectType&& value) noexcept { queue_.PushBack(std::forward<ObjectType>(value)); }
    template <typename... ArgsType>
    FORCEINLINE Void Push(ArgsType&&... args) noexcept { queue_.EmplaceBack(std::forward<ArgsType>(args)...); }

    FORCEINLINE Void Clear() noexcept { queue_.Clear(); }

    FORCEINLINE Void Resize(SizeType size) noexcept { queue_.Resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { queue_.Resize(size, value); }

    FORCEINLINE Void Swap(TQueue& queue) noexcept { queue_.Swap(queue); }

protected:
    using SuperType = ZObject;

private:
    ContainerType queue_;
};

}//zengine

#endif // !Z_CORE_T_QUEUE_H_