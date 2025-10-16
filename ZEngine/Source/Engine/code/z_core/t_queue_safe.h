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

#include "t_allocator.h"
#include "t_deque.h"
#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Thread safe queue caintainer.
*/
template<typename _ObjectType, typename _ContainerType = TDeque<_ObjectType>, typename _MutexType = ZMutex>
class TQueueSafe : public ZObject {
public:
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TQueueSafe() noexcept : SuperType_(), queue_() {}
    FORCEINLINE TQueueSafe(const TQueueSafe& _queue) noexcept : SuperType_(_queue)
    {
        TLockGuard lock_guard(const_cast<TQueueSafe*>(&_queue)->mutex_);
        queue_ = _queue.queue_;
    }
    FORCEINLINE TQueueSafe(TQueueSafe&& _queue) noexcept
        : SuperType_(std::forward<TQueueSafe>(_queue))
    {
        TLockGuard lock_guard(_queue.mutex_);
        queue_ = std::move(_queue.queue_);
    }

    FORCEINLINE TQueueSafe(SizeType _size) noexcept : SuperType_()
    {
        queue_ = _ContainerType(_size);

    }
    FORCEINLINE TQueueSafe(SizeType _size, const _ObjectType& _val) noexcept : SuperType_()
    {
        queue_ = _ContainerType(_size, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE TQueueSafe(_InputIterator _first, _InputIterator _last) noexcept : SuperType_()
    {
        queue_ = _ContainerType(_first, _last);
    }
    FORCEINLINE TQueueSafe(InitializerList_ _init_list) noexcept : SuperType_(), queue_(_init_list)
    {
        queue_ = _ContainerType(_init_list);
    }
    FORCEINLINE ~TQueueSafe() noexcept {}

    FORCEINLINE TQueueSafe& operator=(const TQueueSafe& _queue) noexcept {
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(const_cast<TQueueSafe*>(&_queue)->mutex_);
        SuperType_::operator=(_queue);
        queue_ = _queue.queue_;
        return *this;
    }
    FORCEINLINE TQueueSafe& operator=(TQueueSafe&& _queue) noexcept {
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_queue.mutex_);
        SuperType_::operator=(std::forward<TQueueSafe>(_queue));
        queue_ = std::move(_queue.queue_);
        return *this;
    }
    FORCEINLINE TQueueSafe& operator=(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        queue_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.Assign(_size, _val);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator _first, InputIterator _last) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.Assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.Assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_ == _queue.queue_; 
    }
    NODISCARD FORCEINLINE Bool operator!=(const TQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_ != _queue.queue_; 
    }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { 
        TLockGuard lock_guard(mutex_);
        return queue_.Front(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.Front(); 
    }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_.Back(); 
    }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.Back(); 
    }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.Size(); 
    }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.Capacity(); 
    }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { 
        TLockGuard lock_guard(*const_cast<_MutexType*>(&mutex_));
        return queue_.Empty(); 
    }

    FORCEINLINE Void Pop() noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.PopFront(); 
    }

    FORCEINLINE Void Push(const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.PushBack(_val); 
    }
    FORCEINLINE Void Push(_ObjectType&& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.PushBack(std::forward<_ObjectType>(_val)); 
    }
    template <typename... ArgsType>
    FORCEINLINE Void Push(ArgsType&&... _args) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.EmplaceBack(std::forward<ArgsType>(_args)...); 
    }

    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.Clear(); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.Resize(_size);
    }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.Resize(_size, _val); 
    }

    FORCEINLINE Void Swap(TQueueSafe& _queue) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.Swap(_queue.queue_); 
    }

protected:
    using SuperType_ = ZObject;

private:
    _ContainerType queue_;
    _MutexType mutex_;
};

}//zengine