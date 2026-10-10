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

#include "internal/l_fmt.h"

#include "t_list.h"
#include "t_optional.h"
#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {

/** @brief Enumerates error codes for FString operations. */
enum FStringErrorCodeEnum : ReturnType::ValueType {
    /** @brief Represents a linking error in the FString module. */
    kFStringErrorCode_LinkError = kErrorCodeBase_FString,
    /** @brief Represents a general system error. */
    kFStringErrorCode_SystemOrLibraryError,
    /** @brief Indicates a null pointer was passed as a parameter. */
    kFStringErrorCode_NullptrParam,
    /** @brief Indicates a parameter is out of its valid range. */
    kFStringErrorCode_ParamOutOfRange,
    /** @brief Represents an invalid string format. */
    kFStringErrorCode_InvalidString,
    /** @brief Error when a string cannot be converted to a number. */
    kFStringErrorCode_StringToNumberCanNotTransform,
    /** @brief Error when a converted number is out of the range of the target type. */
    kFStringErrorCode_StringToNumberOutOfRange,
    /** @brief Error when converting a number to a string failed. */
    kFStringErrorCode_NumberToStringError,
    /** @brief Error related to string formatting. */
    kFStringErrorCode_FormatError,
};

} // error_code
} // zengine

namespace zengine {
namespace string {
namespace internal {

/**
 * @brief Internal function to generate a ZString using fmt arguments.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @param _arg_num The number of arguments used to reserve memory efficiently.
 * @return The formatted ZString.
 */
CORE_DLLAPI NODISCARD ZString GenerateString(
    ZStringView _format,
    fmt::format_args _args, 
    SizeType _arg_num
) noexcept;

/**
 * @brief Internal function to generate a string into a buffer using fmt arguments.
 * @param _str The destination buffer.
 * @param _max_length The maximum length of the buffer.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @return The number of characters written.
 */
CORE_DLLAPI NODISCARD SizeType GenerateString(
    Char* _str, 
    SizeType _max_length,
    ZStringView _format, 
    fmt::format_args _args
) noexcept;

/**
 * @brief Internal function to generate a string into a buffer without a null terminator using fmt arguments.
 * @param _str The destination buffer.
 * @param _max_length The maximum length of the buffer.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @return The number of characters written.
 */
CORE_DLLAPI NODISCARD SizeType GenerateStringNoEnd(
    Char* _str,
    SizeType _max_length,
    ZStringView _format,
    fmt::format_args _args
) noexcept;

} // internal
} // string
} // zengine

namespace zengine {
namespace string {

/**
 * @brief Translates a narrow character string (ZStringView) to a wide character string (ZWString).
 *
 * Implementation Principle:
 * Uses the simdutf library to perform high-performance conversion from UTF-8 (narrow) to either
 * UTF-16 (Windows) or UTF-32 (Unix-like), depending on the size of WChar.
 *
 * @param _str The narrow string view to convert.
 * @return A ZWString containing the converted wide string. Returns an empty string if the input is invalid.
 */
CORE_DLLAPI NODISCARD ZWString StringToWString(ZStringView _str) noexcept;

/**
 * @brief Translates a wide character string (ZWStringView) to a narrow character string (ZString).
 *
 * Implementation Principle:
 * Uses the simdutf library to perform high-performance conversion from either UTF-16 or UTF-32 (wide)
 * back to UTF-8 (narrow).
 *
 * @param _str The wide string view to convert.
 * @return A ZString containing the converted narrow string. Returns an empty string if the input is invalid.
 */
CORE_DLLAPI NODISCARD ZString WStringToString(ZWStringView _str) noexcept;

/**
 * @brief Splits a string view by a specified delimiter into a list of strings, skipping any empty entries.
 * @param _str The string view to be split.
 * @param _token The character to split the string by.
 * @return A TList<ZString> containing the non-empty substrings.
 */
CORE_DLLAPI NODISCARD TList<ZString> SplitToStringSkipEmpty(ZStringView _str, const Char _token) noexcept;

/**
 * @brief Splits a string view by a specified delimiter into a list of string views, skipping any empty entries.
 * @param _str The string view to be split.
 * @param _token The character to split the string by.
 * @return A TList<ZStringView> containing the non-empty substrings.
 */
CORE_DLLAPI NODISCARD TList<ZStringView> SplitToStringViewSkipEmpty(ZStringView _str, const Char _token) noexcept;

/**
 * @brief Converts a string view to an Int8.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Int8 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Int8> StringToInt8(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to an Int16.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Int16 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Int16> StringToInt16(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to an Int32.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Int32 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Int32> StringToInt32(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to an Int64.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Int64 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Int64> StringToInt64(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a UInt8.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted UInt8 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<UInt8> StringToUInt8(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a UInt16.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted UInt16 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<UInt16> StringToUInt16(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a UInt32.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted UInt32 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<UInt32> StringToUInt32(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a UInt64.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted UInt64 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<UInt64> StringToUInt64(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a Float32.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Float32 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Float32> StringToFloat32(ZStringView _str) noexcept;

/**
 * @brief Converts a string view to a Float64.
 * @param _str The string view to convert.
 * @return A TOptional containing the converted Float64 if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<Float64> StringToFloat64(ZStringView _str) noexcept;

/**
 * @brief Converts an Int8 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Int8ToString(Int8 _num) noexcept;

/**
 * @brief Converts an Int16 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Int16ToString(Int16 _num) noexcept;

/**
 * @brief Converts an Int32 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Int32ToString(Int32 _num) noexcept;

/**
 * @brief Converts an Int64 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Int64ToString(Int64 _num) noexcept;

/**
 * @brief Converts a UInt8 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> UInt8ToString(UInt8 _num) noexcept;

/**
 * @brief Converts a UInt16 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> UInt16ToString(UInt16 _num) noexcept;

/**
 * @brief Converts a UInt32 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> UInt32ToString(UInt32 _num) noexcept;

/**
 * @brief Converts a UInt64 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> UInt64ToString(UInt64 _num) noexcept;

/**
 * @brief Converts a Float32 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Float32ToString(Float32 _num) noexcept;

/**
 * @brief Converts a Float64 number to a ZString.
 * @param _num The number to convert.
 * @return A TOptional containing the resulting ZString if successful, or empty if conversion fails.
 */
CORE_DLLAPI NODISCARD TOptional<ZString> Float64ToString(Float64 _num) noexcept;

/**
 * @brief Generates a formatted ZString.
 *
 * Implementation Principle:
 * Wraps fmt::make_format_args to create a type-safe argument list and forwards it to the
 * internal GenerateString function which handles memory reservation and formatting.
 *
 * @tparam _ArgsType The types of the format arguments.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @return The formatted ZString.
 */
template<typename... _ArgsType>
NODISCARD ZString GenerateString(ZStringView _format, _ArgsType&&... _args) noexcept {
    return internal::GenerateString(_format, fmt::make_format_args(_args...), sizeof...(_args));
}

/**
 * @brief Generates a formatted string into a buffer.
 *
 * Implementation Principle:
 * Uses fmt::vformat_to_n to write directly into the provided buffer, ensuring the maximum
 * length is not exceeded.
 *
 * @tparam _ArgsType The types of the format arguments.
 * @param _str The destination buffer.
 * @param _max_length The maximum length of the buffer.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @return The number of characters written.
 */
template<typename... _ArgsType>
SizeType GenerateString(
    Char* _str, 
    SizeType _max_length, 
    ZStringView _format, 
    _ArgsType&&... _args
) noexcept {
    return internal::GenerateString(_str, _max_length, _format, fmt::make_format_args(_args...));
}

/**
 * @brief Generates a formatted string into a buffer without a null terminator.
 *
 * Implementation Principle:
 * Similar to GenerateString, but does not append a null terminator at the end of the operation,
 * useful for filling parts of a larger buffer.
 *
 * @tparam _ArgsType The types of the format arguments.
 * @param _str The destination buffer.
 * @param _max_length The maximum length of the buffer.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @return The number of characters written.
 */
template<typename... _ArgsType>
SizeType GenerateStringNoEnd(
    Char* _str,
    SizeType _max_length,
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    return internal::GenerateStringNoEnd(_str, _max_length, _format, fmt::make_format_args(_args...));
}

} // string
} // zengine