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
#ifndef Z_CORE_F_CONSOLE_H_
#define Z_CORE_F_CONSOLE_H_

#include "internal/z_drive.h"

#include "z_mutex.h"

namespace zengine{

namespace error_code {

enum ZPrintManagerErrorCodeEnum : ReturnType {
    kZPrintManagerErrorCodeLinkError = kErrorCodeBaseZPrintManager,
    kZPrintManagerErrorCodeParamOutOfRange,
};

}//error_code

namespace console{

using PrintColourType = UInt16;
using LanguageType = const Char*;

//language chinese
inline constexpr Char kLanguageChinese[] = "zh_CN.UTF-8";

enum PrintTextColourEnum : PrintColourType {
    kPrintTextColourMin = 0x00u,
    kPrintTextColourDarkBlack = 0x00u,
    kPrintTextColourDarkBlue = 0x01u,
    kPrintTextColourDarkGreen = 0x02u,
    kPrintTextColourDarkCyan = 0x03u,
    kPrintTextColourDarkRed = 0x04u,
    kPrintTextColourDarkPurple = 0x05u,
    kPrintTextColourDarkYellow = 0x06u,
    kPrintTextColourDarkWhite = 0x07u,
    kPrintTextColourLightBlack = 0x08u,
    kPrintTextColourLightBlue = 0x09u,
    kPrintTextColourLightGreen = 0x0Au,
    kPrintTextColourLightCyan = 0x0Bu,
    kPrintTextColourLightRed = 0x0Cu,
    kPrintTextColourLightPurple = 0x0Du,
    kPrintTextColourLightYellow = 0x0Eu,
    kPrintTextColourLightWhite = 0x0Fu,
    kPrintTextColourMax = 0x10u,
};

enum PrintBackgroundColourEnum : PrintColourType {
    kPrintBackgroundColourMin = 0x00u,
    kPrintBackgroundColourDarkBlack = 0x00u,
    kPrintBackgroundColourDarkBlue = 0x10u,
    kPrintBackgroundColourDarkGreen = 0x20u,
    kPrintBackgroundColourDarkCyan = 0x30u,
    kPrintBackgroundColourDarkRed = 0x40u,
    kPrintBackgroundColourDarkPurple = 0x50u,
    kPrintBackgroundColourDarkYellow = 0x60u,
    kPrintBackgroundColourDarkWhite = 0x70u,
    kPrintBackgroundColourLightBlack = 0x80u,
    kPrintBackgroundColourLightBlue = 0x90u,
    kPrintBackgroundColourLightGreen = 0xA0u,
    kPrintBackgroundColourLightCyan = 0xB0u,
    kPrintBackgroundColourLightRed = 0xC0u,
    kPrintBackgroundColourLightPurple = 0xD0u,
    kPrintBackgroundColourLightYellow = 0xE0u,
    kPrintBackgroundColourLightWhite = 0xF0u,
    kPrintBackgroundColourMax = 0x10u
};

/*
    Sets language.
*/
CORE_DLLAPI Void SetLanguage(LanguageType _language) noexcept;

/*
    Sets the current print colour.
*/
CORE_DLLAPI Void SetPrintColour(PrintTextColourEnum _text_colour, PrintBackgroundColourEnum _background_colour) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char* _format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char* _format, ArgListType _args) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the _format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* _format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* _format, ArgListType _args) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour, 
    const Char* _format, ...
) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour, 
    const Char* _format, 
    ArgListType _args
) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const WChar* _format, ...
) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const WChar* _format, 
    ArgListType _args
) noexcept;

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintTrace(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourLightWhite, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintMessage(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourDarkWhite, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintStart(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourLightYellow, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintProcess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourDarkYellow, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFinish(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourLightGreen, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintSuccess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourDarkGreen, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFailure(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourDarkRed, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintError(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColourDarkPurple, kPrintBackgroundColourDarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

}//console
}//zengine

#endif // !Z_CORE_F_CONSOLE_H_