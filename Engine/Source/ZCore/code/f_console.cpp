#define CORE_DLLFILE

#include"f_console.h"

namespace zengine {
namespace console {

CORE_DLLAPI const Void SetConsoleOutputColour(const ConsoleTextColour test_colour,
                                              const ConsoleBackgroundColour background_colour) noexcept {
    internal::ZConsoleSettings::Instance().set_text_colour(test_colour);
    internal::ZConsoleSettings::Instance().set_background_colour(background_colour);
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        (ConsoleOutputColourType)test_colour | (ConsoleOutputColourType)background_colour);    
}

}//console
}//zengine
