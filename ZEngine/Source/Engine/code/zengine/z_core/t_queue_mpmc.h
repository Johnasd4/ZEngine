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

#include "internal/l_moodycamel.h"

#include "t_atom.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief High-performance Multi-Producer Multi-Consumer (MPMC) lock-free FIFO queue.
 *
 * Implementation Principle:
 * This container wraps moodycamel::ConcurrentQueue and operates on a decentralized, block-based sub-queue
 * architecture. Each producer thread can utilize an exclusive sub-queue via ProducerToken to eliminate lock
 * contention and cache-line bouncing. Internal storage is partitioned into linked contiguous memory blocks
 * allocated dynamically to accommodate bursts without blocking. Consumers utilize ConsumerTokens to track
 * search offsets and perform work-stealing across producer sub-queues via lightweight round-robin traversal,
 * ensuring high throughput and wait-free/lock-free guarantees across heterogeneous threads.
 *
 * @tparam _ObjectType The type of the elements stored within the queue.
 */
template<typename _ObjectType>
class TQueueMPMC : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

    /** @brief Alias for the underlying moodycamel concurrent queue type. */
    using QueueType_ = moodycamel::ConcurrentQueue<_ObjectType, internal::ZEngineQueueTraits>;

    /** @brief Alias for the producer token type used for zero-contention enqueue operations. */
    using ProducerToken_ = moodycamel::ProducerToken;

    /** @brief Alias for the consumer token type used for zero-contention dequeue operations. */
    using ConsumerToken_ = moodycamel::ConsumerToken;

    /**
     * @brief Default explicit constructor. Initializes an empty MPMC queue with default reserved capacity.
     */
    FORCEINLINE explicit TQueueMPMC() noexcept
        : SuperType_()
        , queue_()
    {}

    /**
     * @brief Explicit constructor. Initializes an empty MPMC queue with preallocated element capacity.
     * @param _capacity The initial element capacity reserved in advance.
     */
    FORCEINLINE explicit TQueueMPMC(SizeType _capacity) noexcept
        : SuperType_()
        , queue_(_capacity)
    {}

    TQueueMPMC(const TQueueMPMC&) = delete;

    /**
     * @brief Move constructor. Transfers internal buffer ownership from another queue instance.
     * @param _deque The rvalue reference to the source queue to move from.
     */
    FORCEINLINE TQueueMPMC(TQueueMPMC&& _deque) noexcept 
        : SuperType_(std::move(_deque))
        , queue_(std::move(_deque.queue_))
    {}

    /**
     * @brief Destructor. Destroys all stored elements and releases underlying block resources.
     */
    FORCEINLINE ~TQueueMPMC() noexcept = default;

    TQueueMPMC& operator=(const TQueueMPMC&) = delete;

    /**
     * @brief Move assignment operator. Transfers internal buffer ownership from another queue instance.
     * @param _deque The rvalue reference to the source queue to move from.
     * @return Reference to this queue instance.
     */
    FORCEINLINE TQueueMPMC& operator=(TQueueMPMC&& _deque) noexcept {
        if (this != &_deque) {
            SuperType_::operator=(std::move(_deque));
            queue_ = std::move(_deque.queue_);
        }
        return *this;
    }

    /**
     * @brief Strictly compares whether two queue instances share the exact same physical memory address.
     * @param _deque The target queue instance to compare against.
     * @return True if both references point to the identical memory instance, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TQueueMPMC& _deque) const noexcept {
        return this == &_deque; 
    }

    /**
     * @brief Strictly compares whether two queue instances reside at distinct memory addresses.
     * @param _deque The target queue instance to compare against.
     * @return True if both references point to different memory instances, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TQueueMPMC& _deque) const noexcept {
        return this != &_deque;
    }

    /**
     * @brief Retrieves the estimated quantity of active elements currently contained within the queue.
     * @return The approximate number of valid elements.
     */
    NODISCARD FORCEINLINE SizeType GetApproximateSize() const noexcept {
        return static_cast<SizeType>(queue_.size_approx());
    }

    /**
     * @brief Factory method creating an associated producer token for zero-contention thread-local pushes.
     * @return A newly constructed ProducerToken_ instance bound to this queue.
     */
    NODISCARD FORCEINLINE ProducerToken_ MakeProducerToken() noexcept {
        return ProducerToken_(queue_);
    }

    /**
     * @brief Factory method creating an associated consumer token for optimized thread-local pops.
     * @return A newly constructed ConsumerToken_ instance bound to this queue.
     */
    NODISCARD FORCEINLINE ConsumerToken_ MakeConsumerToken() noexcept {
        return ConsumerToken_(queue_);
    }

    /**
     * @brief Inserts an element by copy using implicit thread-to-subqueue lookup.
     * @param _obj The const reference of the object to copy into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(const _ObjectType& _obj) noexcept {
        queue_.enqueue(_obj);
    }

    /**
     * @brief Inserts an element using move semantics with implicit thread-to-subqueue lookup.
     * @param _obj The rvalue reference of the object to move into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(_ObjectType&& _obj) noexcept {
        queue_.enqueue(std::move(_obj));
    }

    /**
     * @brief In-place constructs an element at the back using implicit thread-to-subqueue lookup.
     * @tparam _ArgsType Variadic argument types forwarded to the element's constructor.
     * @param _args The arguments forwarded for constructing the new element.
     * @return Void.
     */
    template<typename... _ArgsType>
    FORCEINLINE Void EmplaceBack(_ArgsType&&... _args) noexcept {
        queue_.enqueue(_ObjectType(std::forward<_ArgsType>(_args)...));
    }

    /**
     * @brief Inserts an element by copy using an explicit producer token to bypass lookup contention.
     * @param _token Reference to the producer token bound to the calling thread.
     * @param _obj The const reference of the object to copy into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(ProducerToken_& _token, const _ObjectType& _obj) noexcept {
        queue_.enqueue(_token, _obj);
    }

    /**
     * @brief Inserts an element using move semantics and an explicit producer token to bypass lookup contention.
     * @param _token Reference to the producer token bound to the calling thread.
     * @param _obj The rvalue reference of the object to move into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(ProducerToken_& _token, _ObjectType&& _obj) noexcept {
        queue_.enqueue(_token, std::move(_obj));
    }

    /**
     * @brief In-place constructs an element using an explicit producer token to bypass lookup contention.
     * @tparam _ArgsType Variadic argument types forwarded to the element's constructor.
     * @param _token Reference to the producer token bound to the calling thread.
     * @param _args The arguments forwarded for constructing the new element.
     * @return Void.
     */
    template<typename... _ArgsType>
    FORCEINLINE Void EmplaceBack(ProducerToken_& _token, _ArgsType&&... _args) noexcept {
        queue_.enqueue(_token, _ObjectType(std::forward<_ArgsType>(_args)...));
    }

    /**
     * @brief Attempts to dequeue the front element using implicit round-robin sub-queue traversal.
     * @param _out_obj Reference to the destination object where the dequeued element will be stored.
     * @return True if an element was successfully dequeued, false if the queue was empty.
     */
    NODISCARD FORCEINLINE Bool TryPopFront(_ObjectType& _out_obj) noexcept {
        return queue_.try_dequeue(_out_obj);
    }

    /**
     * @brief Attempts to dequeue the front element using an explicit consumer token to optimize search locality.
     * @param _token Reference to the consumer token bound to the calling thread.
     * @param _out_obj Reference to the destination object where the dequeued element will be stored.
     * @return True if an element was successfully dequeued, false if the queue was empty.
     */
    NODISCARD FORCEINLINE Bool TryPopFront(ConsumerToken_& _token, _ObjectType& _out_obj) noexcept {
        return queue_.try_dequeue(_token, _out_obj);
    }

    /**
     * @brief Clears all elements from the queue.
     */
	Void Clear() noexcept {
		_ObjectType temp;
		while (queue_.try_dequeue(temp)) {}
	}

    /**
     * @brief Clears all elements from the queue.]
	 * @param _token Reference to the consumer token bound to the calling thread.
     */
    Void Clear(ConsumerToken_& _token) noexcept {
        _ObjectType temp;
        while (queue_.try_dequeue(_token, temp)) {}
    }

private:
    /** @brief Underlying moodycamel concurrent queue instance. */
    QueueType_ queue_;
};

} // zengine