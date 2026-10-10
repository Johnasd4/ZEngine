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
#include "t_array.h"
#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A standard Stack container adapter (LIFO - Last In, First Out).
 *
 * This class functions as a container adapter, which wraps an underlying linear container
 * (defaulting to TArray) and provides a specific set of functions to access elements.
 * Elements are pushed and popped from the "back" of the container, which represents the
 * top of the stack. It relies on the underlying container for memory management and
 * storage logic.
 *
 * @tparam _ObjectType The type of elements stored in the stack.
 * @tparam _ContainerType The underlying container type, default is TArray using global memory pool.
 */
template<typename _ObjectType, typename _ContainerType = TArray<_ObjectType, DEFAULT_USE_GLOBAL_MEMORY_POOL>>
class TStack : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying container type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     * Constructs an empty stack with no elements.
     */
    FORCEINLINE TStack() noexcept 
        : SuperType_()
        , stack_() 
    {}

    /**
     * @brief Copy constructor.
     * Constructs a stack with the contents of another stack by copying the underlying container.
     * @param _stack The stack to copy from.
     */
    FORCEINLINE TStack(const TStack& _stack) noexcept 
        : SuperType_(_stack)
        , stack_(_stack.stack_) 
    {}

    /**
     * @brief Move constructor.
     * Constructs a stack by moving the contents of another stack's underlying container.
     * @param _stack The stack to move from.
     */
    FORCEINLINE TStack(TStack&& _stack) noexcept 
        : SuperType_(std::move(_stack))
        , stack_(std::move(_stack.stack_)) 
    {}

    /**
     * @brief Constructs a stack with a specific initial capacity.
     * @param _size The initial size/capacity of the stack.
     */
    FORCEINLINE TStack(SizeType _size) noexcept 
        : SuperType_()
        , stack_(_size) 
    {}

    /**
     * @brief Constructs a stack with a specific size and fills it with a given value.
     * @param _size The initial size of the stack.
     * @param _val The value to fill the stack with.
     */
    FORCEINLINE TStack(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_()
        , stack_(_size, _val) 
    {}

    /**
     * @brief Range constructor.
     * Constructs a stack with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TStack(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_()
        , stack_(_first, _last) 
    {}

    /**
     * @brief Initializer list constructor.
     * Constructs a stack with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TStack(InitializerList_ _init_list) noexcept 
        : SuperType_()
        , stack_(_init_list) 
    {}
 
    /**
     * @brief Destructor.
     * Destroys the stack and releases allocated resources.
     */
    FORCEINLINE ~TStack() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * Replaces the contents with a copy of the contents of another stack.
     * Checks for self-assignment to avoid unnecessary operations.
     * @param _stack The stack to copy from.
     * @return A reference to this stack.
     */
    FORCEINLINE TStack& operator=(const TStack& _stack) noexcept { 
        if (this == &_stack) {
            return *this;
        }
        SuperType_::operator=(_stack);
        stack_ = _stack.stack_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Replaces the contents by moving the contents of another stack.
     * Checks for self-assignment to avoid unnecessary operations.
     * @param _stack The stack to move from.
     * @return A reference to this stack.
     */
    FORCEINLINE TStack& operator=(TStack&& _stack) noexcept { 
        if (this == &_stack) {
            return *this;
        }
        SuperType_::operator=(std::move(_stack));
        stack_ = std::move(_stack.stack_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * Replaces the contents with those from the initializer list.
     * @param _init_list The initializer list.
     * @return A reference to this stack.
     */
    FORCEINLINE TStack& operator=(InitializerList_ _init_list) noexcept {
        stack_ = _init_list;
        return *this;
    }

    /**
     * @brief Checks if the stack is empty.
     * @return True if the stack contains no elements, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return stack_.IsEmpty(); }

    /**
     * @brief Checks if the stack is full.
     * @return True if the stack cannot hold more elements (mostly for fixed-size containers), false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsFull() const noexcept { return stack_.IsFull(); }

    /**
     * @brief Returns the number of elements in the stack.
     * @return The number of elements currently stored.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return stack_.GetSize(); }

    /**
     * @brief Returns the capacity of the stack.
     * @return The number of elements the stack can hold before reallocating storage.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return stack_.GetCapacity(); }

    /**
     * @brief Equality comparison operator.
     * Checks if the contents of two stacks are equal.
     * @param _stack The stack to compare with.
     * @return True if the stacks are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TStack& _stack) noexcept { return stack_ == _stack.stack_; }

    /**
     * @brief Inequality comparison operator.
     * Checks if the contents of two stacks are not equal.
     * @param _stack The stack to compare with.
     * @return True if the stacks are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TStack& _stack) noexcept { return stack_ != _stack.stack_; }

    /**
     * @brief Accesses the top element of the stack.
     * @return A reference to the top element.
     */
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return stack_.Back(); }

    /**
     * @brief Accesses the top element of the stack (const).
     * @return A const reference to the top element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return stack_.Back(); }

    /**
     * @brief Removes the top element from the stack.
     * Does nothing if the stack is empty (depending on underlying container behavior).
     */
    FORCEINLINE Void PopBack() noexcept { stack_.PopBack(); }

    /**
     * @brief Attempts to remove the top element from the stack.
     * Use this method to safely pop an element and optionally retrieve it.
     * @param _obj_ptr Optional pointer to where the popped element should be moved. Can be nullptr.
     * @return True if an element was popped (stack was not empty), false otherwise.
     */
    FORCEINLINE Bool TryPopBack(_ObjectType* _obj_ptr = nullptr) noexcept {
        if (stack_.IsEmpty()) {
            return false;
        }

        if (_obj_ptr != nullptr) {
            *_obj_ptr = std::move(stack_.Back());
        }
        stack_.PopBack();

        return true;
    }

    /**
     * @brief Adds an element to the top of the stack.
     * @param _val The value to copy into the stack.
     */
    FORCEINLINE decltype(auto) PushBack(const _ObjectType& _val) noexcept {
        return stack_.PushBack(_val); 
    }

    /**
     * @brief Adds an element to the top of the stack using move semantics.
     * @param _val The value to move into the stack.
     */
    FORCEINLINE decltype(auto) PushBack(_ObjectType&& _val) noexcept {
        return stack_.PushBack(std::move(_val)); 
    }

    /**
     * @brief Constructs and adds an element to the top of the stack in-place.
     * @tparam ArgsType The types of the arguments for element construction.
     * @param _args The arguments to forward to the constructor of the element.
     */
    template <typename... ArgsType>
    FORCEINLINE decltype(auto) EmplaceBack(ArgsType&&... _args) noexcept {
        return stack_.EmplaceBack(std::forward<ArgsType>(_args)...); 
    }

    /**
     * @brief Clears the contents of the stack.
     * Removes all elements from the stack.
     */
    FORCEINLINE Void Clear() noexcept { stack_.Clear(); }

    /**
     * @brief Reserves storage for at least the specified number of elements.
     * @param _size The new capacity of the stack.
     */
    FORCEINLINE Void Reserve(SizeType _size) noexcept { stack_.Reserve(_size); }

    /**
     * @brief Swaps the contents of this stack with another.
     * @param _stack The stack to swap with.
     */
    FORCEINLINE Void Swap(TStack& _stack) noexcept { stack_.Swap(_stack.stack_); }

private:
    /** @brief The underlying container used to store elements. */
    _ContainerType stack_;
};

/**
 * @brief Thread-safe Stack container adapter (LIFO).
 *
 * A thread-safe First-In-First-Out (FIFO) data structure. It wraps a standard container
 * and protects all member functions with a ZMutex to ensure thread safety during concurrent
 * access. It prevents data races by locking the mutex for the duration of every operation.
 * It uses the same underlying principles as TStack but adds a synchronization layer.
 *
 * @tparam _ObjectType The type of elements stored in the stack.
 * @tparam _ContainerType The underlying container type, default is TArray using global memory pool.
 */
template<typename _ObjectType, typename _ContainerType = TArray<_ObjectType, DEFAULT_USE_GLOBAL_MEMORY_POOL>>
class TStackSafe : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying container type. */
    using InitializerList_ = std::initializer_list<_ObjectType>;

    /**
     * @brief Default constructor.
     * Constructs an empty thread-safe stack.
     */
    FORCEINLINE TStackSafe() noexcept 
        : SuperType_()
        , mutex_()
        , stack_() 
    {}

    /**
     * @brief Copy constructor.
     * Thread-safe copy construction. Locks the source stack's mutex before copying the data.
     * @param _stack The stack to copy from.
     */
    FORCEINLINE TStackSafe(const TStackSafe& _stack) noexcept 
        : SuperType_(_stack)
    {
        TLockGuard lock_guard(_stack.mutex_);
        stack_ = _stack.stack_;

    }

    /**
     * @brief Move constructor.
     * Thread-safe move construction. Locks the source stack's mutex before moving the data.
     * @param _stack The stack to move from.
     */
    FORCEINLINE TStackSafe(TStackSafe&& _stack) noexcept
        : SuperType_(std::move(_stack))
        , mutex_()
    {
        TLockGuard lock_guard(_stack.mutex_);
        stack_ = std::move(_stack.stack_);
    }

    /**
     * @brief Constructs a stack with a specific size.
     * @param _size The initial size of the stack.
     */
    FORCEINLINE TStackSafe(SizeType _size) noexcept 
        : SuperType_()
        , mutex_()
        , stack_(_size) 
    {}

    /**
     * @brief Constructs a stack with a specific size and fills it with a value.
     * @param _size The initial size.
     * @param _val The value to fill the stack with.
     */
    FORCEINLINE TStackSafe(SizeType _size, const _ObjectType& _val) noexcept 
        : SuperType_()
        , mutex_()
        , stack_(_size, _val) 
    {}

    /**
     * @brief Range constructor.
     * Constructs a stack with the contents of the range [_first, _last).
     * @tparam _InputIterator The type of the input iterator.
     * @param _first The beginning of the range.
     * @param _last The end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE TStackSafe(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_()
        , mutex_()
        , stack_(_first, _last) 
    {}

    /**
     * @brief Initializer list constructor.
     * Constructs a stack with the contents of the initializer list.
     * @param _init_list The initializer list to initialize from.
     */
    FORCEINLINE TStackSafe(InitializerList_ _init_list) noexcept 
        : SuperType_()
        , mutex_()
        , stack_(_init_list) 
    {}

    /**
     * @brief Destructor.
     * Destroys the stack and releases resources.
     */
    FORCEINLINE ~TStackSafe() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * Thread-safe assignment. Locks both this stack and the source stack to prevent race conditions.
     * Uses T2LockGuard to avoid deadlocks. Checks for self-assignment.
     * @param _stack The stack to copy from.
     * @return A reference to this stack.
     */
    FORCEINLINE TStackSafe& operator=(const TStackSafe& _stack) noexcept {
        if (this == &_stack) {
            return *this;
        }
        T2LockGuard lock_guard(mutex_, _stack.mutex_);
        SuperType_::operator=(_stack);
        stack_ = _stack.stack_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Thread-safe move assignment. Locks both stacks using T2LockGuard to avoid deadlocks.
     * Checks for self-assignment.
     * @param _stack The stack to move from.
     * @return A reference to this stack.
     */
    FORCEINLINE TStackSafe& operator=(TStackSafe&& _stack) noexcept {
        if (this == &_stack) {
            return *this;
        }
        T2LockGuard lock_guard(mutex_, _stack.mutex_);
        SuperType_::operator=(std::move(_stack));
        stack_ = std::move(_stack.stack_);
        return *this;
    }

    /**
     * @brief Initializer list assignment operator.
     * Replaces the contents with those from the initializer list. Locks this stack.
     * @param _init_list The initializer list.
     * @return A reference to this stack.
     */
    FORCEINLINE TStackSafe& operator=(InitializerList_ _init_list) noexcept {
        TLockGuard lock_guard(mutex_);
        stack_ = _init_list;
        return *this;
    }

    /**
     * @brief Equality comparison operator.
     * Thread-safe comparison. Locks both stacks using T2LockGuard.
     * @param _stack The stack to compare with.
     * @return True if the stacks are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TStackSafe& _stack) noexcept { 
        T2LockGuard lock_guard(mutex_, _stack.mutex_);
        return stack_ == _stack.stack_; 
    }

    /**
     * @brief Inequality comparison operator.
     * Thread-safe comparison. Locks both stacks using T2LockGuard.
     * @param _stack The stack to compare with.
     * @return True if the stacks are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TStackSafe& _stack) noexcept { 
        T2LockGuard lock_guard(mutex_, _stack.mutex_);
        return stack_ != _stack.stack_; 
    }

    /**
     * @brief Returns the number of elements in the stack.
     * Thread-safe access.
     * @return The number of elements currently stored.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return stack_.GetSize(); 
    }

    /**
     * @brief Returns the capacity of the stack.
     * Thread-safe access.
     * @return The number of elements the stack can hold before reallocating.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { 
        TLockGuard lock_guard(mutex_); 
        return stack_.GetCapacity(); 
    }

    /**
     * @brief Tries to pop the front element from the stack safely.
     * If the stack is not empty, the front element is moved to _obj_ptr (if provided)
     * and removed from the stack.
     * @param _obj_ptr Pointer to store the popped element. Can be nullptr if only popping is desired.
     * @return True if an element was popped (stack was not empty), false otherwise.
     */
    FORCEINLINE Bool TryPopBack(_ObjectType* _obj_ptr = nullptr) noexcept { 
        TLockGuard lock_guard(mutex_);
        if (stack_.IsEmpty()) {
            return false;
        }

        if (_obj_ptr != nullptr) {
            *_obj_ptr = std::move(stack_.Back());
        }
        stack_.PopBack();

        return true;
    }

    /**
     * @brief Adds an element to the end of the stack.
     * Thread-safe access.
     * @param _val The value to add.
     */
    FORCEINLINE decltype(auto) PushBack(const _ObjectType& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return stack_.PushBack(_val);
    }

    /**
     * @brief Adds an element to the end of the stack (move semantics).
     * Thread-safe access.
     * @param _val The value to move into the stack.
     */
    FORCEINLINE decltype(auto) PushBack(_ObjectType&& _val) noexcept {
        TLockGuard lock_guard(mutex_);
        return stack_.PushBack(std::move(_val));
    }

    /**
     * @brief Constructs and adds an element to the end of the stack.
     * Thread-safe access.
     * @tparam ArgsType The types of the arguments.
     * @param _args The arguments to forward to the constructor of the element.
     */
    template <typename... ArgsType>
    FORCEINLINE decltype(auto) EmplaceBack(ArgsType&&... _args) noexcept {
        TLockGuard lock_guard(mutex_);
        return stack_.EmplaceBack(std::forward<ArgsType>(_args)...);
    }

    /**
     * @brief Clears the contents of the stack.
     * Thread-safe access.
     */
    FORCEINLINE Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_);
        stack_.Clear(); 
    }

    /**
     * @brief Reserves storage for at least the specified number of elements.
     * Thread-safe access.
     * @param _size The new capacity of the stack.
     */
    FORCEINLINE Void Reserve(SizeType _size) noexcept { 
        TLockGuard lock_guard(mutex_); 
        stack_.Reserve(_size); 
    }

    /**
     * @brief Swaps the contents of this stack with another safely.
     * Locks both stacks using T2LockGuard to ensure thread safety and avoid deadlocks.
     * @param _stack The stack to swap with.
     */
    FORCEINLINE Void Swap(TStackSafe& _stack) noexcept { 
        T2LockGuard lock_guard(mutex_, _stack.mutex_);
        stack_.Swap(_stack.stack_); 
    }

private:
    /** @brief Mutex to ensure thread safety, cache aligned to avoid false sharing. */
    CACHE_ALIGN mutable ZMutex mutex_;
    /** @brief The underlying container used to store elements. */
    _ContainerType stack_;
};

} // zengine