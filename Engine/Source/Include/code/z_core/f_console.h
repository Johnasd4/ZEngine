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
    Sets the current print colour.
*/
CORE_DLLAPI Void SetPrintColour(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const CChar* format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const CChar* format, ArgListType args) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const TChar* format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const TChar* format, ArgListType args) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const CChar* format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const CChar* format, ArgListType args) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const TChar* format, ...) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const TChar* format, ArgListType args) noexcept;

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintMessage(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourDarkWhite, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintStart(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourLightYellow, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintProcess(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourDarkYellow, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintFinish(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourLightGreen, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintSuccess(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourDarkGreen, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintFailure(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourDarkRed, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

template<typename CharType, typename... ArgsType>
FORCEINLINE Void PrintError(const CharType* format, ArgsType&&... args) noexcept {
    Print(kPrintTextColourDarkPurple, kPrintBackgroundColourDarkBlack, format, std::forward<ArgsType>(args)...);
}

}//console
}//zengine

#endif // !Z_CORE_F_CONSOLE_H_