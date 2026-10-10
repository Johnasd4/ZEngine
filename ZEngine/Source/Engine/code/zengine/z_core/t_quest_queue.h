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
#include "t_unique_lock.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"

#pragma warning(disable: 4834)

namespace zengine {

/**
 * @brief Thread-safe queue designed for concurrent producer-consumer data workflows.
 *
 * Implementation Principles:
 * This class wraps a standard-like deque container with a mutable ZMutex and a ZConditionVariable
 * to provide safe concurrent access. To avoid false sharing on multi-core architectures, the class
 * and internal mutex use CACHE_ALIGN. The locking strategy utilizes minimal scope to reduce lock
 * contention, notifying condition variables safely outside the locked critical sections.
 * It features "drain semantics" upon being aborted: when Abort() is called, new pushes are securely
 * rejected, but active consumers can still safely empty all remaining items in the queue before
 * returning false and shutting down gracefully.
 *
 * @tparam _ObjectType The type of elements stored in the queue.
 * @tparam _ContainerType The underlying container type, defaults to TDeque using a global memory pool.
 */
template<typename _ObjectType, typename _ContainerType = TDeque<_ObjectType, DEFAULT_USE_GLOBAL_MEMORY_POOL>>
class CACHE_ALIGN TQuestQueue : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

    /** @brief Alias for the underlying initializer list type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor. Constructs an empty thread-safe queue.
     */
    FORCEINLINE TQuestQueue() noexcept
        : SuperType_()
        , mutex_()
        , aborted_(false)
        , queue_()
    {}

    /**
     * @brief Copy constructor.
     * Thread-safe copy construction. Locks the source queue before copying to avoid data races.
     *
     * @param _queue The target queue to copy data and aborted state from.
     */
    FORCEINLINE TQuestQueue(const TQuestQueue& _queue) noexcept
        : SuperType_(_queue)
        , mutex_()
    {
        TLockGuard lock_guard(_queue.mutex_);
        aborted_ = _queue.aborted_;
        queue_ = _queue.queue_;
    }

    /**
     * @brief Move constructor.
     * Thread-safe move construction. Locks the source queue before moving its contents.
     *
     * @param _queue The target queue to move data and aborted state from.
     */
    FORCEINLINE TQuestQueue(TQuestQueue&& _queue) noexcept
        : SuperType_(std::move(_queue))
        , mutex_()
    {
        TLockGuard lock_guard(_queue.mutex_);
        aborted_ = _queue.aborted_;
        queue_ = std::move(_queue.queue_);
    }

    /**
     * @brief Constructs a queue with a specific initial size.
     *
     * @param _size The initial size of the queue.
     */
    FORCEINLINE TQuestQueue(SizeType _size) noexcept
        : SuperType_()
        , mutex_()
        , aborted_(false)
        , queue_(_size)
    {}

    /**
     * @brief Constructs a queue with a specific size and fills it with a designated value.
     *
     * @param _size The initial size of the queue.
     * @param _val The value to fill the queue elements with.
     */
    FORCEINLINE TQuestQueue(SizeType _size, const _ObjectType& _val) noexcept
        : SuperType_()
        , mutex_()
        , aborted_(false)
        , queue_(_size, _val)
    {}

    /**
     * @brief Range constructor. Constructs a queue with the contents of the range [_first, _last).
     *
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range to copy from.
     * @param _last The end of the range to copy to.
     */
    template <typename _InputIterator>
    FORCEINLINE TQuestQueue(_InputIterator _first, _InputIterator _last) noexcept
        : SuperType_()
        , mutex_()
        , aborted_(false)
        , queue_(_first, _last)
    {}

    /**
     * @brief Initializer list constructor. Constructs a queue with contents from an initializer list.
     *
     * @param _init_list The initializer list to populate the queue with.
     */
    FORCEINLINE TQuestQueue(InitializerList_ _init_list) noexcept
        : SuperType_()
        , mutex_()
        , aborted_(false)
        , queue_(_init_list)
    {}

    /**
     * @brief Destructor. Automatically cleans up resources upon object destruction.
     */
    FORCEINLINE ~TQuestQueue() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * Thread-safe assignment. Locks both this queue and the source queue safely to prevent deadlocks.
     *
     * @param _queue The queue to safely copy contents and state from.
     * @return A reference to this updated queue instance.
     */
    TQuestQueue& operator=(const TQuestQueue& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        {
            T2LockGuard lock_guard(mutex_, _queue.mutex_);
            SuperType_::operator=(_queue);
            aborted_ = _queue.aborted_;
            queue_ = _queue.queue_;
        }
        cv_.NotifyAll();
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Thread-safe move assignment. Locks both queues safely before transferring internal state.
     *
     * @param _queue The queue to safely move contents and state from.
     * @return A reference to this updated queue instance.
     */
    TQuestQueue& operator=(TQuestQueue&& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        {
            T2LockGuard lock_guard(mutex_, _queue.mutex_);
            SuperType_::operator=(std::move(_queue));
            aborted_ = _queue.aborted_;
            queue_ = std::move(_queue.queue_);
        }
        cv_.NotifyAll();
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * Thread-safe assignment from an initializer list. Also resets the aborted state.
     *
     * @param _init_list The initializer list used to entirely replace the queue contents.
     * @return A reference to this updated queue instance.
     */
    TQuestQueue& operator=(InitializerList_ _init_list) noexcept {
        {
            TLockGuard lock_guard(mutex_);
            aborted_ = false;
            queue_ = _init_list;
        }
        cv_.NotifyAll();
        return *this;
    }

    /**
     * @brief Equality comparison operator.
     * Thread-safe comparison. Locks both queues before comparing underlying contents.
     *
     * @param _queue The target queue to compare against.
     * @return True if the internal contents of both queues are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TQuestQueue& _queue) noexcept {
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        return queue_ == _queue.queue_;
    }

    /**
     * @brief Inequality comparison operator.
     * Thread-safe comparison. Locks both queues before comparing underlying contents.
     *
     * @param _queue The target queue to compare against.
     * @return True if the internal contents of the queues are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TQuestQueue& _queue) noexcept {
        T2LockGuard lock_guard(mutex_, _queue.mutex_);
        return queue_ != _queue.queue_;
    }

    /**
     * @brief Returns the number of elements safely currently inside the queue.
     *
     * @return The number of total elements within the queue.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.GetSize();
    }

    /**
     * @brief Returns the capacity of the queue safely.
     *
     * @return The maximum number of elements the queue can hold before a dynamic reallocation occurs.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept {
        TLockGuard lock_guard(mutex_);
        return queue_.GetCapacity();
    }

    /**
     * @brief Safely blocks and attempts to pop the front element from the queue.
     * Implements drain semantics: ensures waiting threads can consume remaining items during an abort.
     *
     * @param _obj_ptr Pointer to store the popped element. Can be nullptr if only popping is needed.
     * @return True if an element was successfully popped, false if the queue is empty AND aborted.
     */
    FORCEINLINE Bool WaitAndPopFront(_ObjectType* _obj_ptr) noexcept {
        TUniqueLock lock(mutex_);
        while (queue_.IsEmpty() && !aborted_) {
            cv_.Wait(lock);
        }

        if (queue_.IsEmpty() && aborted_) {
            return false;
        }

        if (_obj_ptr != nullptr) {
            *_obj_ptr = std::move(queue_.Front());
        }
        queue_.PopFront();

        return true;
    }

    /**
     * @brief Thread-safe push operation to add a new copy of an element to the back of the queue.
     *
     * @param _val The constant reference to the value being added.
     * @return True if successfully pushed, false if the queue is aborted and rejected the value.
     */
    FORCEINLINE Bool PushBack(const _ObjectType& _val) noexcept {
        {
            TLockGuard lock_guard(mutex_);
            if(aborted_) {
                return false;
            }
            queue_.PushBack(_val);
        }
        cv_.NotifyOne();
        return true;
    }

    /**
     * @brief Thread-safe push operation using move semantics to add an element to the back of the queue.
     *
     * @param _val The r-value reference to the value being moved into the queue.
     * @return True if successfully pushed, false if the queue is aborted and rejected the value.
     */
    FORCEINLINE Bool PushBack(_ObjectType&& _val) noexcept {
        {
            TLockGuard lock_guard(mutex_);
            if (aborted_) {
                return false;
            }
            queue_.PushBack(std::move(_val));
        }
        cv_.NotifyOne();
        return true;
    }

    /**
     * @brief Thread-safe emplace operation to construct an element in place at the back of the queue.
     *
     * @tparam ArgsType The types of the arguments being forwarded to the constructor.
     * @param _args The arguments to forward to the constructor of the underlying element type.
     * @return True if successfully emplaced, false if the queue is aborted and rejected the item.
     */
    template <typename... ArgsType>
    FORCEINLINE Bool EmplaceBack(ArgsType&&... _args) noexcept {
        {
            TLockGuard lock_guard(mutex_);
            if (aborted_) {
                return false;
            }
            queue_.EmplaceBack(std::forward<ArgsType>(_args)...);
        }
        cv_.NotifyOne();
        return true;
    }

    /**
     * @brief Safely aborts the queue state to immediately reject new items and wakes up all waiting threads.
     * Allows currently blocked consumers to exit gracefully or consume residual elements.
     */
    Void Abort() noexcept {
        {
            TLockGuard lock_guard(mutex_);
            aborted_ = true;
        }
        cv_.NotifyAll();
    }

    /**
     * @brief Safely resets the aborted state of the queue, allowing it to start receiving elements again.
     */
    Void Reset() noexcept {
        TLockGuard lock_guard(mutex_);
        aborted_ = false;
    }

    /**
     * @brief Thread-safe operation to completely clear the current contents of the queue.
     */
    FORCEINLINE Void Clear() noexcept {
        TLockGuard lock_guard(mutex_);
        queue_.Clear();
    }

    /**
     * @brief Thread-safe operation to reserve storage capacity in advance.
     *
     * @param _size The target capacity size to reserve in the underlying container.
     */
    FORCEINLINE Void Reserve(SizeType _size) noexcept {
        TLockGuard lock_guard(mutex_);
        queue_.Reserve(_size);
    }

    /**
     * @brief Swaps the contents and states of this queue with another securely without deadlocks.
     * Triggers a notification on both condition variables as elements have been displaced.
     *
     * @param _queue The target queue to swap states and contents with.
     */
    FORCEINLINE Void Swap(TQuestQueue& _queue) noexcept {
        {
            T2LockGuard lock_guard(mutex_, _queue.mutex_);
            queue_.Swap(_queue.queue_);
            zengine::Swap(&aborted_, &_queue.aborted_);
        }
        cv_.NotifyAll();
        _queue.cv_.NotifyAll();
    }

private:
    /** @brief Mutex to ensure thread safety, cache aligned to avoid false sharing. */
    CACHE_ALIGN mutable ZMutex mutex_;
    /** @brief Condition variable utilized to block consumers and wake them up correctly. */
    ZConditionVariable cv_;
    /** @brief Flag denoting the aborted state, rejecting pushes and notifying consumers. */
    Bool aborted_;
    /** @brief The underlying standard-like container acting as the physical storage. */
    _ContainerType queue_;
};

} // zengine

#pragma warning(default: 4834)