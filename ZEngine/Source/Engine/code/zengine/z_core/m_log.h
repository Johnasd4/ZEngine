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

#include "f_console.h"
#include "m_log/f_log.h"

#ifndef PROJECT_NAME
#define PROJECT_NAME "Unknown"
#endif

/** 
 * @brief Checks if the specified condition is true. If it is, logs an error and returns the error code. 
 * @note This macro is used to handle error conditions where the function should exit if the condition is met. 
 * @param _condition The boolean condition to check. 
 * @param _err_code The error code to return if the condition is true. 
 * @param ... Additional arguments for the error message format string. 
 */
#define Z_CHECK(_condition, _err_code, ...)\
    if(_condition) {\
        zengine::log::LogError(\
            zengine::Time(),\
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

/** 
 * @brief Logs an error message with detailed context (file, function, line, codes). 
 * @param _err_code The error code associated with the error. 
 * @param _link_code The link code associated with the error. 
 * @param ... The format string and arguments for the log message. 
 */
#define Z_LOG_ERROR(_err_code, _link_code, ...)\
    zengine::log::LogError(\
        zengine::Time(),\
        PROJECT_NAME,\
        __FILE__,\
        __func__,\
        __LINE__,\
        _err_code,\
        _link_code,\
        __VA_ARGS__ \
    );

/** 
 * @brief Logs a trace message with context information. 
 * @param ... The format string and arguments for the trace message. 
 */
#define Z_LOG_TRACE(...)\
    zengine::log::LogTrace(zengine::Time(), PROJECT_NAME, __FILE__, __func__, __LINE__, __VA_ARGS__);

/** 
 * @brief Logs a general informational message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_MESSAGE(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kMessage, __VA_ARGS__);

/** 
 * @brief Logs a start event message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_START(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kStart, __VA_ARGS__);

/** 
 * @brief Logs a process event message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_PROCESS(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kProcess, __VA_ARGS__);

/** 
 * @brief Logs a finish event message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_FINISH(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kFinish, __VA_ARGS__);

/** 
 * @brief Logs a success event message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_SUCCESS(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kSuccess, __VA_ARGS__);

/** 
 * @brief Logs a failure event message. 
 * @param ... The format string and arguments for the message. 
 */
#define Z_LOG_FAILURE(...)\
    zengine::log::LogInfo(zengine::Time(), zengine::log::InfoLogTypeEnum::kFailure, __VA_ARGS__);

#if USE_DEBUG_LOG
/** 
 * @brief Logs a trace message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the trace message. 
 */
#define Z_DEBUG_LOG_TRACE(...)\
    Z_LOG_TRACE(__VA_ARGS__);
#else
/** 
 * @brief Logs a trace message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the trace message. 
 */
#define Z_DEBUG_LOG_TRACE(...)\
    ;
#endif

#if USE_DEBUG_LOG
/** 
 * @brief Logs a general message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_MESSAGE(...)\
    Z_LOG_MESSAGE(__VA_ARGS__);
#else
/** 
 * @brief Logs a general message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_MESSAGE(...)\
    ;
#endif

#if USE_DEBUG_LOG
/** 
 * @brief Logs a start event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_START(...)\
    Z_LOG_START(__VA_ARGS__);
#else
/** 
 * @brief Logs a start event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_START(...)\
    ;
#endif

#ifdef USE_DEBUG_LOG
/** 
 * @brief Logs a process event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_PROCESS(...)\
    Z_LOG_PROCESS(__VA_ARGS__);
#else
/** 
 * @brief Logs a process event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_PROCESS(...)\
    ;
#endif

#if USE_DEBUG_LOG
/** 
 * @brief Logs a finish event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_FINISH(...)\
    Z_LOG_FINISH(__VA_ARGS__);
#else
/** 
 * @brief Logs a finish event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_FINISH(...)\
    ;
#endif

#if USE_DEBUG_LOG
/** 
 * @brief Logs a success event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_SUCCESS(...)\
    Z_LOG_SUCCESS(__VA_ARGS__);
#else
/** 
 * @brief Logs a success event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_SUCCESS(...)\
    ;
#endif

#if USE_DEBUG_LOG
/** 
 * @brief Logs a failure event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_FAILURE(...)\
    Z_LOG_FAILURE(__VA_ARGS__);
#else
/** 
 * @brief Logs a failure event message only if debug logging is enabled (USE_DEBUG_LOG). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_DEBUG_LOG_FAILURE(...)\
    ;
#endif

#if USE_CONSOLE_PRINT
/** 
 * @brief Prints a message to the console if console printing is enabled (USE_CONSOLE_PRINT). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_PRINT(...)\
    zengine::console::Print(__VA_ARGS__);
#else
/** 
 * @brief Prints a message to the console if console printing is enabled (USE_CONSOLE_PRINT). 
 * @param ... The format string and arguments for the message. 
 */
#define Z_PRINT(...)\
    ;
#endif