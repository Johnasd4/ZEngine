#ifndef Z_CORE_F_CONSOLE_H_
#define Z_CORE_F_CONSOLE_H_

#include"internal/drive.h"

#include"z_mutex.h"

namespace zengine{
namespace console{

using ConsoleOutputColourType = UInt16;

enum ConsoleTextColour : ConsoleOutputColourType {
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

enum ConsoleBackgroundColour : ConsoleOutputColourType {
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
class ZConsoleSettings {
public:

    static ZConsoleSettings& Instance() {
        static ZConsoleSettings instance;
        return instance;
    }

    __forceinline Void set_text_colour(const ConsoleTextColour test_colour) { text_colour_ = test_colour; }
    __forceinline Void set_background_colour(const ConsoleBackgroundColour background_colour) { 
        background_colour_ = background_colour; 
    }

    __forceinline const ConsoleTextColour text_colour() { return text_colour_; }
    __forceinline const ConsoleBackgroundColour background_colour() { return background_colour_; }
    __forceinline ZMutex& console_output_mutex() { return console_output_mutex_; }

private:
    ZConsoleSettings() : text_colour_(ConsoleTextColour::kConsoleTextColourLightWhite)
                       , background_colour_(ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack) {}

    ConsoleTextColour text_colour_;
    ConsoleBackgroundColour background_colour_;
    ZMutex console_output_mutex_;
};

}//internal

CORE_DLLAPI const Void SetConsoleOutputColour(const ConsoleTextColour test_colour,
                                              const ConsoleBackgroundColour background_colour) noexcept;

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
template<typename FormatType, typename... ArgsType>
__forceinline Void Print(FormatType&& format, ArgsType&&... args) {
    internal::ZConsoleSettings::Instance().console_output_mutex().lock();
    printf(std::forward<FormatType>(format), std::forward<ArgsType>(args)...);
    internal::ZConsoleSettings::Instance().console_output_mutex().unlock();
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
template<typename FormatType, typename... ArgsType>
__forceinline Void Print(const ConsoleTextColour text_colour,
                         const ConsoleBackgroundColour background_colour, 
                         FormatType&& format, ArgsType&&... args) {
    internal::ZConsoleSettings::Instance().console_output_mutex().lock();
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), static_cast<UInt16>(text_colour) | static_cast<UInt16>(background_colour));
    printf(std::forward<FormatType>(format), std::forward<ArgsType>(args)...);
    //Changes the console output colour back.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        static_cast<UInt16>(internal::ZConsoleSettings::Instance().text_colour()) |
        static_cast<UInt16>(internal::ZConsoleSettings::Instance().background_colour()));
    internal::ZConsoleSettings::Instance().console_output_mutex().unlock();
}

}//console
}//zengine

#endif // !Z_CORE_INTERNAL_F_CONSOLE_H_