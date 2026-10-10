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

#include "z_object.h"

namespace zengine {

/**
 * @brief Fixed-size double-ended queue container based on a ring buffer.
 * 
 * Implementation Principles:
 * This container implements a ring buffer by mapping logical indices to physical memory 
 * without using the modulo (%) operator. Instead, it relies on a bitwise AND operation (&) 
 * with a pre-calculated mask (kSizeMask). This requires that _Capacity is a strict power of 2. 
 * The physical front points at the next available space, while the back points at the last 
 * pushed element. The logical size is dynamically evaluated via the difference 
 * (back_index_ - front_index_), utilizing unsigned integer arithmetic overflow to handle 
 * index wraparounds.
 * 
 * @tparam _ObjectType The type of elements stored in the deque.
 * @tparam _Capacity The fixed maximum capacity of the deque. Must be a strict positive power of 2.
 * @tparam _IfCallConstructorAndDestructor Flag to determine if objects need manual reset on removal.
 */
template<
    typename _ObjectType, 
    SizeType _Capacity, 
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
class WORD_ALIGN TFixedDeque : public ZObject {
    STATIC_ASSERT((_Capacity & (_Capacity - 1)) == 0, "TFixedDeque: _Capacity not power of 2!");
    STATIC_ASSERT(_Capacity > 0, "TFixedDeque: _Capacity must be positive!");

public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying standard array type used for fixed storage. */
    using STDFixedArray_ = std::array<_ObjectType, _Capacity>;
    /** @brief Pre-calculated mask for bitwise modulo operations to wrap indices. */
    static constexpr SizeType kSizeMask = _Capacity - 1;

    /**
     * @brief Default constructor. Initializes an empty double-ended queue.
     */
    FORCEINLINE constexpr TFixedDeque() noexcept 
        : SuperType_()
        , deque_()
        , front_index_(0ULL)
        , back_index_(0ULL)
    {}

    /**
     * @brief Copy constructor. Creates a copy of another deque instance.
     * @param _deque The source deque to copy the content from.
     */
    FORCEINLINE constexpr TFixedDeque(const TFixedDeque& _deque) noexcept 
        : SuperType_(_deque)
    {
        CopyP(_deque);
    }

    /**
     * @brief Move constructor. Transfers the contents from another deque, leaving the source empty.
     * @param _deque The source deque to move the content from.
     */
    FORCEINLINE constexpr TFixedDeque(TFixedDeque&& _deque) noexcept 
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
    FORCEINLINE constexpr TFixedDeque(_ArgsType&&... _args) noexcept
        : SuperType_()
        , deque_{ static_cast<_ObjectType>(std::forward<_ArgsType>(_args))... }
        , front_index_(0ULL)
        , back_index_(_Capacity)
    {}

    /**
     * @brief Destructor. Destroys the container and releases its resources.
     */
    FORCEINLINE constexpr ~TFixedDeque() noexcept = default;

    /**
     * @brief Copy assignment operator. Replaces the current contents with a copy from another deque.
     * @param _deque The source deque to copy from.
     * @return Reference to this deque instance.
     */
    FORCEINLINE TFixedDeque& operator=(const TFixedDeque& _deque) noexcept {
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
    FORCEINLINE TFixedDeque& operator=(TFixedDeque&& _deque) noexcept {
        if (this == &_deque) {
            return *this;
        }
        SuperType_::operator=(std::move(_deque));
        MoveP(std::move(_deque));
        return *this;
    }

    /**
     * @brief Checks whether the deque contains no elements.
     * @return True if the deque is empty, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool IsEmpty() const noexcept { 
        return front_index_ == back_index_; 
    }

    /**
     * @brief Checks whether the deque has reached its maximum capacity.
     * @return True if the deque is full, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool IsFull() const noexcept { 
        return back_index_ - front_index_ == _Capacity; 
    }

    /**
     * @brief Retrieves the current number of elements stored inside the deque.
     * @return The size of the active elements.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { 
        return back_index_ - front_index_; 
    }

    /**
     * @brief Retrieves the maximum capacity the deque can hold.
     * @return The predefined maximum capacity.
     */
    NODISCARD FORCEINLINE static constexpr SizeType GetCapacity() noexcept { 
        return _Capacity; 
    }

    /**
     * @brief Equality operator to compare whether two deques are the same instance.
     * @param _deque The target deque to compare against.
     * @return True if both references point to the same physical object instance, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedDeque& _deque) noexcept { 
        return this == &_deque; 
    }

    /**
     * @brief Inequality operator to compare whether two deques are different instances.
     * @param _deque The target deque to compare against.
     * @return True if they are distinct object instances, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedDeque& _deque) noexcept { 
        return this != &_deque;
    }

    /**
     * @brief Accesses the element at a specific index without bounds checking.
     * @param _index The logical internal index of the element to retrieve (0 to Size - 1).
     * @return Modifiable reference to the accessed element.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](SizeType _index) noexcept { 
        ASSERT(_index < back_index_ - front_index_, "TFixedDeque: _index out of bounds!");
        return deque_[(front_index_ + _index) & kSizeMask];
    }

    /**
     * @brief Accesses the constant element at a specific index without bounds checking.
     * @param _index The logical internal index of the element to retrieve (0 to Size - 1).
     * @return Constant reference to the accessed element.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](SizeType _index) const noexcept { 
        ASSERT(_index < back_index_ - front_index_, "TFixedDeque: _index out of bounds!");
        return deque_[(front_index_ + _index) & kSizeMask];
    }

    /**
     * @brief Accesses the element at a specific index with bounds checking.
     * @param _index The logical internal index of the element to retrieve.
     * @return Modifiable reference to the element at the provided index.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& At(SizeType _index) { 
        if (_index >= back_index_ - front_index_) {
            throw std::out_of_range("TFixedDeque: Index out of logical bounds");
        }
        return deque_[(front_index_ + _index) & kSizeMask];
    }

    /**
     * @brief Accesses the constant element at a specific index with bounds checking.
     * @param _index The logical internal index of the element to retrieve.
     * @return Constant reference to the element at the provided index.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(SizeType _index) const { 
        if (_index >= back_index_ - front_index_) {
            throw std::out_of_range("TFixedDeque: Index out of logical bounds");
        }
        return deque_[(front_index_ + _index) & kSizeMask];
    }

    /**
     * @brief Retrieves a reference to the element at the front.
     * @return Modifiable reference to the element situated at the front.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept {
        ASSERT(front_index_ != back_index_, "TFixedDeque: Deque is empty!");
        return deque_[front_index_ & kSizeMask];
    }

    /**
     * @brief Retrieves a constant reference to the element at the front.
     * @return Constant reference to the element situated at the front.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { 
        ASSERT(front_index_ != back_index_, "TFixedDeque: Deque is empty!");
        return deque_[front_index_ & kSizeMask];
    }

    /**
     * @brief Retrieves a reference to the element at the back.
     * @return Modifiable reference to the element located at the back.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { 
        ASSERT(front_index_ != back_index_, "TFixedDeque: Deque is empty!");
        return deque_[(back_index_ - 1) & kSizeMask];
    }

    /**
     * @brief Retrieves a constant reference to the element at the back.
     * @return Constant reference to the element located at the back.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { 
        ASSERT(front_index_ != back_index_, "TFixedDeque: Deque is empty!");
        return deque_[(back_index_ - 1) & kSizeMask];
    }

    /**
     * @brief Removes the element at the front of the deque.
     */
    constexpr Void PopFront() noexcept {
        if (front_index_ == back_index_) {
            return;
        }
        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[front_index_ & kSizeMask] = _ObjectType();
        }
        ++front_index_;
        return;
    }  

    /**
     * @brief Removes the element at the back of the deque.
     */
    constexpr Void PopBack() noexcept {
        if (front_index_ == back_index_) {
            return;
        }
        --back_index_;
        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[back_index_ & kSizeMask] = _ObjectType();
        }
        return;
    }

    /**
     * @brief Inserts an element to the front of the deque.
     * @param _obj The constant object source to copy into the front slot.
     * @return True if the push was successful, false if the deque has exhausted its capacity.
     */
    NODISCARD constexpr Bool PushFront(const _ObjectType& _obj) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }
        --front_index_;
        deque_[front_index_ & kSizeMask] = _obj;
        return true;
    }

    /**
     * @brief Inserts an element to the front of the deque utilizing move semantics.
     * @param _obj The rvalue object reference to move into the front slot.
     * @return True if the move push was successful, false if the deque has exhausted its capacity.
     */
    NODISCARD constexpr Bool PushFront(_ObjectType&& _obj) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }
        --front_index_;
        deque_[front_index_ & kSizeMask] = std::move(_obj);
        return true;
    }

    /**
     * @brief Constructs an element in-place at the front of the deque.
     * @tparam _ArgsType The variadic parameter types expected by the object's constructor.
     * @param _args The parameters forwarded for the element's construction.
     * @return True if the construction succeeded, false if the deque has reached capacity limits.
     */
    template<typename... _ArgsType>
    NODISCARD constexpr Bool EmplaceFront(_ArgsType&&... _args) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }

        --front_index_;

        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[front_index_ & kSizeMask] = _ObjectType(std::forward<_ArgsType>(_args)...);
        } else {
            new (&deque_[front_index_ & kSizeMask]) _ObjectType(std::forward<_ArgsType>(_args)...);
        }
        return true;
    }

    /**
     * @brief Inserts an element to the back of the deque.
     * @param _obj The constant object source to copy into the back slot.
     * @return True if the push was successful, false if the deque has exhausted its capacity.
     */
    NODISCARD constexpr Bool PushBack(const _ObjectType& _obj) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }
        deque_[back_index_ & kSizeMask] = _obj;
        ++back_index_;
        return true;
    }

    /**
     * @brief Inserts an element to the back of the deque utilizing move semantics.
     * @param _obj The rvalue object reference to move into the back slot.
     * @return True if the move push was successful, false if the deque has exhausted its capacity.
     */
    NODISCARD constexpr Bool PushBack(_ObjectType&& _obj) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }
        deque_[back_index_ & kSizeMask] = std::move(_obj);
        ++back_index_;
        return true;
    }

    /**
     * @brief Constructs an element in-place at the back of the deque.
     * @tparam _ArgsType The variadic parameter types expected by the object's constructor.
     * @param _args The parameters forwarded for the element's construction.
     * @return True if the construction succeeded, false if the deque has reached capacity limits.
     */
    template<typename... _ArgsType>
    NODISCARD constexpr Bool EmplaceBack(_ArgsType&&... _args) noexcept {
        if (back_index_ - front_index_ >= _Capacity) {
            return false;
        }

        if constexpr (_IfCallConstructorAndDestructor) {
            deque_[back_index_ & kSizeMask] = _ObjectType(std::forward<_ArgsType>(_args)...);
        } else {
            new (&deque_[back_index_ & kSizeMask]) _ObjectType(std::forward<_ArgsType>(_args)...);
        }

        ++back_index_;
        return true;
    }

    /**
     * @brief Changes the size of the deque.
     * @param _new_size The target logical size required for the deque adjustment.
     * @return Void.
     */
    constexpr Void Resize(SizeType _new_size) noexcept {
        SizeType size = back_index_ - front_index_;

        // max size limit
        if (_new_size > _Capacity) {
            _new_size = _Capacity;
        }

        // no change
        if (_new_size == size) {
            return;
        }

        // shrink
        if (_new_size < size) {
            SizeType shrink_count = size - _new_size;
            for (SizeType count = 0; count < shrink_count; ++count) {
                --back_index_;
                if constexpr (_IfCallConstructorAndDestructor) {
                    deque_[back_index_ & kSizeMask] = _ObjectType();
                }
            }
        }
        // expand
        else {
            SizeType expand_count = _new_size - size;
            for (SizeType count = 0; count < expand_count; ++count) {
                if constexpr (_IfCallConstructorAndDestructor) {
                    deque_[back_index_ & kSizeMask] = _ObjectType();
                }
                ++back_index_;
            }
        }
    }

    /**
     * @brief Clears all elements in the deque and resets the indices.
     * @return Void.
     */
    constexpr Void Clear() noexcept { 
        if constexpr (_IfCallConstructorAndDestructor) {
            for (SizeType index = front_index_; index != back_index_; ++index) {
                deque_[index & kSizeMask] = _ObjectType();
            }
        }
        front_index_ = 0ULL;
        back_index_ = 0ULL;
    }

    /**
     * @brief Fills the deque with a specified value.
     * @param _value The value instance to fill the internal buffer with.
     * @return Void.
     */
    constexpr Void Fill(const _ObjectType& _value) noexcept {
        deque_.fill(_value); 
        front_index_ = 0ULL;
        back_index_ = _Capacity;
    }

    /**
     * @brief Swaps the content of this deque with another deque.
     * @param _deque The counterpart deque to swap data with.
     * @return Void.
     */
    FORCEINLINE constexpr Void Swap(TFixedDeque& _deque) noexcept { 
        deque_.swap(_deque.deque_); 
        ::zengine::Swap(&front_index_, &_deque.front_index_);
        ::zengine::Swap(&back_index_, &_deque.back_index_);
    }

private:
    /**
     * @brief Copies the properties of another deque.
     * @param _deque The external deque reference targeting for copy extraction.
     * @return Void.
     */
    FORCEINLINE Void CopyP(const TFixedDeque& _deque) noexcept {
        deque_ = _deque.deque_;
        front_index_ = _deque.front_index_;
        back_index_ = _deque.back_index_;
    }

    /**
     * @brief Moves the properties of another deque.
     * @param _deque The external deque rvalue reference targeting for move extraction.
     * @return Void.
     */
    FORCEINLINE Void MoveP(TFixedDeque&& _deque) noexcept {
        deque_ = std::move(_deque.deque_);
        front_index_ = _deque.front_index_;
        back_index_ = _deque.back_index_;
        _deque.front_index_ = 0ULL;
        _deque.back_index_ = 0ULL;
    }

private:
    /** @brief Fundamental rigid-size standard array allocating the underlying physical block topology. */
    STDFixedArray_ deque_;
    /** @brief Absolute unmasked index referencing the subsequent free front queue space. */
    SizeType front_index_;
    /** @brief Absolute unmasked index pointing dynamically to the true rear filled element. */
    SizeType back_index_;
};

} // zengine