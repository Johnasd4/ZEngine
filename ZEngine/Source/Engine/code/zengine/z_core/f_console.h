/*
Copyright (c) YuLin Zhu
code
Code
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

#include "library/l_fmt.h"

#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {

/** @brief Enumerates error codes for the console module, inheriting from ReturnType. */
enum FConsoleErrorCodeEnum : ReturnType {
    /** @brief Error code indicating a linking error within the console module. */
    kFConsoleErrorCode_LinkError = kErrorCodeBase_FConsole,
    /** @brief Error code indicating a system-level error occurred. */
    kFConsoleErrorCode_SystemError,
    /** @brief Error code indicating a null pointer was passed as a parameter. */
    kFConsoleErrorCode_NullptrParam,
    /** @brief Error code indicating a parameter is out of the valid range. */
    kFConsoleErrorCode_ParamOutOfRange,
    /** @brief Error code indicating a format error occurred during string generation. */
    kFConsoleErrorCode_FormatError
};

}//error_code
}//zengine

namespace zengine {
namespace console {
namespace internal {

/**
 * @brief Internal function to print formatted string output using format arguments.
 * @param _format The format string view.
 * @param _args The format arguments.
 * @param _arg_num The number of arguments.
 */
CORE_DLLAPI Void PrintP(ZStringView _format, fmt::format_args _args, SizeType _arg_num) noexcept;

/**
 * @brief Internal function to print formatted wide string output using wide format arguments.
 * @param _format The wide format string view.
 * @param _args The wide format arguments.
 * @param _arg_num The number of arguments.
 */
CORE_DLLAPI Void PrintP(ZWStringView _format, fmt::wformat_args _args, SizeType _arg_num) noexcept;

}//internal
}//console
}//zengine

namespace zengine {
namespace console {

/**
 * @brief Prints a ZStringView to the console.
 * @param _str_view The string view content to be printed.
 */
CORE_DLLAPI Void Print(ZStringView _str_view) noexcept;

/**
 * @brief Prints a ZWStringView (wide string view) to the console.
 * @param _str_view The wide string view content to be printed.
 */
CORE_DLLAPI Void Print(ZWStringView _str_view) noexcept;

/**
 * @brief Prints formatted output to the console using a format string and arguments.
 * @tparam _ArgsType The types of the arguments to be formatted.
 * @param _format The format string view.
 * @param _args The variable arguments to be formatted and printed.
 */
template<typename... _ArgsType>
Void Print(ZStringView _format, _ArgsType&&... _args) noexcept {
    internal::PrintP(_format, fmt::make_format_args(_args...), sizeof...(_args));
}

/**
 * @brief Prints formatted output to the console using a wide format string and arguments.
 * @tparam _ArgsType The types of the arguments to be formatted.
 * @param _format The wide format string view.
 * @param _args The variable arguments to be formatted and printed.
 */
template<typename... _ArgsType>
Void Print(ZWStringView _format, _ArgsType&&... _args) noexcept {
    internal::PrintP(_format, fmt::make_wformat_args(_args...), sizeof...(_args));
}

}//console
}//zengine