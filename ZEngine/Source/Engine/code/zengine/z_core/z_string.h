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

#include <string>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {
namespace internal {

/**
 * @brief A string container class inheriting from ZObject.
 *
 * This class wraps std::basic_string with a custom allocator (TAllocator) to integrate
 * with the ZEngine ecosystem. It provides extended functionality for string manipulation,
 * conversion, and object management.
 *
 * @tparam _CharType The character type of the string (e.g., char, wchar_t).
 * @tparam _IfAllocFromThreadLocalMemoryPool Flag indicating if memory should be allocated from the thread-local pool.
 */
template<typename _CharType, Bool _IsGlobal>
class CORE_DLLAPI TString : public ZObject<_IsGlobal> {
public:
    /** @brief Alias for the underlying standard string type with custom allocator. */
    using STDString_ = std::basic_string<_CharType, std::char_traits<_CharType>, TAllocator<_CharType>>;
    /** @brief Iterator type. */
    using Iterator_ = typename STDString_::iterator;
    /** @brief Constant iterator type. */
    using ConstIterator_ = typename STDString_::const_iterator;
    /** @brief Reverse iterator type. */
    using ReverseIterator_ = typename STDString_::reverse_iterator;
    /** @brief Constant reverse iterator type. */
    using ConstReverseIterator_ = typename STDString_::const_reverse_iterator;
    /** @brief Alias for initializer list. */
    using InitializerList_ = std::initializer_list<_CharType>;

    /**
     * @brief Constant representing the end of the string or an invalid position.
     * Maps to std::string::npos.
     */
    static constexpr SizeType kEnd = std::string::npos;

    /**
     * @brief Default constructor.
     * Initializes an empty string.
     */
    FORCEINLINE constexpr TString() noexcept 
        : SuperType_()
        , str_() 
    {}
    
    /**
     * @brief Copy constructor.
     * Creates a new string as a copy of the given string.
     * 
     * @param _str The source string to copy.
     */
    FORCEINLINE constexpr TString(const TString& _str) noexcept 
        : SuperType_(_str)
        , str_(_str.str_) 
    {}
    
    /**
     * @brief Move constructor.
     * Moves the contents from the given string to the new string.
     * 
     * @param _str The source string to move.
     */
    FORCEINLINE constexpr TString(TString&& _str) noexcept 
        : SuperType_(std::forward<TString>(_str))
        , str_(std::move(_str.str_)) 
    {}
    
    /** 
     * @brief Substring constructor.
     * Creates a string from a substring of another string.
     * 
     * @param _str Source string.
     * @param _pos Starting position in the source string.
     * @param _len Length of the substring (default is kEnd, meaning until the end).
     */
    FORCEINLINE constexpr TString(const TString& _str, SizeType _pos, SizeType _len = kEnd) noexcept
        : SuperType_()
        , str_(_str.str_, _pos, _len) 
    {}
    
    /**
     * @brief Constructor from a C-style string.
     * 
     * @param _str Null-terminated C-style string.
     */
    FORCEINLINE constexpr TString(const _CharType* _str) noexcept 
        : SuperType_()
        , str_(_str) 
    {}
    
    /**
     * @brief Constructor from a C-style string with a specific size.
     * 
     * @param _str Pointer to the character array.
     * @param _size Number of characters to copy.
     */
    FORCEINLINE constexpr TString(const _CharType* _str, SizeType _size) noexcept 
        : SuperType_()
        , str_(_str, _size) 
    {}
    
    /**
     * @brief Fill constructor.
     * Creates a string with `_size` copies of character `_val`.
     * 
     * @param _size The number of characters.
     * @param _val The character value to repeat.
     */
    FORCEINLINE constexpr TString(SizeType _size, _CharType& _val) noexcept 
        : SuperType_()
        , str_(_size, _val) 
    {}
    
    /**
     * @brief Range constructor.
     * Creates a string from the range `[_first, _last)`.
     * 
     * @tparam _InputIterator Type of the input iterator.
     * @param _first Iterator to the beginning of the range.
     * @param _last Iterator to the end of the range.
     */
    template <typename _InputIterator>
    FORCEINLINE constexpr TString(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_()
        , str_(_first, _last) 
    {}
    
    /**
     * @brief Initializer list constructor.
     * 
     * @param _init_list The list of characters to initialize the string with.
     */
    FORCEINLINE constexpr TString(InitializerList_ _init_list) noexcept 
        : SuperType_()
        , str_(_init_list) 
    {}
    
    /**
     * @brief Constructor from the underlying std::basic_string type (Copy).
     * 
     * @param _std_str The source standard string.
     */
    FORCEINLINE constexpr TString(const STDString_& _std_str) noexcept 
        : SuperType_()
        , str_(_std_str) 
    {}
    
    /**
     * @brief Constructor from the underlying std::basic_string type (Move).
     * 
     * @param _std_str The source standard string to move.
     */
    FORCEINLINE constexpr TString(STDString_&& _std_str) noexcept 
        : SuperType_()
        , str_(std::forward<STDString_>(_std_str)) 
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE constexpr ~TString() noexcept {}

    /**
     * @brief Copy assignment operator.
     * 
     * @param _str The string to copy.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(const TString& _str) noexcept {
        SuperType_::operator=(_str);
        str_ = _str.str_;
        return *this;
    }
    
    /**
     * @brief Move assignment operator.
     * 
     * @param _str The string to move.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(TString&& _str) noexcept {
        SuperType_::operator=(std::forward<TString>(_str));
        str_ = std::move(_str.str_);
        return *this;
    }
    
    /**
     * @brief Assigns a C-style string to this string.
     * 
     * @param _str The C-style string to assign.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(const _CharType* _str) noexcept {
        str_ = _str;
        return *this;
    }
    
    /**
     * @brief Assigns a single character to this string.
     * 
     * @param _char The character to assign.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(_CharType _char) noexcept {
        str_ = _char;
        return *this;
    }
    
    /**
     * @brief Assigns an initializer list to this string.
     * 
     * @param _init_list The initializer list.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(InitializerList_ _init_list) noexcept {
        str_ = _init_list;
        return *this;
    }
    
    /**
     * @brief Assigns a standard string to this string (Copy).
     * 
     * @param _std_str The standard string to copy.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(const STDString_& _std_str) noexcept {
        str_ = _std_str;
        return *this;
    }
    
    /**
     * @brief Assigns a standard string to this string (Move).
     * 
     * @param _std_str The standard string to move.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator=(STDString_&& _std_str) noexcept {
        str_ = std::move(_std_str);
        return *this;
    }

    /**
     * @brief Replaces the contents with a copy of another string.
     * 
     * @param _str The source string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(const TString& _str) noexcept {
        str_.assign(_str.str_);
        return *this;
    }
    
    /**
     * @brief Replaces the contents by moving another string.
     * 
     * @param _str The source string (rvalue).
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(TString&& _str) noexcept {
        str_.assign(std::move(_str.str_));
        return *this;
    }
    
    /**
     * @brief Replaces the contents with a substring of another string.
     * 
     * @param _str The source string.
     * @param _pos The starting position in the source string.
     * @param _len The length of the substring (default kEnd).
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(const TString& _str, SizeType _pos, SizeType _len = kEnd) noexcept {
        str_.assign(_str.str_, _pos, _len);
        return *this;
    }
    
    /**
     * @brief Replaces the contents with a C-style string.
     * 
     * @param _str The C-style string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(const _CharType* _str) noexcept {
        str_.assign(_str);
        return *this;
    }
    
    /**
     * @brief Replaces the contents with the first `_size` characters of a C-style string.
     * 
     * @param _str The C-style string.
     * @param _size The number of characters to copy.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(const _CharType* _str, SizeType _size) noexcept {
        str_.assign(_str, _size);
        return *this;
    }
    
    /**
     * @brief Replaces the contents with `_size` copies of a character.
     * 
     * @param _size The number of characters.
     * @param _val The character value.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(const SizeType _size, const _CharType& _val) noexcept {
        str_.assign(_size, _val);
        return *this;
    }
    
    /**
     * @brief Replaces the contents with characters from a range.
     * 
     * @tparam _InputIterator Type of the input iterator.
     * @param _first Iterator to the start of the range.
     * @param _last Iterator to the end of the range.
     * @return TString& Reference to this string.
     */
    template <class _InputIterator>
    FORCEINLINE constexpr TString& Assign(_InputIterator _first, _InputIterator _last) noexcept {
        str_.assign(_first, _last);
        return *this;
    }
    
    /**
     * @brief Replaces the contents with an initializer list.
     * 
     * @param _init_list The initializer list.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Assign(InitializerList_ _init_list) noexcept {
        str_.assign(_init_list);
        return *this;
    }

    /* Comparison Operators */

    /**
     * @brief Checks if two strings are equal.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ == _right_str.str_;
    }

    /**
     * @brief Checks if a string and a C-style string are equal.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ == _right_str;
    }

    /**
     * @brief Checks if a C-style string and a string are equal.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str == _right_str.str_;
    }
    
    /**
     * @brief Checks if two strings are not equal.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if not equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ != _right_str.str_;
    }

    /**
     * @brief Checks if a string and a C-style string are not equal.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if not equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ != _right_str;
    }

    /**
     * @brief Checks if a C-style string and a string are not equal.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if not equal, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str != _right_str.str_;
    }
    
    /**
     * @brief Checks if the left string is greater than the right string.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if left > right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ > _right_str.str_;
    }

    /**
     * @brief Checks if the string is greater than the C-style string.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if left > right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ > _right_str;
    }

    /**
     * @brief Checks if the C-style string is greater than the string.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if left > right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str > _right_str.str_;
    }
    
    /**
     * @brief Checks if the left string is greater than or equal to the right string.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if left >= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ >= _right_str.str_;
    }

    /**
     * @brief Checks if the string is greater than or equal to the C-style string.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if left >= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ >= _right_str;
    }

    /**
     * @brief Checks if the C-style string is greater than or equal to the string.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if left >= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str >= _right_str.str_;
    }
    
    /**
     * @brief Checks if the left string is less than the right string.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if left < right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ < _right_str.str_;
    }

    /**
     * @brief Checks if the string is less than the C-style string.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if left < right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ < _right_str;
    }

    /**
     * @brief Checks if the C-style string is less than the string.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if left < right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str < _right_str.str_;
    }
    
    /**
     * @brief Checks if the left string is less than or equal to the right string.
     * @param _left_str The left operand.
     * @param _right_str The right operand.
     * @return Bool True if left <= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ <= _right_str.str_;
    }

    /**
     * @brief Checks if the string is less than or equal to the C-style string.
     * @param _left_str The left operand (TString).
     * @param _right_str The right operand (C-string).
     * @return Bool True if left <= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str,  const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ <= _right_str;
    }

    /**
     * @brief Checks if the C-style string is less than or equal to the string.
     * @param _left_str The left operand (C-string).
     * @param _right_str The right operand (TString).
     * @return Bool True if left <= right, false otherwise.
     */
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str <= _right_str.str_;
    }

    /**
     * @brief Appends another string to this string.
     * @param _str The string to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator+=(const TString& _str) noexcept { 
        str_ += _str.str_; 
        return *this;
    }

    /**
     * @brief Appends a C-style string to this string.
     * @param _str The C-style string to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator+=(const _CharType* _str) noexcept { 
        str_ += _str; 
        return *this;
    }

    /**
     * @brief Appends a character to this string.
     * @param _char The character to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator+=(const _CharType _char) noexcept { 
        str_ += _char; 
        return *this;
    }

    /**
     * @brief Appends an initializer list to this string.
     * @param _init_list The initializer list to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& operator+=(InitializerList_ _init_list) noexcept { 
        str_ += _init_list; 
        return *this;
    }

    /* Concatenation Operators */

    /**
     * @brief Concatenates two strings.
     * @param _left_str The left string.
     * @param _right_str The right string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        TString str;
        str.Reserve(_left_str.GetSize() + _right_str.GetSize());
        str.Append(_left_str);
        str.Append(_right_str);
        return str;
    }

    /**
     * @brief Concatenates an rvalue string and a const string.
     * @param _left_str The left string (rvalue).
     * @param _right_str The right string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        TString&& _left_str, const TString& _right_str
    ) noexcept {
        _left_str.Append(_right_str);
        return std::move(_left_str);
    }

    /**
     * @brief Concatenates a const string and an rvalue string.
     * @param _left_str The left string.
     * @param _right_str The right string (rvalue).
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const TString& _left_str, TString&& _right_str
    ) noexcept {
        _right_str.Insert(0, _left_str);
        return std::move(_right_str);
    }

    /**
     * @brief Concatenates two rvalue strings.
     * @param _left_str The left string (rvalue).
     * @param _right_str The right string (rvalue).
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        TString&& _left_str, TString&& _right_str
    ) noexcept {
        _left_str.Append(_right_str);
        return std::move(_left_str);
    }

    /**
     * @brief Concatenates a string and a C-style string.
     * @param _left_str The left string.
     * @param _right_str The right C-style string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        TString str;
        const SizeType right_str_len = std::char_traits<_CharType>::length(_right_str);
        str.Reserve(_left_str.GetSize() + right_str_len);
        str.Append(_left_str);
        str.Append(_right_str, right_str_len);
        return str;
    }

    /**
     * @brief Concatenates an rvalue string and a C-style string.
     * @param _left_str The left string (rvalue).
     * @param _right_str The right C-style string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        TString&& _left_str, const _CharType* _right_str
    ) noexcept {
        const SizeType right_str_len = std::char_traits<_CharType>::length(_right_str);
        _left_str.Append(_right_str, right_str_len);
        return std::move(_left_str);
    }

    /**
     * @brief Concatenates a C-style string and a string.
     * @param _left_str The left C-style string.
     * @param _right_str The right string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        TString str;
        const SizeType left_str_len = std::char_traits<_CharType>::length(_left_str);
        str.Reserve(left_str_len + _right_str.GetSize());
        str.Append(_left_str, left_str_len);
        str.Append(_right_str);
        return str;
    }

    /**
     * @brief Concatenates a C-style string and an rvalue string.
     * @param _left_str The left C-style string.
     * @param _right_str The right string (rvalue).
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const _CharType* _left_str, TString&& _right_str
    ) noexcept {
        const SizeType left_str_len = std::char_traits<_CharType>::length(_left_str);
        _right_str.Insert(0, _left_str, left_str_len);
        return std::move(_right_str);
    }

    /**
     * @brief Concatenates a string and a character.
     * @param _left_str The left string.
     * @param _right_str The right character.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const TString& _left_str, const _CharType _right_str
    ) noexcept {
        TString str;
        str.Reserve(_left_str.GetSize() + 1);
        str.Append(_left_str);
        str.PushBack(_right_str);
        return str;
    }

    /**
     * @brief Concatenates an rvalue string and a character.
     * @param _left_str The left string (rvalue).
     * @param _right_str The right character.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        TString&& _left_str, const _CharType _right_str
    ) noexcept {
        _left_str.PushBack(_right_str);
        return std::move(_left_str);
    }

    /**
     * @brief Concatenates a character and a string.
     * @param _left_str The left character.
     * @param _right_str The right string.
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const _CharType _left_str, const TString& _right_str
    ) noexcept {
        TString str;
        str.Reserve(1 + _right_str.GetSize());
        str.PushBack(_left_str);
        str.Append(_right_str);
        return str;
    }

    /**
     * @brief Concatenates a character and an rvalue string.
     * @param _left_str The left character.
     * @param _right_str The right string (rvalue).
     * @return TString A new TString containing the result.
     */
    friend NODISCARD FORCEINLINE TString operator+(
        const _CharType _left_str, TString&& _right_str
    ) noexcept {
        _right_str.Insert(0, 1, _left_str);
        return std::move(_right_str);
    }

    /**
     * @brief Accesses the character at the specified index.
     * Does not perform bounds checking.
     * 
     * @param _index The index of the character.
     * @return _CharType& Reference to the character.
     */
    NODISCARD FORCEINLINE constexpr _CharType& operator[](const SizeType _index) noexcept { return str_[_index]; }
    
    /**
     * @brief Accesses the character at the specified index (const).
     * Does not perform bounds checking.
     * 
     * @param _index The index of the character.
     * @return const _CharType& Const reference to the character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept {
        return str_[_index];
    }

    /**
     * @brief Accesses the character at the specified index with bounds checking.
     * 
     * @param _index The index of the character.
     * @return _CharType& Reference to the character.
     */
    NODISCARD FORCEINLINE constexpr _CharType& At(SizeType _index) { return str_.at(_index); }
    
    /**
     * @brief Accesses the character at the specified index with bounds checking (const).
     * 
     * @param _index The index of the character.
     * @return const _CharType& Const reference to the character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const { return str_.at(_index); }

    /**
     * @brief Returns a reference to the first character.
     * 
     * @return _CharType& Reference to the first character.
     */
    NODISCARD FORCEINLINE constexpr _CharType& Front() noexcept { return str_.front(); }
    
    /**
     * @brief Returns a reference to the first character (const).
     * 
     * @return const _CharType& Const reference to the first character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& Front() const noexcept { return str_.front(); }
    
    /**
     * @brief Returns a reference to the last character.
     * 
     * @return _CharType& Reference to the last character.
     */
    NODISCARD FORCEINLINE constexpr _CharType& Back() noexcept { return str_.back(); }
    
    /**
     * @brief Returns a reference to the last character (const).
     * 
     * @return const _CharType& Const reference to the last character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& Back() const noexcept { return str_.back(); }
    
    /**
     * @brief Returns a pointer to the underlying character array.
     * 
     * @return _CharType* Pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr _CharType* GetDataPtr() noexcept { return str_.data(); }
    
    /**
     * @brief Returns a pointer to the underlying character array (const).
     * 
     * @return const _CharType* Const pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr const _CharType* GetDataPtr() const noexcept { return str_.data(); }
    
    /**
     * @brief Returns a reference to the underlying standard string object.
     * 
     * @return STDString_& Reference to the std::basic_string.
     */
    NODISCARD FORCEINLINE constexpr STDString_& STDString() noexcept { return str_; }
    
    /**
     * @brief Returns a reference to the underlying standard string object (const).
     * 
     * @return const STDString_& Const reference to the std::basic_string.
     */
    NODISCARD FORCEINLINE constexpr const STDString_& STDString() const noexcept { return str_; }

    /**
     * @brief Returns the number of characters in the string.
     * 
     * @return SizeType The size of the string.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { return static_cast<SizeType>(str_.size()); }
    
    /**
     * @brief Returns the current capacity of the string.
     * 
     * @return SizeType The number of characters that can be held without reallocation.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetCapacity() const noexcept { return str_.capacity(); }
    
    /**
     * @brief Checks if the string is empty.
     * 
     * @return Bool True if the string is empty, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool IsEmpty() const noexcept { return str_.empty(); }

    /**
     * @brief Returns an iterator to the beginning of the string.
     * 
     * @return Iterator_ Iterator to the first character.
     */
    NODISCARD FORCEINLINE constexpr Iterator_ Begin() noexcept { return str_.begin(); }
    
    /**
     * @brief Returns an iterator to the beginning of the string (const).
     * 
     * @return ConstIterator_ Const iterator to the first character.
     */
    NODISCARD FORCEINLINE constexpr ConstIterator_ Begin() const noexcept { return str_.begin(); }
    
    /**
     * @brief Returns a const iterator to the beginning of the string.
     * 
     * @return ConstIterator_ Const iterator to the first character.
     */
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstBegin() const noexcept { return str_.cbegin(); }
    
    /**
     * @brief Returns a reverse iterator to the beginning of the reversed string.
     * 
     * @return ReverseIterator_ Reverse iterator to the last character.
     */
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseBegin() noexcept { return str_.rbegin(); }
    
    /**
     * @brief Returns a reverse iterator to the beginning of the reversed string (const).
     * 
     * @return ConstReverseIterator_ Const reverse iterator to the last character.
     */
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseBegin() const noexcept { return str_.rbegin(); }
    
    /**
     * @brief Returns a const reverse iterator to the beginning of the reversed string.
     * 
     * @return ConstReverseIterator_ Const reverse iterator to the last character.
     */
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseBegin() const noexcept { return str_.crbegin(); }
    
    /**
     * @brief Returns an iterator to the end of the string.
     * 
     * @return Iterator_ Iterator to the element following the last character.
     */
    NODISCARD FORCEINLINE constexpr Iterator_ End() noexcept { return str_.end(); }
    
    /**
     * @brief Returns an iterator to the end of the string (const).
     * 
     * @return ConstIterator_ Const iterator to the element following the last character.
     */
    NODISCARD FORCEINLINE constexpr ConstIterator_ End() const noexcept { return str_.end(); }
    
    /**
     * @brief Returns a const iterator to the end of the string.
     * 
     * @return ConstIterator_ Const iterator to the element following the last character.
     */
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstEnd() const noexcept { return str_.cend(); }
    
    /**
     * @brief Returns a reverse iterator to the end of the reversed string.
     * 
     * @return ReverseIterator_ Reverse iterator to the element preceding the first character.
     */
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseEnd() noexcept { return str_.rend(); }
    
    /**
     * @brief Returns a reverse iterator to the end of the reversed string (const).
     * 
     * @return ConstReverseIterator_ Const reverse iterator to the element preceding the first character.
     */
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseEnd() const noexcept { return str_.rend(); }
    
    /**
     * @brief Returns a const reverse iterator to the end of the reversed string.
     * 
     * @return ConstReverseIterator_ Const reverse iterator to the element preceding the first character.
     */
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseEnd() const noexcept { return str_.crend(); }

    /**
     * @brief Appends a character to the end of the string.
     * 
     * @param _char The character to append.
     */
    FORCEINLINE constexpr Void PushBack(const _CharType _char) noexcept {
        str_.push_back(_char);
    }
    
    /**
     * @brief Removes the last character from the string.
     */
    FORCEINLINE constexpr Void PopBack() noexcept {
        str_.pop_back();
    }

    /**
     * @brief Appends a string to the end of this string.
     * 
     * @param _str The string to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(const TString& _str) noexcept { 
        str_.append(_str.str_); 
        return *this;
    }
    
    /**
     * @brief Appends a substring of another string to the end.
     * 
     * @param _str The source string.
     * @param _pos The starting position in the source string.
     * @param _len The number of characters to append (default kEnd).
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(const TString& _str, SizeType _pos, SizeType _len = kEnd) noexcept {
        str_.append(_str.str_, _pos, _len);
        return *this;
    }
    
    /**
     * @brief Appends a C-style string to the end.
     * 
     * @param _str The C-style string to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(const _CharType* _str) noexcept { 
        str_.append(_str); 
        return *this;
    }
    
    /**
     * @brief Appends the first `_str_len` characters of a C-style string to the end.
     * 
     * @param _str The C-style string.
     * @param _str_len The number of characters to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(const _CharType* _str, SizeType _str_len) noexcept {
        str_.append(_str, _str_len);
        return *this;
    }
    
    /**
     * @brief Appends `_size` copies of a character to the end.
     * 
     * @param _size The number of copies.
     * @param _char The character to append.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(SizeType _size, const _CharType _char) noexcept { 
        str_.append(_size, _char); 
        return *this;
    }
    
    /**
     * @brief Appends characters from an initializer list to the end.
     * 
     * @param _init_list The initializer list.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Append(InitializerList_ _init_list) noexcept { 
        str_.append(_init_list); 
        return *this;
    }

    /**
     * @brief Inserts a string at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _str The string to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const TString& _str) noexcept {
        str_.insert(_pos, _str.str_); 
        return *this;
    }
    
    /**
     * @brief Inserts a substring of another string at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _str The source string.
     * @param _str_pos The starting position in the source string.
     * @param _len The length of the substring to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(
        const SizeType _pos, const TString& _str, SizeType _str_pos, SizeType _len = kEnd
    ) noexcept {
        str_.insert(_pos, _str.str_, _str_pos, _len);
        return *this;
    }
    
    /**
     * @brief Inserts a C-style string at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _str The C-style string to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _str) noexcept {
        str_.insert(_pos, _str);
        return *this;
    }
    
    /**
     * @brief Inserts the first `_str_len` characters of a C-style string at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _str The C-style string.
     * @param _str_len The number of characters to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _str, SizeType _str_len) noexcept {
        str_.insert(_pos, _str, _str_len);
        return *this;
    }
    
    /**
     * @brief Inserts `_size` copies of a character at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _size The number of copies.
     * @param _char The character to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, SizeType _size, const _CharType _char) noexcept {
        str_.insert(_pos, _size, _char); 
        return *this;
    }
    
    /**
     * @brief Inserts an initializer list at the specified position.
     * 
     * @param _pos The insertion position.
     * @param _init_list The initializer list.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, InitializerList_ _init_list) noexcept {
        str_.insert(_pos, _init_list); 
        return *this;
    }
    
    /**
     * @brief Inserts `_size` copies of a character at the iterator position.
     * 
     * @param _pos Iterator to the insertion position.
     * @param _size The number of copies.
     * @param _char The character to insert.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, SizeType _size, const _CharType _char) noexcept {
        str_.insert(_pos, _size, _char);
        return *this;
    }
    
    /**
     * @brief Inserts an initializer list at the iterator position.
     * 
     * @param _pos Iterator to the insertion position.
     * @param _init_list The initializer list.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        str_.insert(_pos, _init_list);
        return *this;
    }
    
    /**
     * @brief Inserts a range of characters at the iterator position.
     * 
     * @tparam InputIterator Type of the input iterator.
     * @param _pos Iterator to the insertion position.
     * @param _first Iterator to the start of the range.
     * @param _last Iterator to the end of the range.
     * @return TString& Reference to this string.
     */
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Insert(
        const ConstIterator_ _pos, InputIterator _first, InputIterator _last
    ) noexcept {
        str_.insert(_pos, _first, _last);
        return *this;
    }

    /**
     * @brief Replaces a portion of the string with another string.
     * 
     * @param _pos Start position of the replacement.
     * @param _len Number of characters to replace.
     * @param _str The replacement string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(const SizeType _pos, const SizeType _len, const TString& _str) noexcept {
        str_.replace(_pos, _len, _str.str_);
        return *this;
    }
    
    /**
     * @brief Replaces a portion of the string with a substring of another string.
     * 
     * @param _pos Start position of the replacement.
     * @param _len Number of characters to replace.
     * @param _str The source string.
     * @param _str_pos Start position in the source string.
     * @param _str_len Length of the source substring (default kEnd).
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const TString& _str, SizeType _str_pos, SizeType _str_len = kEnd
    ) noexcept {
        str_.replace(_pos, _len, _str.str_, _str_pos, _str_len);
        return *this;
    }
    
    /**
     * @brief Replaces a portion of the string with a C-style string.
     * 
     * @param _pos Start position of the replacement.
     * @param _len Number of characters to replace.
     * @param _str The replacement C-style string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _str
    ) noexcept {
        str_.replace(_pos, _len, _str);
        return *this;
    }
    
    /**
     * @brief Replaces a portion of the string with the first `_str_len` characters of a C-style string.
     * 
     * @param _pos Start position of the replacement.
     * @param _len Number of characters to replace.
     * @param _str The replacement C-style string.
     * @param _str_len Number of characters from the C-style string to use.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _str, SizeType _str_len
    ) noexcept {
        str_.replace(_pos, _len, _str, _str_len);
        return *this;
    }
    
    /**
     * @brief Replaces a portion of the string with `_size` copies of a character.
     * 
     * @param _pos Start position of the replacement.
     * @param _len Number of characters to replace.
     * @param _size Number of copies of the character.
     * @param _char The replacement character.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, SizeType _size, const _CharType _char
    ) noexcept {
        str_.replace(_pos, _len, _size, _char);
        return *this;
    }
    
    /**
     * @brief Replaces the range `[_first, _last)` with another string.
     * 
     * @param _first Iterator to the start of the range to replace.
     * @param _last Iterator to the end of the range to replace.
     * @param _str The replacement string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const TString& _str
    ) noexcept {
        str_.replace(_first, _last, _str.str_);
        return *this;
    }
    
    /**
     * @brief Replaces the range `[_first, _last)` with the first `_str_len` characters of a C-style string.
     * 
     * @param _first Iterator to the start of the range to replace.
     * @param _last Iterator to the end of the range to replace.
     * @param _str The replacement C-style string.
     * @param _str_len The number of characters to use.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _str, SizeType _str_len
    ) noexcept {
        str_.replace(_first, _last, _str, _str_len);
        return *this;
    }
    
    /**
     * @brief Replaces the range `[_first, _last)` with a C-style string.
     * 
     * @param _first Iterator to the start of the range to replace.
     * @param _last Iterator to the end of the range to replace.
     * @param _str The replacement C-style string.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _str
    ) noexcept {
        str_.replace(_first, _last, _str);
        return *this;
    }
    
    /**
     * @brief Replaces the range `[_first, _last)` with `_size` copies of a character.
     * 
     * @param _first Iterator to the start of the range to replace.
     * @param _last Iterator to the end of the range to replace.
     * @param _size The number of copies.
     * @param _char The replacement character.
     * @return TString& Reference to this string.
     */
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, SizeType _size, const _CharType _char
    ) noexcept {
        str_.replace(_first, _last, _size, _char);
        return *this;
    }
    
    /**
     * @brief Replaces the range `[_first, _last)` with the range `[_str_first, _str_last)`.
     * 
     * @tparam InputIterator Type of the input iterator.
     * @param _first Iterator to the start of the range to replace.
     * @param _last Iterator to the end of the range to replace.
     * @param _str_first Iterator to the start of the replacement range.
     * @param _str_last Iterator to the end of the replacement range.
     * @return TString& Reference to this string.
     */
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, InputIterator _str_first, InputIterator _str_last
    ) noexcept {
        str_.replace(_first, _last, _str_first, _str_last);
        return *this;
    }

    /**
     * @brief Finds the first occurrence of a string.
     * 
     * @param _str The string to find.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType Find(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the first occurrence of a C-style string.
     * 
     * @param _str The C-style string to find.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType Find(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find(_str, _start_pos);
    }
    
    /**
     * @brief Finds the first occurrence of the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string to find.
     * @param _str_len The length of the substring to find.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType Find(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the first occurrence of a character.
     * 
     * @param _char The character to find.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType Find(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find(_char, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of a string.
     * 
     * @param _str The string to find.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType ReverseFind(const TString& _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.rfind(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of a C-style string.
     * 
     * @param _str The C-style string to find.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType ReverseFind(const _CharType* _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.rfind(_str, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string to find.
     * @param _str_len The length of the substring to find.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType ReverseFind(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = kEnd
    ) noexcept {
        return str_.rfind(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the last occurrence of a character.
     * 
     * @param _char The character to find.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType ReverseFind(const _CharType _char, const SizeType _start_pos = kEnd) noexcept {
        return str_.rfind(_char, _start_pos);
    }
    
    /**
     * @brief Finds the first occurrence of any character from the given string.
     * 
     * @param _str The string containing the characters to search for.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindFirstOf(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the first occurrence of any character from the given C-style string.
     * 
     * @param _str The C-style string containing the characters to search for.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_str, _start_pos);
    }
    
    /**
     * @brief Finds the first occurrence of any character from the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string containing the characters to search for.
     * @param _str_len The number of characters to check.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindFirstOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find_first_of(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the first occurrence of a specific character.
     * 
     * @param _char The character to search for.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_char, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of any character from the given string.
     * 
     * @param _str The string containing the characters to search for.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindLastOf(const TString& _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_of(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of any character from the given C-style string.
     * 
     * @param _str The C-style string containing the characters to search for.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType* _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_of(_str, _start_pos);
    }
    
    /**
     * @brief Finds the last occurrence of any character from the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string containing the characters to search for.
     * @param _str_len The number of characters to check.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindLastOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = kEnd
    ) noexcept {
        return str_.find_last_of(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the last occurrence of a specific character.
     * 
     * @param _char The character to search for.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType _char, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_of(_char, _start_pos);
    }
    
    /**
     * @brief Finds the first character not present in the given string.
     * 
     * @param _str The string containing the characters to avoid.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first character not in _str, or kEnd if all match.
     */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the first character not present in the given C-style string.
     * 
     * @param _str The C-style string containing the characters to avoid.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first character not in _str, or kEnd if all match.
     */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_str, _start_pos);
    }
    
    /**
     * @brief Finds the first character not present in the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string containing the characters to avoid.
     * @param _str_len The number of characters to check.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first character not in the substring, or kEnd if all match.
     */
    FORCEINLINE constexpr SizeType FindFirstNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find_first_not_of(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the first character that is not the specified character.
     * 
     * @param _char The character to avoid.
     * @param _start_pos The position to start searching from.
     * @return SizeType The index of the first character that is not _char, or kEnd.
     */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_char, _start_pos);
    }
    
    /**
     * @brief Finds the last character not present in the given string.
     * 
     * @param _str The string containing the characters to avoid.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last character not in _str, or kEnd.
     */
    FORCEINLINE constexpr SizeType FindLastNotOf(const TString& _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_not_of(_str.str_, _start_pos);
    }
    
    /**
     * @brief Finds the last character not present in the given C-style string.
     * 
     * @param _str The C-style string containing the characters to avoid.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last character not in _str, or kEnd.
     */
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType* _str, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_not_of(_str, _start_pos);
    }
    
    /**
     * @brief Finds the last character not present in the first `_str_len` characters of a C-style string.
     * 
     * @param _str_start The C-style string containing the characters to avoid.
     * @param _str_len The number of characters to check.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last character not in the substring, or kEnd.
     */
    FORCEINLINE constexpr SizeType FindLastNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = kEnd
    ) noexcept {
        return str_.find_last_not_of(_str_start, _start_pos, _str_len);
    }
    
    /**
     * @brief Finds the last character that is not the specified character.
     * 
     * @param _char The character to avoid.
     * @param _start_pos The position to start searching backwards from.
     * @return SizeType The index of the last character that is not _char, or kEnd.
     */
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType _char, const SizeType _start_pos = kEnd) noexcept {
        return str_.find_last_not_of(_char, _start_pos);
    }

    /**
     * @brief Creates a substring from this string.
     * 
     * @param _pos The starting position.
     * @param _len The length of the substring (default kEnd).
     * @return TString A new TString containing the substring.
     */
    FORCEINLINE TString SubString(const SizeType _pos, const SizeType _len = kEnd) noexcept {
        TString str;
        str.str_ = std::move(str_.substr(_pos, _len));
        return str;
    }

    /**
     * @brief Compares this string with another string.
     * 
     * @param _str The string to compare with.
     * @return Int32 0 if equal, < 0 if this < _str, > 0 if this > _str.
     */
    FORCEINLINE constexpr Int32 Compare(const TString& _str) noexcept { return str_.compare(_str.str_); }
    
    /**
     * @brief Compares a substring of this string with another string.
     * 
     * @param _pos The start position in this string.
     * @param _len The length of the substring in this string.
     * @param _str The string to compare with.
     * @return Int32 0 if equal, < 0 if substring < _str, > 0 if substring > _str.
     */
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const TString& _str) noexcept {
        return str_.compare(_pos, _len, _str.str_);
    }
    
    /**
     * @brief Compares a substring of this string with a substring of another string.
     * 
     * @param _pos The start position in this string.
     * @param _len The length of the substring in this string.
     * @param _str The other string.
     * @param _str_pos The start position in the other string.
     * @param _str_len The length of the substring in the other string.
     * @return Int32 0 if equal, < 0 if this substring < other substring, > 0 if this substring > other substring.
     */
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const TString& _str, SizeType _str_pos, SizeType _str_len
    ) noexcept {
        return str_.compare(_pos, _len, _str.str_, _str_pos, _str_len);
    }
    
    /**
     * @brief Compares this string with a C-style string.
     * 
     * @param _str The C-style string to compare with.
     * @return Int32 0 if equal, < 0 if this < _str, > 0 if this > _str.
     */
    FORCEINLINE constexpr Int32 Compare(const _CharType* _str) noexcept {
        return str_.compare(_str);
    }
    
    /**
     * @brief Compares a substring of this string with a C-style string.
     * 
     * @param _pos The start position in this string.
     * @param _len The length of the substring in this string.
     * @param _str The C-style string to compare with.
     * @return Int32 0 if equal, < 0 if substring < _str, > 0 if substring > _str.
     */
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const _CharType* _str) noexcept {
        return str_.compare(_pos, _len, _str);
    }
    
    /**
     * @brief Compares a substring of this string with a C-style string of a specified length.
     * 
     * @param _pos The start position in this string.
     * @param _len The length of the substring in this string.
     * @param _str The C-style string to compare with.
     * @param _str_len The length of the C-style string to use.
     * @return Int32 0 if equal, < 0 if substring < _str substring, > 0 if substring > _str substring.
     */
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const _CharType* _str, SizeType _str_len
    ) noexcept {
        return str_.compare(_pos, _len, _str, _str_len);
    }

    /**
     * @brief Checks if the string starts with the given prefix.
     * 
     * @param _str The prefix string.
     * @return Bool True if it starts with the prefix, false otherwise.
     */
    FORCEINLINE constexpr Bool StartWith(const TString& _str) noexcept { 
        return str_.starts_with(_str.str_); 
    }
    
    /**
     * @brief Checks if the string starts with the given prefix (C-style).
     * 
     * @param _str The prefix C-style string.
     * @return Bool True if it starts with the prefix, false otherwise.
     */
    FORCEINLINE constexpr Bool StartWith(const _CharType* _str) noexcept {
        return str_.starts_with(_str);
    }
    
    /**
     * @brief Checks if the string starts with the given character.
     * 
     * @param _char The character prefix.
     * @return Bool True if it starts with the character, false otherwise.
     */
    FORCEINLINE constexpr Bool StartWith(const _CharType _char) noexcept {
        return str_.starts_with(_char);
    }
    
    /**
     * @brief Checks if the string ends with the given suffix.
     * 
     * @param _str The suffix string.
     * @return Bool True if it ends with the suffix, false otherwise.
     */
    FORCEINLINE constexpr Bool EndWith(const TString& _str) noexcept {
        return str_.ends_with(_str.str_);
    }
    
    /**
     * @brief Checks if the string ends with the given suffix (C-style).
     * 
     * @param _str The suffix C-style string.
     * @return Bool True if it ends with the suffix, false otherwise.
     */
    FORCEINLINE constexpr Bool EndWith(const _CharType* _str) noexcept {
        return str_.ends_with(_str);
    }
    
    /**
     * @brief Checks if the string ends with the given character.
     * 
     * @param _char The character suffix.
     * @return Bool True if it ends with the character, false otherwise.
     */
    FORCEINLINE constexpr Bool EndWith(const _CharType _char) noexcept {
        return str_.ends_with(_char);
    }

    /**
     * @brief Erases the character at the specified position.
     * 
     * @param _pos Iterator to the character to erase.
     * @return Iterator_ Iterator to the character following the erased one.
     */
    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _pos) noexcept { return str_.erase(_pos); }
    
    /**
     * @brief Erases the characters in the range `[_first, _last)`.
     * 
     * @param _first Iterator to the start of the range.
     * @param _last Iterator to the end of the range.
     * @return Iterator_ Iterator to the character following the last erased character.
     */
    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return str_.erase(_first, _last); 
    }
    
    /**
     * @brief Clears the contents of the string, making it empty.
     */
    FORCEINLINE constexpr Void Clear() noexcept { str_.clear(); }

    /**
     * @brief Resizes the string to contain `_size` characters.
     * 
     * @param _size The new size.
     */
    FORCEINLINE constexpr Void Resize(const SizeType _size) noexcept { str_.resize(_size); }
    
    /**
     * @brief Resizes the string to contain `_size` characters, filling new elements with `_char`.
     * 
     * @param _size The new size.
     * @param _char The character to use for padding.
     */
    FORCEINLINE constexpr Void Resize(const SizeType _size, const _CharType& _char) noexcept { 
        str_.resize(_size, _char);
    }

    /**
     * @brief Requests that the string capacity be at least enough to contain `_capacity` characters.
     * 
     * @param _capacity The desired capacity.
     */
    FORCEINLINE constexpr Void Reserve(const SizeType _capacity) noexcept { str_.reserve(_capacity); }
    
    /**
     * @brief Requests the container to reduce its capacity to fit its size.
     */
    FORCEINLINE constexpr Void ShrinkToFit() noexcept { str_.shrink_to_fit(); }

    /**
     * @brief Swaps the contents of this string with another.
     * 
     * @param _str The other string.
     */
    FORCEINLINE constexpr Void Swap(TString& _str) noexcept { str_.swap(_str.str_); }

    /**
     * @brief Computes a hash value for the string.
     * 
     * @return SizeType The hash value.
     */
    NODISCARD FORCEINLINE SizeType Hash() const noexcept {
        return std::hash<STDString_>()(str_);
    }

protected:
    /** @brief Alias for the base object type. */
    using SuperType_ = ZObject<_IsGlobal>;

private:
    /** @brief The internal standard string object. */
    STDString_ str_;
};

}//internal
}//zengine

namespace zengine {

using ZString = internal::TString<Char, true>;
using ZWString = internal::TString<WChar, true>;
using ZStringLocal = internal::TString<Char, false>;
using ZWStringLocal = internal::TString<WChar, false>;

}//zengine

namespace std {
template<>
struct hash<zengine::ZString> {
    zengine::SizeType operator()(const zengine::ZString& _str) const noexcept {
        return _str.Hash();
    }
};
template<>
struct hash<zengine::ZWString> {
    zengine::SizeType operator()(const zengine::ZWString& _str) const noexcept {
        return _str.Hash();
    }
};
template<>
struct hash<zengine::ZStringLocal> {
    zengine::SizeType operator()(const zengine::ZStringLocal& _str) const noexcept {
        return _str.Hash();
    }
};
template<>
struct hash<zengine::ZWStringLocal> {
    zengine::SizeType operator()(const zengine::ZWStringLocal& _str) const noexcept {
        return _str.Hash();
    }
};
}//std