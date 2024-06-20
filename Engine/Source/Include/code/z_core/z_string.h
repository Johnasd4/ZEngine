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
#ifndef Z_CORE_Z_STRING_H_
#define Z_CORE_Z_STRING_H_

#include "internal/z_drive.h"

#include <string>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

namespace internal {

/*
    String caintainer.
*/
template<typename CharType>
class TString : public ZObject {
public:
    using STDString = std::basic_string<CharType, std::char_traits<CharType>, TAllocator<CharType>>;;
    using Iterator = STDString::iterator;
    using ConstIterator = STDString::const_iterator;
    using ReverseIterator = STDString::reverse_iterator;
    using ConstReverseIterator = STDString::const_reverse_iterator;
    using InitializerList = std::initializer_list<Char>;

    FORCEINLINE constexpr TString() noexcept : SuperType(), string_() {}
    FORCEINLINE constexpr TString(const TString& string) noexcept : SuperType(), string_(string.string_) {}
    FORCEINLINE constexpr TString(const TString& string, SizeType pos, SizeType len = -1) noexcept
            : SuperType(), string_(string.string_, pos, len) {}
    FORCEINLINE constexpr TString(TString&& string) noexcept : SuperType(), string_(std::move(string.string_)) {}

    FORCEINLINE constexpr TString(const CharType* string) noexcept : SuperType(), string_(string) {}
    FORCEINLINE constexpr TString(const CharType* string, SizeType size) noexcept : SuperType(), string_(string, size) {}
    FORCEINLINE constexpr TString(SizeType size, CharType& value) noexcept : SuperType(), string_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE constexpr TString(InputIterator first, InputIterator last) noexcept : SuperType(), string_(first, last) {}
    FORCEINLINE constexpr TString(InitializerList init_list) noexcept : SuperType(), string_(init_list) {}
 
    FORCEINLINE constexpr ~TString() noexcept {}

    FORCEINLINE constexpr TString& operator=(const TString& string) noexcept {
        string_.operator=(string.string_);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(TString&& string) noexcept {
        string_.operator=(std::move(string.string_));
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(const CharType* string) noexcept {
        string_.operator=(string);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(CharType c) noexcept {
        string_.operator=(c);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(InitializerList init_list) noexcept {
        string_.operator=(init_list);
        return *this;
    }

    FORCEINLINE constexpr TString& Assign(const TString& string) noexcept {
        return string_.assign(string);
    }
    FORCEINLINE constexpr TString& Assign(const TString& string, SizeType pos, SizeType len = -1) noexcept {
        return string_.assign(string, pos, len);
    }
    FORCEINLINE constexpr TString& Assign(TString&& string) noexcept {
        return string_.assign(string);
    }
    FORCEINLINE constexpr TString& Assign(const CharType* string) noexcept {
        return string_.assign(string);
    }
    FORCEINLINE constexpr TString& Assign(const CharType* string, SizeType size) noexcept {
        return string_.assign(string, size);
    }
    FORCEINLINE constexpr TString& Assign(const SizeType size, const CharType& value) noexcept {
        return string_.assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE constexpr TString& Assign(InputIterator first, InputIterator last) noexcept {
        return string_.assign(first, last);
    }
    FORCEINLINE constexpr TString& Assign(InitializerList init_list) noexcept {
        return string_.assign(init_list);
    }

    NODISCARD FORCEINLINE constexpr Bool operator==(const TString& string) noexcept { return string_ == string; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TString& string) noexcept { return string_ != string; }

    NODISCARD FORCEINLINE constexpr CharType& operator[](const SizeType index) noexcept { return string_[index]; }
    NODISCARD FORCEINLINE constexpr const CharType& operator[](const SizeType index) const noexcept { 
        return string_[index]; 
    }

    FORCEINLINE constexpr TString& operator+=(const TString& string) noexcept { return string_ += string.string_; }
    FORCEINLINE constexpr TString& operator+=(const CharType* string) noexcept { return string_ += string; }
    FORCEINLINE constexpr TString& operator+=(const CharType c) noexcept { return string_ += c; }
    FORCEINLINE constexpr TString& operator+=(InitializerList init_list) noexcept { return string_ += init_list; }

    NODISCARD FORCEINLINE constexpr CharType& At(IndexType index) noexcept { return string_.at(index); }
    NODISCARD FORCEINLINE constexpr const CharType& At(IndexType index) const noexcept { return string_.at(index); }

    NODISCARD FORCEINLINE constexpr CharType& Front() noexcept { return string_.front(); }
    NODISCARD FORCEINLINE constexpr const CharType& Front() const noexcept { return string_.front(); }
    NODISCARD FORCEINLINE constexpr CharType& Back() noexcept { return string_.back(); }
    NODISCARD FORCEINLINE constexpr const CharType& Back() const noexcept { return string_.back(); }
    NODISCARD FORCEINLINE constexpr CharType* DataPtr() noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const CharType* DataPtr() const noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const CharType* String() const noexcept { return string_.data(); }

    NODISCARD FORCEINLINE constexpr IndexType Size() const noexcept { return static_cast<IndexType>(string_.size()); }
    NODISCARD FORCEINLINE constexpr IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE constexpr Bool Empty() const noexcept { return string_.empty(); }

    NODISCARD FORCEINLINE constexpr Iterator Begin() noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator Begin() const noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator ConstBegin() const noexcept { return string_.cbegin(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator ReverseBegin() noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator ReverseBegin() const noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator ConstReverseBegin() const noexcept { return string_.crbegin(); }
    NODISCARD FORCEINLINE constexpr Iterator End() noexcept { return string_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator End() const noexcept { return string_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator ConstEnd() const noexcept { return string_.cend(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator ReverseEnd() noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator ReverseEnd() const noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator ConstReverseEnd() const noexcept { return string_.crend(); }

    FORCEINLINE constexpr TString& Append(const TString& string) noexcept { return string_.append(string.string_); }
    FORCEINLINE constexpr TString& Append(const TString& string, SizeType pos, SizeType len = -1) noexcept {
        return string_.append(string.string_, pos, len);
    }
    FORCEINLINE constexpr TString& Append(const CharType* string) noexcept { return string_.append(string); }
    FORCEINLINE constexpr TString& Append(const CharType* string, SizeType size) noexcept { 
        return string_.append(string, size); 
    }
    FORCEINLINE constexpr TString& Append(SizeType size, const CharType c) noexcept { return string_.append(size, c); }
    FORCEINLINE constexpr TString& Append(InitializerList init_list) noexcept { return string_.append(init_list); }

    FORCEINLINE constexpr TString& Insert(const SizeType pos, const TString& string) noexcept {
        return string_.insert(pos, string.string_); 
    }
    FORCEINLINE constexpr TString& Insert(const SizeType pos, const TString& string, SizeType str_pos, SizeType len = -1) noexcept {
        return string_.insert(pos, string.string_, str_pos, len);
    }
    FORCEINLINE constexpr TString& Insert(const SizeType pos, const CharType* string) noexcept {
        return string_.insert(pos, string);
    }
    FORCEINLINE constexpr TString& Insert(const SizeType pos, const CharType* string, SizeType size) noexcept {
        return string_.insert(pos, string, size);
    }
    FORCEINLINE constexpr TString& Insert(const SizeType pos, SizeType size, const CharType c) noexcept {
        return string_.insert(pos, size, c); 
    }
    FORCEINLINE constexpr TString& Insert(const SizeType pos, InitializerList init_list) noexcept {
        return string_.insert(pos, init_list); 
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator pos, SizeType size, const CharType c) noexcept {
        return string_.insert(pos, size, c);
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator pos, InitializerList init_list) noexcept {
        return string_.insert(pos, init_list);
    }
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Insert(const ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return string_.insert(pos, first, last);
    }
    //replace
    //find
    //rfind
    //find_first_of
    //find_first_not_of
    //find_last_not_of
    //substr
    //compare
    //erase
    FORCEINLINE constexpr Iterator Erase(ConstIterator pos) noexcept { return string_.erase(pos); }
    FORCEINLINE constexpr Iterator Erase(ConstIterator first, ConstIterator last) noexcept { 
        return string_.erase(first, last); 
    }
    FORCEINLINE constexpr Void Clear() noexcept { string_.clear(); }

    FORCEINLINE constexpr Void Resize(const SizeType size) noexcept { string_.resize(size); }
    FORCEINLINE constexpr Void Resize(const SizeType size, const CharType& c) noexcept { string_.resize(size, c); }

    FORCEINLINE constexpr Void Reserve(const SizeType capacity) noexcept { string_.reserve(capacity); }
    FORCEINLINE constexpr Void ShrinkToFit() noexcept { string_.shrink_to_fit(); }

    FORCEINLINE constexpr Void Swap(TString& string) noexcept { string_.swap(string); }

protected:
    using SuperType = ZObject;

private:
    STDString string_;
};



//operator+
//operator==

}//internal

using ZString = internal::TString<Char>;
using ZWString = internal::TString<WChar>;

}//zengine

#endif // !Z_CORE_Z_STRING_H_