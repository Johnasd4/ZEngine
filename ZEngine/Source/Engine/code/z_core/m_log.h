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

#include "t_fixed_string.h"
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

#ifndef PROJECT_NAME
#define PROJECT_NAME L"Unknown"
#endif

/*
    Checks the condition, returns if false.
*/
#define Z_CHECK(_condition, _err_code, ...)\
    if(_condition) {\
        zengine::log::LogError(\
            ::time(nullptr),\
            PROJECT_NAME,\
            __FILE__,\
            __func__,\
            __LINE__,\
            _err_code,\
            0,\
            __VA_ARGS__);\
        return _err_code;\
    }

/*
    Log error.
*/
#define Z_LOG_ERROR(_err_code, _link_code, ...)\
    zengine::log::LogError(\
        ::time(nullptr),\
        PROJECT_NAME,\
        __FILE__,\
        __func__,\
        __LINE__,\
        _err_code,\
        _link_code,\
        __VA_ARGS__);

/*
    Log trace.
*/
#define Z_LOG_TRACE(...)\
    zengine::log::LogTrace(::time(nullptr), PROJECT_NAME, __FILE__, __func__, __VA_ARGS__);

/*
    Log message.
*/
#define Z_LOG_MESSAGE(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoMessage, __VA_ARGS__);

/*
    Log start.
*/
#define Z_LOG_START(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoStart, __VA_ARGS__);

/*
    Log process.
*/
#define Z_LOG_PROCESS(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoProcess, __VA_ARGS__);

/*
    Log finish.
*/
#define Z_LOG_FINISH(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoFinish, __VA_ARGS__);

/*
    Log success.
*/
#define Z_LOG_SUCCESS(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoSuccess, __VA_ARGS__);

/*
    Log failure.
*/
#define Z_LOG_FAILURE(...)\
    zengine::log::LogInfo(::time(nullptr), kLogInfoFailure, __VA_ARGS__);

namespace zengine {

namespace error_code {

enum MLogErrorCode : ReturnType {
    kMLogErrorCodeLinkError = kErrorCodeBaseMLog,
    kMLogErrorCodeLogQueueOverflow,
    kMLogErrorCodePortIDOutOfRange,
    kMLogErrorCodeLogPortOutputFunctionFull,
    kMLogErrorCodeLogPortOutputFunctionAlreadyRegistered,
    kMLogErrorCodeLogPortInputFunctionAlreadyRegistered,
    kMLogErrorCodeLogPortInputFunctionUnregisteredFailed,
    kMLogErrorCodeLogPortFull
};

}//error_code

enum LogInfoEnum : IndexType {
    kLogInfoMin = 0,
    kLogInfoMessage = kLogInfoMin,
    kLogInfoStart,
    kLogInfoProcess,
    kLogInfoFinish,
    kLogInfoSuccess,
    kLogInfoFailure,
    kLogInfoMax
};

namespace log {

/*
    Base class of the log.
*/
class ZLog : public ZObject {
public:
    //max size of the log message string.
    static constexpr Int32 kMsgMaxSize = 512;
    //max size of the output log string.
    static constexpr Int32 kLogMaxSize = 2048;
    //max size of the log message string.
    static constexpr Int32 kLogFileMaxNum = 10;
    //the root path of the log files
    static constexpr WChar kLogFileRootPathDir[] = L".\\log";

    using MsgString_ = FixedStringUnion<kMsgMaxSize>;
    using OutputString_ = FixedStringUnion<kLogMaxSize>;

    /*
        Create and get the log path.
    */
    CORE_DLLAPI static const WChar* CreateAndGetLogPath() noexcept;

    /*
        Override it to output different formats, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void GenerateLogString(const ZLog* _log_ptr, OutputString_* _output_str_ptr) noexcept;

    /*
        Default console output log string, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void FileOutputLogString(const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    /*
        Default file output log string, uses WString(wchar_t).
    */
    CORE_DLLAPI static Void ConsoleOutputLogString(
            const ZLog* _log_ptr, const ZLog::OutputString_& _output_str) noexcept;

    CORE_DLLAPI ZLog() noexcept;
    CORE_DLLAPI ZLog(const Char* _format, ...) noexcept;
    CORE_DLLAPI ZLog(const Char* _format, ArgListType _args) noexcept;
    CORE_DLLAPI ZLog(const WChar* _format, ...) noexcept;
    CORE_DLLAPI ZLog(const WChar* _format, ArgListType _args) noexcept;

protected:
    using SuperType_ = ZObject;

    NODISCARD FORCEINLINE const MsgString_& LogMsgPtr() const noexcept { return log_msg_str_; }
private:
    MsgString_ log_msg_str_;
};

/*
    Log error message and error location.
*/
CORE_DLLAPI Void LogError(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code,
    const WChar* _format,
    ...
) noexcept;

/*
    Log trace message and trace location.
*/
CORE_DLLAPI Void LogTrace(
    TimeType _raw_time,
    const WChar* _proj_name,
    const Char* _file_dir,
    const Char* _func_name,
    const WChar* _format,
    ...
) noexcept;

/*
    Log info message.
*/
CORE_DLLAPI Void LogInfo(
    TimeType _raw_time,
    LogInfoEnum _info_type,
    const WChar* _format,
    ...
) noexcept;


/*
    Register the log server port input function, the function will be called when log happens.
    Each port can have 1 input function and 8 output function.
    Port -1(max port num - 1) is error log, 2 output function used. 
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port 0~5 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType RegisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept;

/*
    Removes the log server port output function.
    Port -1 is error log, -2 is trace log, port 0~5 is not used.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port 0~5 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType UnregisterLogServerInputFunction(
    IndexType _port_id, 
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept;

/*
    Register the log server port output function, the function will be called when log happens.
    Port -1 is error log, -2 is trace log, port 0~5 is not used.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port 0~5 is not used.
*/
CORE_DLLAPI NODISCARD ReturnType RegisterLogServerOutputFunction(
    IndexType _port_id, 
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept;

/*
    Removes the log server port output function.
    Port -1 is error log, -2 is trace log, port 0~5 is not used.
    Port -1(max port num - 1) is error log, 2 output function used.
    Port -2(max port num - 2) is trace log, 2 output function used.
    Port 0~5 is not used.
*/
CORE_DLLAPI Void UnregisterLogServerOutputFunction(
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept;

}//log
}//zengine

#endif // !Z_CORE_M_LOG_H_
