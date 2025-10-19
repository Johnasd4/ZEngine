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

#include "internal/z_drive.h"

#include <string>

#include "m_log.h"
#include "t_allocator.h"
#include "t_list.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZStringErrorCode : ReturnType {
    kZStringErrorCode_LinkError = kErrorCodeBase_ZString,
    kZStringErrorCode_SystemError,
    kZStringErrorCode_NullptrParam,
    kZStringErrorCode_ParamOutOfRange,
    kZStringErrorCode_InvalidString
};
}//error_code
}//zengine

namespace zengine {
namespace internal {

/*
    String container.
*/
template<typename _CharType>
class TString : public ZObject {
public:
    using STDString_ = std::basic_string<_CharType, std::char_traits<_CharType>, TContainerAllocator<_CharType>>;;
    using Iterator_ = STDString_::iterator;
    using ConstIterator_ = STDString_::const_iterator;
    using ReverseIterator_ = STDString_::reverse_iterator;
    using ConstReverseIterator_ = STDString_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_CharType>;

    FORCEINLINE constexpr TString() noexcept : SuperType_(), str_() {}
    FORCEINLINE constexpr TString(const TString& _str) noexcept : SuperType_(_str), str_(_str.str_) {}
    FORCEINLINE constexpr TString(const TString& _str, SizeType _pos, SizeType _len = -1) noexcept
        : SuperType_(), str_(_str.str_, _pos, _len) {}
    FORCEINLINE constexpr TString(TString&& _str) noexcept 
        : SuperType_(std::forward<TString>(_str)), str_(std::move(_str.str_)) {}

    FORCEINLINE constexpr TString(const _CharType* _str) noexcept : SuperType_(), str_(_str) {}
    FORCEINLINE constexpr TString(const _CharType* _str, SizeType _size) noexcept 
        : SuperType_(), str_(_str, _size) {}
    FORCEINLINE constexpr TString(SizeType _size, _CharType& _val) noexcept : SuperType_(), str_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE constexpr TString(_InputIterator _first, _InputIterator _last) noexcept 
        : SuperType_(), str_(_first, _last) {}
    FORCEINLINE constexpr TString(InitializerList_ _init_list) noexcept : SuperType_(), str_(_init_list) {}
    FORCEINLINE constexpr TString(const STDString_& _std_string) noexcept : SuperType_(), str_(_std_string) {}
    FORCEINLINE constexpr TString(STDString_&& _std_string) noexcept 
        : SuperType_(), str_(std::forward<STDString_>(_std_string)) {}

    FORCEINLINE constexpr ~TString() noexcept {}

    FORCEINLINE constexpr TString& operator=(const TString& _str) noexcept {
        SuperType_::operator=(_str);
        str_ = _str.str_;
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(TString&& _str) noexcept {
        SuperType_::operator=(std::forward<TString>(_str));
        str_ = std::move(_str.str_);
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(const _CharType* _str) noexcept {
        str_ = _str;
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(_CharType _char) noexcept {
        str_ = _char;
        return *this;
    }
    FORCEINLINE constexpr TString& operator=(InitializerList_ _init_list) noexcept {
        str_ = _init_list;
        return *this;
    }

    FORCEINLINE constexpr TString& Assign(const TString& _str) noexcept {
        str_.assign(_str.str_);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const TString& _str, SizeType _pos, SizeType _len = -1) noexcept {
        str_.assign(_str, _pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(TString&& _str) noexcept {
        str_.assign(_str);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const _CharType* _str) noexcept {
        str_.assign(_str);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const _CharType* _str, SizeType _size) noexcept {
        str_.assign(_str, _size);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(const SizeType _size, const _CharType& _val) noexcept {
        str_.assign(_size, _val);
        return *this;
    }
    template <class InputIterator>
    FORCEINLINE constexpr TString& Assign(InputIterator _first, InputIterator _last) noexcept {
        str_.assign(_first, _last);
        return *this;
    }
    FORCEINLINE constexpr TString& Assign(InitializerList_ _init_list) noexcept {
        str_.assign(_init_list);
        return *this;
    }

    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ == _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ == _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator==(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str == _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ != _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ != _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator!=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str != _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ > _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ > _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str > _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ >= _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ >= _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator>=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str >= _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ < _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const TString& _left_str, const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ < _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str < _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str, const TString& _right_str
    ) noexcept {
        return _left_str.str_ <= _right_str.str_;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const TString& _left_str,  const _CharType* _right_str
    ) noexcept {
        return _left_str.str_ <= _right_str;
    }
    friend NODISCARD FORCEINLINE constexpr Bool operator<=(
        const _CharType* _left_str, const TString& _right_str
    ) noexcept {
        return _left_str <= _right_str.str_;
    }

    NODISCARD FORCEINLINE constexpr _CharType& operator[](const SizeType _index) noexcept { return str_[_index]; }
    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept { 
        return str_[_index]; 
    }

    FORCEINLINE constexpr TString& operator+=(const TString& _str) noexcept { 
        str_ += _str.str_; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(const _CharType* _str) noexcept { 
        str_ += _str; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(const _CharType _char) noexcept { 
        str_ += _char; 
        return *this;
    }
    FORCEINLINE constexpr TString& operator+=(InitializerList_ _init_list) noexcept { 
        str_ += _init_list; 
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

    NODISCARD FORCEINLINE constexpr _CharType& At(IndexType index) noexcept { return str_.at(index); }
    NODISCARD FORCEINLINE constexpr const _CharType& At(IndexType index) const noexcept { return str_.at(index); }

    NODISCARD FORCEINLINE constexpr _CharType& Front() noexcept { return str_.front(); }
    NODISCARD FORCEINLINE constexpr const _CharType& Front() const noexcept { return str_.front(); }
    NODISCARD FORCEINLINE constexpr _CharType& Back() noexcept { return str_.back(); }
    NODISCARD FORCEINLINE constexpr const _CharType& Back() const noexcept { return str_.back(); }
    NODISCARD FORCEINLINE constexpr _CharType* DataPtr() noexcept { return str_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return str_.data(); }
    NODISCARD FORCEINLINE constexpr const _CharType* String() const noexcept { return str_.data(); }

    NODISCARD FORCEINLINE constexpr IndexType Size() const noexcept { return static_cast<IndexType>(str_.size()); }
    NODISCARD FORCEINLINE constexpr IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE constexpr Bool Empty() const noexcept { return str_.empty(); }

    NODISCARD FORCEINLINE constexpr Iterator_ Begin() noexcept { return str_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ Begin() const noexcept { return str_.begin(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstBegin() const noexcept { return str_.cbegin(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseBegin() noexcept { return str_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseBegin() const noexcept { return str_.rbegin(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseBegin() const noexcept { return str_.crbegin(); }
    NODISCARD FORCEINLINE constexpr Iterator_ End() noexcept { return str_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ End() const noexcept { return str_.end(); }
    NODISCARD FORCEINLINE constexpr ConstIterator_ ConstEnd() const noexcept { return str_.cend(); }
    NODISCARD FORCEINLINE constexpr ReverseIterator_ ReverseEnd() noexcept { return str_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ReverseEnd() const noexcept { return str_.rend(); }
    NODISCARD FORCEINLINE constexpr ConstReverseIterator_ ConstReverseEnd() const noexcept { return str_.crend(); }

    FORCEINLINE constexpr Void PushBack(const _CharType _char) noexcept {
        str_.push_back(_char);
    }
    FORCEINLINE constexpr Void PopBack() noexcept {
        str_.pop_back();
    }

    FORCEINLINE constexpr TString& Append(const TString& _str) noexcept { 
        str_.append(_str.str_); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const TString& _str, SizeType _pos, SizeType _len = -1) noexcept {
        str_.append(_str.str_, _pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const _CharType* _str) noexcept { 
        str_.append(_str); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(const _CharType* _str, SizeType _str_len) noexcept {
        str_.append(_str, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Append(SizeType _size, const _CharType _char) noexcept { 
        str_.append(_size, _char); 
        return *this;
    }
    FORCEINLINE constexpr TString& Append(InitializerList_ _init_list) noexcept { 
        str_.append(_init_list); 
        return *this;
    }

    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const TString& _str) noexcept {
        str_.insert(_pos, _str.str_); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(
        const SizeType _pos, const TString& _str, SizeType _str_pos, SizeType _len = -1
    ) noexcept {
        str_.insert(_pos, _str.str_, _str_pos, _len);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _str) noexcept {
        str_.insert(_pos, _str);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, const _CharType* _str, SizeType _str_len) noexcept {
        str_.insert(_pos, _str, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, SizeType _size, const _CharType _char) noexcept {
        str_.insert(_pos, _size, _char); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const SizeType _pos, InitializerList_ _init_list) noexcept {
        str_.insert(_pos, _init_list); 
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, SizeType _size, const _CharType _char) noexcept {
        str_.insert(_pos, _size, _char);
        return *this;
    }
    FORCEINLINE constexpr TString& Insert(const ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        str_.insert(_pos, _init_list);
        return *this;
    }
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Insert(
        const ConstIterator_ _pos, InputIterator _first, InputIterator _last
    ) noexcept {
        str_.insert(_pos, _first, _last);
        return *this;
    }

    FORCEINLINE constexpr TString& Replace(const SizeType _pos, const SizeType _len, const TString& _str) noexcept {
        str_.replace(_pos, _len, _str.str_);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const TString& _str, SizeType _str_pos, SizeType _str_len = -1
    ) noexcept {
        str_.replace(_pos, _len, _str.str_, _str_pos, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _str
    ) noexcept {
        str_.replace(_pos, _len, _str);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, const _CharType* _str, SizeType _str_len
    ) noexcept {
        str_.replace(_pos, _len, _str, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const SizeType _pos, const SizeType _len, SizeType _size, const _CharType _char
    ) noexcept {
        str_.replace(_pos, _len, _size, _char);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const TString& _str
    ) noexcept {
        str_.replace(_first, _last, _str);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _str, SizeType _str_len
    ) noexcept {
        str_.replace(_first, _last, _str, _str_len);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, const _CharType* _str
    ) noexcept {
        str_.replace(_first, _last, _str);
        return *this;
    }
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, SizeType _size, const _CharType _char
    ) noexcept {
        str_.replace(_first, _last, _size, _char);
        return *this;
    }
    template <typename InputIterator>
    FORCEINLINE constexpr TString& Replace(
        const ConstIterator_ _first, const ConstIterator_ _last, InputIterator _str_first, InputIterator _str_last
    ) noexcept {
        str_.replace(_first, _last, _str_first, _str_last);
        return *this;
    }

    FORCEINLINE constexpr SizeType Find(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find(_str.str_, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find(_str, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const TString& _str, const SizeType _start_pos = -1) noexcept {
        return str_.rfind(_str.str_, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const _CharType* _str, const SizeType _start_pos = -1) noexcept {
        return str_.rfind(_str, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReserveFind(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return str_.rfind(_str_start, _start_pos, _str_len);
    }
    FORCEINLINE constexpr SizeType ReserveFind(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return str_.rfind(_char, _start_pos);
    }
    /*
        Finds the first char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstOf(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_str.str_, _start_pos);
    }
    /*
        Finds the first char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_str, _start_pos);
    }
    /*
        Finds the first char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find_first_of(_str_start, _start_pos, _str_len);
    }
    /*
        Finds the first char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_of(_char, _start_pos);
    }
    /*
        Finds the last char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastOf(const TString& _str, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_of(_str.str_, _start_pos);
    }
    /*
        Finds the last char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType* _str, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_of(_str, _start_pos);
    }
    /*
        Finds the last char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return str_.find_last_of(_str_start, _start_pos, _str_len);
    }
    /*
        Finds the last char pos which the char is in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastOf(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_of(_char, _start_pos);
    }
    /*
        Finds the first char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const TString& _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_str.str_, _start_pos);
    }
    /*
        Finds the first char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_str, _start_pos);
    }
    /*
        Finds the first char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return str_.find_first_not_of(_str_start, _start_pos, _str_len);
    }
    /*
        Finds the first char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindFirstNotOf(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return str_.find_first_not_of(_char, _start_pos);
    }
    /*
        Finds the last char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastNotOf(const TString& _str, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_not_of(_str.str_, _start_pos);
    }
    /*
        Finds the last char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType* _str, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_not_of(_str, _start_pos);
    }
    /*
        Finds the last char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastNotOf(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return str_.find_last_not_of(_str_start, _start_pos, _str_len);
    }
    /*
        Finds the last char pos which the char is not in the given string.
    */
    FORCEINLINE constexpr SizeType FindLastNotOf(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return str_.find_last_not_of(_char, _start_pos);
    }

    FORCEINLINE constexpr TString SubString(const SizeType _pos, const SizeType _len = -1) noexcept {
        TString str;
        str.str_ = std::move(str_.substr(_pos, _len));
        return str;
    }

    FORCEINLINE constexpr Int32 Compare(const TString& _str) noexcept { return str_.compare(_str.str_); }
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const TString& _str) noexcept {
        return str_.compare(_pos, _len, _str.str_);
    }
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const TString& _str, SizeType _str_pos, SizeType _str_len
    ) noexcept {
        return str_.compare(_pos, _len, _str.str_, _str_pos, _str_len);
    }
    FORCEINLINE constexpr Int32 Compare(const _CharType* _str) noexcept {
        return str_.compare(_str);
    }
    FORCEINLINE constexpr Int32 Compare(SizeType _pos, SizeType _len, const _CharType* _str) noexcept {
        return str_.compare(_pos, _len, _str);
    }
    FORCEINLINE constexpr Int32 Compare(
        SizeType _pos, SizeType _len, const _CharType* _str, SizeType _str_len
    ) noexcept {
        return str_.compare(_pos, _len, _str, _str_len);
    }

    FORCEINLINE constexpr Bool StartWith(const TString& _str) noexcept { 
        return str_.starts_with(_str.str_); 
    }
    FORCEINLINE constexpr Bool StartWith(const _CharType* _str) noexcept {
        return str_.starts_with(_str);
    }
    FORCEINLINE constexpr Bool StartWith(const _CharType _char) noexcept {
        return str_.starts_with(_char);
    }
    FORCEINLINE constexpr Bool EndWith(const TString& _str) noexcept {
        return str_.ends_with(_str.str_);
    }
    FORCEINLINE constexpr Bool EndWith(const _CharType* _str) noexcept {
        return str_.ends_with(_str);
    }
    FORCEINLINE constexpr Bool EndWith(const _CharType _char) noexcept {
        return str_.ends_with(_char);
    }

    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _pos) noexcept { return str_.erase(_pos); }
    FORCEINLINE constexpr Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept { 
        return str_.erase(_first, _last); 
    }
    FORCEINLINE constexpr Void Clear() noexcept { str_.clear(); }

    FORCEINLINE constexpr Void Resize(const SizeType _size) noexcept { str_.resize(_size); }
    FORCEINLINE constexpr Void Resize(const SizeType _size, const _CharType& _char) noexcept { 
        str_.resize(_size, _char); 
    }

    FORCEINLINE constexpr Void Reserve(const SizeType _capacity) noexcept { str_.reserve(_capacity); }
    
    FORCEINLINE constexpr Void ShrinkToFit() noexcept { str_.shrink_to_fit(); }

    FORCEINLINE constexpr Void Swap(TString& _str) noexcept { str_.swap(_str); }

    NODISCARD FORCEINLINE constexpr SizeType Hash() const noexcept {
        return std::hash<STDString_>()(str_);
    }

    template<typename _NumberType>
    requires kIsNumber<_NumberType>
    FORCEINLINE constexpr Void FromNum(_NumberType _num) noexcept {
        if constexpr (kSameType<_CharType, Char>) {
            str_ = std::to_string(_num);
        }
        else {
            str_ = std::to_wstring(_num);
        }
    }

    NODISCARD ReturnType ToInt32(Int32* _ans_ptr) noexcept {
        Int32 ans = 0;
        ReturnType ret_val = kOK;
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            const Char* str = str_.c_str();
            Char* err_str;
            *_ans_ptr = std::strtol(str, &err_str, 10);
            if (str == err_str) {
                ret_val = error_code::kZStringErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, 0, L"std::strtol() link error! Wrong Parameter!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, 0, L"std::strtol() link error! Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            const WChar* str = str_.c_str();
            WChar* err_str;
            *_ans_ptr = std::wcstol(str, &err_str, 10);
            if (str == err_str) {
                ret_val = error_code::kZStringErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, 0, L"std::wcstol() link error! Wrong Parameter!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, 0, L"std::wcstol() link error! Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD TList<TString<_CharType>> Split(const _CharType _token) noexcept {
        ReturnType ret_val = kOK;
        IndexType start_index = 0;
        IndexType end_index = 0;
        IndexType str_len = 0;
        _CharType temp_char = '\0';
        TList<TString<_CharType>> result_list;
        while (end_index != str_.size()) {
            if (str_[end_index] != _token) {
                ++end_index;
                continue;
            }
            else if (start_index == end_index) {
                ++end_index;
                start_index = end_index;
                continue;
            }
            str_len = end_index - start_index;
            temp_char = str_[end_index];
            if constexpr (kSameType<_CharType, Char>) {
                str_[end_index] = '\0';
            }
            else if constexpr (kSameType<_CharType, WChar>) {
                str_[end_index] = L'\0';
            }
            result_list.PushBack(TString<_CharType>(&str_[start_index]));
            str_[end_index] = temp_char;
            ++end_index;
            start_index = end_index;
        };
        if (start_index != end_index) {
            str_len = end_index - start_index;
            result_list.PushBack(TString<_CharType>(&str_[start_index]));
        }
        return result_list;
    };

protected:
    using SuperType_ = ZObject;

private:
    STDString_ str_;
};

}//internal

using ZString = internal::TString<Char>;
using ZWString = internal::TString<WChar>;

namespace string {

/*
    Translate narrow string to wide string, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ZWString String2WString(const Char* _str) noexcept;

/*
    Translate wide string to narrow string, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ZString WString2String(const WChar* _str) noexcept;

}//string

}//zengine

namespace std {
    template<>
    struct hash<zengine::ZString> {
        size_t operator()(const zengine::ZString& _str) const noexcept {
            return _str.Hash();
        }
    };

    template<>
    struct hash<zengine::ZWString> {
        size_t operator()(const zengine::ZWString& _str) const noexcept {
            return _str.Hash();
        }
    };
}