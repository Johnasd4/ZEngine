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

#include "t_atom.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A lock-free, thread-safe Single-Producer Single-Consumer (SPSC) queue with fixed capacity.
 *
 * Implementation Principle:
 * This queue utilizes a contiguous std::array of a fixed power-of-2 capacity. Index wrapping is handled
 * efficiently via unsigned integer overflow and a pre-calculated bitwise mask, avoiding expensive modulo
 * operations. To maximize concurrent throughput and eliminate cache line bouncing (false sharing),
 * consumer-written variables (front_index_, back_index_cache_) and producer-written variables
 * (back_index_, front_index_cache_) are separated and aligned using CACHE_ALIGN.
 *
 * @tparam _ObjectType The type of elements stored in the queue.
 * @tparam _Capacity The maximum capacity of the queue (must be a power of 2).
 * @tparam _IfCallConstructorAndDestructor Determines whether to call constructor/destructor on elements.
 */
template<
    typename _ObjectType, 
    SizeType _Capacity, 
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
class WORD_ALIGN TQueueSPSC : public ZObject {
    STATIC_ASSERT((_Capacity& (_Capacity - 1)) == 0, "TFixedQueueSPSC: _Capacity not power of 2!");
    STATIC_ASSERT(_Capacity > 0, "TFixedQueueSPSC: _Capacity must be positive!");
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

    /** @brief Alias for the underlying standard array type used for fixed storage. */
    using STDFixedArray_ = std::array<_ObjectType, _Capacity>;

    /** @brief Pre-calculated mask for rapid bitwise modulo operations to wrap indices. */
    static constexpr SizeType kCapacityMask = _Capacity - 1;

    /** @brief Default constructor. Initializes an empty double-ended queue. */
    FORCEINLINE TFixedQueueSPSC() noexcept 
        : SuperType_()
        , deque_()
        , front_index_(0ULL)
        , back_index_cache_(0ULL)
        , back_index_(0ULL)
        , front_index_cache_(0ULL)
    {}

    /**
     * @brief Copy constructor. Creates a deep copy of another deque instance.
     * @param _deque The source deque to copy the content from.
     */
    FORCEINLINE TFixedQueueSPSC(const TFixedQueueSPSC& _deque) noexcept : SuperType_(_deque){ CopyP(_deque); }

    /**
     * @brief Move constructor. Transfers the contents from another deque, leaving the source empty.
     * @param _deque The source deque to move the content from.
     */
    FORCEINLINE TFixedQueueSPSC(TFixedQueueSPSC&& _deque) noexcept 
        : SuperType_(std::move(_deque)) 
    {
        MoveP(std::move(_deque)); 
    }

    /**
     * @brief Variadic constructor to initialize array elements sequentially.
     * @tparam _ArgsType The types of the provided variadic arguments.
     * @param _args The arguments used to initialize the underlying array buffer.
     */
    template<typename... _ArgsType>
    requires(sizeof...(_ArgsType) == _Capacity)
    FORCEINLINE TFixedQueueSPSC(_ArgsType&&... _args) noexcept
        : SuperType_()
        , deque_{ static_cast<_ObjectType>(std::forward<_ArgsType>(_args))... }
        , front_index_(0ULL)
        , back_index_cache_(_Capacity)
        , back_index_(_Capacity)
        , front_index_cache_(0ULL)
    {}

    /**
     * @brief Destructor. Destroys the container and releases its resources.
     */
    FORCEINLINE ~TFixedQueueSPSC() noexcept = default;

    /**
     * @brief Copy assignment operator. Replaces the current contents with a copy from another deque.
     * @param _deque The source deque to copy from.
     * @return Reference to this deque instance.
     */
    FORCEINLINE TFixedQueueSPSC& operator=(const TFixedQueueSPSC& _deque) noexcept {
        if (this == &_deque) {
            return *this;
        }
        SuperType_::operator=(_deque);
        CopyP(_deque);
        return *this;
    }

    /**
     * @brief Move assignment operator. Replaces the current contents by moving from another deque.
     * @param _deque The source deque to move from.
     * @return Reference to this deque instance.
     */
    FORCEINLINE TFixedQueueSPSC& operator=(TFixedQueueSPSC&& _deque) noexcept {
        if (this == &_deque) {
            return *this;
        }
        SuperType_::operator=(std::move(_deque));
        MoveP(std::move(_deque));
        return *this;
    }

    /**
     * @brief Retrieves the current number of valid elements stored inside the deque.
     * @return The size of the active elements.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { 
        return back_index_.Load(ZMemoryOrderEnum::kRelaxed) - front_index_.Load(ZMemoryOrderEnum::kRelaxed);
    }

    /**
     * @brief Retrieves the absolute maximum capacity the deque can hold.
     * @return The predefined maximum capacity.
     */
    NODISCARD FORCEINLINE static SizeType GetCapacity() noexcept { return _Capacity; }

    /**
     * @brief Equality operator to strictly compare whether two deques are the identical memory instance.
     * @param _deque The target deque to compare against.
     * @return True if both references point to the same physical object instance, False otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TFixedQueueSPSC& _deque) noexcept { 
        return this == &_deque; 
    }

    /**
     * @brief Inequality operator to strictly compare whether two deques are different memory instances.
     * @param _deque The target deque to compare against.
     * @return True if they are distinct object instances, False otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TFixedQueueSPSC& _deque) noexcept { 
        return this != &_deque;
    }

    /**
     * @brief Inserts a copy of the given object to the physical back of the deque storage.
     * @param _obj The constant object source to copy into the back slot.
     * @return True if the push was successful, False if the deque has exhausted its capacity.
     */
    NODISCARD Bool PushBack(const _ObjectType& _obj) noexcept {
        SizeType current_back_index = back_index_.Load(ZMemoryOrderEnum::kRelaxed);

        // check if the queue is full
        if (current_back_index - front_index_cache_ >= _Capacity) {
            front_index_cache_ = front_index_.Load(ZMemoryOrderEnum::kAcquire);
            if (current_back_index - front_index_cache_ >= _Capacity) {
                return false;
            }
        }

        // insert the object into the back slot
        deque_[current_back_index & kCapacityMask] = _obj;

        // increment the back index to logically append the element
        back_index_.Store(current_back_index + 1, ZMemoryOrderEnum::kRelease);
        return true;
    }

    /**
     * @brief Inserts an object utilizing move semantics to the physical back of the deque.
     * @param _obj The rvalue object reference to move into the back slot.
     * @return True if the move push was successful, False if the deque has exhausted its capacity.
     */
    NODISCARD Bool PushBack(_ObjectType&& _obj) noexcept {
        SizeType current_back_index = back_index_.Load(ZMemoryOrderEnum::kRelaxed);

        // check if the queue is full
        if (current_back_index - front_index_cache_ >= _Capacity) {
            front_index_cache_ = front_index_.Load(ZMemoryOrderEnum::kAcquire);
            if (current_back_index - front_index_cache_ >= _Capacity) {
                return false;
            }
        }

        // insert the object into the back slot using move semantics
        deque_[current_back_index & kCapacityMask] = std::move(_obj);

        // increment the back index to logically append the element
        back_index_.Store(current_back_index + 1, ZMemoryOrderEnum::kRelease);
        return true;
    }

    /**
     * @brief Eliminates the element stored at the logical front of the deque container.
     * @return Void.
     */
    Void PopFront() noexcept {
        SizeType current_front_index = front_index_.Load(ZMemoryOrderEnum::kRelaxed);

        // check if the queue is empty
        if (back_index_cache_ == current_front_index) {
            back_index_cache_ = back_index_.Load(ZMemoryOrderEnum::kAcquire);
            if (back_index_cache_ == current_front_index) {
                return;
            }
        }

        // reset the slot if the flag is enabled
        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[current_front_index & kCapacityMask] = _ObjectType();
        }

        // increment the front index to logically remove the element
        front_index_.Store(current_front_index + 1, ZMemoryOrderEnum::kRelease);
        return;
    }

    /**
     * @brief Eliminates the element stored at the logical front of the deque container and retrieves it.
     * @param _out_obj The output parameter to store the popped element.
     * @return True if the element was successfully popped, False if the deque was already empty.
     */
    NODISCARD Bool TryPopFront(_ObjectType& _out_obj) noexcept {
        SizeType current_front_index = front_index_.Load(ZMemoryOrderEnum::kRelaxed);

        // check if the queue is empty
        if (back_index_cache_ == current_front_index) {
            back_index_cache_ = back_index_.Load(ZMemoryOrderEnum::kAcquire);
            if (back_index_cache_ == current_front_index) {
                return false;
            }
        }

        // move the object out of the deque and into the output parameter
        _out_obj = std::move(deque_[current_front_index & kCapacityMask]);

        // reset the slot if the flag is enabled
        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[current_front_index & kCapacityMask] = _ObjectType();
        }

        // increment the front index to logically remove the element
        front_index_.Store(current_front_index + 1, ZMemoryOrderEnum::kRelease);
        return true;
    }  

    /**
     * @brief In-place constructs a brand new element strictly at the back boundary of the container.
     * @tparam _ArgsType The arbitrary variadic parameter types expected by the object's constructor.
     * @param _args The direct parameters forwarded for the element's construction.
     * @return True if the construction succeeded, False if the deque has reached capacity limits.
     */
    template<typename... _ArgsType>
    NODISCARD Bool EmplaceBack(_ArgsType&&... _args) noexcept {
        SizeType current_back_index = back_index_.Load(ZMemoryOrderEnum::kRelaxed);

        // check if the queue is full
        if (current_back_index - front_index_cache_ >= _Capacity) {
            front_index_cache_ = front_index_.Load(ZMemoryOrderEnum::kAcquire);
            if (current_back_index - front_index_cache_ >= _Capacity) {
                return false;
            }
        }

        // construct the object in-place at the back slot
        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[current_back_index & kCapacityMask] = _ObjectType(std::forward<_ArgsType>(_args)...);
        } else {
            new (&deque_[current_back_index & kCapacityMask]) _ObjectType(std::forward<_ArgsType>(_args)...);
        }

        // increment the back index to logically append the element
        back_index_.Store(current_back_index + 1, ZMemoryOrderEnum::kRelease);
        return true;
    }

    /**
     * @brief Wholly resets the complete deque structure, aggressively pushing indices back to zero.
     * @return Void.
     */
    Void Clear() noexcept { 
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType current_front_index = front_index_.Load(ZMemoryOrderEnum::kRelaxed);
            SizeType current_back_index = back_index_.Load(ZMemoryOrderEnum::kRelaxed);
            for (SizeType index = current_front_index; index != current_back_index; ++index) {
                deque_[index & kCapacityMask] = _ObjectType();
            }
        }
        front_index_.Store(0ULL, ZMemoryOrderEnum::kRelaxed);
        back_index_cache_ = 0ULL;
        back_index_.Store(0ULL, ZMemoryOrderEnum::kRelaxed);
        front_index_cache_ = 0ULL;
    }

    /**
     * @brief Exhaustively populates the maximal capacity of the deque with the target specified value.
     * @param _value The definitive value instance to fill the internal buffer up with.
     * @return Void.
     */
    Void Fill(const _ObjectType& _value) noexcept {
        deque_.fill(_value); 
        front_index_.Store(0ULL, ZMemoryOrderEnum::kRelaxed);
        back_index_cache_ = _Capacity;
        back_index_.Store(_Capacity, ZMemoryOrderEnum::kRelaxed);
        front_index_cache_ = 0ULL;
    }

    /**
     * @brief Performs a full and cohesive content exchange between this deque and the specified counterpart.
     * @param _deque The identical counterpart deque to swap structural data with.
     * @return Void.
     */
    FORCEINLINE Void Swap(TFixedQueueSPSC& _deque) noexcept { 
        deque_.swap(_deque.deque_); 
        SizeType temp_index = front_index_.Load(ZMemoryOrderEnum::kRelaxed);
        front_index_.Store(_deque.front_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        _deque.front_index_.Store(temp_index, ZMemoryOrderEnum::kRelaxed);
        ::zengine::Swap(&back_index_cache_, &_deque.back_index_cache_);
        temp_index = back_index_.Load(ZMemoryOrderEnum::kRelaxed);
        back_index_.Store(_deque.back_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        _deque.back_index_.Store(temp_index, ZMemoryOrderEnum::kRelaxed);
        ::zengine::Swap(&front_index_cache_, &_deque.front_index_cache_);
    }

private:
    /**
     * @brief Internal foundational utility intended to rigidly duplicate the properties of another deque.
     * @param _deque The distinct external deque reference targeting for deep copy extraction.
     * @return Void.
     */
    FORCEINLINE Void CopyP(const TFixedQueueSPSC& _deque) noexcept {
        deque_ = _deque.deque_;
        front_index_.Store(_deque.front_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        back_index_cache_ = _deque.back_index_cache_;
        back_index_.Store(_deque.back_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        front_index_cache_ = _deque.front_index_cache_;
    }

    /**
     * @brief Internal foundational utility intended to safely extract properties from another target deque.
     * @param _deque The distinct external deque rvalue reference targeting for logic extraction.
     * @return Void.
     */
    FORCEINLINE Void MoveP(TFixedQueueSPSC&& _deque) noexcept {
        deque_ = std::move(_deque.deque_);
        front_index_.Store(_deque.front_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        back_index_cache_ = _deque.back_index_cache_;
        back_index_.Store(_deque.back_index_.Load(ZMemoryOrderEnum::kRelaxed), ZMemoryOrderEnum::kRelaxed);
        front_index_cache_ = _deque.front_index_cache_;
        _deque.front_index_.Store(0ULL, ZMemoryOrderEnum::kRelaxed);
        _deque.back_index_cache_ = 0ULL;
        _deque.back_index_.Store(0ULL, ZMemoryOrderEnum::kRelaxed);
        _deque.front_index_cache_ = 0ULL;
    }

    /** @brief Fundamental rigid-size standard array allocating the underlying physical block topology. */
    STDFixedArray_ deque_;
    /** @brief Absolute unmasked incremental index referencing the subsequent free front queue space. */
    CACHE_ALIGN TAtom<SizeType> front_index_;
    /** @brief Cached value of the back index used by the consumer to avoid redundant atomic reads. */
    SizeType back_index_cache_;
    /** @brief Absolute unmasked incremental index pointing dynamically to the true rear filled element. */
    CACHE_ALIGN TAtom<SizeType> back_index_;
    /** @brief Cached value of the front index used by the producer to avoid redundant atomic reads. */
    SizeType front_index_cache_;
};

} // zengine