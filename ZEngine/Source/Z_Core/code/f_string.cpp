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
            _format.GetSTDStringView(),
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
    SizeType _max_length,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    if (_str == nullptr || _max_length == 0ULL) {
        return 0ULL;
    }
    try {
        auto result = fmt::vformat_to_n(
            _str,
            _max_length - 1,
            _format.GetSTDStringView(),
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
    SizeType _max_length,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    if (_str == nullptr || _max_length == 0ULL) {
        return 0ULL;
    }
    try {
        auto result = fmt::vformat_to_n(
            _str,
            _max_length,
            _format.GetSTDStringView(),
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

} // internal
} // string
} // zengine

namespace zengine {
namespace string {

CORE_DLLAPI NODISCARD ZWString StringToWString(ZStringView _str) noexcept {
    if (_str.GetSize() == 0Ull) {
        return ZWString();
    }

    SizeType length = _str.GetSize();
    SizeType w_length = 0;

    // If utf16 or utf32 is needed, calculate the required length first.
    if constexpr (sizeof(WChar) == 2ULL) {
        // Windows (UTF-16)
        w_length = simdutf::utf16_length_from_utf8(
            _str.GetDataPtr(), 
            length
        );
    }
    else {
        // Linux/macOS (UTF-32)
        w_length = simdutf::utf32_length_from_utf8(
            _str.GetDataPtr(), 
            length
        );
    }

    ZWString ans_str;
    ans_str.Resize(w_length);

    // convert utf8 to utf16 or utf32
    if constexpr (sizeof(WChar) == 2ULL) {
        simdutf::convert_utf8_to_utf16(
            _str.GetDataPtr(), 
            length, 
            reinterpret_cast<char16_t*>(ans_str.GetDataPtr())
        );
    }
    else {
        simdutf::convert_utf8_to_utf32(
            _str.GetDataPtr(), 
            length, 
            reinterpret_cast<char32_t*>(ans_str.GetDataPtr())
        );
    }

    return ans_str;
}

CORE_DLLAPI NODISCARD ZString WStringToString(ZWStringView _str) noexcept {
    if (_str.GetSize() == 0Ull) {
        return ZString();
    }

    SizeType w_length = _str.GetSize();
    SizeType length = 0;

    // Calculate the required length first.
    if constexpr (sizeof(WChar) == 2ULL) {
        // Windows (UTF-16)
        length = simdutf::utf8_length_from_utf16(
            reinterpret_cast<const char16_t*>(_str.GetDataPtr()), 
            w_length
        );
    }
    else {
        // Linux/macOS (UTF-32)
        length = simdutf::utf8_length_from_utf32(
            reinterpret_cast<const char32_t*>(_str.GetDataPtr()), 
            w_length
        );
    }

    ZString ans_str;
    ans_str.Resize(length);

    // convert utf16 or utf32 to utf8
    if constexpr (sizeof(WChar) == 2) {
        simdutf::convert_utf16_to_utf8(
            reinterpret_cast<const char16_t*>(_str.GetDataPtr()), 
            w_length, 
            ans_str.GetDataPtr()
        );
    }
    else {
        simdutf::convert_utf32_to_utf8(
            reinterpret_cast<const char32_t*>(_str.GetDataPtr()), 
            w_length, 
            ans_str.GetDataPtr()
        );
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
NODISCARD static TOptional<_NumberType> StringToNumberP(ZStringView _str) noexcept {
    ReturnType ret_val = kOK;

    STATIC_ASSERT(kIsNumber<_NumberType>,
        "string::StringToNumber(): _NumberType must be a number type!"
        );

    _NumberType result;
    auto [error_pos_ptr, error_code] = std::from_chars(_str.GetDataPtr(), _str.GetDataPtr() + _str.GetSize(), result);
    if (error_code != std::errc()) {
        if (error_code == std::errc::invalid_argument) {
            ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
            Z_LOG_ERROR(ret_val, 0, "Can not transform to number!");
        }
        else if (error_code == std::errc::result_out_of_range) {
            ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
            Z_LOG_ERROR(ret_val, 0, "Number out of range!");
        }
        else {
            ret_val = error_code::kFStringErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "Unknown error when converting string to number!");
        }
        return ret_val;
    }

    return result;
}

CORE_DLLAPI NODISCARD TOptional<Int8> StringToInt8(ZStringView _str) noexcept {
    return StringToNumberP<Int8>(_str);
}
CORE_DLLAPI NODISCARD TOptional<Int16> StringToInt16(ZStringView _str) noexcept {
    return StringToNumberP<Int16>(_str);
}
CORE_DLLAPI NODISCARD TOptional<Int32> StringToInt32(ZStringView _str) noexcept {
    return StringToNumberP<Int32>(_str);
}
CORE_DLLAPI NODISCARD TOptional<Int64> StringToInt64(ZStringView _str) noexcept {
    return StringToNumberP<Int64>(_str);
}
CORE_DLLAPI NODISCARD TOptional<UInt8> StringToUInt8(ZStringView _str) noexcept {
    return StringToNumberP<UInt8>(_str);
}
CORE_DLLAPI NODISCARD TOptional<UInt16> StringToUInt16(ZStringView _str) noexcept {
    return StringToNumberP<UInt16>(_str);
}
CORE_DLLAPI NODISCARD TOptional<UInt32> StringToUInt32(ZStringView _str) noexcept {
    return StringToNumberP<UInt32>(_str);
}
CORE_DLLAPI NODISCARD TOptional<UInt64> StringToUInt64(ZStringView _str) noexcept {
    return StringToNumberP<UInt64>(_str);
}
CORE_DLLAPI NODISCARD TOptional<Float32> StringToFloat32(ZStringView _str) noexcept {
    return StringToNumberP<Float32>(_str);
}
CORE_DLLAPI NODISCARD TOptional<Float64> StringToFloat64(ZStringView _str) noexcept {
    return StringToNumberP<Float64>(_str);
}

template<typename _NumberType>
NODISCARD static TOptional<ZString> NumberToStringP(_NumberType _num) noexcept {
    STATIC_ASSERT(kIsNumber<_NumberType>,
        "string::NumberToString(): _NumberType must be a number type!"
    );
    ReturnType ret_val = kOK;

    constexpr SizeType kMaxBufferSize = 64;
    Char buffer[kMaxBufferSize];

    auto result = std::to_chars(buffer, buffer + kMaxBufferSize, _num);
    if (result.ec != std::errc()) {
        ret_val = error_code::kFStringErrorCode_NumberToStringError;
        Z_LOG_ERROR(ret_val, 0, "Number to string error!");
        return ret_val;
    }

    return TOptional<ZString>(buffer, static_cast<SizeType>(result.ptr - buffer));
}

CORE_DLLAPI NODISCARD TOptional<ZString> Int8ToString(Int8 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> Int16ToString(Int16 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> Int32ToString(Int32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> Int64ToString(Int64 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> UInt8ToString(UInt8 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> UInt16ToString(UInt16 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> UInt32ToString(UInt32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> UInt64ToString(UInt64 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> Float32ToString(Float32 _num) noexcept {
    return NumberToStringP(_num);
}
CORE_DLLAPI NODISCARD TOptional<ZString> Float64ToString(Float64 _num) noexcept {
    return NumberToStringP(_num);
}

} // string
} // zengine