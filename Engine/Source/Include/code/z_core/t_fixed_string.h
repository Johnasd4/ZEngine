/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_T_FIXED_STRING_H_
#define Z_CORE_T_FIXED_STRING_H_

#include "internal/z_drive.h"

#include <array>

#include "z_object.h"

namespace zengine {

namespace internal {

/*
    Array caintainer.
*/
template<typename _CharType, IndexType kCapacity>
requires kIsChar<_CharType>
class TFixedString : public ZObject {
public:
    using STDArray_ = std::array<_CharType, kCapacity>;
    using Iterator_ = STDArray_::iterator;
    using ConstIterator_ = STDArray_::const_iterator;
    using ReverseIterator_ = STDArray_::reverse_iterator;
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_CharType>;

    FORCEINLINE constexpr TFixedString() noexcept : SuperType_(), string_() {}
    FORCEINLINE constexpr TFixedString(const TFixedString& _array) noexcept : SuperType_(), string_(_array.string_) {}
    FORCEINLINE constexpr TFixedString(TFixedString&& _array) noexcept 
        : SuperType_(), string_(std::move(_array.string_)) {}
    FORCEINLINE TFixedString(InitializerList_ _init_list) noexcept : SuperType_(), string_(_init_list) {}
    template<typename... _ArgsType>
    FORCEINLINE constexpr TFixedString(_CharType* _format, _ArgsType&&... _args) noexcept : SuperType_() {
        if constexpr (kSameType<_CharType, Char>) {
            sprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            swprintf(DataPtr(), _format, std::forward<_ArgsType>(_args)...);
        }
    }

    FORCEINLINE constexpr ~TFixedString() noexcept {}

    NODISCARD FORCEINLINE Bool operator=(const TFixedString& _str) noexcept {
        return memcpy(DataPtr(), _str.DataPtr(), sizeof(_CharType) * kCapacity);
    }

    NODISCARD FORCEINLINE Bool operator==(const TFixedString& _str) noexcept { 
        return strcmp(DataPtr(), _str.DataPtr()) == 0;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TFixedString& _str) noexcept {
        return strcmp(DataPtr(), _str.DataPtr()) != 0;
    }
     
    NODISCARD FORCEINLINE constexpr _CharType& operator[](IndexType _index) noexcept { return string_[_index]; }
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](IndexType _index) const noexcept { 
        return string_[_index]; 
    }

    NODISCARD FORCEINLINE constexpr _CharType& At(IndexType _index) noexcept { return string_.at(_index); }
    NODISCARD FORCEINLINE constexpr const _CharType& At(IndexType _index) const noexcept { return string_.at(_index); } 
    NODISCARD FORCEINLINE constexpr _CharType* DataPtr() noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return string_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return string_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return string_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return string_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return string_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return string_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return string_.crend(); }

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
    FORCEINLINE Void Fill(const _CharType& _val) noexcept { string_.fill(_val); }
    FORCEINLINE Void Swap(TFixedString& _array) noexcept { string_.swap(_array); }

protected:
    using SuperType_ = ZObject;

private:
    STDArray_ string_;
};

}//internal

template<IndexType kCapacity>
using TFixedString = internal::TFixedString<Char, kCapacity>;
template<IndexType kCapacity>
using TWFixedString = internal::TFixedString<WChar, kCapacity>;

template <IndexType kCapacity>
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
    TWFixedString<kCapacity / sizeof(WChar)> w_str_;
};

}//zengine

#endif // !Z_CORE_T_FIXED_STRING_H_