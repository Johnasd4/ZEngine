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

#include "internal/z_drive.h"

#include "z_system_time.h"

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

enum LogMessageType : IndexType {
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
CORE_DLLAPI extern Void LogError(TimeType raw_time,
                                 const CChar* err_file, 
                                 const CChar* err_func,
                                 Int32 err_line, 
                                 ReturnType err_code,
                                 ReturnType link_code,
                                 const CChar* format,
                                 ...) noexcept;

}//internal
}//zengine

/*
    Checks the condition, returns if true.
*/
#define Z_CHECK(condition, err_code, format, ...)\
    if(condition) {\
        zengine::internal::LogError(time(nullptr), __FILE__, __func__, __LINE__, err_code, 0, format, __VA_ARGS__);\
        return err_code;\
    }

/*
    Log error.
*/
#define Z_LOG_ERROR(err_code, link_code, format, ...)\
    zengine::internal::LogError(time(nullptr), __FILE__, __func__, __LINE__, err_code, link_code, format, __VA_ARGS__);

#endif // !Z_CORE_M_LOG_H_
