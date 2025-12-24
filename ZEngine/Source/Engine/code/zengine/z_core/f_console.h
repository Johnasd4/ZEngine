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

#include "z_string_view.h"

namespace zengine {
namespace console {

/** @brief Enumeration of text styles supported by the console output. */
enum TextStyleEnum : Int32 {
    /** @brief No text style. */
    kTextStyle_None = 0,
    /** @brief Bold text style. */
    kTextStyle_Bold = fmt::emphasis::bold,
    /** @brief Faint text style. */
    kTextStyle_Faint = fmt::emphasis::faint,
    /** @brief Italic text style. */
    kTextStyle_Italic = fmt::emphasis::italic,
    /** @brief Underline text style. */
    kTextStyle_Underline = fmt::emphasis::underline,
    /** @brief Blink text style. */
    kTextStyle_Blink = fmt::emphasis::blink,
    /** @brief Reverse colour text style (swaps foreground and background). */
    kTextStyle_ReverseColour = fmt::emphasis::reverse,
    /** @brief Conceal text style (text is hidden). */
    kTextStyle_Conceal = fmt::emphasis::conceal,
    /** @brief Strikethrough text style. */
    kTextStyle_StrikeThrough = fmt::emphasis::strikethrough
};

/** @brief Constant representing a color that indicates no change to the current terminal color. */
static constexpr Colour kNoChangeColour = Colour(0, 0, 0, 0);

}//console
}//zengine

namespace zengine {
namespace console {
namespace internal {

/** 
 * @brief Internal function to print formatted string output using format arguments. 
 * @param _format The format string view. 
 * @param _args The format arguments. 
 */
CORE_DLLAPI Void Print(
    ZStringView _format, 
    fmt::format_args _args
) noexcept;

/** 
 * @brief Internal function to print formatted string output with specific colors and styles. 
 * @param _front_colour The foreground color of the text. Use kNoChangeColour to use default colour. 
 * @param _back_colour The background color of the text. Use kNoChangeColour to use default colour. 
 * @param _text_style The style attributes of the text. 
 * @param _format The format string view. 
 * @param _args The format arguments. 
 */
CORE_DLLAPI Void Print(
    Colour _front_colour,
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format, 
    fmt::format_args _args
) noexcept;

/** 
 * @brief Internal function to print formatted string output immediately (flushing the buffer). 
 * @param _format The format string view. 
 * @param _args The format arguments. 
 */
CORE_DLLAPI Void PrintImmediately(
    ZStringView _format, 
    fmt::format_args _args
) noexcept;

/** 
 * @brief Internal function to print formatted output immediately with colors and styles. 
 * @param _front_colour The foreground color of the text. Use kNoChangeColour to use default colour. 
 * @param _back_colour The background color of the text. Use kNoChangeColour to use default colour. 
 * @param _text_style The style attributes of the text. 
 * @param _format The format string view. 
 * @param _args The format arguments. 
 */
CORE_DLLAPI Void PrintImmediately(
    Colour _front_colour,
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format,
    fmt::format_args _args
) noexcept;

}//internal
}//console
}//zengine

namespace zengine {
namespace console {

/** 
 * @brief Prints formatted output to the console using a format string and arguments. 
 * @tparam _ArgsType The types of the arguments to be formatted. 
 * @param _format The format string view. 
 * @param _args The variable arguments to be formatted and printed. 
 */
template<typename... _ArgsType>
FORCEINLINE Void Print(
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    internal::Print(
        _format, 
        fmt::make_format_args(_args...)
    );
}

/** 
 * @brief Prints formatted output to the console with specified colors and styles. 
 * @tparam _ArgsType The types of the arguments to be formatted. 
 * @param _front_colour The foreground color of the text. Use kNoChangeColour to use default colour. 
 * @param _back_colour The background color of the text. Use kNoChangeColour to use default colour. 
 * @param _text_style The style attributes of the text. 
 * @param _format The format string view. 
 * @param _args The variable arguments to be formatted and printed. 
 */
template<typename... _ArgsType>
FORCEINLINE Void Print(
    Colour _front_colour, 
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format, 
    _ArgsType&&... _args
) noexcept {
    internal::Print(
        _front_colour,
        _back_colour,
        _text_style,
        _format, 
        fmt::make_format_args(_args...)
    );
}

/** 
 * @brief Prints formatted output to the console immediately, ensuring the buffer is flushed. 
 * @tparam _ArgsType The types of the arguments to be formatted. 
 * @param _format The format string view. 
 * @param _args The variable arguments to be formatted and printed. 
 */
template<typename... _ArgsType>
FORCEINLINE Void PrintImmediately(
    ZStringView _format, 
    _ArgsType&&... _args
) noexcept {
    internal::PrintImmediately(
        _format, 
        fmt::make_format_args(_args...)
    );
}

/** 
 * @brief Prints formatted output immediately with specified colors and styles, flushing the buffer. 
 * @tparam _ArgsType The types of the arguments to be formatted. 
 * @param _front_colour The foreground color of the text. Use kNoChangeColour to use default colour. 
 * @param _back_colour The background color of the text. Use kNoChangeColour to use default colour. 
 * @param _text_style The style attributes of the text. 
 * @param _format The format string view. 
 * @param _args The variable arguments to be formatted and printed. 
 */
template<typename... _ArgsType>
FORCEINLINE Void PrintImmediately(
    Colour _front_colour,
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format,
    _ArgsType&&... _args
) noexcept {
    internal::PrintImmediately(
        _front_colour,
        _back_colour,
        _text_style,
        _format,
        fmt::make_format_args(_args...)
    );
}

/** @brief Flushes the console output buffer. */
CORE_DLLAPI Void Flush() noexcept;

}//console
}//zengine