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
template<typename _ObjectType, typename _ContainerType = TDeque<_ObjectType>>
class TQueue : public ZObject {
public:
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TQueue() noexcept : SuperType_(), queue_() {}
    FORCEINLINE TQueue(const TQueue& _queue) noexcept : SuperType_(_queue), queue_(_queue.queue_) {}
    FORCEINLINE TQueue(TQueue&& _queue) noexcept 
        : SuperType_(std::forward<TQueue>(_queue)), queue_(std::move(_queue.queue_)) {}

    FORCEINLINE TQueue(SizeType _size) noexcept : SuperType_(), queue_(_size) {}
    FORCEINLINE TQueue(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), queue_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TQueue(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), queue_(_first, _last) {}
    FORCEINLINE TQueue(InitializerList_ _init_list) noexcept : SuperType_(), queue_(_init_list) {}
 
    FORCEINLINE ~TQueue() noexcept {}

    FORCEINLINE TQueue& operator=(const TQueue& _queue) noexcept { 
        SuperType_::operator=(_queue);
        queue_ = _queue.queue_;
        return *this;
    }
    FORCEINLINE TQueue& operator=(TQueue&& _queue) noexcept { 
        SuperType_::operator=(std::forward<TQueue>(_queue));
        queue_ = std::move(_queue.queue_);
        return *this;
    }
    FORCEINLINE TQueue& operator=(InitializerList_ _init_list) noexcept {
        queue_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return queue_.Assign(_size, _val);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator _first, InputIterator _last) noexcept {
        return queue_.Assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return queue_.Assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TQueue& _queue) noexcept { return queue_ == _queue; }
    NODISCARD FORCEINLINE Bool operator!=(const TQueue& _queue) noexcept { return queue_ != _queue; }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return queue_.Front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return queue_.Front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return queue_.Back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return queue_.Back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return queue_.Size(); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return queue_.Capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return queue_.Empty(); }

    FORCEINLINE Void Pop() noexcept { queue_.PopFront(); }

    FORCEINLINE Void Push(const _ObjectType& _val) noexcept { queue_.PushBack(_val); }
    FORCEINLINE Void Push(_ObjectType&& _val) noexcept { queue_.PushBack(std::forward<_ObjectType>(_val)); }
    template <typename... ArgsType>
    FORCEINLINE Void Push(ArgsType&&... _args) noexcept { queue_.EmplaceBack(std::forward<ArgsType>(_args)...); }

    FORCEINLINE Void Clear() noexcept { queue_.Clear(); }

    FORCEINLINE Void Resize(SizeType _size) noexcept { queue_.Resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { queue_.Resize(_size, _val); }

    FORCEINLINE Void Swap(TQueue& _queue) noexcept { queue_.Swap(_queue); }

protected:
    using SuperType_ = ZObject;

private:
    _ContainerType queue_;
};

}//zengine

#endif // !Z_CORE_T_QUEUE_H_