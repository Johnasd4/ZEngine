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
#ifndef Z_CORE_M_LOG_H_
#define Z_CORE_M_LOG_H_

#include "internal/drive.h"

#include <ctime>

#ifdef _DEBUG
//If console log.
#define USE_CONSOLE_LOG true
//If file log.
#define USE_FILE_LOG true
#else
//If console log.
#define USE_CONSOLE_LOG false
//If file log.
#define USE_FILE_LOG true
#endif

namespace zengine {

enum LogMessageType : Int32 {
    kLogMessage,
    kLogStart,
    kLogProcess,
    kLogFinish,
    kLogSuccess,
    kLogFailure
};

namespace internal {

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogError(const time_t time_raw, 
                                 const CChar* error_file, 
                                 const CChar* error_funcion,
                                 Int32 error_line, 
                                 const CChar* format,
                                 ...) noexcept;

}//internal
}//zengine

/*
    Return the specified value and output an error message when the condition is true.
*/
#define Z_RETURN(condition, return_value, format, ...)\
    if(condition) {\
        zengine::internal::LogError(clock(), __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);\
        return return_value;\
    }

#endif // !Z_CORE_M_LOG_H_
