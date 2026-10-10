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

#include "f_string.h"
#include "z_object.h"
#include "z_string_view.h"

namespace zengine {
namespace internal {

/**
 * @brief Fixed-size string container template class.
 *
 * This class wraps std::array to provide a fixed-capacity string implementation with
 * formatting capabilities and ZEngine object semantics.
 *
 * @tparam _CharType The character type (e.g., Char, WChar).
 */
template<typename _CharType, SizeType kCapacity>
class TFixedString : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying standard array type. */
    using STDFixedArray_ = std::array<_CharType, kCapacity>;
    /** @brief Iterator type for the string. */
    using Iterator_ = typename STDFixedArray_::iterator;
    /** @brief Constant iterator type for the string. */
    using ConstIterator_ = typename STDFixedArray_::const_iterator;
    /** @brief Reverse iterator type for the string. */
    using ReverseIterator_ = typename STDFixedArray_::reverse_iterator;
    /** @brief Constant reverse iterator type for the string. */
    using ConstReverseIterator_ = typename STDFixedArray_::const_reverse_iterator;
    /** @brief Initializer list type for the string. */
    using InitializerList_ = std::initializer_list<_CharType>;

public:
    /**
     * @brief Default constructor.
     * Initializes the fixed string with default values.
     */
    FORCEINLINE constexpr TFixedString() noexcept 
        : SuperType_()
        , str_() 
    {}

    /**
     * @brief Copy constructor.
     * Creates a new fixed string as a copy of the given string.
     * 
     * @param _str The other fixed string to copy from.
     */
    FORCEINLINE constexpr TFixedString(const TFixedString& _str) noexcept 
        : SuperType_(_str)
        , str_(_str.str_) 
    {}

    /**
     * @brief Move constructor.
     * Moves the contents from the given fixed string to the new one.
     * 
     * @param _str The other fixed string to move from.
     */
    FORCEINLINE constexpr TFixedString(TFixedString&& _str) noexcept 
        : SuperType_(std::move(_str))
        , str_(std::move(_str.str_)) 
    {}

    /**
     * @brief Constructor with format string and arguments.
     * Generates a string based on the format and arguments provided.
     *
     * @tparam _ArgsType The types of the arguments.
     * @param _format The format string.
     * @param _args The arguments to format.
     */
    template<typename... _ArgsType>
    FORCEINLINE constexpr TFixedString(ZStringView _format, _ArgsType&&... _args) noexcept 
        : SuperType_() 
    {
        STATIC_ASSERT((kIsSameType<_CharType, Char>), "TFixedString: _CharType must be Char!");
        string::GenerateString(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Destructor.
     */
    FORCEINLINE constexpr ~TFixedString() noexcept = default;

    /**
     * @brief Copy assignment operator.
     *
     * @param _str The other fixed string to copy from.
     * @return TFixedString& Reference to this object.
     */
    FORCEINLINE TFixedString& operator=(const TFixedString& _str) noexcept {
        SuperType_::operator=(_str);
        str_ = _str.str_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param _str The other fixed string to move from.
     * @return TFixedString& Reference to this object.
     */
    FORCEINLINE TFixedString& operator=(TFixedString&& _str) noexcept {
        SuperType_::operator=(std::move(_str));
        str_ = std::move(_str.str_);
        return *this;
    }

    /**
     * @brief Assigns a formatted string to the container.
     *
     * @tparam _ArgsType The types of the arguments.
     * @param _format The format string.
     * @param _args The arguments to format.
     * @return SizeType The number of characters written.
     */
    template<typename... _ArgsType>
    FORCEINLINE constexpr SizeType Assign(ZStringView _format, _ArgsType&&... _args) noexcept {
        STATIC_ASSERT((kIsSameType<_CharType, Char>), "TFixedString: _CharType must be Char!");
        return string::GenerateString(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Assigns a formatted string to the container without appending a null terminator.
     *
     * @tparam _ArgsType The types of the arguments.
     * @param _format The format string.
     * @param _args The arguments to format.
     * @return SizeType The number of characters written.
     */
    template<typename... _ArgsType>
    FORCEINLINE constexpr SizeType AssignNoEnd(ZStringView _format, _ArgsType&&... _args) noexcept {
        STATIC_ASSERT((kIsSameType<_CharType, Char>), "TFixedString: _CharType must be Char!");
        return string::GenerateStringNoEnd(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Gets the pointer to the underlying data.
     *
     * @return _CharType* Pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr _CharType* GetDataPtr() noexcept { return str_.data(); }

    /**
     * @brief Gets the pointer to the underlying data (const).
     *
     * @return const _CharType* Constant pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr const _CharType* GetDataPtr() const noexcept { return str_.data(); }

    /**
     * @brief Gets the capacity of the fixed string.
     *
     * @return SizeType The capacity.
     */
    NODISCARD FORCEINLINE static constexpr SizeType GetCapacity() noexcept { return kCapacity; }

    /**
     * @brief Checks if this string is equal to another fixed string.
     * Performs a C-style string comparison.
     *
     * @param _str The other fixed string to compare with.
     * @return Bool True if strings are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TFixedString& _str) noexcept { 
        if constexpr (kIsSameType<_CharType, Char>) {
            return strcmp(GetDataPtr(), _str.GetDataPtr()) == 0;
        }
        else {
            return wcscmp(GetDataPtr(), _str.GetDataPtr()) == 0;
        }
    }

    /**
     * @brief Checks if this string is not equal to another fixed string.
     * Performs a C-style string comparison.
     *
     * @param _str The other fixed string to compare with.
     * @return Bool True if strings are not equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TFixedString& _str) noexcept {
        if constexpr (kIsSameType<_CharType, Char>) {
            return strcmp(GetDataPtr(), _str.GetDataPtr()) == 0;
        }
        else {
            return wcscmp(GetDataPtr(), _str.GetDataPtr()) == 0;
        }
    }

    /**
     * @brief Access element at the given index.
     * Does not perform bounds checking.
     *
     * @param _index The index of the element.
     * @return _CharType& Reference to the element.
     */
    NODISCARD FORCEINLINE constexpr _CharType& operator[](SizeType _index) noexcept { return str_[_index]; }

    /**
     * @brief Access element at the given index (const).
     * Does not perform bounds checking.
     *
     * @param _index The index of the element.
     * @return const _CharType& Constant reference to the element.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](SizeType _index) const noexcept { 
        return str_[_index]; 
    }

    /**
     * @brief Access element at the given index with bounds checking.
     *
     * @param _index The index of the element.
     * @return _CharType& Reference to the element.
     */
    NODISCARD FORCEINLINE constexpr _CharType& At(SizeType _index) noexcept { return str_.at(_index); }

    /**
     * @brief Access element at the given index with bounds checking (const).
     *
     * @param _index The index of the element.
     * @return const _CharType& Constant reference to the element.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const noexcept { return str_.at(_index); } 

    /**
     * @brief Returns an iterator to the beginning.
     *
     * @return Iterator_ Iterator to the beginning.
     */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return str_.begin(); }

    /**
     * @brief Returns a constant iterator to the beginning.
     *
     * @return ConstIterator_ Constant iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return str_.begin(); }

    /**
     * @brief Returns a constant iterator to the beginning (explicit const).
     *
     * @return ConstIterator_ Constant iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return str_.cbegin(); }

    /**
     * @brief Returns a reverse iterator to the beginning.
     *
     * @return ReverseIterator_ Reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return str_.rbegin(); }

    /**
     * @brief Returns a constant reverse iterator to the beginning.
     *
     * @return ConstReverseIterator_ Constant reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return str_.rbegin(); }

    /**
     * @brief Returns a constant reverse iterator to the beginning (explicit const).
     *
     * @return ConstReverseIterator_ Constant reverse iterator to the beginning.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return str_.crbegin(); }

    /**
     * @brief Returns an iterator to the end.
     *
     * @return Iterator_ Iterator to the end.
     */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return str_.end(); }

    /**
     * @brief Returns a constant iterator to the end.
     *
     * @return ConstIterator_ Constant iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return str_.end(); }

    /**
     * @brief Returns a constant iterator to the end (explicit const).
     *
     * @return ConstIterator_ Constant iterator to the end.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return str_.cend(); }

    /**
     * @brief Returns a reverse iterator to the end.
     *
     * @return ReverseIterator_ Reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return str_.rend(); }

    /**
     * @brief Returns a constant reverse iterator to the end.
     *
     * @return ConstReverseIterator_ Constant reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return str_.rend(); }

    /**
     * @brief Returns a constant reverse iterator to the end (explicit const).
     *
     * @return ConstReverseIterator_ Constant reverse iterator to the end.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return str_.crend(); }

    /**
     * @brief Fills the container with a specific value.
     *
     * @param _val The value to fill with.
     */
    FORCEINLINE Void Fill(const _CharType& _val) noexcept { str_.fill(_val); }

    /**
     * @brief Swaps the contents with another TFixedString.
     *
     * @param _array The other TFixedString to swap with.
     */
    FORCEINLINE Void Swap(TFixedString& _array) noexcept { str_.swap(_array.str_); }

private:
    /** @brief The underlying standard array storage. */
    STDFixedArray_ str_;
};

} // internal

/** @brief Alias for a fixed-size narrow character string. */
template<SizeType kCapacity>
using TFixedString = internal::TFixedString<Char, kCapacity>;
/** @brief Alias for a fixed-size wide character string. */
template<SizeType kCapacity>
using TFixedWString = internal::TFixedString<WChar, kCapacity>;

} // zengine