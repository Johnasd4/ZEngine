#define CORE_DLLFILE

#include "m_error_message.h"

#include "f_console_output.h"

namespace zengine {
namespace internal {

CORE_DLLAPI Void ConsolePrintErrorMessage(const CChar* error_title, 
                                          const CChar* error_file, 
                                          const CChar* error_funcion,
                                          Int32 error_line, 
                                          const CChar* error_message) noexcept {
    console::Print(
        console::ConsoleOutputTextColour::kConsoleTextColourLightRed, 
        console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "\n***** %s *****\nFile£º%s\nFunction£º%s\nLine£º%d\nMessage£º%s\n***** %s *****\n", 
        error_title, error_file, error_funcion, error_line, error_message, error_title);
}

CORE_DLLAPI Void LogErrorMessage(const CChar* error_title,
                                 const CChar* error_file,
                                 const CChar* error_funcion,
                                 Int32 error_line,
                                 const CChar* error_message) noexcept {
    static ZMutex log_mutex;
    log_mutex.Lock();
    //TODO(Johnasd4):The log output.

    log_mutex.Unlock();
}

}//internal
}//zengine