/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include <array>

#include "z_object.h"

namespace zengine {

namespace internal {

/*
    Array container.
*/
template<typename _CharType, SizeType kCapacity>
class TFixedString : public ZObject {
public:
    using STDArray_ = std::array<_CharType, kCapacity>;
    using Iterator_ = STDArray_::iterator;
    using ConstIterator_ = STDArray_::const_iterator;
    using ReverseIterator_ = STDArray_::reverse_iterator;
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_CharType>;

    FORCEINLINE constexpr TFixedString() noexcept : SuperType_(), str_() {}
    FORCEINLINE constexpr TFixedString(const TFixedString& _str) noexcept : SuperType_(_str), str_(_str.str_) {}
    FORCEINLINE constexpr TFixedString(TFixedString&& _str) noexcept 
        : SuperType_(std::forward<TFixedString>(_str)), str_(std::move(_str.str_)) {}
    template<typename... _ArgsType>
    FORCEINLINE constexpr TFixedString(const _CharType* _format, _ArgsType&&... _args) noexcept : SuperType_() {
        if constexpr (kSameType<_CharType, Char>) {
            sprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            swprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
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
     
    NODISCARD FORCEINLINE constexpr _CharType& operator[](SizeType _index) noexcept { return str_[_index]; }
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](SizeType _index) const noexcept { 
        return str_[_index]; 
    }

    NODISCARD FORCEINLINE constexpr _CharType& At(SizeType _index) noexcept { return str_.at(_index); }
    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const noexcept { return str_.at(_index); } 
    NODISCARD FORCEINLINE constexpr _CharType* DataPtr() noexcept { return str_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return str_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* String() const noexcept { return str_.data(); }

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

    template<typename... _ArgsType>
    FORCEINLINE constexpr Void SetString(const _CharType* _format, _ArgsType&&... _args) noexcept {
        if constexpr (kSameType<_CharType, Char>) {
            sprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            swprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
    }
    FORCEINLINE constexpr Void SetString(const _CharType* _format, ArgListType _args) noexcept {
        if constexpr (kSameType<_CharType, Char>) {
            vsprintf(DataPtr(), _format, _args);
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            vswprintf(DataPtr(), _format, _args);
        }
    }
    FORCEINLINE Void Fill(const _CharType& _val) noexcept { str_.fill(_val); }
    FORCEINLINE Void Swap(TFixedString& _array) noexcept { str_.swap(_array.str_); }

protected:
    using SuperType_ = ZObject;

private:
    STDArray_ str_;
};

}//internal

template<SizeType kCapacity>
using TFixedString = internal::TFixedString<Char, kCapacity>;
template<SizeType kCapacity>
using TFixedWString = internal::TFixedString<WChar, kCapacity>;

template <SizeType kCapacity>
union FixedStringUnion {
public:
    FORCEINLINE FixedStringUnion() noexcept : str_() {}
    FORCEINLINE FixedStringUnion(const FixedStringUnion& _str) noexcept {
        str_ = _str.str_;
    }
    FORCEINLINE ~FixedStringUnion() noexcept {}

    FORCEINLINE FixedStringUnion& operator=(const FixedStringUnion& _str) noexcept {
        str_ = _str.str_;
    }

    TFixedString<kCapacity / sizeof(Char)> str_;
    TFixedWString<kCapacity / sizeof(WChar)> w_str_;
};

}//zengine