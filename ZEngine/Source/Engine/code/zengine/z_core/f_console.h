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

namespace zengine {
namespace error_code {
enum FConsoleErrorCodeEnum : ReturnType {
    kFConsoleErrorCode_LinkError = kErrorCodeBase_FConsole,
    kFConsoleErrorCode_SystemError,
    kFConsoleErrorCode_NullptrParam,
    kFConsoleErrorCode_ParamOutOfRange
};
}//error_code
}//zengine

namespace zengine {
namespace console {

using PrintColourType = UInt16;

enum PrintTextColourEnum : PrintColourType {
    kPrintTextColour_Min = 0x00U,
    kPrintTextColour_DarkBlack = 0x00U,
    kPrintTextColour_DarkBlue = 0x01U,
    kPrintTextColour_DarkGreen = 0x02U,
    kPrintTextColour_DarkCyan = 0x03U,
    kPrintTextColour_DarkRed = 0x04U,
    kPrintTextColour_DarkPurple = 0x05U,
    kPrintTextColour_DarkYellow = 0x06U,
    kPrintTextColour_DarkWhite = 0x07U,
    kPrintTextColour_LightBlack = 0x08U,
    kPrintTextColour_LightBlue = 0x09U,
    kPrintTextColour_LightGreen = 0x0AU,
    kPrintTextColour_LightCyan = 0x0BU,
    kPrintTextColour_LightRed = 0x0CU,
    kPrintTextColour_LightPurple = 0x0DU,
    kPrintTextColour_LightYellow = 0x0EU,
    kPrintTextColour_LightWhite = 0x0FU,
    kPrintTextColour_Max = 0x10U,
};

enum PrintBackgroundColourEnum : PrintColourType {
    kPrintBackgroundColour_Min = 0x00U,
    kPrintBackgroundColour_DarkBlack = 0x00U,
    kPrintBackgroundColour_DarkBlue = 0x10U,
    kPrintBackgroundColour_DarkGreen = 0x20U,
    kPrintBackgroundColour_DarkCyan = 0x30U,
    kPrintBackgroundColour_DarkRed = 0x40U,
    kPrintBackgroundColour_DarkPurple = 0x50U,
    kPrintBackgroundColour_DarkYellow = 0x60U,
    kPrintBackgroundColour_DarkWhite = 0x70U,
    kPrintBackgroundColour_LightBlack = 0x80U,
    kPrintBackgroundColour_LightBlue = 0x90U,
    kPrintBackgroundColour_LightGreen = 0xA0U,
    kPrintBackgroundColour_LightCyan = 0xB0U,
    kPrintBackgroundColour_LightRed = 0xC0U,
    kPrintBackgroundColour_LightPurple = 0xD0U,
    kPrintBackgroundColour_LightYellow = 0xE0U,
    kPrintBackgroundColour_LightWhite = 0xF0U,
    kPrintBackgroundColour_Max = 0x10u
};

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