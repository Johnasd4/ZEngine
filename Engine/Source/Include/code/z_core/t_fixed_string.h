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

/*
    Array caintainer.
*/
template<typename CharType, IndexType kCapacity>
requires kIsChar<CharType>
class TFixedString : public ZObject {
public:
    using STDArray = std::array<CharType, kCapacity>;
    using Iterator = STDArray::iterator;
    using ConstIterator = STDArray::const_iterator;
    using ReverseIterator = STDArray::reverse_iterator;
    using ConstReverseIterator = STDArray::const_reverse_iterator;
    using InitializerList = std::initializer_list<CharType>;

    FORCEINLINE constexpr TFixedString() noexcept : SuperType(), string_() {}
    FORCEINLINE constexpr TFixedString(const TFixedString& array) noexcept : SuperType(), string_(array.string_) {}
    FORCEINLINE constexpr TFixedString(TFixedString&& array) noexcept : SuperType(), string_(std::move(array.string_)) {}
    FORCEINLINE TFixedString(InitializerList init_list) noexcept : SuperType(), string_(init_list) {}
    template<typename... ArgsType>
    FORCEINLINE constexpr TFixedString(CharType* format, ArgsType&&... args) noexcept : SuperType() {
        if constexpr (kSameType<CharType, CChar>) {
            sprintf(DataPtr(), format, std::forward<ArgsType>(args)...);
        }
        else if constexpr (kSameType<CharType, TChar>) {
            swprintf(DataPtr(), format, std::forward<ArgsType>(args)...);
        }
    }

    FORCEINLINE constexpr ~TFixedString() noexcept {}

    NODISCARD FORCEINLINE Bool operator=(const TFixedString& str) noexcept {
        return memcpy(DataPtr(), str.DataPtr(), sizeof(CharType) * kCapacity);
    }

    NODISCARD FORCEINLINE Bool operator==(const TFixedString& str) noexcept { 
        return strcmp(DataPtr(), str.DataPtr()) == 0;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TFixedString& str) noexcept {
        return strcmp(DataPtr(), str.DataPtr()) != 0;
    }
     
    NODISCARD FORCEINLINE constexpr CharType& operator[](IndexType index) noexcept { return string_[index]; }
    NODISCARD FORCEINLINE constexpr const CharType& operator[](IndexType index) const noexcept { return string_[index]; }

    NODISCARD FORCEINLINE constexpr CharType& At(IndexType index) noexcept { return string_.at(index); }
    NODISCARD FORCEINLINE constexpr const CharType& At(IndexType index) const noexcept { return string_.at(index); } 
    NODISCARD FORCEINLINE constexpr CharType* DataPtr() noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const CharType* DataPtr() const noexcept { return string_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return string_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return string_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return string_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return string_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return string_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return string_.crend(); }

    template<typename... ArgsType>
    FORCEINLINE constexpr Void SetString(const CharType* format, ArgsType&&... args) noexcept {
        if constexpr (kSameType<CharType, CChar>) {
            sprintf(DataPtr(), format, std::forward<ArgsType>(args)...);
        }
        else if constexpr (kSameType<CharType, TChar>) {
            swprintf(DataPtr(), format, std::forward<ArgsType>(args)...);
        }
    }
    FORCEINLINE constexpr Void SetString(const CharType* format, ArgListType args) noexcept {
        if constexpr (kSameType<CharType, CChar>) {
            vsprintf(DataPtr(), format, args);
        }
        else if constexpr (kSameType<CharType, TChar>) {
            vswprintf(DataPtr(), format, args);
        }
    }
    FORCEINLINE Void Fill(const CharType& value) noexcept { string_.fill(value); }
    FORCEINLINE Void Swap(TFixedString& array) noexcept { string_.swap(array); }

protected:
    using SuperType = ZObject;

private:
    STDArray string_;
};

template <IndexType kCapacity>
union FixedStringUnion {
public:
    FORCEINLINE FixedStringUnion() noexcept : c_str() {}
    FORCEINLINE FixedStringUnion(const FixedStringUnion& string) noexcept { 
        c_str = string.c_str;
    }
    FORCEINLINE ~FixedStringUnion() noexcept {}

    FORCEINLINE FixedStringUnion& operator=(const FixedStringUnion& string) noexcept {
        c_str = string.c_str;
    }

    TFixedString<CChar, kCapacity / sizeof(CChar)> c_str;
    TFixedString<TChar, kCapacity / sizeof(TChar)> t_str;
};

}//zengine

#endif // !Z_CORE_T_FIXED_STRING_H_