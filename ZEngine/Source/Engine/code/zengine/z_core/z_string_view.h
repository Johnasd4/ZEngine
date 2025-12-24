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
#include "z_object.h"

namespace zengine {
namespace internal {

/**
 * @brief A lightweight, non-owning reference to a string or a substring.
 *
 * This class wraps std::basic_string_view to provide a consistent interface compatible
 * with the ZEngine object system. It is designed to be efficient for copying and passing by value.
 *
 * @tparam _CharType The character type of the string (e.g., char, wchar_t).
 */
template<typename _CharType>
class TStringView {
    static_assert(kIsChar<_CharType>, "TStringView: Requires a char type.");

public:
    /** @brief Alias for the standard string type. */
    template<typename _AllocatorType>
    using STDString_ = std::basic_string<_CharType, std::char_traits<_CharType>, _AllocatorType>;
    /** @brief Alias for the standard string view type. */
    using STDStringView_ = std::basic_string_view<_CharType, std::char_traits<_CharType>>;

    /** @brief Represents the end position of a string or an invalid index. */
    static inline constexpr SizeType kEnd = std::string::npos;

    /**
     * @brief Default constructor. Initializes an empty string view.
     */
    FORCEINLINE constexpr TStringView() noexcept
        : str_view_()
    {}

    /**
     * @brief Copy constructor.
     * @param _str_view The string view object to copy from.
     */
    FORCEINLINE constexpr TStringView(const TStringView& _str_view) noexcept
        : str_view_(_str_view.str_view_)
    {}

    /**
     * @brief Constructs a string view representing a substring of another view.
     * @param _str_view The source string view.
     * @param _pos The starting position of the substring.
     * @param _len The length of the substring. Defaults to kEnd (until the end of the string).
     */
    constexpr TStringView(const TStringView& _str_view, SizeType _pos, SizeType _len = kEnd) noexcept
        : str_view_(&_str_view[_pos], _len)
    {}

    /**
     * @brief Constructs a string view from a character pointer and explicit size.
     * @param _str Pointer to the character array.
     * @param _size The number of characters to include in the view.
     */
    FORCEINLINE constexpr TStringView(const _CharType* _str, SizeType _size) noexcept
        : str_view_(_str, _size)
    {}

    /**
     * @brief Constructs a string view from a null-terminated C-style string.
     * @tparam _PointerType The type of the pointer (must be compatible with _CharType*).
     * @param _str The null-terminated C-string.
     */
    template<typename _PointerType>
    requires kSameType<kDecayType<_PointerType>, const _CharType*> || kSameType<kDecayType<_PointerType>, _CharType*>
    FORCEINLINE constexpr TStringView(_PointerType&& _str) noexcept
        : str_view_(_str)
    {}

    /**
     * @brief Constructs a string view from a ZEngine TString object.
     * @param _str The source TString object.
     */
    template<Bool _IsGlobal>
    FORCEINLINE constexpr TStringView(const TString<_CharType, _IsGlobal>& _str) noexcept
        : str_view_(_str.STDString())
    {}

    /**
     * @brief Constructs a string view from a std::basic_string.
     * @tparam _AllocatorType The allocator type used by the std::basic_string.
     * @param _std_str The source std::basic_string object.
     */
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView(const STDString_<_AllocatorType>& _std_str) noexcept
        : str_view_(_std_str)
    {}

    /**
     * @brief Constructs a string view from a std::basic_string_view.
     * @param _std_str The source std::basic_string_view object.
     */
    FORCEINLINE constexpr TStringView(const STDStringView_& _std_str) noexcept
        : str_view_(_std_str)
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE constexpr ~TStringView() noexcept {}

    /**
     * @brief Assignment operator. Copies the view from another TStringView.
     * @param _str_view The source string view.
     * @return TStringView& Reference to this object.
     */
    FORCEINLINE constexpr TStringView& operator=(const TStringView& _str_view) noexcept {
        str_view_ = _str_view.str_view_;
        return *this;
    }

    /**
     * @brief Assignment operator. Assigns from a null-terminated C-style string.
     * @tparam _PointerType The type of the pointer.
     * @param _str The null-terminated C-string.
     * @return TStringView& Reference to this object.
     */
    template<typename _PointerType>
    requires kSameType<kDecayType<_PointerType>, const _CharType*> || kSameType<kDecayType<_PointerType>, _CharType*>
    FORCEINLINE constexpr TStringView& operator=(_PointerType&& _str) noexcept {
        str_view_ = _str;
        return *this;
    }

    /**
     * @brief Assignment operator. Assigns from a ZEngine TString object.
     * @param _str The source TString object.
     * @return TStringView& Reference to this object.
     */
    template<Bool _IsGlobal>
    FORCEINLINE constexpr TStringView& operator=(const TString<_CharType, _IsGlobal>& _str) noexcept {
        str_view_ = _str.STDString();
        return *this;
    }

    /**
     * @brief Assignment operator. Assigns from a std::basic_string.
     * @tparam _AllocatorType The allocator type of the std::basic_string.
     * @param _std_str The source std::basic_string.
     * @return TStringView& Reference to this object.
     */
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView& operator=(const STDString_<_AllocatorType>& _std_str) noexcept {
        str_view_ = _std_str;
        return *this;
    }

    /**
     * @brief Replaces the current view with a copy of another TStringView.
     * @param _str_view The source string view.
     * @return TStringView& Reference to this object.
     */
    FORCEINLINE constexpr TStringView& Assign(const TStringView& _str_view) noexcept {
        str_view_ = _str_view.str_view_;
        return *this;
    }

    /**
     * @brief Replaces the current view with a substring of another TStringView.
     * @param _str_view The source string view.
     * @param _pos The starting position in the source view.
     * @param _len The length of the substring. Defaults to kEnd.
     * @return TStringView& Reference to this object.
     */
    FORCEINLINE constexpr TStringView& Assign(
        const TStringView& _str_view,
        SizeType _pos,
        SizeType _len = kEnd
    ) noexcept {
        str_view_ = STDStringView_{&_str_view[_pos], _len};
        return *this;
    }

    /**
     * @brief Replaces the current view with a character array and explicit size.
     * @param _str Pointer to the character array.
     * @param _size The number of characters.
     * @return TStringView& Reference to this object.
     */
    FORCEINLINE constexpr TStringView& Assign(const _CharType* _str, SizeType _size) noexcept {
        str_view_ = STDStringView_{_str, _size};
        return *this;
    }

    /**
     * @brief Replaces the current view with a null-terminated C-style string.
     * @tparam _PointerType The type of the pointer.
     * @param _str The null-terminated C-string.
     * @return TStringView& Reference to this object.
     */
    template<typename _PointerType>
    requires kSameType<kDecayType<_PointerType>, const _CharType*> || kSameType<kDecayType<_PointerType>, _CharType*>
    FORCEINLINE constexpr TStringView& Assign(_PointerType&& _str) noexcept {
        str_view_ = _str;
        return *this;
    }

    /**
     * @brief Replaces the current view with the content of a ZEngine TString.
     * @param _str The source TString object.
     * @return TStringView& Reference to this object.
     */
    template<Bool _IsGlobal>
    FORCEINLINE constexpr TStringView& Assign(const TString<_CharType, _IsGlobal>& _str) noexcept {
        str_view_ = _str.STDString();
        return *this;
    }

    /**
     * @brief Replaces the current view with the content of a std::basic_string.
     * @tparam _AllocatorType The allocator type of the std::basic_string.
     * @param _std_str The source std::basic_string.
     * @return TStringView& Reference to this object.
     */
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView& Assign(const STDString_<_AllocatorType>& _std_str) noexcept {
        str_view_ = _std_str;
        return *this;
    }

    /**
     * @brief Implicit conversion to the underlying std::basic_string_view.
     * Allows TStringView to be passed directly to APIs expecting std::string_view.
     * @return STDStringView_ A copy of the internal string view.
     */
    NODISCARD FORCEINLINE operator STDStringView_() const noexcept {
        return str_view_;
    }

    /**
     * @brief Implicit conversion to a ZEngine TString (global memory).
     * Creates a new TString instance containing a copy of the view's data.
     * @return TString<_CharType, true> A new TString object.
     */
    NODISCARD FORCEINLINE operator TString<_CharType, true>() const noexcept {
        return TString<_CharType, true>(str_view_.data(), str_view_.size());
    }

    /**
     * @brief Implicit conversion to a ZEngine TString (thread-local memory).
     * Creates a new TString instance containing a copy of the view's data.
     * @return TString<_CharType, false> A new TString object.
     */
    NODISCARD FORCEINLINE operator TString<_CharType, false>() const noexcept {
        return TString<_CharType, false>(str_view_.data(), str_view_.size());
    }

    /**
     * @brief Accesses the character at the specified index.
     * This function does not perform bounds checking.
     * @param _index The index of the character to access.
     * @return const _CharType& A const reference to the character.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept {
        return str_view_[_index];
    }

    /**
     * @brief Accesses the character at the specified index with bounds checking.
     * @param _index The index of the character to access.
     * @return const _CharType& A const reference to the character.
     * @throw std::out_of_range If _index is invalid.
     */
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const {
        return str_view_.at(_index);
    }

    /**
     * @brief Returns a pointer to the underlying character array.
     * @return const _CharType* A const pointer to the data.
     */
    NODISCARD FORCEINLINE constexpr const _CharType* GetDataPtr() const noexcept {
        return str_view_.data();
    }

    /**
     * @brief Accesses the underlying std::basic_string_view object.
     * @return STDStringView_& A reference to the internal std::basic_string_view.
     */
    NODISCARD FORCEINLINE constexpr STDStringView_& STDStringView() noexcept { return str_view_; }

    /**
     * @brief Accesses the underlying std::basic_string_view object (const).
     * @return const STDStringView_& A const reference to the internal std::basic_string_view.
     */
    NODISCARD FORCEINLINE constexpr const STDStringView_& STDStringView() const noexcept { return str_view_; }

    /**
     * @brief Returns the number of characters in the view.
     * @return SizeType The size of the view.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { return str_view_.size(); }

    /**
     * @brief Checks if the view is empty.
     * @return Bool True if the size is 0, false otherwise.
     */
    NODISCARD FORCEINLINE constexpr Bool IsEmpty() const noexcept { return str_view_.empty(); }

    /**
     * @brief Finds the first occurrence of a substring within the view.
     * @param _str_view The substring to search for.
     * @param _start_pos The position to start the search from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    NODISCARD FORCEINLINE constexpr SizeType Find(
        const TStringView& _str_view,
        const SizeType _start_pos = 0ULL
    ) noexcept {
        return str_view_.find(_str_view.str_view_, _start_pos);
    }

    /**
     * @brief Finds the first occurrence of a character within the view.
     * @param _char The character to search for.
     * @param _start_pos The position to start the search from.
     * @return SizeType The index of the first occurrence, or kEnd if not found.
     */
    NODISCARD FORCEINLINE constexpr SizeType Find(
        const _CharType _char,
        const SizeType _start_pos = 0ULL
    ) noexcept {
        return str_view_.find(_char, _start_pos);
    }

    /**
     * @brief Finds the last occurrence of a substring within the view (searching backwards).
     * @param _str_view The substring to search for.
     * @param _start_pos The position to start the backward search from. Defaults to kEnd.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    NODISCARD FORCEINLINE constexpr SizeType ReverseFind(
        const TStringView& _str_view,
        const SizeType _start_pos = kEnd
    ) noexcept {
        return str_view_.rfind(_str_view.str_view_, _start_pos);
    }

    /**
     * @brief Finds the last occurrence of a character within the view (searching backwards).
     * @param _char The character to search for.
     * @param _start_pos The position to start the backward search from. Defaults to kEnd.
     * @return SizeType The index of the last occurrence, or kEnd if not found.
     */
    NODISCARD FORCEINLINE constexpr SizeType ReverseFind(
        const _CharType _char,
        const SizeType _start_pos = kEnd
    ) noexcept {
        return str_view_.rfind(_char, _start_pos);
    }

    /**
     * @brief Creates a new string view representing a substring of the current view.
     * @param _pos The starting position of the substring.
     * @param _len The length of the substring. Defaults to kEnd.
     * @return TStringView A new TStringView instance.
     */
    NODISCARD FORCEINLINE constexpr TStringView SubString(const SizeType _pos, const SizeType _len = kEnd) noexcept {
        return TStringView{ str_view_.substr(_pos, _len) };
    }

    /**
     * @brief Swaps the contents of this view with another.
     * @param _str_view The other string view to swap with.
     */
    FORCEINLINE constexpr Void Swap(TStringView& _str_view) noexcept {
        str_view_.swap(_str_view.str_view_);
    }

    /**
     * @brief Computes a hash value for the string view.
     * @return SizeType The hash value.
     */
    NODISCARD FORCEINLINE SizeType Hash() const noexcept {
        return std::hash<STDStringView_>{}(str_view_);
    }

    /**
     * @brief Converts the view to a ZEngine TString (owning copy).
     * @return TString<_CharType> A new TString containing the data.
     */
    NODISCARD FORCEINLINE TString<_CharType, true> ToString() const noexcept {
        return TString<_CharType, true>{ GetDataPtr(), GetSize() };
    }

    /**
     * @brief Converts the view to a ZEngine TString (owning copy).
     * @return TString<_CharType> A new TString containing the data.
     */
    NODISCARD FORCEINLINE TString<_CharType, false> ToStringLocal() const noexcept {
        return TString<_CharType, false>{ GetDataPtr(), GetSize() };
    }

private:
    static Void* operator new(SizeType) = delete;
    static Void operator delete(Void*) = delete;

    /** @brief The internal standard string view object. */
    STDStringView_ str_view_;
};

}//internal
}//zengine

namespace zengine {

/** @brief Alias for a string view of char type. */
using ZStringView = internal::TStringView<Char>;
/** @brief Alias for a string view of wchar_t type. */
using ZWStringView = internal::TStringView<WChar>;

}//zengine

namespace std {

template<>
struct hash<zengine::ZStringView> {
    size_t operator()(const zengine::ZStringView& _str_view) const noexcept {
        return _str_view.Hash();
    }
};
template<>
struct hash<zengine::ZWStringView> {
    size_t operator()(const zengine::ZWStringView& _str_view) const noexcept {
        return _str_view.Hash();
    }
};

}//std