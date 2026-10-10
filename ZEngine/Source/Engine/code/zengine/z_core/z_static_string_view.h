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

#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace internal {

/**
 * @brief A template class representing a static string view.
 *
 * This class acts as a lightweight wrapper around a raw string pointer and size, typically used for string literals
 * or constant compile-time strings to avoid memory allocation. It provides read-only access to the underlying
 * character sequence.
 *
 * @tparam _CharType The character type of the string (e.g., char, wchar_t).
 */
template<typename _CharType>
class TStaticStringView {
    STATIC_ASSERT(kIsChar<_CharType>, "TStaticStringView: Requires a char type.");
    NO_NEW_DELETE
public:
    /** @brief Alias for the standard string view type. */
    using STDStringView_ = std::basic_string_view<_CharType, std::char_traits<_CharType>>;

    /** @brief Represents the end position of a string or an invalid index. */
    static inline constexpr SizeType kEnd = std::string::npos;

    /**
     * @brief Constructs an empty static string view.
     */
    FORCEINLINE constexpr TStaticStringView() noexcept
        : str_("")
        , size_(0ULL)
    {}

    /**
     * @brief Copy constructor.
     * @param _str_view The source static string view to copy.
     */
    FORCEINLINE constexpr TStaticStringView(const TStaticStringView& _str_view) noexcept
        : str_(_str_view.str_)
        , size_(_str_view.size_)
    {}

    /**
     * @brief Constructs a static string view from a string literal at compile time.
     * @tparam _StringLength The length of the string literal (deduced automatically).
     * @param _str The string literal array.
     */
    template <SizeType _StringLength>
    explicit FORCEINLINE consteval TStaticStringView(const _CharType(&_str)[_StringLength]) noexcept
        : str_(_str)
        , size_(_StringLength - 1ULL)
    {}

    /**
     * @brief Constructs a static string view from a substring of another view.
     * @param _str_view The source static string view.
     * @param _pos The starting position of the substring.
     * @param _size The length of the substring. Defaults to kEnd.
     */
    FORCEINLINE constexpr TStaticStringView(
        const TStaticStringView& _str_view, 
        SizeType _pos, 
        SizeType _size = kEnd
    ) noexcept
        : TStaticStringView(_str_view.SubString(_pos, _size))
    {}

    /**
     * @brief Destroys the static string view.
     */
    FORCEINLINE constexpr ~TStaticStringView() noexcept = default;

    /**
     * @brief Assigns a string literal to the static string view.
     * @tparam _StringLength The length of the string literal (deduced automatically).
     * @param _str The string literal array.
     * @return A reference to this static string view.
     */
    template <SizeType _StringLength>
    FORCEINLINE consteval TStaticStringView& operator=(const _CharType(&_str)[_StringLength]) noexcept {
        str_ = _str;
        size_ = _StringLength - 1ULL;
        return *this;
    }

    /**
     * @brief Assigns the value of another static string view to this one.
     * @param _str_view The source static string view.
     * @return A reference to this static string view.
     */
    FORCEINLINE constexpr TStaticStringView& operator=(const TStaticStringView& _str_view) noexcept {
        str_ = _str_view.str_;
        size_ = _str_view.size_;
        return *this;
    }

    /**
     * @brief Checks if the string view is empty.
     * @return True if size is 0, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool IsEmpty() const noexcept { return size_ == 0ULL; }

    /**
     * @brief Returns the raw pointer to the character sequence.
     * @return A pointer to the underlying character array.
     */
    NODISCARD FORCEINLINE constexpr const _CharType* GetDataPtr() const noexcept {
        return str_;
    }

    /**
     * @brief Returns the size of the string view.
     * @return The number of characters in the view.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { return size_; }

    /**
     * @brief Converts the static string view to a TStringView.
     * @return A TStringView representing the same character sequence.
     */
    NODISCARD FORCEINLINE operator TStringView<_CharType>() const noexcept {
        return TStringView<_CharType>(str_, size_);
    }

    /**
     * @brief Converts the static string view to a TString.
     * @return A TString containing a copy of the character sequence.
     */
    NODISCARD FORCEINLINE operator TString<_CharType>() const noexcept {
        return TString<_CharType>(str_, size_);
    }

    /**
     * @brief Accesses the character at the specified index.
     * @param _index The index of the character to retrieve.
     * @return A const reference to the character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept {
        return str_[_index];
    }

    /**
     * @brief Accesses the character at the specified index.
     * @param _index The index of the character to retrieve.
     * @return A const reference to the character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const {
        return str_[_index];
    }

    /**
     * @brief Creates a substring view from the current view.
     * @param _pos The starting position of the substring.
     * @param _size The length of the substring. Defaults to kEnd (until the end).
     * @return A new TStaticStringView representing the substring.
     */
    NODISCARD FORCEINLINE constexpr TStaticStringView SubString(
        const SizeType _pos, 
        const SizeType _size = kEnd
    ) const noexcept {
        if (_pos >= size_) {
            return TStaticStringView{};
        }

        SizeType real_size = _size;
        if (real_size == kEnd || _pos + real_size > size_) {
            real_size = size_ - _pos;
        }

        return TStaticStringView(str_ + _pos, real_size);
    }

    /**
     * @brief Swaps the contents of this view with another view.
     * @param _str_view The other static string view to swap with.
     */
    FORCEINLINE constexpr Void Swap(TStaticStringView& _str_view) noexcept {
        ::zengine::Swap(this, &_str_view);
    }

    /**
     * @brief Calculates the hash value of the string view.
     * @return The hash value.
     */
    NODISCARD FORCEINLINE SizeType Hash() const noexcept {
        return std::hash<STDStringView_>{}(STDStringView_(str_, size_));
    }

    /**
     * @brief Converts the view to a string object.
     * @return A TString containing the characters.
     */
    NODISCARD FORCEINLINE TString<_CharType> ToString() const noexcept {
        return TString<_CharType>{ GetDataPtr(), GetSize() };
    }

    /**
     * @brief Converts the view to a string object (alias for ToString).
     * @return A TString containing the characters.
     */
    NODISCARD FORCEINLINE TString<_CharType> ToStringView() const noexcept {
        return TStringView<_CharType>{ GetDataPtr(), GetSize() };
    }

private:
    /**
     * @brief Private constructor to create a view from a pointer and size.
     * @param _ptr The pointer to the character sequence.
     * @param _size The number of characters.
     */
    FORCEINLINE constexpr TStaticStringView(const _CharType* _ptr, SizeType _size) noexcept
        : str_(_ptr), size_(_size) {
    }

private:
    /** @brief Pointer to the character sequence. */
    const _CharType* str_;
    /** @brief The length of the character sequence. */
    SizeType size_;
};

} // internal
} // zengine

namespace zengine {

/** @brief Alias for a string view of char type. */
using ZStaticStringView = internal::TStaticStringView<Char>;
/** @brief Alias for a string view of wchar_t type. */
using ZWStaticStringView = internal::TStaticStringView<WChar>;

} // zengine

namespace std {

template<>
struct hash<zengine::ZStaticStringView> {
    size_t operator()(const zengine::ZStaticStringView& _str_view) const noexcept {
        return _str_view.Hash();
    }
};
template<>
struct hash<zengine::ZWStaticStringView> {
    size_t operator()(const zengine::ZWStaticStringView& _str_view) const noexcept {
        return _str_view.Hash();
    }
};

} // std