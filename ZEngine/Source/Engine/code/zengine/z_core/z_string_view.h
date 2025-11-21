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

#include "m_log.h"
#include "t_vector.h"
#include "z_string.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZStringViewErrorCode : ReturnType {
    kZStringViewErrorCode_LinkError = kErrorCodeBase_ZStringView,
    kZStringViewErrorCode_SystemError,
    kZStringViewErrorCode_NullptrParam,
    kZStringViewErrorCode_ParamOutOfRange,
    kZStringViewErrorCode_InvalidString,
    kZStringViewErrorCode_StringToNumberCanNotTransform,
    kZStringViewErrorCode_StringToNumberOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace internal {

/*
    String view class. No heap memory applied.
    Member:
        _CharType str_: Points at an existing string.
        SizeType size_: The string size.
*/
template<typename _CharType>
class TStringView : public ZObject {
public:
    template<typename _AllocatorType>
    using STDString_ = std::basic_string<_CharType, std::char_traits<_CharType>, _AllocatorType>;

    static constexpr SizeType kFindEnd = std::string::npos;

    FORCEINLINE constexpr TStringView() noexcept 
        : SuperType_(), str_(nullptr), size_(0ULL) {}
    FORCEINLINE constexpr TStringView(const TStringView& _str) noexcept 
        : SuperType_(_str), str_(_str.str_), size_(_str.size_) {}
    FORCEINLINE constexpr TStringView(TStringView&& _str) noexcept 
        : SuperType_(std::forward<TStringView>(_str)), str_(_str.str_), size_(_str.size_) { _str.size_ = 0; }
    constexpr TStringView(const TStringView& _str, SizeType _pos, SizeType _len = -1) noexcept
        : SuperType_(), str_(&_str.str_[_pos])
    {
        size_ = _len < _str.size_ ? _len : _str.size_;
        SizeType sub_str_size = _str.size_ - _pos;
        size_ = _len < sub_str_size ? _len : sub_str_size;
    }
    FORCEINLINE constexpr TStringView(const _CharType* _str, SizeType _size) noexcept 
        : SuperType_(), str_(_str), size_(_size) {}
    FORCEINLINE constexpr TStringView(const _CharType* _str) noexcept
        : SuperType_(), str_(_str) 
    {
        if constexpr (kSameType<_CharType, Char>) {
            size_ = strlen(str_);
        }
        else {
            size_ = wcslen(str_);
        }
    }
    FORCEINLINE constexpr TStringView(const TString<_CharType>& _str) noexcept 
        : SuperType_(), str_(_str.String()), size_(_str.Size()) {}
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView(const STDString_<_AllocatorType>& _std_string) noexcept
        : SuperType_(), str_(_std_string.c_str()), size_(_std_string.size()) {}

    FORCEINLINE constexpr ~TStringView() noexcept {}

    FORCEINLINE constexpr TStringView& operator=(const TStringView& _str) noexcept {
        SuperType_::operator=(_str);
        str_ = _str.str_;
        size_ = _str.size_;
        return *this;
    }
    FORCEINLINE constexpr TStringView& operator=(TStringView&& _str) noexcept {
        SuperType_::operator=(std::forward<TStringView>(_str));
        str_ = _str.str_;
        size_ = _str.size_;
        _str.size_ = 0ULL;
        return *this;
    }
    FORCEINLINE constexpr TStringView& operator=(const _CharType* _str) noexcept {
        str_ = _str;
        if constexpr (kSameType<_CharType, Char>) {
            size_ = strlen(str_);
        }
        else {
            size_ = wcslen(str_);
        }
        return *this;
    }
    FORCEINLINE constexpr TStringView& operator=(const TString<_CharType>& _str) noexcept {
        str_ = _str.String();
        size_ = _str.Size();
        return *this;
    }
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView& operator=(const STDString_<_AllocatorType>& _std_string) noexcept {
        str_ = _std_string.c_str();
        size_ = _std_string.size();
        return *this;
    }

    FORCEINLINE constexpr TStringView& SetViewString(const TStringView& _str) noexcept {
        str_ = _str.str_;
        size_ = _str.size_;
        return *this;
    }
    FORCEINLINE constexpr TStringView& SetViewString(TStringView&& _str) noexcept {
        str_ = _str.str_;
        size_ = _str.size_;
        _str.size_ = 0ULL;
        return *this;
    }
    constexpr TStringView& SetViewString(
        const TStringView& _str, 
        SizeType _pos, 
        SizeType _len = -1
    ) noexcept {
        size_ = _len < _str.size_ ? _len : _str.size_;
        SizeType sub_str_size = _str.size_ - _pos;
        size_ = _len < sub_str_size ? _len : sub_str_size;
        return *this;
    }
    FORCEINLINE constexpr TStringView& SetViewString(const _CharType* _str, SizeType _size) noexcept {
        str_ = _str;
        size_ = _size;
        return *this;
    }
    FORCEINLINE constexpr TStringView& SetViewString(const _CharType* _str) noexcept {
        str_ = _str;
        if constexpr (kSameType<_CharType, Char>) {
            size_ = strlen(str_);
        }
        else {
            size_ = wcslen(str_);
        }
        return *this;
    }
    FORCEINLINE constexpr TStringView& SetViewString(const TString<_CharType>& _str) noexcept {
        str_ = _str.String();
        size_ = _str.Size();
        return *this;
    }
    template<typename _AllocatorType>
    FORCEINLINE constexpr TStringView& SetViewString(const STDString_<_AllocatorType>& _std_string) noexcept {
        str_ = _std_string.c_str();
        size_ = _std_string.size();
        return *this;
    }

    NODISCARD FORCEINLINE constexpr const _CharType& operator[](const SizeType _index) const noexcept { 
        return str_[_index]; 
    }

    NODISCARD FORCEINLINE constexpr const _CharType& At(SizeType _index) const noexcept { return str_[_index]; }

    NODISCARD FORCEINLINE constexpr const _CharType* DataPtr() const noexcept { return str_; }
    NODISCARD FORCEINLINE constexpr TString<_CharType> String() const noexcept { 
        return TString<_CharType>(str_, size_);
    }

    NODISCARD FORCEINLINE constexpr SizeType Size() const noexcept { return size_; }
    NODISCARD FORCEINLINE constexpr Bool Empty() const noexcept { return size_ == 0ULL; }

    FORCEINLINE constexpr SizeType Find(const TStringView& _str, const SizeType _start_pos = 0) noexcept {
        return FindP(_str.str_, _str.size_, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType* _str, const SizeType _start_pos = 0) noexcept {
        SizeType size;
        if constexpr (kSameType<_CharType, Char>) {
            size = strlen(_str);
        }
        else {
            size = wcslen(_str);
        }
        return FindP(_str, size, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = 0
    ) noexcept {
        return FindP(_str_start, _str_len, _start_pos);
    }
    FORCEINLINE constexpr SizeType Find(const _CharType _char, const SizeType _start_pos = 0) noexcept {
        return FindP(_char, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReverseFind(const TStringView& _str, const SizeType _start_pos = -1) noexcept {
        return ReverseFindP(_str.str_, _str.size_, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReverseFind(const _CharType* _str, const SizeType _start_pos = -1) noexcept {
        SizeType size;
        if constexpr (kSameType<_CharType, Char>) {
            size = strlen(_str);
        }
        else {
            size = wcslen(_str);
        }
        return ReverseFindP(_str, size, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReverseFind(
        const _CharType* _str_start, const SizeType _str_len, const SizeType _start_pos = -1
    ) noexcept {
        return ReverseFindP(_str_start, _str_len, _start_pos);
    }
    FORCEINLINE constexpr SizeType ReverseFind(const _CharType _char, const SizeType _start_pos = -1) noexcept {
        return ReverseFindP(_char, _start_pos);
    }

    constexpr TStringView SubString(const SizeType _pos, const SizeType _len = -1) noexcept {
        TStringView str;
        str.str_ = &str_[_pos];
        size_ = _len < size_ ? _len : size_;
        size_ = size_ < _pos ? 0 : size_ - _pos;
        return str;
    }

    FORCEINLINE constexpr Void Clear() noexcept { size_ = 0ULL; }
  
    constexpr Void Swap(TStringView& _str) noexcept { 
        const _CharType* temp_str = _str.str_;
        _str.str_ = str_;
        str_ = temp_str;
        SizeType temp_size = _str.size_;
        _str.size_ = size_;
        size_ = temp_size;
    }

    NODISCARD FORCEINLINE constexpr SizeType Hash() const noexcept {
        return size_;
    }

    NODISCARD ReturnType ToInt32(Int32* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtol(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstol(str_, &err_str, 10);
            if (str_ == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType ToInt64(Int64* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtoll(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstoll(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType ToUInt32(UInt32* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtoul(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstoul(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType ToUInt64(UInt64* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtoull(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstoull(string.String(), &err_str, 10);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType ToFloat32(Float32* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtof(string.String(), &err_str);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstof(string.String(), &err_str);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType ToFloat64(Float64* _ans_ptr) noexcept {
        ReturnType ret_val = kOK;
        TString<_CharType> string = String();
        if constexpr (kSameType<_CharType, Char>) {
            Int32& err_ref = errno;
            err_ref = 0;
            Char* err_str;
            *_ans_ptr = std::strtod(string.String(), &err_str);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        else if constexpr (kSameType<_CharType, WChar>) {
            Int32& err_ref = errno;
            err_ref = 0;
            WChar* err_str;
            *_ans_ptr = std::wcstod(string.String(), &err_str);
            if (string.String() == err_str) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberCanNotTransform;
                Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
            }
            else if (err_ref == ERANGE) {
                ret_val = error_code::kZStringViewErrorCode_StringToNumberOutOfRange;
                Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
            }
        }
        return ret_val;
    }

    NODISCARD TList<TString<_CharType>> Split(const _CharType _token) noexcept {
        ReturnType ret_val = kOK;
        SizeType start_index = 0;
        SizeType end_index = 0;
        SizeType str_len = 0;
        _CharType temp_char = '\0';
        TList<TString<_CharType>> result_list;
        while (end_index != size_) {
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
            result_list.EmplaceBack(&str_[start_index], str_len);
            ++end_index;
            start_index = end_index;
        };
        if (start_index != end_index) {
            str_len = end_index - start_index;
            result_list.EmplaceBack(&str_[start_index], str_len);
        }
        return result_list;
    };

protected:
    using SuperType_ = ZObject;

private:
    NODISCARD SizeType FindP(const _CharType* _pattern, SizeType _pattern_size, SizeType _start_pos) noexcept {
        if (size_ == 0ULL) {
            return kFindEnd;
        }
        if (_pattern_size == 0ULL) {
            return 0ULL;
        }

        //build next vector
        TVector<SizeType> next_vector(_pattern_size, 0ULL);
        SizeType same_char_count = 0ULL;
        for (SizeType pattern_index = 1ULL; pattern_index < _pattern_size - 1; ++pattern_index) {
            while (same_char_count > 0ULL && _pattern[pattern_index] != _pattern[same_char_count]) {
                same_char_count = next_vector[same_char_count - 1ULL];
            }
            if (_pattern[pattern_index] == _pattern[same_char_count]) {
                ++same_char_count;
            }
            next_vector[pattern_index] = same_char_count;
        }

        //matching
        same_char_count = 0ULL;
        for (SizeType str_index = _start_pos; str_index < size_; ++str_index) {
            while (same_char_count > 0ULL && str_[str_index] != _pattern[same_char_count]) {
                same_char_count = next_vector[same_char_count - 1ULL];
            }
            if (str_[str_index] == _pattern[same_char_count]) {
                ++same_char_count;
            }
            //str_found
            if (same_char_count == _pattern_size) {
                return str_index - _pattern_size + 1ULL;
            }
        }

        //not found
        return kFindEnd;
    }
    NODISCARD SizeType ReverseFindP(const _CharType* _pattern, SizeType _pattern_size, SizeType _start_pos) noexcept {
        if (size_ == 0ULL) {
            return kFindEnd;
        }
        if (_pattern_size == 0ULL) {
            return size_ - 1ULL;
        }

        //build next vector
        TVector<SizeType> next_vector(_pattern_size, 0ULL);
        SizeType same_char_count = 0ULL;

        //second char
        for (SizeType pattern_index = _pattern_size - 3ULL; pattern_index < _pattern_size; --pattern_index) {
            while (same_char_count > 0ULL && _pattern[pattern_index] != _pattern[_pattern_size - 1 - same_char_count]) {
                same_char_count = next_vector[same_char_count - 1ULL];
            }
            if (_pattern[pattern_index] == _pattern[_pattern_size - 1 - same_char_count]) {
                ++same_char_count;
            }
            next_vector[_pattern_size - 1 - pattern_index] = same_char_count;
        }

        //matching
        same_char_count = 0ULL;
        SizeType start_index = size_ - 1ULL;
        if (start_index > _start_pos) {
            start_index = _start_pos;
        }
        for (SizeType str_index = start_index; str_index < size_; --str_index) {
            while (same_char_count > 0ULL && str_[str_index] != _pattern[_pattern_size - 1 - same_char_count]) {
                same_char_count = next_vector[same_char_count - 1ULL];
            }
            if (str_[str_index] == _pattern[_pattern_size - 1 - same_char_count]) {
                ++same_char_count;
            }
            //str_found
            if (same_char_count == _pattern_size) {
                return str_index;
            }
        }

        //not found
        return kFindEnd;
    }
    NODISCARD SizeType FindP(const _CharType _pattern, SizeType _start_pos) noexcept {
        //matching
        for (SizeType str_index = 0ULL; str_index < size_; ++str_index) {
            //str_found
            if (str_[str_index] == _pattern) {
                return str_index;
            }
        }
        //not found
        return kFindEnd;
    }
    NODISCARD SizeType ReverseFindP(const _CharType _pattern, SizeType _start_pos) noexcept {
        //matching
        SizeType start_index = size_ - 1ULL;
        if (start_index > _start_pos) {
            start_index = _start_pos;
        }
        for (SizeType str_index = start_index; str_index < size_; --str_index) {
            //str_found
            if (str_[str_index] == _pattern) {
                return str_index;
            }
        }
        //not found
        return kFindEnd;
    }

    const _CharType* str_;
    SizeType size_;
};

}//internal

using ZStringView = internal::TStringView<Char>;
using ZWStringView = internal::TStringView<WChar>;

}//zengine

namespace std {
    template<>
    struct hash<zengine::ZStringView> {
        size_t operator()(const zengine::ZStringView& _str) const noexcept {
            return _str.Hash();
        }
    };

    template<>
    struct hash<zengine::ZWStringView> {
        size_t operator()(const zengine::ZWStringView& _str) const noexcept {
            return _str.Hash();
        }
    };
}//std