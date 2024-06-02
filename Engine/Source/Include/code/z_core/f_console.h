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

#include "internal/drive.h"

#include "z_mutex.h"

namespace zengine{
namespace console{

using ConsoleOutputColourType = UInt16;

enum ConsoleOutputTextColourType : ConsoleOutputColourType {
    kConsoleTextColourDarkBlack = 0x00u,
    kConsoleTextColourDarkBlue = 0x01u,
    kConsoleTextColourDarkGreen = 0x02u,
    kConsoleTextColourDarkCyan = 0x03u,
    kConsoleTextColourDarkRed = 0x04u,
    kConsoleTextColourDarkPurple = 0x05u,
    kConsoleTextColourDarkYellow = 0x06u,
    kConsoleTextColourDarkWhite = 0x07u,
    kConsoleTextColourLightBlack = 0x08u,
    kConsoleTextColourLightBlue = 0x09u,
    kConsoleTextColourLightGreen = 0x0Au,
    kConsoleTextColourLightCyan = 0x0Bu,
    kConsoleTextColourLightRed = 0x0Cu,
    kConsoleTextColourLightPurple = 0x0Du,
    kConsoleTextColourLightYellow = 0x0Eu,
    kConsoleTextColourLightWhite = 0x0Fu
};

enum ConsoleOutputBackgroundColourType : ConsoleOutputColourType {
    kConsoleBackgroundColourDarkBlack = 0x00u,
    kConsoleBackgroundColourDarkBlue = 0x10u,
    kConsoleBackgroundColourDarkGreen = 0x20u,
    kConsoleBackgroundColourDarkCyan = 0x30u,
    kConsoleBackgroundColourDarkRed = 0x40u,
    kConsoleBackgroundColourDarkPurple = 0x50u,
    kConsoleBackgroundColourDarkYellow = 0x60u,
    kConsoleBackgroundColourDarkWhite = 0x70u,
    kConsoleBackgroundColourLightBlack = 0x80u,
    kConsoleBackgroundColourLightBlue = 0x90u,
    kConsoleBackgroundColourLightGreen = 0xA0u,
    kConsoleBackgroundColourLightCyan = 0xB0u,
    kConsoleBackgroundColourLightRed = 0xC0u,
    kConsoleBackgroundColourLightPurple = 0xD0u,
    kConsoleBackgroundColourLightYellow = 0xE0u,
    kConsoleBackgroundColourLightWhite = 0xF0u
};

namespace internal {

/*
    Singleton class that contains the console settings.
*/
class ZConsoleOutputSettings {
private:
    static constexpr ConsoleOutputTextColourType kDefaultTextColour = kConsoleTextColourLightWhite;
    static constexpr ConsoleOutputBackgroundColourType kDefaultBackgroundColour = kConsoleBackgroundColourDarkBlack;

public:

    NODISCARD static ZConsoleOutputSettings& InstanceP() {
        static ZConsoleOutputSettings instance;
        return instance;
    }

    FORCEINLINE Void SetTextColour(ConsoleOutputTextColourType test_colour) { text_colour_ = test_colour; }
    FORCEINLINE Void SetBackgroundColour(ConsoleOutputBackgroundColourType background_colour) { 
        background_colour_ = background_colour; 
    }

    NODISCARD FORCEINLINE ConsoleOutputTextColourType TextColour() const { return text_colour_; }
    NODISCARD FORCEINLINE ConsoleOutputBackgroundColourType BackgroundColour() const { return background_colour_; }
    NODISCARD FORCEINLINE ZMutex& ConsoleOutputMutex() { return console_output_mutex_; }

private:
    ZConsoleOutputSettings() : text_colour_(kDefaultTextColour), background_colour_(kDefaultBackgroundColour) {}

    ConsoleOutputTextColourType text_colour_;
    ConsoleOutputBackgroundColourType background_colour_;
    ZMutex console_output_mutex_;
};

}//internal

CORE_DLLAPI extern Void SetConsoleOutputColour(ConsoleOutputTextColourType test_colour,
                                               ConsoleOutputBackgroundColourType background_colour) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
template<typename CharType, typename... ArgsType>
Void Print(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    settings.ConsoleOutputMutex().Unlock();
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
template<typename CharType, typename... ArgsType>
Void Print(ConsoleOutputTextColourType text_colour, ConsoleOutputBackgroundColourType background_colour, 
           const CharType* format, ArgsType&&... args) noexcept{
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(text_colour) | static_cast<UInt16>(background_colour));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintMessage(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourDarkWhite) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintStart(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourLightYellow) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintProcess(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourDarkYellow) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintFinish(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourLightGreen) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintSuccess(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourDarkGreen) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintFailure(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourDarkRed) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf(format, std::forward<ArgsType>(args)...);
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(format, std::forward<ArgsType>(args)...);
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

template<typename CharType, typename... ArgsType>
Void PrintError(const CharType* format, ArgsType&&... args) noexcept {
    static internal::ZConsoleOutputSettings& settings = internal::ZConsoleOutputSettings::InstanceP();
    settings.ConsoleOutputMutex().Lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(kConsoleTextColourDarkPurple) |
                                         static_cast<UInt16>(kConsoleBackgroundColourDarkBlack));
    if constexpr (kSameType<CharType, CChar>) {
        printf("\n--------------------------------------------------------------------------------\n");
        printf(format, std::forward<ArgsType>(args)...);
        printf("--------------------------------------------------------------------------------\n");
    }
    else if constexpr (kSameType<CharType, TChar>) {
        wprintf(L"\n--------------------------------------------------------------------------------\n");
        wprintf(format, std::forward<ArgsType>(args)...);
        wprintf(L"--------------------------------------------------------------------------------\n");
    }
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        static_cast<UInt16>(settings.TextColour()) |
        static_cast<UInt16>(settings.BackgroundColour()));
    settings.ConsoleOutputMutex().Unlock();
}

}//console
}//zengine

#endif // !Z_CORE_F_CONSOLE_H_