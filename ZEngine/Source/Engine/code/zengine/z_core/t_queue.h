/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "t_allocator.h"
#include "t_deque.h"
#include "t_lock_guard.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"

#pragma warning(disable: 4834)

namespace zengine {

/**
 * @brief Queue container.
 *
 * A First-In-First-Out (FIFO) data structure adapter. It allows elements to be inserted at the back
 * (pushed) and removed from the front (popped). This implementation wraps an underlying container
 * (defaulting to TDeque) to provide standard queue semantics.
 *
 * @tparam _ObjectType The type of elements stored in the queue.
 * @tparam _ContainerType The underlying container type, default is TDeque using global memory pool.
 */
template<typename _ObjectType, typename _ContainerType = TDeque<_ObjectType, DEFAULT_USE_GLOBAL_MEMORY_POOL>>
class TQueue : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying container type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     * Constructs an empty queue.
     */
    FORCEINLINE TQueue() noexcept 
        : SuperType_()
        , queue_() 
    {}

    /**
     * @brief Copy constructor.
     * Constructs a queue with the contents of another queue.
     * @param _queue The queue to copy from.
     */
    FORCEINLINE TQueue(const TQueue& _queue) noexcept 
        : SuperType_(_queue)
        , queue_(_queue.queue_) 
    {}

    /**
     * @brief Move constructor.
     * Constructs a queue by moving the contents of another queue.
     * @param _queue The queue to move from.
     */
    FORCEINLINE TQueue(TQueue&& _queue) noexcept 
        : SuperType_(std::move(_queue))
        , queue_(std::move(_queue.queue_)) 
    {}

    /**
     * @brief Constructs a queue with a specific size.
     * @param _size The initial size of the queue.
     */
    FORCEINLINE TQueue(SizeType _size) noexcept 
        : SuperType_()
        , queue_(_size) 
    {}

    /**
     * @brief Constructs a queue with a specific size and fills it with a value.
     * @param _size The initial size.
     * @param _val The value to fill the queue with.
     */
    FORCEINLINE TQueue(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_()
        , queue_(_size, _val) 
    {}

    /**
     * @brief Range constructor.
     * Constructs a queue with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TQueue(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_()
        , queue_(_first, _last) 
    {}

    /**
     * @brief Initializer list constructor.
     * Constructs a queue with the contents of the initializer list.
     * @param _init_list The initializer list.
     */
    FORCEINLINE TQueue(InitializerList_ _init_list) noexcept 
        : SuperType_()
        , queue_(_init_list) 
    {}
 
    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TQueue() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * Replaces the contents with a copy of the contents of another queue.
     * @param _queue The queue to copy from.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueue& operator=(const TQueue& _queue) noexcept { 
        if(this == &_queue) {
            return *this;
        }
        SuperType_::operator=(_queue);
        queue_ = _queue.queue_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Replaces the contents by moving the contents of another queue.
     * @param _queue The queue to move from.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueue& operator=(TQueue&& _queue) noexcept { 
        if (this == &_queue) {
            return *this;
        }

        SuperType_::operator=(std::move(_queue));
        queue_ = std::move(_queue.queue_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * Replaces the contents with those from the initializer list.
     * @param _init_list The initializer list.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueue& operator=(InitializerList_ _init_list) noexcept {
        queue_ = _init_list;
        return *this;
    }


    /**
     * @brief Checks if the queue is empty.
     * @return True if the queue is empty, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return queue_.IsEmpty(); }

    /**
     * @brief Checks if the queue is full.
     * @return True if the queue is full (usually false for dynamic containers), false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return queue_.IsFull(); }

    /**
     * @brief Returns the number of elements in the queue.
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return queue_.GetSize(); }

    /**
     * @brief Returns the capacity of the queue.
     * @return The number of elements the queue can hold before reallocating.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return queue_.GetCapacity(); }

    /**
     * @brief Equality comparison operator.
     * @param _queue The queue to compare with.
     * @return True if the queues are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TQueue& _queue) noexcept { return queue_ == _queue.queue_; }

    /**
     * @brief Inequality comparison operator.
     * @param _queue The queue to compare with.
     * @return True if the queues are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TQueue& _queue) noexcept { return queue_ != _queue.queue_; }

    /**
     * @brief Accesses the first element.
     * @return A reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return queue_.Front(); }

    /**
     * @brief Accesses the first element (const).
     * @return A const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return queue_.Front(); }

    /**
     * @brief Removes the first element from the queue.
     */
    FORCEINLINE Void PopFront() noexcept { queue_.PopFront(); }

    /**
     * @brief Tries to pop the front element from the queue.
     * If the queue is not empty, the front element is moved to _out_obj
     * and removed from the queue.
     * @param _out_obj Reference to store the popped element.
     * @return True if an element was popped (queue was not empty), false otherwise.
     */
    FORCEINLINE Bool TryPopFront(_ObjectType& _out_obj) noexcept {
        if (queue_.IsEmpty()) {
            return false;
        }

        _out_obj = std::move(queue_.Front());
        queue_.PopFront();

        return true;
    }

    /**
     * @brief Adds an element to the end of the queue.
     * @param _val The value to add.
     */
    FORCEINLINE decltype(auto) PushBack(const _ObjectType& _val) noexcept {
        return queue_.PushBack(_val);
    }

    /**
     * @brief Adds an element to the end of the queue (move semantics).
     * @param _val The value to move into the queue.
     */
    FORCEINLINE decltype(auto) PushBack(_ObjectType&& _val) noexcept {
        return queue_.PushBack(std::move(_val));
    }

    /**
     * @brief Constructs and adds an element to the end of the queue.
     * @tparam ArgsType The types of the arguments.
     * @param _args The arguments to forward to the constructor of the element.
     */
    template <typename... ArgsType>
    FORCEINLINE decltype(auto) EmplaceBack(ArgsType&&... _args) noexcept {
        return queue_.EmplaceBack(std::forward<ArgsType>(_args)...); 
    }

    /**
     * @brief Clears the contents of the queue.
     */
    FORCEINLINE Void Clear() noexcept { queue_.Clear(); }

    /**
     * @brief Reserves storage for at least the specified number of elements.
     * @param _size The new capacity of the queue.
     */
    FORCEINLINE Void Reserve(SizeType _size) noexcept { queue_.Reserve(_size); }

    /**
     * @brief Swaps the contents of this queue with another.
     * @param _queue The queue to swap with.
     */
    FORCEINLINE Void Swap(TQueue& _queue) noexcept { queue_.Swap(_queue.queue_); }

private:
    /** @brief The underlying container used to store elements. */
    _ContainerType queue_;
};

/**
 * @brief Thread-safe Queue container.
 *
 * A thread-safe First-In-First-Out (FIFO) data structure. It wraps a standard container
 * and protects all member functions with a mutex to ensure thread safety during concurrent
 * access. It supports standard queue operations in a thread-safe manner.
 *
 * @tparam _ObjectType The type of elements stored in the queue.
 * @tparam _ContainerType The underlying container type, default is TDeque using global memory pool.
 */
template<typename _ObjectType, typename _ContainerType = TDeque<_ObjectType, DEFAULT_USE_GLOBAL_MEMORY_POOL>>
class CACHE_ALIGN TQueueSafe : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying container type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     * Constructs an empty thread-safe queue.
     */
    FORCEINLINE TQueueSafe() noexcept 
        : SuperType_()
        , mutex_()
        , queue_() 
    {}

    /**
     * @brief Copy constructor.
     * Thread-safe copy construction. Locks the source queue before copying.
     * @param _queue The queue to copy from.
     */
    FORCEINLINE TQueueSafe(const TQueueSafe& _queue) noexcept 
        : SuperType_(_queue)
    {
        TLockGuard lock_guard(_queue.mutex_);
        queue_ = _queue.queue_;

    }

    /**
     * @brief Move constructor.
     * Thread-safe move construction. Locks the source queue before moving.
     * @param _queue The queue to move from.
     */
    FORCEINLINE TQueueSafe(TQueueSafe&& _queue) noexcept
        : SuperType_(std::move(_queue))
        , mutex_()
    {
        TLockGuard lock_guard(_queue.mutex_);
        queue_ = std::move(_queue.queue_);
    }

    /**
     * @brief Constructs a queue with a specific size.
     * @param _size The initial size of the queue.
     */
    FORCEINLINE TQueueSafe(SizeType _size) noexcept 
        : SuperType_()
        , mutex_()
        , queue_(_size) 
    {}

    /**
     * @brief Constructs a queue with a specific size and fills it with a value.
     * @param _size The initial size.
     * @param _val The value to fill the queue with.
     */
    FORCEINLINE TQueueSafe(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_()
        , mutex_()
        , queue_(_size, _val) 
    {}

    /**
     * @brief Range constructor.
     * Constructs a queue with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TQueueSafe(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_()
        , mutex_()
        , queue_(_first, _last) 
    {}

    /**
     * @brief Initializer list constructor.
     * Constructs a queue with the contents of the initializer list.
     * @param _init_list The initializer list.
     */
    FORCEINLINE TQueueSafe(InitializerList_ _init_list) noexcept 
        : SuperType_()
        , mutex_()
        , queue_(_init_list) 
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TQueueSafe() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * Thread-safe assignment. Locks both this queue and the source queue to prevent race conditions.
     * @param _queue The queue to copy from.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueueSafe& operator=(const TQueueSafe& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        SuperType_::operator=(_queue);
        queue_ = _queue.queue_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Thread-safe move assignment. Locks both queues.
     * @param _queue The queue to move from.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueueSafe& operator=(TQueueSafe&& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        SuperType_::operator=(std::move(_queue));
        queue_ = std::move(_queue.queue_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * Replaces the contents with those from the initializer list. Locks this queue.
     * @param _init_list The initializer list.
     * @return A reference to this queue.
     */
    FORCEINLINE TQueueSafe& operator=(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        queue_ = _init_list;
        return *this;
    }

    /**
     * @brief Equality comparison operator.
     * Thread-safe comparison. Locks both queues.
     * @param _queue The queue to compare with.
     * @return True if the queues are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TQueueSafe& _queue) noexcept { 
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        return queue_ == _queue.queue_; 
    }

    /**
     * @brief Inequality comparison operator.
     * Thread-safe comparison. Locks both queues.
     * @param _queue The queue to compare with.
     * @return True if the queues are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TQueueSafe& _queue) noexcept { 
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        return queue_ != _queue.queue_; 
    }

    /**
     * @brief Returns the number of elements in the queue.
     * @return The number of elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_.GetSize(); 
    }

    /**
     * @brief Returns the capacity of the queue.
     * @return The number of elements the queue can hold before reallocating.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return queue_.GetCapacity(); 
    }

    /**
     * @brief Pop the front element from the queue safely.
     */
    FORCEINLINE Void PopFront() noexcept {
        TLockGuard lock_guard(mutex_);
        queue_.PopFront();
    }

    /**
     * @brief Tries to pop the front element from the queue safely.
     * If the queue is not empty, the front element is moved to _out_obj
     * and removed from the queue.
     * @param _out_obj Reference to store the popped element.
     * @return True if an element was popped (queue was not empty), false otherwise.
     */
    NODISCARD FORCEINLINE Bool TryPopFront(_ObjectType& _out_obj) noexcept {
        TLockGuard lock_guard(mutex_);
        
        // check if the queue is empty
        if (queue_.IsEmpty()) {
            return false;
        }

        // pop the front element and move it to _out_obj
        _out_obj = std::move(queue_.Front());
        queue_.PopFront();

        return true;
    }

    /**
     * @brief Adds an element to the end of the queue.
     * @param _val The value to add.
     */
    FORCEINLINE decltype(auto) PushBack(const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.PushBack(_val);
    }

    /**
     * @brief Adds an element to the end of the queue (move semantics).
     * @param _val The value to move into the queue.
     */
    FORCEINLINE decltype(auto) PushBack(_ObjectType&& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.PushBack(std::move(_val));
    }

    /**
     * @brief Constructs and adds an element to the end of the queue.
     * @tparam ArgsType The types of the arguments.
     * @param _args The arguments to forward to the constructor of the element.
     */
    template <typename... ArgsType>
    FORCEINLINE decltype(auto) EmplaceBack(ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.EmplaceBack(std::forward<ArgsType>(_args)...);
    }

    /**
     * @brief Clears the contents of the queue.
     */
    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_);
        queue_.Clear(); 
    }

    /**
     * @brief Reserves storage for at least the specified number of elements.
     * @param _size The new capacity of the queue.
     */
    FORCEINLINE Void Reserve(SizeType _size) noexcept { 
        TLockGuard lock_guard(mutex_); 
        queue_.Reserve(_size); 
    }

    /**
     * @brief Swaps the contents of this queue with another safely.
     * Locks both queues to ensure thread safety.
     * @param _queue The queue to swap with.
     */
    FORCEINLINE Void Swap(TQueueSafe& _queue) noexcept { 
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        queue_.Swap(_queue.queue_); 
    }

private:
    /** @brief Mutex to ensure thread safety, cache aligned to avoid false sharing. */
    CACHE_ALIGN mutable ZMutex mutex_;
    /** @brief The underlying container used to store elements. */
    _ContainerType queue_;
};

} // zengine

#pragma warning(default: 4834)