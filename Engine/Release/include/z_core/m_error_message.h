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
#ifndef Z_CORE_M_ERROR_MESSAGE_H_
#define Z_CORE_M_ERROR_MESSAGE_H_

#include "internal/drive.h"

#ifdef _DEBUG
//If console print's return error .
#define USE_CONSOLE_PRINT_RETRUN_ERROR_MESSAGE true
//If log's return error .
#define USE_LOG_RETRUN_ERROR_MESSAGE true
//If console print's debug error .
#define USE_CONSOLE_PRINT_DEBUG_ERROR_MESSAGE true
//If log's debug error .
#define USE_LOG_DEBUG_ERROR_MESSAGE true
#else
//If console print's return error .
#define USE_CONSOLE_PRINT_RETRUN_ERROR_MESSAGE false
//If log's return error .
#define USE_LOG_RETRUN_ERROR_MESSAGE false
//If console print's debug error .
#define USE_CONSOLE_PRINT_DEBUG_ERROR_MESSAGE false
//If log's debug error .
#define USE_LOG_DEBUG_ERROR_MESSAGE false
#endif

/*
    Return the specified value and output an error message when the condition is true.
*/
#define RETURN(condition, return_value, error_message)\
    if(condition) {\
        CONSOLE_PRINT_RETRUN_ERROR_MESSAGE(error_message);\
        LOG_RETRUN_ERROR_MESSAGE(error_message);\
        return return_value;\
    }

/*
    Ends the program and output the error message..
*/
#define DEBUG(condition, error_message)\
    if(condition) {\
        CONSOLE_PRINT_DEBUG_ERROR_MESSAGE(error_message);\
        LOG_DEBUG_ERROR_MESSAGE(error_message);\
        exit(EXIT_FAILURE);\
    }

namespace zengine {
namespace internal {

/*
    Console error message and error location.
*/
CORE_DLLAPI extern Void ConsolePrintErrorMessage(const CChar* error_title,
                                                 const CChar* error_file, 
                                                 const CChar* error_funcion,
                                                 Int32 error_line, 
                                                 const CChar* error_message) noexcept;

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogErrorMessage(const CChar* error_title,
                                        const CChar* error_file, 
                                        const CChar* error_funcion,
                                        Int32 error_line, 
                                        const CChar* error_message) noexcept;

}//internal
}//zengine

namespace zengine {
namespace internal {

constexpr CChar kReturnErrorMessageErrorTitle[] = "RETURN ERROR";
constexpr CChar kDebugErrorMessageErrorTitle[] = "DEBUG ERROR";

}//internal
}//zengine

#if USE_CONSOLE_PRINT_RETRUN_ERROR_MESSAGE
#define CONSOLE_PRINT_RETRUN_ERROR_MESSAGE(error_message)\
    zengine::internal::ConsolePrintErrorMessage(zengine::internal::kReturnErrorMessageErrorTitle,\
                                                __FILE__, __func__, __LINE__, error_message);    
#else
#define CONSOLE_PRINT_RETRUN_ERROR_MESSAGE(_errorMessage) ;
#endif // USE_CONSOLE_PRINT_RETRUN_ERROR_MESSAGE

#if USE_LOG_RETRUN_ERROR_MESSAGE
#define LOG_RETRUN_ERROR_MESSAGE(error_message)\
    zengine::internal::LogErrorMessage(zengine::internal::kReturnErrorMessageErrorTitle,\
                                       __FILE__, __func__, __LINE__, error_message);    
#else
#define LOG_RETRUN_ERROR_MESSAGE(_errorMessage) ;
#endif // USE_LOG_RETRUN_ERROR_MESSAGE

#if USE_CONSOLE_PRINT_DEBUG_ERROR_MESSAGE
#define CONSOLE_PRINT_DEBUG_ERROR_MESSAGE(error_message)\
    zengine::internal::ConsolePrintErrorMessage(zengine::internal::kDebugErrorMessageErrorTitle,\
                                                __FILE__, __func__, __LINE__, error_message);    
#else
#define CONSOLE_PRINT_DEBUG_ERROR_MESSAGE(_errorMessage) ;
#endif // USE_ERROR_CONSOLE_PRINT

#if USE_LOG_DEBUG_ERROR_MESSAGE
#define LOG_DEBUG_ERROR_MESSAGE(error_message)\
    zengine::internal::LogErrorMessage(zengine::internal::kDebugErrorMessageErrorTitle,\
                                       __FILE__, __func__, __LINE__, error_message);    
#else
#define LOG_DEBUG_ERROR_MESSAGE(_errorMessage) ;
#endif // LOG_DEBUG_ERROR_MESSAGE


#endif // !Z_CORE_M_ERROR_MESSAGE_H_
