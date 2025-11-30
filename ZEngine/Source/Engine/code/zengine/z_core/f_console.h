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

enum class PrintTextColourEnum : PrintColourType {
    kDarkBlack = 0x00U,
    kDarkBlue = 0x01U,
    kDarkGreen = 0x02U,
    kDarkCyan = 0x03U,
    kDarkRed = 0x04U,
    kDarkPurple = 0x05U,
    kDarkYellow = 0x06U,
    kDarkWhite = 0x07U,
    kLightBlack = 0x08U,
    kLightBlue = 0x09U,
    kLightGreen = 0x0AU,
    kLightCyan = 0x0BU,
    kLightRed = 0x0CU,
    kLightPurple = 0x0DU,
    kLightYellow = 0x0EU,
    kLightWhite = 0x0FU
};

enum class PrintBackgroundColourEnum : PrintColourType {
    kDarkBlack = 0x00U,
    kDarkBlue = 0x10U,
    kDarkGreen = 0x20U,
    kDarkCyan = 0x30U,
    kDarkRed = 0x40U,
    kDarkPurple = 0x50U,
    kDarkYellow = 0x60U,
    kDarkWhite = 0x70U,
    kLightBlack = 0x80U,
    kLightBlue = 0x90U,
    kLightGreen = 0xA0U,
    kLightCyan = 0xB0U,
    kLightRed = 0xC0U,
    kLightPurple = 0xD0U,
    kLightYellow = 0xE0U,
    kLightWhite = 0xF0U
};

/*
    Sets the current print colour.
*/
CORE_DLLAPI Void SetPrintColour(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour
) noexcept;

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
    Print(
        PrintTextColourEnum::kLightWhite, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintMessage(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kDarkWhite, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintStart(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kLightYellow, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintProcess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kDarkYellow, 
        PrintBackgroundColourEnum::kDarkBlack,
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFinish(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kLightGreen,
        PrintBackgroundColourEnum::kDarkBlack,
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintSuccess(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kDarkGreen, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintFailure(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kDarkRed, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format, 
        std::forward<_ArgsType>(_args)...
    );
}

template<typename _CharType, typename... _ArgsType>
FORCEINLINE Void PrintError(const _CharType* _format, _ArgsType&&... _args) noexcept {
    Print(
        PrintTextColourEnum::kDarkPurple, 
        PrintBackgroundColourEnum::kDarkBlack, 
        _format,
        std::forward<_ArgsType>(_args)...
    );
}

}//console
}//zengine