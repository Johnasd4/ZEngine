#ifndef Z_CORE_M_ERROR_MESSAGE_H_
#define Z_CORE_M_ERROR_MESSAGE_H_

#include"internal/drive.h"

//Weather console print's the error that happens.
#define USE_CONSOLE_PRINT_ERROR_MESSAGE true

/*
    Return the specified value and output an error message when the condition is true.
*/
#define RETURN(condition, return_value, error_message) \
    if(condition)    \
    {    \
        CONSOLE_PRINT_ERROR_MESSAGE(error_message);    \
        return return_value;    \
    }

namespace zengine {
namespace internal {

/*
    Console error message and error location.
*/
CORE_DLLAPI extern Void ConsolePrintErrorMessage(const CChar* const error_file, const CChar* const error_funcion, 
                                                 const Int32 error_line, const CChar* const error_message) noexcept;

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogErrorMessage(const CChar* const error_file, const CChar* const error_funcion,
                                        const Int32 error_line, const CChar* const error_message) noexcept;

}//internal
}//zengine

/* 
    The macro that controls the console error output.
*/
#if USE_CONSOLE_PRINT_ERROR_MESSAGE
#define CONSOLE_PRINT_ERROR_MESSAGE(error_message)    \
    zengine::internal::ConsolePrintErrorMessage(__FILE__, __func__, __LINE__, error_message);    
#else
#define CONSOLE_PRINT_ERROR_MESSAGE(_errorMessage) ;
#endif // USE_ERROR_CONSOLE_PRINT

#endif // !Z_CORE_M_ERROR_MESSAGE_H_
