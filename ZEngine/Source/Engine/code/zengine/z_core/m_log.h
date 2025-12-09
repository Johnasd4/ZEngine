/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "z_system_time.h"

#include "m_log/f_log.h"

#ifndef PROJECT_NAME
#define PROJECT_NAME "Unknown"
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
            __VA_ARGS__ \
        );\
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
        __VA_ARGS__ \
    );

/*
    Log trace.
    2025/10/10-23:28:14 | <Include> main.cpp-main | Trace...
*/
#define Z_LOG_TRACE(...)\
    zengine::log::LogTrace(zengine::TimeSec(), PROJECT_NAME, __FILE__, __func__, __LINE__, __VA_ARGS__);

/*
    Log message.
    2025/10/10-23:28:14 | Message | Message...
*/
#define Z_LOG_MESSAGE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kMessage, __VA_ARGS__);

/*
    Log start.
    2025/10/10-23:28:14 | Start | Start...
*/
#define Z_LOG_START(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kStart, __VA_ARGS__);

/*
    Log process.
    2025/10/10-23:28:14 | Process | Process 1...
*/
#define Z_LOG_PROCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kProcess, __VA_ARGS__);

/*
    Log finish.
    2025/10/10-23:28:14 | Finish | Finish...
*/
#define Z_LOG_FINISH(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kFinish, __VA_ARGS__);

/*
    Log success.
    2025/10/10-23:28:14 | Failure | Failure...
*/
#define Z_LOG_SUCCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kSuccess, __VA_ARGS__);

/*
    Log failure.
    2025/10/10-23:28:14 | Success | Success...
*/
#define Z_LOG_FAILURE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kFailure, __VA_ARGS__);

/*
    Log trace.
    2025/10/10-23:28:14 | <Include> main.cpp-main | Trace...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_TRACE(...)\
    zengine::log::LogTrace(zengine::TimeSec(), PROJECT_NAME, __FILE__, __func__, __LINE__, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_TRACE(...)\
    ;
#endif

/*
    Log message.
    2025/10/10-23:28:14 | Message | Message...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_MESSAGE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kMessage, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_MESSAGE(...)\
    ;
#endif


/*
    Log start.
    2025/10/10-23:28:14 | Start | Start...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_START(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kStart, __VA_ARGS__);
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
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kProcess, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_PROCESS(...)\
    ;
#endif

/*
    Log finish.
    2025/10/10-23:28:14 | Finish | Finish...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_FINISH(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kFinish, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_FINISH(...)\
    ;
#endif

/*
    Log success.
    2025/10/10-23:28:14 | Failure | Failure...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_SUCCESS(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kSuccess, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_SUCCESS(...)\
    ;
#endif

/*
    Log failure.
    2025/10/10-23:28:14 | Success | Success...
*/
#if USE_DEBUG_LOG
#define Z_DEBUG_LOG_FAILURE(...)\
    zengine::log::LogInfo(zengine::TimeSec(), zengine::log::InfoLogTypeEnum::kFailure, __VA_ARGS__);
#else
#define Z_DEBUG_LOG_FAILURE(...)\
    ;
#endif