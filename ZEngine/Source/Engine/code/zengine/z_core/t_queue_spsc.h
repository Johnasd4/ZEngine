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
 * @brief High-performance Single-Producer Single-Consumer (SPSC) lock-free FIFO queue.
 *
 * Implementation Principle:
 * This container wraps moodycamel::ReaderWriterQueue and operates on a block-based circular ring buffer
 * architecture. The internal storage consists of linked memory blocks where each individual block functions
 * as a contiguous ring buffer using atomic head and tail indices for wait-free execution.
 * When preallocated capacity is exceeded during enqueue operations, new blocks of exponential sizes (capped
 * at _MaxBlockSize) are dynamically allocated and linked to accommodate bursts without blocking the consumer
 * thread or invalidating existing data.
 *
 * @tparam _ObjectType The type of the elements stored within the queue.
 * @tparam _MaxBlockSize Upper capacity limit for dynamically allocated individual blocks (must be a power of 2).
 */
template<
    typename _ObjectType, 
    SizeType _MaxBlockSize = 512
>
class TQueueSPSC : public ZObject {
public:
    STATIC_ASSERT((_MaxBlockSize& (_MaxBlockSize - 1)) == 0, "TQueueSPSC: _MaxBlockSize not power of 2!");
    STATIC_ASSERT(_MaxBlockSize > 0, "TQueueSPSC: _MaxBlockSize must be positive!");
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

    /** @brief Alias for the underlying moodycamel SPSC queue. */
    using QueueType_ = moodycamel::ReaderWriterQueue<_ObjectType, _MaxBlockSize>;

    /**
     * @brief Explicit constructor. Initializes an empty queue with preallocated element capacity.
     * @param _capacity The initial capacity of elements to reserve.
     */
    FORCEINLINE explicit TQueueSPSC(SizeType _capacity) noexcept
        : SuperType_()
        , queue_(_capacity)
    {}

    /** @brief Deleted copy constructor. */
    TQueueSPSC(const TQueueSPSC&) = delete;

    /**
     * @brief Move constructor. Transfers internal buffer ownership from another queue instance.
     * @param _deque The rvalue reference to the source queue to move from.
     */
    FORCEINLINE TQueueSPSC(TQueueSPSC&& _deque) noexcept 
        : SuperType_(std::move(_deque))
        , queue_(std::move(_deque.queue_))
    {}

    /**
     * @brief Destructor. Destroys all stored elements and releases underlying block resources.
     */
    FORCEINLINE ~TQueueSPSC() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    TQueueSPSC& operator=(const TQueueSPSC&) = delete;

    /**
     * @brief Move assignment operator. Transfers internal buffer ownership from another queue instance.
     * @param _deque The rvalue reference to the source queue to move from.
     * @return Reference to this queue instance.
     */
    FORCEINLINE TQueueSPSC& operator=(TQueueSPSC&& _deque) noexcept {
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
    NODISCARD FORCEINLINE Bool operator==(const TQueueSPSC& _deque) const noexcept {
        return this == &_deque; 
    }

    /**
     * @brief Strictly compares whether two queue instances reside at distinct memory addresses.
     * @param _deque The target queue instance to compare against.
     * @return True if both references point to different memory instances, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TQueueSPSC& _deque) const noexcept {
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
     * @brief Inserts an element by copy. Dynamically allocates blocks if initial capacity is exhausted.
     * @param _obj The const reference of the object to copy into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(const _ObjectType& _obj) noexcept {
        queue_.enqueue(_obj);
    }

    /**
     * @brief Inserts an element using move semantics. Dynamically allocates blocks if capacity is exhausted.
     * @param _obj The rvalue reference of the object to move into the queue.
     * @return Void.
     */
    FORCEINLINE Void PushBack(_ObjectType&& _obj) noexcept {
        queue_.enqueue(std::move(_obj));
    }

    /**
     * @brief In-place constructs an element at the back. Dynamically allocates blocks if capacity is exhausted.
     * @tparam _ArgsType Variadic argument types forwarded to the element's constructor.
     * @param _args The arguments forwarded for constructing the new element.
     * @return Void.
     */
    template<typename... _ArgsType>
    FORCEINLINE Void EmplaceBack(_ArgsType&&... _args) noexcept {
        queue_.emplace(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Attempts to enqueue an element by copy without allocating memory.
     * @param _obj The const reference of the object to copy into the queue.
     * @return True if the element was successfully enqueued, false if the queue is full.
     */
    NODISCARD FORCEINLINE Bool TryPushBack(const _ObjectType& _obj) noexcept {
        return queue_.try_enqueue(_obj);
    }

    /**
     * @brief Attempts to enqueue an element using move semantics without allocating memory.
     * @param _obj The rvalue reference of the object to move into the queue.
     * @return True if the element was successfully enqueued, false if the queue is full.
     */
    NODISCARD FORCEINLINE Bool TryPushBack(_ObjectType&& _obj) noexcept {
        return queue_.try_enqueue(std::move(_obj));
    }

    /**
     * @brief Attempts to in-place construct an element at the back without allocating memory.
     * @tparam _ArgsType Variadic argument types forwarded to the element's constructor.
     * @param _args The arguments forwarded for constructing the new element.
     * @return True if construction succeeded, false if the queue is full.
     */
    template<typename... _ArgsType>
    NODISCARD FORCEINLINE Bool TryEmplaceBack(_ArgsType&&... _args) noexcept {
        return queue_.try_emplace(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Attempts to dequeue the front element and outputs it through the provided parameter.
     * @param _out_obj Reference to the destination object where the dequeued element will be stored.
     * @return True if an element was successfully dequeued, false if the queue was empty.
     */
    NODISCARD FORCEINLINE Bool TryPopFront(_ObjectType& _out_obj) noexcept {
        return queue_.try_dequeue(_out_obj);
    }  

    /**
     * @brief Clears all elements from the queue.
     */
	Void Clear() noexcept {
		_ObjectType temp;
		while (queue_.try_dequeue(temp)) {}
	}

private:
    /** @brief Underlying moodycamel lock-free queue instance. */
    QueueType_ queue_;
};

} // zengine