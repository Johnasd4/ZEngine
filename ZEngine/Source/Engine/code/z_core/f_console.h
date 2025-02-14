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
    kZPrintManagerErrorCodeLinkError = kErrorCodeBase_ZPrintManager,
    kZPrintManagerErrorCodeParamOutOfRange,
};

}//error_code

namespace console{

using PrintColourType = UInt16;
using LanguageType = const Char*;

//language chinese
inline constexpr Char kLanguageChinese[] = "zh_CN.UTF-8";

enum PrintTextColourEnum : PrintColourType {
    kPrintTextColour_Min = 0x00u,
    kPrintTextColour_DarkBlack = 0x00u,
    kPrintTextColour_DarkBlue = 0x01u,
    kPrintTextColour_DarkGreen = 0x02u,
    kPrintTextColour_DarkCyan = 0x03u,
    kPrintTextColour_DarkRed = 0x04u,
    kPrintTextColour_DarkPurple = 0x05u,
    kPrintTextColour_DarkYellow = 0x06u,
    kPrintTextColour_DarkWhite = 0x07u,
    kPrintTextColour_LightBlack = 0x08u,
    kPrintTextColour_LightBlue = 0x09u,
    kPrintTextColour_LightGreen = 0x0Au,
    kPrintTextColour_LightCyan = 0x0Bu,
    kPrintTextColour_LightRed = 0x0Cu,
    kPrintTextColour_LightPurple = 0x0Du,
    kPrintTextColour_LightYellow = 0x0Eu,
    kPrintTextColour_LightWhite = 0x0Fu,
    kPrintTextColour_Max = 0x10u,
};

enum PrintBackgroundColourEnum : PrintColourType {
    kPrintBackgroundColour_Min = 0x00u,
    kPrintBackgroundColour_DarkBlack = 0x00u,
    kPrintBackgroundColour_DarkBlue = 0x10u,
    kPrintBackgroundColour_DarkGreen = 0x20u,
    kPrintBackgroundColour_DarkCyan = 0x30u,
    kPrintBackgroundColour_DarkRed = 0x40u,
    kPrintBackgroundColour_DarkPurple = 0x50u,
    kPrintBackgroundColour_DarkYellow = 0x60u,
    kPrintBackgroundColour_DarkWhite = 0x70u,
    kPrintBackgroundColour_LightBlack = 0x80u,
    kPrintBackgroundColour_LightBlue = 0x90u,
    kPrintBackgroundColour_LightGreen = 0xA0u,
    kPrintBackgroundColour_LightCyan = 0xB0u,
    kPrintBackgroundColour_LightRed = 0xC0u,
    kPrintBackgroundColour_LightPurple = 0xD0u,
    kPrintBackgroundColour_LightYellow = 0xE0u,
    kPrintBackgroundColour_LightWhite = 0xF0u,
    kPrintBackgroundColour_Max = 0x10u
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
    Print(kPrintTextColour_LightWhite, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintMessage(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_DarkWhite, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintStart(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_LightYellow, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintProcess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_DarkYellow, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFinish(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_LightGreen, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintSuccess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_DarkGreen, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFailure(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_DarkRed, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintError(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(kPrintTextColour_DarkPurple, kPrintBackgroundColour_DarkBlack, _format, std::forward<_ArgsType>(_args)...);
}

}//console
}//zengine

#endif // !Z_CORE_F_CONSOLE_H_