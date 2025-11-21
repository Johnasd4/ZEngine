/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "f_console.h"
#include "t_fixed_string.h"
#include "z_system_time.h"

#include "log/f_log.h"
#include "log/z_log.h"

#ifndef PROJECT_NAME
#define PROJECT_NAME L"Unknown"
#endif

/*
    Checks the condition, returns if false.
    --------------------------------------------------------------------------------
    Time: 2025/10/10-23:28:14
    Project: Include
    File: main.cpp
    Function: main
    Line: 68
    Error Code: 0x1
    Link Code: 0x2
    Message: Error...
    --------------------------------------------------------------------------------
*/
#define Z_CHECK(_condition, _err_code, ...)\
    if(_condition) {\
        zengine::log::LogError(\
            zengine::TimeSec(),\
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
    --------------------------------------------------------------------------------
    Time: 2025/10/10-23:28:14
    Project: Include
    File: main.cpp
    Function: main
    Line: 68
    Error Code: 0x1
    Link Code: 0x2
    Message: Error...
    --------------------------------------------------------------------------------
*/
#define Z_LOG_ERROR(_err_code, _link_code, ...)\
    zengine::log::LogError(\
        zengine::TimeSec(),\
        PROJECT_NAME,\
        __FILE__,\
        __func__,\
        __LINE__,\
        _err_code,\
        _link_code,\
        __VA_ARGS__);

/*
    Log trace.
    2025/10/10-23:28:14 | <Include> main.cpp-main | Trace...
*/
#define Z_LOG_TRACE(...)\
    zengine::log::LogTrace(zengine::TimeSec(), PROJECT_NAME, __FILE__, __func__, __VA_ARGS__);

/*
    Log message.
    2025/10/10-23:28:14 | Message | Message...
*/
#define Z_LOG_MESSAGE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Message, __VA_ARGS__);

/*
    Log start.
    2025/10/10-23:28:14 | Start | Start...
*/
#define Z_LOG_START(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Start, __VA_ARGS__);

/*
    Log process.
    2025/10/10-23:28:14 | Process | Process 1...
*/
#define Z_LOG_PROCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Process, __VA_ARGS__);

/*
    Log finish.
    2025/10/10-23:28:14 | Finish | Finish...
*/
#define Z_LOG_FINISH(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Finish, __VA_ARGS__);

/*
    Log success.
    2025/10/10-23:28:14 | Failure | Failure...
*/
#define Z_LOG_SUCCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Success, __VA_ARGS__);

/*
    Log failure.
    2025/10/10-23:28:14 | Success | Success...
*/
#define Z_LOG_FAILURE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Failure, __VA_ARGS__);

/*
    Log trace.
    2025/10/10-23:28:14 | <Include> main.cpp-main | Trace...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_TRACE(...)\
    zengine::log::LogTrace(zengine::TimeSec(), PROJECT_NAME, __FILE__, __func__, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_TRACE(...)\
    ;
#endif

/*
    Log message.
    2025/10/10-23:28:14 | Message | Message...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_MESSAGE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Message, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_MESSAGE(...)\
    ;
#endif


/*
    Log start.
    2025/10/10-23:28:14 | Start | Start...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_START(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Start, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_START(...)\
    ;
#endif

/*
    Log process.
    2025/10/10-23:28:14 | Process | Process 1...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_PROCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Process, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_PROCESS(...)\
    ;
#endif

/*
    Log finish.
    2025/10/10-23:28:14 | Finish | Finish...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_FINISH(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Finish, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_FINISH(...)\
    ;
#endif

/*
    Log success.
    2025/10/10-23:28:14 | Failure | Failure...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_SUCCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Success, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_SUCCESS(...)\
    ;
#endif

/*
    Log failure.
    2025/10/10-23:28:14 | Success | Success...
*/
#ifdef USE_DEBUG_LOG
#define Z_DEBUG_LOG_FAILURE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::kInfoLogType_Failure, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_FAILURE(...)\
    ;
#endif

/*
    Outputs the message to the console.
*/
#ifdef USE_CONSOLE_PRINT
#define Z_PRINT(...)\
    zengine::console::Print(__VA_ARGS__);
#else
#define Z_PRINT(...)\
    ;
#endif
