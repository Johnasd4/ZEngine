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

#include "../drive.h"

#include <iterator>
#include <compare>
#include <type_traits>
#include <concepts>

namespace zengine {
namespace internal {

template<typename _ObjectType>
class TOrdetDataIterator;
template<typename _ObjectType>
class TOrdetDataConstIterator;
template<typename _ObjectType>
class TOrdetDataReverseIterator;
template<typename _ObjectType>
class TOrdetDataConstReverseIterator;

/**
 * @brief A contiguous, random-access iterator for ordered data.
 *
 * This iterator satisfies the C++20 contiguous_iterator concept and allows
 * modification of the underlying elements.
 *
 * @tparam _ObjectType The type of the elements being iterated.
 */
template<typename _ObjectType>
class TOrdetDataIterator {
public:
    /** tag indicating the iterator models a contiguous range in memory. */
    using iterator_concept  = std::contiguous_iterator_tag;
    /** tag indicating the iterator supports random access operations. */
    using iterator_category = std::random_access_iterator_tag;
    /** The type of the values iterated over. */
    using value_type        = _ObjectType;
    /** Type used to represent the distance between two iterators. */
    using difference_type   = std::ptrdiff_t;
    /** Pointer to the value type. */
    using pointer           = _ObjectType*;
    /** Reference to the value type. */
    using reference         = _ObjectType&;

    /**
     * @brief Default constructor. Initializes an invalid iterator.
     */
    FORCEINLINE TOrdetDataIterator() = default;
    
    /**
     * @brief Constructor initializing the iterator with a pointer to an element.
     * @param _obj_ptr Pointer to the element.
     */
    FORCEINLINE explicit TOrdetDataIterator(pointer _obj_ptr) noexcept 
        : obj_ptr_(_obj_ptr) 
    {}

    // C++20 Default Rule of Zero: Uses compiler-generated copy/move constructors.

    /**
     * @brief Dereferences the iterator to access the element.
     * @return A reference to the current element.
     */
    NODISCARD FORCEINLINE reference operator*() const noexcept { return *obj_ptr_; }

    /**
     * @brief Accesses the member of the element.
     * @return A pointer to the current element.
     */
    NODISCARD FORCEINLINE pointer operator->() const noexcept { return obj_ptr_; }

    /**
     * @brief Retrieves the raw pointer to the current element.
     * @return The underlying raw pointer.
     */
    NODISCARD FORCEINLINE pointer Ptr() const noexcept { return obj_ptr_; }

    /**
     * @brief Pre-increment operator. Advances the iterator to the next element.
     * @return A reference to this iterator after incrementing.
     */
    FORCEINLINE TOrdetDataIterator& operator++() noexcept {
        ++obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-increment operator. Advances the iterator but returns the previous state.
     * @return A copy of the iterator before incrementing.
     */
    FORCEINLINE TOrdetDataIterator operator++(int) noexcept {
        auto tmp = *this;
        ++obj_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator. Moves the iterator to the previous element.
     * @return A reference to this iterator after decrementing.
     */
    FORCEINLINE TOrdetDataIterator& operator--() noexcept {
        --obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator. Moves the iterator back but returns the previous state.
     * @return A copy of the iterator before decrementing.
     */
    FORCEINLINE TOrdetDataIterator operator--(int) noexcept {
        auto tmp = *this;
        --obj_ptr_;
        return tmp;
    }

    /**
     * @brief Compound assignment addition. Advances the iterator by the specified offset.
     * @param _num The number of positions to advance.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataIterator& operator+=(difference_type _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Compound assignment subtraction. Moves the iterator back by the specified offset.
     * @param _num The number of positions to move back.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataIterator& operator-=(difference_type _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Addition operator. Creates a new iterator advanced by the offset.
     * @param _num The offset to add.
     * @return A new iterator at the resulting position.
     */
    NODISCARD FORCEINLINE TOrdetDataIterator operator+(difference_type _num) const noexcept {
        return TOrdetDataIterator(obj_ptr_ + _num);
    }

    /**
     * @brief Subtraction operator. Creates a new iterator moved back by the offset.
     * @param _num The offset to subtract.
     * @return A new iterator at the resulting position.
     */
    NODISCARD FORCEINLINE TOrdetDataIterator operator-(difference_type _num) const noexcept {
        return TOrdetDataIterator(obj_ptr_ - _num);
    }

    /**
     * @brief Non-member addition operator to support integer + iterator syntax.
     * @param _num The offset to add.
     * @param _it The iterator.
     * @return A new iterator advanced by the offset.
     */
    NODISCARD FORCEINLINE friend TOrdetDataIterator operator+(
        difference_type _num, 
        const TOrdetDataIterator& _it
    ) noexcept {
        return TOrdetDataIterator(_it.obj_ptr_ + _num);
    }

    /**
     * @brief Computes the distance between two iterators.
     * @param lhs The left-hand side iterator.
     * @param rhs The right-hand side iterator.
     * @return The number of elements between the two iterators.
     */
    NODISCARD FORCEINLINE friend difference_type operator-(
        const TOrdetDataIterator& lhs, 
        const TOrdetDataIterator& rhs
    ) noexcept {
        return lhs.obj_ptr_ - rhs.obj_ptr_;
    }

    /**
     * @brief Three-way comparison operator (spaceship operator).
     * @param other The iterator to compare against.
     * @return The ordering relationship between the iterators.
     */
    NODISCARD FORCEINLINE auto operator<=>(const TOrdetDataIterator&) const = default;

protected:
    /** Friend declaration allowing conversion to const iterator. */
    friend class TOrdetDataConstIterator<_ObjectType>;

private:
    /** Deleted operator new to prevent heap allocation of the iterator itself. */
    static Void* operator new(SizeType) = delete;
    /** Deleted operator delete. */
    static Void operator delete(Void*) = delete;

    /** @brief The internal raw pointer to the element. */
    pointer obj_ptr_ = nullptr;
};

/**
 * @brief A contiguous, random-access const iterator for ordered data.
 *
 * This iterator provides read-only access to the underlying elements.
 *
 * @tparam _ObjectType The type of the elements being iterated.
 */
template<typename _ObjectType>
class TOrdetDataConstIterator {
public:
    /** tag indicating the iterator models a contiguous range in memory. */
    using iterator_concept  = std::contiguous_iterator_tag;
    /** tag indicating the iterator supports random access operations. */
    using iterator_category = std::random_access_iterator_tag;
    /** The type of the values iterated over. */
    using value_type        = _ObjectType;
    /** Type used to represent the distance between two iterators. */
    using difference_type   = std::ptrdiff_t;
    /** Const pointer to the value type. */
    using pointer           = const _ObjectType*;
    /** Const reference to the value type. */
    using reference         = const _ObjectType&;

    /**
     * @brief Default constructor. Initializes an invalid iterator.
     */
    FORCEINLINE TOrdetDataConstIterator() = default;
    
    /**
     * @brief Constructor initializing the iterator with a const pointer.
     * @param _obj_ptr Pointer to the element.
     */
    FORCEINLINE explicit TOrdetDataConstIterator(const _ObjectType* _obj_ptr) noexcept 
        : obj_ptr_(_obj_ptr) 
    {}
    
    /**
     * @brief Conversion constructor from a non-const iterator.
     * @param _it The non-const iterator to convert.
     */
    FORCEINLINE TOrdetDataConstIterator(const TOrdetDataIterator<_ObjectType>& _it) noexcept
        : obj_ptr_(_it.obj_ptr_) 
    {}

    /**
     * @brief Dereferences the iterator to access the element (read-only).
     * @return A const reference to the current element.
     */
    NODISCARD FORCEINLINE reference operator*() const noexcept { return *obj_ptr_; }

    /**
     * @brief Accesses the member of the element (read-only).
     * @return A const pointer to the current element.
     */
    NODISCARD FORCEINLINE pointer operator->() const noexcept { return obj_ptr_; }

    /**
     * @brief Retrieves the raw const pointer to the current element.
     * @return The underlying raw const pointer.
     */
    NODISCARD FORCEINLINE pointer Ptr() const noexcept { return obj_ptr_; }

    /**
     * @brief Pre-increment operator. Advances the iterator to the next element.
     * @return A reference to this iterator after incrementing.
     */
    FORCEINLINE TOrdetDataConstIterator& operator++() noexcept {
        ++obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-increment operator. Advances the iterator but returns the previous state.
     * @return A copy of the iterator before incrementing.
     */
    FORCEINLINE TOrdetDataConstIterator operator++(int) noexcept {
        auto tmp = *this;
        ++obj_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator. Moves the iterator to the previous element.
     * @return A reference to this iterator after decrementing.
     */
    FORCEINLINE TOrdetDataConstIterator& operator--() noexcept {
        --obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator. Moves the iterator back but returns the previous state.
     * @return A copy of the iterator before decrementing.
     */
    FORCEINLINE TOrdetDataConstIterator operator--(int) noexcept {
        auto tmp = *this;
        --obj_ptr_;
        return tmp;
    }

    /**
     * @brief Compound assignment addition. Advances the iterator by the specified offset.
     * @param _num The number of positions to advance.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstIterator& operator+=(difference_type _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Compound assignment subtraction. Moves the iterator back by the specified offset.
     * @param _num The number of positions to move back.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstIterator& operator-=(difference_type _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Addition operator. Creates a new iterator advanced by the offset.
     * @param _num The offset to add.
     * @return A new iterator at the resulting position.
     */
    NODISCARD FORCEINLINE TOrdetDataConstIterator operator+(difference_type _num) const noexcept {
        return TOrdetDataConstIterator(obj_ptr_ + _num);
    }

    /**
     * @brief Subtraction operator. Creates a new iterator moved back by the offset.
     * @param _num The offset to subtract.
     * @return A new iterator at the resulting position.
     */
    NODISCARD FORCEINLINE TOrdetDataConstIterator operator-(difference_type _num) const noexcept {
        return TOrdetDataConstIterator(obj_ptr_ - _num);
    }

    /**
     * @brief Non-member addition operator to support integer + iterator syntax.
     * @param _num The offset to add.
     * @param _it The iterator.
     * @return A new iterator advanced by the offset.
     */
    NODISCARD FORCEINLINE friend TOrdetDataConstIterator operator+(
        difference_type _num, 
        const TOrdetDataConstIterator& _it
    ) noexcept {
        return TOrdetDataConstIterator(_it.obj_ptr_ + _num);
    }

    /**
     * @brief Computes the distance between two iterators.
     * @param lhs The left-hand side iterator.
     * @param rhs The right-hand side iterator.
     * @return The number of elements between the two iterators.
     */
    NODISCARD FORCEINLINE friend difference_type operator-(
        const TOrdetDataConstIterator& lhs, 
        const TOrdetDataConstIterator& rhs
    ) noexcept {
        return lhs.obj_ptr_ - rhs.obj_ptr_;
    }

    /**
     * @brief Three-way comparison operator (spaceship operator).
     * @param other The iterator to compare against.
     * @return The ordering relationship between the iterators.
     */
    NODISCARD FORCEINLINE auto operator<=>(const TOrdetDataConstIterator&) const = default;

private:
    /** Deleted operator new to prevent heap allocation of the iterator itself. */
    static Void* operator new(SizeType) = delete;
    /** Deleted operator delete. */
    static Void operator delete(Void*) = delete;

    /** @brief The internal raw const pointer to the element. */
    const _ObjectType* obj_ptr_ = nullptr;
};

/**
 * @brief A random-access reverse iterator for ordered data.
 *
 * This iterator moves in the opposite direction of the forward iterator.
 * Incrementing this iterator moves the underlying pointer backwards in memory.
 *
 * @tparam _ObjectType The type of the elements being iterated.
 */
template<typename _ObjectType>
class TOrdetDataReverseIterator {
public:
    /** tag indicating the iterator models a random access range. */
    using iterator_concept  = std::random_access_iterator_tag;
    /** tag indicating the iterator supports random access operations. */
    using iterator_category = std::random_access_iterator_tag;
    /** The type of the values iterated over. */
    using value_type        = _ObjectType;
    /** Type used to represent the distance between two iterators. */
    using difference_type   = std::ptrdiff_t;
    /** Pointer to the value type. */
    using pointer           = _ObjectType*;
    /** Reference to the value type. */
    using reference         = _ObjectType&;

    /**
     * @brief Default constructor. Initializes an invalid reverse iterator.
     */
    FORCEINLINE TOrdetDataReverseIterator() = default;
    
    /**
     * @brief Constructor initializing the reverse iterator with a pointer.
     * Note: The pointer should typically point to the element *after* the last valid element.
     * @param _obj_ptr Pointer to the current position (conceptually one past the element).
     */
    FORCEINLINE explicit TOrdetDataReverseIterator(pointer _obj_ptr) noexcept 
        : obj_ptr_(_obj_ptr) 
    {}

    /**
     * @brief Dereferences the iterator to access the element.
     * Returns the element preceding the current internal pointer.
     * @return A reference to the current element.
     */
    NODISCARD FORCEINLINE reference operator*() const noexcept {
        pointer temp = obj_ptr_;
        return *(--temp); // dereference the previous position
    }

    /**
     * @brief Accesses the member of the element.
     * @return A pointer to the current element.
     */
    NODISCARD FORCEINLINE pointer operator->() const noexcept {
        pointer temp = obj_ptr_;
        return --temp;
    }
    
    /**
     * @brief Retrieves the base pointer corresponding to the current position.
     * @return The underlying pointer (points to the element *after* the one dereferenced).
     */
    NODISCARD FORCEINLINE pointer base() const noexcept { return obj_ptr_; }

    /**
     * @brief Retrieves the actual raw pointer to the element being accessed.
     * @return The pointer to the element logically pointed to by this iterator.
     */
    NODISCARD FORCEINLINE pointer Ptr() const noexcept { return obj_ptr_ - 1; }

    /**
     * @brief Pre-increment operator. Moves the iterator logically forward (physically backward).
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataReverseIterator& operator++() noexcept {
        --obj_ptr_; // Reverse: ++ means moving memory address backward
        return *this;
    }

    /**
     * @brief Post-increment operator. Moves the iterator logically forward but returns previous state.
     * @return A copy of the iterator before incrementing.
     */
    FORCEINLINE TOrdetDataReverseIterator operator++(int) noexcept {
        auto tmp = *this;
        --obj_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator. Moves the iterator logically backward (physically forward).
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataReverseIterator& operator--() noexcept {
        ++obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator. Moves the iterator logically backward but returns previous state.
     * @return A copy of the iterator before decrementing.
     */
    FORCEINLINE TOrdetDataReverseIterator operator--(int) noexcept {
        auto tmp = *this;
        ++obj_ptr_;
        return tmp;
    }

    /**
     * @brief Compound assignment addition. Moves the iterator logically forward by offset.
     * @param _num The logical distance to advance.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataReverseIterator& operator+=(difference_type _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Compound assignment subtraction. Moves the iterator logically backward by offset.
     * @param _num The logical distance to move back.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataReverseIterator& operator-=(difference_type _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Addition operator. Creates a new iterator logically advanced by the offset.
     * @param _num The logical offset.
     * @return A new reverse iterator.
     */
    NODISCARD FORCEINLINE TOrdetDataReverseIterator operator+(difference_type _num) const noexcept {
        return TOrdetDataReverseIterator(obj_ptr_ - _num);
    }

    /**
     * @brief Subtraction operator. Creates a new iterator logically moved back by the offset.
     * @param _num The logical offset.
     * @return A new reverse iterator.
     */
    NODISCARD FORCEINLINE TOrdetDataReverseIterator operator-(difference_type _num) const noexcept {
        return TOrdetDataReverseIterator(obj_ptr_ + _num);
    }

    /**
     * @brief Non-member addition operator for integer + iterator.
     * @param _num The logical offset.
     * @param _it The iterator.
     * @return A new reverse iterator.
     */
    NODISCARD FORCEINLINE friend TOrdetDataReverseIterator operator+(
        difference_type _num, 
        const TOrdetDataReverseIterator& _it
    ) noexcept {
        return TOrdetDataReverseIterator(_it.obj_ptr_ - _num);
    }

    /**
     * @brief Computes the logical distance between two reverse iterators.
     * @param lhs The left-hand side iterator.
     * @param rhs The right-hand side iterator.
     * @return The logical distance.
     */
    NODISCARD FORCEINLINE friend difference_type operator-(
        const TOrdetDataReverseIterator& lhs, 
        const TOrdetDataReverseIterator& rhs
    ) noexcept {
        return rhs.obj_ptr_ - lhs.obj_ptr_;
    }

    /**
     * @brief Three-way comparison operator for reverse iterators.
     * Note the order is reversed compared to the internal pointers.
     * @param other The iterator to compare against.
     * @return The ordering relationship.
     */
    NODISCARD FORCEINLINE std::strong_ordering operator<=>(const TOrdetDataReverseIterator& other) const noexcept {
        return other.obj_ptr_ <=> obj_ptr_;
    }

    /**
     * @brief Equality operator.
     * @param other The iterator to compare against.
     * @return True if both iterators point to the same location.
     */
    NODISCARD FORCEINLINE bool operator==(const TOrdetDataReverseIterator& other) const noexcept {
        return obj_ptr_ == other.obj_ptr_;
    }

protected:
    /** Friend declaration allowing conversion to const reverse iterator. */
    friend class TOrdetDataConstReverseIterator<_ObjectType>;

private:
    /** Deleted operator new to prevent heap allocation. */
    static Void* operator new(SizeType) = delete;
    /** Deleted operator delete. */
    static Void operator delete(Void*) = delete;

    /** @brief The internal pointer (conceptually "current + 1"). */
    pointer obj_ptr_ = nullptr;
};

/**
 * @brief A random-access const reverse iterator for ordered data.
 *
 * This iterator provides read-only reverse iteration.
 *
 * @tparam _ObjectType The type of the elements being iterated.
 */
template<typename _ObjectType>
class TOrdetDataConstReverseIterator {
public:
    /** tag indicating the iterator models a random access range. */
    using iterator_concept  = std::random_access_iterator_tag;
    /** tag indicating the iterator supports random access operations. */
    using iterator_category = std::random_access_iterator_tag;
    /** The type of the values iterated over. */
    using value_type        = _ObjectType;
    /** Type used to represent the distance between two iterators. */
    using difference_type   = std::ptrdiff_t;
    /** Const pointer to the value type. */
    using pointer           = const _ObjectType*;
    /** Const reference to the value type. */
    using reference         = const _ObjectType&;

    /**
     * @brief Default constructor. Initializes an invalid const reverse iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator() = default;
    
    /**
     * @brief Constructor initializing the iterator with a const pointer.
     * @param _obj_ptr Pointer to the position.
     */
    FORCEINLINE explicit TOrdetDataConstReverseIterator(const _ObjectType* _obj_ptr) noexcept 
        : obj_ptr_(_obj_ptr) 
    {}
    
    /**
     * @brief Conversion constructor from a non-const reverse iterator.
     * @param _it The non-const reverse iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator(const TOrdetDataReverseIterator<_ObjectType>& _it) noexcept
        : obj_ptr_(_it.obj_ptr_) 
    {}

    /**
     * @brief Dereferences the iterator to access the element (read-only).
     * @return A const reference to the element preceding the internal pointer.
     */
    NODISCARD FORCEINLINE reference operator*() const noexcept {
        auto temp = obj_ptr_;
        return *(--temp);
    }

    /**
     * @brief Accesses the member of the element (read-only).
     * @return A const pointer to the current element.
     */
    NODISCARD FORCEINLINE pointer operator->() const noexcept {
        auto temp = obj_ptr_;
        return --temp;
    }

    /**
     * @brief Retrieves the base pointer corresponding to the current position.
     * @return The underlying const pointer.
     */
    NODISCARD FORCEINLINE const _ObjectType* base() const noexcept { return obj_ptr_; }

    /**
     * @brief Retrieves the actual raw const pointer to the element being accessed.
     * @return The const pointer to the element logically pointed to by this iterator.
     */
    NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return obj_ptr_ - 1; }

    /**
     * @brief Pre-increment operator. Moves the iterator logically forward.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator& operator++() noexcept {
        --obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-increment operator. Moves the iterator logically forward but returns previous state.
     * @return A copy of the iterator before incrementing.
     */
    FORCEINLINE TOrdetDataConstReverseIterator operator++(int) noexcept {
        auto tmp = *this;
        --obj_ptr_;
        return tmp;
    }

    /**
     * @brief Pre-decrement operator. Moves the iterator logically backward.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator& operator--() noexcept {
        ++obj_ptr_;
        return *this;
    }

    /**
     * @brief Post-decrement operator. Moves the iterator logically backward but returns previous state.
     * @return A copy of the iterator before decrementing.
     */
    FORCEINLINE TOrdetDataConstReverseIterator operator--(int) noexcept {
        auto tmp = *this;
        ++obj_ptr_;
        return tmp;
    }

    /**
     * @brief Compound assignment addition. Moves the iterator logically forward by offset.
     * @param _num The logical distance to advance.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator& operator+=(difference_type _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }

    /**
     * @brief Compound assignment subtraction. Moves the iterator logically backward by offset.
     * @param _num The logical distance to move back.
     * @return A reference to this iterator.
     */
    FORCEINLINE TOrdetDataConstReverseIterator& operator-=(difference_type _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }

    /**
     * @brief Addition operator. Creates a new iterator logically advanced by the offset.
     * @param _num The logical offset.
     * @return A new const reverse iterator.
     */
    NODISCARD FORCEINLINE TOrdetDataConstReverseIterator operator+(difference_type _num) const noexcept {
        return TOrdetDataConstReverseIterator(obj_ptr_ - _num);
    }

    /**
     * @brief Subtraction operator. Creates a new iterator logically moved back by the offset.
     * @param _num The logical offset.
     * @return A new const reverse iterator.
     */
    NODISCARD FORCEINLINE TOrdetDataConstReverseIterator operator-(difference_type _num) const noexcept {
        return TOrdetDataConstReverseIterator(obj_ptr_ + _num);
    }

    /**
     * @brief Non-member addition operator for integer + iterator.
     * @param _num The logical offset.
     * @param _it The iterator.
     * @return A new const reverse iterator.
     */
    NODISCARD FORCEINLINE friend TOrdetDataConstReverseIterator operator+(
        difference_type _num, 
        const TOrdetDataConstReverseIterator& _it
    ) noexcept {
        return TOrdetDataConstReverseIterator(_it.obj_ptr_ - _num);
    }

    /**
     * @brief Computes the logical distance between two const reverse iterators.
     * @param lhs The left-hand side iterator.
     * @param rhs The right-hand side iterator.
     * @return The logical distance.
     */
    NODISCARD FORCEINLINE friend difference_type operator-(
        const TOrdetDataConstReverseIterator& lhs, 
        const TOrdetDataConstReverseIterator& rhs
    ) noexcept {
        return rhs.obj_ptr_ - lhs.obj_ptr_;
    }

    /**
     * @brief Three-way comparison operator.
     * @param other The iterator to compare against.
     * @return The ordering relationship.
     */
    NODISCARD FORCEINLINE std::strong_ordering operator<=>(
        const TOrdetDataConstReverseIterator& other
    ) const noexcept {
        return other.obj_ptr_ <=> obj_ptr_;
    }

    /**
     * @brief Equality operator.
     * @param other The iterator to compare against.
     * @return True if both iterators point to the same location.
     */
    NODISCARD FORCEINLINE bool operator==(const TOrdetDataConstReverseIterator& other) const noexcept {
        return obj_ptr_ == other.obj_ptr_;
    }

private:
    /** Deleted operator new to prevent heap allocation. */
    static Void* operator new(SizeType) = delete;
    /** Deleted operator delete. */
    static Void operator delete(Void*) = delete;

    /** @brief The internal const pointer. */
    const _ObjectType* obj_ptr_ = nullptr;
};

}//internal
}//zengine