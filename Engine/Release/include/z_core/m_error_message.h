#ifndef Z_CORE_M_ERROR_MESSAGE_H_
#define Z_CORE_M_ERROR_MESSAGE_H_

#include"internal/drive.h"

//Weather console print's the error that happens.
#define USE_ERROR_MESSAGE_CONSOLE_PRINT

/*
    Return the specified value and output an error message when the condition is true.
*/
#define RETURN(condition, return_value, error_message) \
    if(condition)    \
    {    \
        ERR0R_MESSAGE_CONSOLE_PRINT(error_message);    \
        return return_value;    \
    }

namespace zengine {
namespace internal {

/*
    Console error message and error location.
*/
CORE_DLLAPI extern Void ConsoleErrorMessage(const CChar* error_file, const CChar* error_funcion, const Int32 error_line,
    const CChar* error_message) noexcept;

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogErrorMessage(const CChar* error_file, const CChar* error_funcion, const Int32 error_line,
    const CChar* error_message) noexcept;

}//internal
}//zengine

/* 
    The macro that controls the console error output.
*/
#ifdef USE_ERROR_MESSAGE_CONSOLE_PRINT
#define ERR0R_MESSAGE_CONSOLE_PRINT(error_message)    \
        ::zengine::Private::ErrorMessageConsolePrint(__FILE__, __func__, __LINE__, error_message);    
#else
#define ErrorMessageConsolePrint(_errorMessage) ;
#endif // USE_ERROR_CONSOLE_PRINT

#endif // !Z_CORE_M_ERROR_MESSAGE_H_
