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

/*
    Fixed string container.
*/
template<typename _CharType, SizeType kCapacity>
class TFixedString : public ZObject {
public:
    using STDFixedArray_ = std::array<_CharType, kCapacity>;
    using Iterator_ = STDFixedArray_::iterator;
    using ConstIterator_ = STDFixedArray_::const_iterator;
    using ReverseIterator_ = STDFixedArray_::reverse_iterator;
    using ConstReverseIterator_ = STDFixedArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_CharType>;

    FORCEINLINE constexpr TFixedString() noexcept 
        : SuperType_()
        , str_() 
    {}
    FORCEINLINE constexpr TFixedString(const TFixedString& _str) noexcept 
        : SuperType_(_str)
        , str_(_str.str_) 
    {}
    FORCEINLINE constexpr TFixedString(TFixedString&& _str) noexcept 
        : SuperType_(std::forward<TFixedString>(_str))
        , str_(std::move(_str.str_)) 
    {}
    template<typename... _ArgsType>
    FORCEINLINE constexpr TFixedString(ZStringView _format, _ArgsType&&... _args) noexcept 
        : SuperType_() 
    {
        static_assert(kSameType<_CharType, Char>, "TFixedString: _CharType must be Char!");
        string::GenerateString(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    FORCEINLINE constexpr ~TFixedString() noexcept {}

    FORCEINLINE TFixedString& operator=(const TFixedString& _str) noexcept {
        SuperType_::operator=(_str);
        str_ = _str.str_;
        return *this;
    }
    FORCEINLINE TFixedString& operator=(TFixedString&& _str) noexcept {
        SuperType_::operator=(std::forward<TFixedString>(_str));
        str_ = std::move(_str.str_);
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TFixedString& _str) noexcept { 
        if constexpr (kSameType<_CharType, Char>) {
            return strcmp(DataPtr(), _str.DataPtr()) == 0;
        }
        else {
            return wcscmp(DataPtr(), _str.DataPtr()) == 0;
        }
    }
    NODISCARD FORCEINLINE Bool operator!=(const TFixedString& _str) noexcept {
        if constexpr (kSameType<_CharType, Char>) {
            return strcmp(DataPtr(), _str.DataPtr()) == 0;
        }
        else {
            return wcscmp(DataPtr(), _str.DataPtr()) == 0;
        }
    }
     
    template<typename... _ArgsType>
    FORCEINLINE constexpr SizeType Assign(ZStringView _format, _ArgsType&&... _args) noexcept {
        static_assert(kSameType<_CharType, Char>, "TFixedString: _CharType must be Char!");
        return string::GenerateString(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    template<typename... _ArgsType>
    FORCEINLINE constexpr SizeType AssignNoEnd(ZStringView _format, _ArgsType&&... _args) noexcept {
        static_assert(kSameType<_CharType, Char>, "TFixedString: _CharType must be Char!");
        return string::GenerateStringNoEnd(str_.data(), kCapacity, _format, std::forward<_ArgsType>(_args)...);
    }

    NODISCARD FORCEINLINE constexpr _CharType& operator[](SizeType _index) noexcept { return str_[_index]; }
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](SizeType _index) const noexcept { 
        return str_[_index]; 
    }

    NODISCARD FORCEINLINE constexpr _CharType& At(SizeType _index) noexcept { return str_.at(_index); }
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const noexcept { return str_.at(_index); } 
    NODISCARD FORCEINLINE constexpr _CharType* DataPtr() noexcept { return str_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return str_.data(); }

    NODISCARD FORCEINLINE static constexpr SizeType Capacity() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return str_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return str_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return str_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return str_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return str_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return str_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return str_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return str_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return str_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return str_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return str_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return str_.crend(); }

    FORCEINLINE Void Fill(const _CharType& _val) noexcept { str_.fill(_val); }
    FORCEINLINE Void Swap(TFixedString& _array) noexcept { str_.swap(_array.str_); }

protected:
    using SuperType_ = ZObject;

private:
    STDFixedArray_ str_;
};

}//internal

template<SizeType kCapacity>
using TFixedString = internal::TFixedString<Char, kCapacity>;
template<SizeType kCapacity>
using TFixedWString = internal::TFixedString<WChar, kCapacity>;

}//zengine