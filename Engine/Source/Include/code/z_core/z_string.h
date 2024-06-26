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
template<typename _CharType>
class TString : public ZObject {
public:
    using STDString_ = std::basic_string<_CharType, std::char_traits<_CharType>, TAllocator<_CharType>>;;
    using Iterator_ = STDString_::iterator;
    using ConstIterator_ = STDString_::const_iterator;
    using ReverseIterator_ = STDString_::reverse_iterator;
    using ConstReverseIterator_ = STDString_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_CharType>;

    FORCEINLINE constexpr TString() noexcept : SuperType(), string_() {}
    FORCEINLINE constexpr TString(const TString& _string) noexcept : SuperType(), string_(_string.string_) {}
    FORCEINLINE constexpr TString(const TString& _string, SizeType _pos, SizeType _len = -1) noexcept
        : SuperType(), string_(_string.string_, _pos, _len) {}
    FORCEINLINE constexpr TString(TString&& _string) noexcept : SuperType(), string_(std::move(_string.string_)) {}

    FORCEINLINE constexpr TString(const _CharType* _string) noexcept : SuperType(), string_(_string) {}
    FORCEINLINE constexpr TString(const _CharType* _string, SizeType _size) noexcept 
        : SuperType(), string_(_string, _size) {}
    FORCEINLINE constexpr TString(SizeType _size, _CharType& _val) noexcept : SuperType(), string_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE constexpr TString(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType(), string_(_first, _last) {}
    FORCEINLINE constexpr TString(InitializerList_ _init_list) noexcept : SuperType(), string_(_init_list) {}
 
    FORCEINLINE constexpr ~TString() noexcept {}

    FORCEINLINE constexpr TString& operator=(const TString& _string) noexcept {
        string_.operator=(_string.string_);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(TString&& _string) noexcept {
        string_.operator=(std::move(_string.string_));
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(const _CharType* _string) noexcept {
        string_.operator=(_string);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(_CharType _char) noexcept {
        string_.operator=(_char);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(InitializerList_ _init_list) noexcept {
        string_.operator=(_init_list);
        return *this;
    }

    FORCEINLINE constexpr TString& Assign(const TString& _string) noexcept {
        string_.assign(_string.string_);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const TString& _string, SizeType _pos, SizeType _len = -1) noexcept {
        string_.assign(_string, _pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(TString&& _string) noexcept {
        string_.assign(_string);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const _CharType* _string) noexcept {
        string_.assign(_string);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const _CharType* _string, SizeType _size) noexcept {
        string_.assign(_string, _size);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const SizeType _size, const _CharType& _val) noexcept {
        string_.assign(_size, _val);
        return *this;
    }
    template <class InputIterator>
    FORCEINLINE constexpr TString& Assign(InputIterator _first, InputIterator _last) noexcept {
        string_.assign(_first, _last);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(InitializerList_ _init_list) noexcept {
        string_.assign(_init_list);
        return *this;
    }

    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ == _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ == _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str == _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ != _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ != _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str != _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ > _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ > _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str > _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ >= _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ >= _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str >= _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ < _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ < _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str < _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.string_ <= _right_str.string_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str,  const _CharType* _right_str
    ) noexcept {
        return _left_str.string_ <= _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str <= _right_str.string_;
    }

    NODISCARD FORCEINLINE constexpr _CharType& operator[](const SizeType _index) noexcept { return string_[_index]; }
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept { 
        return string_[_index]; 
    }

    FORCEINLINE constexpr TString& operator+=(const TString& _string) noexcept { 
        string_ += _string.string_; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(const _CharType* _string) noexcept { 
        string_ += _string; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(const _CharType _char) noexcept { 
        string_ += _char; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(InitializerList_ _init_list) noexcept { 
        string_ += _init_list; 
        return *this;
    }

    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const TString& _left_str, const TString& _right_str
    ) noexcept  {
        TString str;
        str.Reserve(_left_str.Size() + _right_str.Size());
        str.Append(_left_str);
        str.Append(_right_str);
        return str;
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        TString str;
        if constexpr (kSameType<_CharType, Char>) {
            str.Reserve(strlen(_left_str) + _right_str.Size());
        }
        else {
            str.Reserve(wcsnlen(_left_str) + _right_str.Size());
        }
        str.Append(_left_str);
        str.Append(_right_str);
        return str;
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const _CharType _left_char, const TString& _right_str
    ) noexcept {
        TString str;
        str.Reserve(1 + _right_str.Size());
        str.Append(1, _left_char);
        str.Append(_right_str);
        return str;
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        TString str;
        if constexpr (kSameType<_CharType, Char>) {
            str.Reserve(_left_str.Size() + strlen(_right_str));
        }
        else {
            str.Reserve(_left_str.Size() + wcsnlen(_right_str));
        }
        str.Append(_left_str);
        str.Append(_right_str);
        return str;
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const TString& _left_str, const _CharType _right_char
    ) noexcept {
        TString str;
        str.Reserve(_left_str.Size() + 1);
        str.Append(_left_str);
        str.Append(1, _right_char);
        return str;
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        TString&& _left_str, const TString& _right_str
    ) noexcept {
        return std::move(_left_str.Append(_right_str));
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const TString& _left_str, TString&& _right_str
    ) noexcept {
        return std::move(_right_str.Insert(0, _left_str));
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const _CharType* _left_str, TString&& _right_str
    ) noexcept {
        return std::move(_right_str.Insert(0, _left_str));
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        const _CharType _left_char, TString&& _right_str
    ) noexcept {
        return std::move(_right_str.Insert(0, 1, _left_char));
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        TString&& _left_str, const _CharType* _right_str
    ) noexcept {
        return std::move(_left_str.Append(_right_str));
    }
    friend NODISCARD FORCEINLINE constexpr TString operator+(
        TString&& _left_str, const _CharType _right_char
    ) noexcept {
        _left_str.PushBack(_right_char);
        return std::move(_left_str);
    }

    NODISCARD FORCEINLINE constexpr _CharType& At(IndexType index) noexcept { return string_.at(index); }
    NODISCARD FORCEINLINE constexpr const _CharType& At(IndexType index) const noexcept { return string_.at(index); }

    NODISCARD FORCEINLINE constexpr _CharType& Front() noexcept { return string_.front(); }
    NODISCARD FORCEINLINE constexpr const _CharType& Front() const noexcept { return string_.front(); }
    NODISCARD FORCEINLINE constexpr _CharType& Back() noexcept { return string_.back(); }
    NODISCARD FORCEINLINE constexpr const _CharType& Back() const noexcept { return string_.back(); }
    NODISCARD FORCEINLINE constexpr _CharType* DataPtr() noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return string_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* String() const noexcept { return string_.data(); }

    NODISCARD FORCEINLINE constexpr IndexType Size() const noexcept { return static_cast<IndexType>(string_.size()); }
    NODISCARD FORCEINLINE constexpr IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE constexpr Bool Empty() const noexcept { return string_.empty(); }

    NODISCARD FORCEINLINE constexpr Iterator_ Begin() noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ Begin() const noexcept { return string_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstBegin() const noexcept { return string_.cbegin(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseBegin() noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseBegin() const noexcept { return string_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseBegin() const noexcept { return string_.crbegin(); }
    NODISCARD FORCEINLINE constexpr Iterator_ End() noexcept { return string_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ End() const noexcept { return string_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstEnd() const noexcept { return string_.cend(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseEnd() noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseEnd() const noexcept { return string_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseEnd() const noexcept { return string_.crend(); }

    FORCEINLINE constexpr Void PushBack(const _CharType _char) noexcept {
        string_.push_back(_char);
    }
    FORCEINLINE constexpr Void PopBack() noexcept {
        string_.pop_back();
    }

    FORCEINLINE constexpr TString& Append(const TString& _string) noexcept { 
        string_.append(_string.string_); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const TString& _string, SizeType _pos, SizeType _len = -1) noexcept {
        string_.append(_string.string_, _pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const _CharType* _string) noexcept { 
        string_.append(_string); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const _CharType* _string, SizeType _str_len) noexcept {
        string_.append(_string, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Append(SizeType _size, const _CharType _char) noexcept { 
        string_.append(_size, _char); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(InitializerList_ _init_list) noexcept { 
        string_.append(_init_list); 
        return *this;
    }

    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const TString& _string) noexcept {
        string_.insert(_pos, _string.string_); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(
        const SizeType _pos, const TString& _string, SizeType _str_pos, SizeType _len = -1
    ) noexcept {
        string_.insert(_pos, _string.string_, _str_pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _string) noexcept {
        string_.insert(_pos, _string);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _string, SizeType _str_len) noexcept {
        string_.insert(_pos, _string, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, SizeType _size, const _CharType _char) noexcept {
        string_.insert(_pos, _size, _char); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, InitializerList_ _init_list) noexcept {
        string_.insert(_pos, _init_list); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, SizeType _size, const _CharType _char) noexcept {
        string_.insert(_pos, _size, _char);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        string_.insert(_pos, _init_list);
        return *this;
    }
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Insert(
        const ConstIterator_ _pos, InputIterator _first, InputIterator _last
    ) noexcept {
        string_.insert(_pos, _first, _last);
        return *this;
    }

    FORCEINLINE constexpr TString& Replace(const SizeType _pos, const SizeType _len, const TString& _string) noexcept {
        string_.replace(_pos, _len, _string.string_);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const TString& _string, SizeType _str_pos, SizeType _str_len = -1
    ) noexcept {
        string_.replace(_pos, _len, _string.string_, _str_pos, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _string
    ) noexcept {
        string_.replace(_pos, _len, _string);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _string, SizeType _str_len
    ) noexcept {
        string_.replace(_pos, _len, _string, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, SizeType _size, const _CharType _char
    ) noexcept {
        string_.replace(_pos, _len, _size, _char);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const TString& _string
    ) noexcept {
        string_.replace(_first, _last, _string);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _string, SizeType _str_len
    ) noexcept {
        string_.replace(_first, _last, _string, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _string
    ) noexcept {
        string_.replace(_first, _last, _string);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, SizeType _size, const _CharType _char
    ) noexcept {
        string_.replace(_first, _last, _size, _char);
        return *this;
    }
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, InputIterator _str_first, InputIterator _str_last
    ) noexcept {
        string_.replace(_first, _last, _str_first, _str_last);
        return *this;
    }

    FORCEINLINE constexpr SizeType Find(const TString& _string, const SizeType _start_pos = 0) noexcept {
        return string_.find(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType* _string, const SizeType _start_pos = 0) noexcept {
        return string_.find(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return string_.find(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return string_.find(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const TString& _string, const SizeType _start_pos = -1) noexcept {
        return string_.rfind(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const _CharType* _string, const SizeType _start_pos = -1) noexcept {
        return string_.rfind(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return string_.rfind(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return string_.rfind(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstOf(const TString& _string, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_of(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType* _string, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_of(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return string_.find_first_of(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_of(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastOf(const TString& _string, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_of(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType* _string, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_of(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return string_.find_last_of(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_of(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstNotOf(const TString& _string, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_not_of(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType* _string, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_not_of(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindFirstNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return string_.find_first_not_of(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return string_.find_first_not_of(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastNotOf(const TString& _string, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_not_of(_string.string_, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType* _string, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_not_of(_string, _start_pos);
    }
    FORCEINLINE constexpr SizeType FindLastNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return string_.find_last_not_of(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return string_.find_last_not_of(_char, _start_pos);
    }

    FORCEINLINE constexpr TString SubString(const SizeType _pos, const SizeType _len = -1) noexcept {
        TString string;
        string.string_ = std::move(string_.substr(_pos, _len));
        return *this;
    }

    FORCEINLINE constexpr Int32 Compare(const TString& _string) noexcept { return string_.compare(_string.string_); }
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const TString& _string) noexcept {
        return string_.compare(_pos, _len, _string.string_);
    }
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const TString& _string, SizeType _str_pos, SizeType _str_len
    ) noexcept {
        return string_.compare(_pos, _len, _string.string_, _str_pos, _str_len);
    }
    FORCEINLINE constexpr Int32 Compare(const _CharType* _string) noexcept {
        return string_.compare(_string);
    }
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const _CharType* _string) noexcept {
        return string_.compare(_pos, _len, _string);
    }
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const _CharType* _string, SizeType _str_len
    ) noexcept {
        return string_.compare(_pos, _len, _string, _str_len);
    }

    FORCEINLINE constexpr Bool StartWith(const TString& _string) noexcept { 
        return string_.starts_with(_string.string_); 
    }
    FORCEINLINE constexpr Bool StartWith(const _CharType* _string) noexcept {
        return string_.starts_with(_string);
    }
    FORCEINLINE constexpr Bool StartWith(const _CharType _char) noexcept {
        return string_.starts_with(_char);
    }
    FORCEINLINE constexpr Bool EndWith(const TString& _string) noexcept {
        return string_.ends_with(_string.string_);
    }
    FORCEINLINE constexpr Bool EndWith(const _CharType* _string) noexcept {
        return string_.ends_with(_string);
    }
    FORCEINLINE constexpr Bool EndWith(const _CharType _char) noexcept {
        return string_.ends_with(_char);
    }

    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _pos) noexcept { return string_.erase(_pos); }
    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return string_.erase(_first, _last); 
    }
    FORCEINLINE constexpr Void Clear() noexcept { string_.clear(); }

    FORCEINLINE constexpr Void Resize(const SizeType _size) noexcept { string_.resize(_size); }
    FORCEINLINE constexpr Void Resize(const SizeType _size, const _CharType& _char) noexcept { 
        string_.resize(_size, _char); 
    }

    FORCEINLINE constexpr Void Reserve(const SizeType _capacity) noexcept { string_.reserve(_capacity); }
    FORCEINLINE constexpr Void ShrinkToFit() noexcept { string_.shrink_to_fit(); }

    FORCEINLINE constexpr Void Swap(TString& _string) noexcept { string_.swap(_string); }

protected:
    using SuperType = ZObject;

private:
    STDString_ string_;
};

}//internal

using ZString = internal::TString<Char>;
using ZWString = internal::TString<WChar>;

}//zengine

#endif // !Z_CORE_Z_STRING_H_