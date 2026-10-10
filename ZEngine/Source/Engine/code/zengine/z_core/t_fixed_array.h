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

#include <array>

#include "z_object.h"

namespace zengine {

/**
 * @brief Array container wrapper class around std::array.
 * @details This class inherits from ZObject and wraps std::array to provide a fixed-size contiguous array.
 *          The implementation delegates standard array operations such as iteration, bounds-checked element
 *          access, and swapping directly to the underlying std::array. This leverages standard library
 *          efficiency and safety while integrating with the ZEngine object system. It supports variadic 
 *          construction and constexpr operations.
 * @tparam _ObjectType The type of the elements stored in the array.
 * @tparam _Size The number of elements in the array.
 */
template<typename _ObjectType, SizeType _Size>
class TFixedArray : public ZObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;
    /** @brief Type alias for the underlying std::array. */
    using STDFixedArray_ = std::array<_ObjectType, _Size>;
    /** @brief Type alias for the iterator. */
    using Iterator_ = STDFixedArray_::iterator;
    /** @brief Type alias for the const iterator. */
    using ConstIterator_ = STDFixedArray_::const_iterator;
    /** @brief Type alias for the reverse iterator. */
    using ReverseIterator_ = STDFixedArray_::reverse_iterator;
    /** @brief Type alias for the const reverse iterator. */
    using ConstReverseIterator_ = STDFixedArray_::const_reverse_iterator;

    /**
     * @brief Default constructor.
     */
    FORCEINLINE constexpr TFixedArray() noexcept : SuperType_() {}

    /**
     * @brief Copy constructor.
     * @param _array The source array to copy from.
     */
    FORCEINLINE constexpr TFixedArray(const TFixedArray& _array) noexcept 
        : SuperType_(_array)
        , array_(_array.array_) 
    {}

    /**
     * @brief Move constructor.
     * @param _array The source array to move from.
     */
    FORCEINLINE constexpr TFixedArray(TFixedArray&& _array) noexcept 
        : SuperType_(std::move(_array))
        , array_(std::move(_array.array_)) 
    {}

    /**
     * @brief Variadic constructor to initialize array elements.
     * @details Requires the number of arguments to match the array size.
     * @tparam _ArgsType The types of the arguments.
     * @param _args The arguments to initialize the array with.
     */
    template<typename... _ArgsType>
    requires(sizeof...(_ArgsType) == _Size)
    FORCEINLINE constexpr TFixedArray(_ArgsType&&... _args) noexcept
        : SuperType_()
        , array_{ static_cast<_ObjectType>(std::forward<_ArgsType>(_args))... }
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE constexpr ~TFixedArray() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _array The source array to copy from.
     * @return TFixedArray& A reference to this array.
     */
    FORCEINLINE TFixedArray& operator=(const TFixedArray& _array) noexcept {
        SuperType_::operator=(_array);
        array_ = _array.array_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _array The source array to move from.
     * @return TFixedArray& A reference to this array.
     */
    FORCEINLINE TFixedArray& operator=(TFixedArray&& _array) noexcept {
        SuperType_::operator=(std::move(_array));
        array_ = std::move(_array.array_);
        return *this;
    }

    /**
     * @brief Gets a pointer to the underlying data.
     * @return _ObjectType* A pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType* GetDataPtr() noexcept { return array_.data(); }

    /**
     * @brief Gets a pointer to the underlying data (const).
     * @return const _ObjectType* A const pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType* GetDataPtr() const noexcept { return array_.data(); }

    /**
     * @brief Accesses the underlying std::array object.
     * @return STDFixedArray_& A reference to the internal std::array.
     */
    NODISCARD FORCEINLINE constexpr STDFixedArray_& GetSTDFixedArray() noexcept { return array_; }

    /**
     * @brief Accesses the underlying std::array object (const).
     * @return const STDFixedArray_& A const reference to the internal std::array.
     */
    NODISCARD FORCEINLINE constexpr const STDFixedArray_& GetSTDFixedArray() const noexcept { return array_; }

    /**
     * @brief Gets the size of the array.
     * @return SizeType The number of elements in the array.
     */
    NODISCARD FORCEINLINE static constexpr SizeType GetSize() noexcept { return _Size; }

    /**
     * @brief Gets the capacity of the array.
     * @return SizeType The capacity of the array.
     */
    NODISCARD FORCEINLINE static constexpr SizeType GetCapacity() noexcept { return _Size; }

    /**
     * @brief Checks if two arrays are equal.
     * @param _array The array to compare with.
     * @return Bool True if the arrays are equal, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedArray& _array) noexcept { 
        return array_ == _array.array_; 
    }

    /**
     * @brief Checks if two arrays are not equal.
     * @param _array The array to compare with.
     * @return Bool True if the arrays are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedArray& _array) noexcept { 
        return array_ != _array.array_; 
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param _index The index of the element to access.
     * @return _ObjectType& A reference to the element.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](SizeType _index) noexcept { 
        return array_[_index]; 
    }

    /**
     * @brief Accesses the element at the specified index (const).
     * @param _index The index of the element to access.
     * @return const _ObjectType& A const reference to the element.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](SizeType _index) const noexcept { 
        return array_[_index]; 
    }

    /**
     * @brief Accesses the element at the specified index with bounds checking.
     * @param _index The index of the element to access.
     * @return _ObjectType& A reference to the element.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& At(SizeType _index) { return array_.at(_index); }

    /**
     * @brief Accesses the element at the specified index with bounds checking (const).
     * @param _index The index of the element to access.
     * @return const _ObjectType& A const reference to the element.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(SizeType _index) const { return array_.at(_index); }

    /**
     * @brief Accesses the first element of the array.
     * @return _ObjectType& A reference to the first element.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { return array_.front(); }

    /**
     * @brief Accesses the first element of the array (const).
     * @return const _ObjectType& A const reference to the first element.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { return array_.front(); }

    /**
     * @brief Accesses the last element of the array.
     * @return _ObjectType& A reference to the last element.
     */
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { return array_.back(); }

    /**
     * @brief Accesses the last element of the array (const).
     * @return const _ObjectType& A const reference to the last element.
     */
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { return array_.back(); }

    /**
     * @brief Returns an iterator to the beginning of the array.
     * @return Iterator_ An iterator to the first element.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return array_.begin(); }

    /**
     * @brief Returns an iterator to the beginning of the array (const).
     * @return ConstIterator_ A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return array_.begin(); }

    /**
     * @brief Returns a const iterator to the beginning of the array.
     * @return ConstIterator_ A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return array_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning of the array.
     * @return ReverseIterator_ A reverse iterator to the last element.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return array_.rbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning of the array (const).
     * @return ConstReverseIterator_ A const reverse iterator to the last element.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return array_.rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the beginning of the array.
     * @return ConstReverseIterator_ A const reverse iterator to the last element.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return array_.crbegin(); }

    /**
     * @brief Returns an iterator to the end of the array.
     * @return Iterator_ An iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return array_.end(); }

    /**
     * @brief Returns an iterator to the end of the array (const).
     * @return ConstIterator_ A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return array_.end(); }

    /**
     * @brief Returns a const iterator to the end of the array.
     * @return ConstIterator_ A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return array_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end of the array.
     * @return ReverseIterator_ A reverse iterator to the element preceding the first element.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return array_.rend(); }

    /**
     * @brief Returns a reverse iterator to the end of the array (const).
     * @return ConstReverseIterator_ A const reverse iterator to the element preceding the first element.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return array_.rend(); }

    /**
     * @brief Returns a const reverse iterator to the end of the array.
     * @return ConstReverseIterator_ A const reverse iterator to the element preceding the first element.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return array_.crend(); }

    /**
     * @brief Fills the array with a specific value.
     * @param _val The value to assign to each element.
     * @return Void None.
     */
    FORCEINLINE constexpr Void Fill(const _ObjectType& _val) noexcept { array_.fill(_val); }

    /**
     * @brief Swaps the contents of this array with another array.
     * @param _array The array to swap with.
     * @return Void None.
     */
    FORCEINLINE constexpr Void Swap(TFixedArray& _array) noexcept { array_.swap(_array.array_); }

public:
    /** @brief Alias for the Begin() function, used for stl compatibility. */
    NODISCARD FORCEINLINE Iterator_ begin() noexcept { return Begin(); }
    /** @brief Alias for the Begin() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ begin() const noexcept { return Begin(); }
    /** @brief Alias for the End() function, used for stl compatibility. */
    NODISCARD FORCEINLINE Iterator_ end() noexcept { return End(); }
    /** @brief Alias for the End() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ end() const noexcept { return End(); }

private:
    /** @brief The underlying standard array container. */
    STDFixedArray_ array_;
};

} // zengine