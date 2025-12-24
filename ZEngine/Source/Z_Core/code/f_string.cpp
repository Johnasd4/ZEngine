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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "f_string.h"

#include "m_log.h"
#include "t_array.h"
#include "z_memory.h"

namespace zengine {
namespace string {
namespace internal {

CORE_DLLAPI NODISCARD ZString GenerateString(
    ZStringView _format,
    fmt::format_args _args, 
    SizeType _arg_num
) noexcept {
    ZString str;
    try {
        str.Reserve(_format.GetSize() + _arg_num * 16ULL);
        fmt::vformat_to(
            std::back_inserter(str.STDString()),
            _format.STDStringView(),
            _args
        );
    }
    catch (const fmt::format_error&) {
        str.Clear();
        Z_LOG_ERROR(
            zengine::error_code::kFStringErrorCode_FormatError, 0,
            "console::Print() format error! _format: {}",
            _format
        );
    }
    return str;
}

CORE_DLLAPI NODISCARD SizeType GenerateString(
    Char* _str,
    SizeType _max_len,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    if (_str == nullptr || _max_len == 0ULL) {
        return 0ULL;
    }
    try {
        auto result = fmt::vformat_to_n(
            _str,
            _max_len - 1,
            _format.STDStringView(),
            _args
        );
        _str[result.size] = '\0';
        return result.size;
    }
    catch (const fmt::format_error&) {
        Z_LOG_ERROR(
            zengine::error_code::kFStringErrorCode_FormatError, 0,
            "console::Print() format error! _format: {}",
            _format
        );
        _str[0] = '\0';
        return 0ULL;
    }
}

CORE_DLLAPI NODISCARD SizeType GenerateStringNoEnd(
    Char* _str,
    SizeType _max_len,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    if (_str == nullptr || _max_len == 0ULL) {
        return 0ULL;
    }
    try {
        auto result = fmt::vformat_to_n(
            _str,
            _max_len,
            _format.STDStringView(),
            _args
        );
        return result.size;
    }
    catch (const fmt::format_error&) {
        Z_LOG_ERROR(
            zengine::error_code::kFStringErrorCode_FormatError, 0,
            "console::Print() format error! _format: {}",
            _format
        );
        return 0ULL;
    }
}

}//internal
}//string
}//zengine

namespace zengine {
namespace string {

CORE_DLLAPI NODISCARD ZWString StringToWString(ZStringView _str) noexcept {
    if (_str.GetSize() == 0Ull) {
        return ZWString();
    }

    SizeType len = _str.GetSize();
    SizeType wlen = 0;

    //If utf16 or utf32 is needed, calculate the required length first.
    if constexpr (sizeof(WChar) == 2ULL) {
        // Windows (UTF-16)
        wlen = simdutf::utf16_length_from_utf8(_str.GetDataPtr(), len);
    }
    else {
        // Linux/macOS (UTF-32)
        wlen = simdutf::utf32_length_from_utf8(_str.GetDataPtr(), len);
    }

    ZWString ans_str;
    ans_str.Resize(wlen);

    //convert utf8 to utf16 or utf32
    if constexpr (sizeof(WChar) == 2ULL) {
        simdutf::convert_utf8_to_utf16(_str.GetDataPtr(), len, reinterpret_cast<char16_t*>(ans_str.GetDataPtr()));
    }
    else {
        simdutf::convert_utf8_to_utf32(_str.GetDataPtr(), len, reinterpret_cast<char32_t*>(ans_str.GetDataPtr()));
    }

    return ans_str;
}

CORE_DLLAPI NODISCARD ZString WStringToString(ZWStringView _str) noexcept {
    if (_str.GetSize() == 0Ull) {
        return ZString();
    }

    SizeType wlen = _str.GetSize();
    SizeType len = 0;

    //Calculate the required length first.
    if constexpr (sizeof(WChar) == 2ULL) {
        // Windows (UTF-16)
        len = simdutf::utf8_length_from_utf16(reinterpret_cast<const char16_t*>(_str.GetDataPtr()), wlen);
    }
    else {
        // Linux/macOS (UTF-32)
        len = simdutf::utf8_length_from_utf32(reinterpret_cast<const char32_t*>(_str.GetDataPtr()), wlen);
    }

    ZString ans_str;
    ans_str.Resize(len);

    //convert utf16 or utf32 to utf8
    if constexpr (sizeof(WChar) == 2) {
        simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(_str.GetDataPtr()), wlen, ans_str.GetDataPtr());
    }
    else {
        simdutf::convert_utf32_to_utf8(reinterpret_cast<const char32_t*>(_str.GetDataPtr()), wlen, ans_str.GetDataPtr());
    }

    return ans_str;
}

template<typename _ReturnType>
NODISCARD static TList<_ReturnType> SplitStringSkipEmptyP(ZStringView _str, const Char _token) noexcept {
    SizeType start_index = 0;
    SizeType end_index = 0;
    SizeType str_len = 0;
    TList<_ReturnType> result_list;
    while (end_index != _str.GetSize()) {
        if (_str[end_index] != _token) {
            ++end_index;
            continue;
        }
        else if (start_index == end_index) {
            ++end_index;
            start_index = end_index;
            continue;
        }
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str.GetDataPtr(), start_index, str_len);
        ++end_index;
        start_index = end_index;
    };
    if (start_index != end_index) {
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str.GetDataPtr(), start_index, str_len);
    }
    return result_list;
};

CORE_DLLAPI NODISCARD TList<ZString> SplitToStringSkipEmpty(ZStringView _str, const Char _token) noexcept {
    return SplitStringSkipEmptyP<ZString>(_str, _token);
};

CORE_DLLAPI NODISCARD TList<ZStringView> SplitToStringViewSkipEmpty(ZStringView _str, const Char _token) noexcept {
    return SplitStringSkipEmptyP<ZStringView>(_str, _token);
};

template<typename _NumberType>
NODISCARD static ReturnType StringToNumberP(ZStringView _str, _NumberType* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;

    static_assert(kIsNumber<_NumberType>,
        "string::StringToNumber(): _NumberType must be a number type!"
        );

    auto [error_pos_ptr, error_code] = std::from_chars(_str.GetDataPtr(), _str.GetDataPtr() + _str.GetSize(), *_ans_ptr);
    if (error_code != std::errc()) {
        if (error_code == std::errc::invalid_argument) {
            ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
            Z_LOG_ERROR(ret_val, 0, "Can not transform to number!");
        }
        else if (error_code == std::errc::result_out_of_range) {
            ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
            Z_LOG_ERROR(ret_val, 0, "Number out of range!");
        }
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Int8* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Int16* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Int32* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Int64* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, UInt8* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, UInt16* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, UInt32* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, UInt64* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Float32* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}
CORE_DLLAPI NODISCARD ReturnType StringToNumber(ZStringView _str, Float64* _ans_ptr) noexcept {
    return StringToNumberP(_str, _ans_ptr);
}

template<typename _NumberType>
NODISCARD static ZString NumberToStringP(_NumberType _num) noexcept {
    static_assert(kIsNumber<_NumberType>,
        "string::NumberToString(): _NumberType must be a number type!"
        );

    constexpr SizeType kMaxBufferSize = 64;
    Char buffer[kMaxBufferSize];

    auto result = std::to_chars(buffer, buffer + kMaxBufferSize, _num);
    if (result.ec != std::errc()) {
        Z_LOG_ERROR(error_code::kFStringErrorCode_NumberToStringError, 0, "Number to string error!");
        return ZString();
    }

    return ZString(buffer, static_cast<SizeType>(result.ptr - buffer));
}

CORE_DLLAPI NODISCARD ZString NumberToString(Int8 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(Int16 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(Int32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(Int64 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(UInt8 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(UInt16 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(UInt32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(UInt64 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(Float32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD ZString NumberToString(Float64 _num) noexcept {
    return NumberToStringP(_num);
}

}//string
}//zengine