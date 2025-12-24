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

#include "../drive.h"

#include "../z_string_view.h"

namespace zengine {
namespace error_code {

/** @brief Enumeration of error codes specific to the logging module. */
enum MLogErrorCodeEnum : ReturnType {
    /** @brief Base error code for logging link errors. */
    kMLogErrorCode_LinkError = kErrorCodeBase_MLog,
    /** @brief Error indicating that the log queue has overflowed. */
    kMLogErrorCode_LogQueueOverflow,
    /** @brief Error indicating that the specified port ID is out of the valid range. */
    kMLogErrorCode_PortIDOutOfRange,
    /** @brief Error indicating that the output function registry for the log port is full. */
    kMLogErrorCode_LogPortOutputFunctionFull,
    /** @brief Error indicating that the output function is already registered to the log port. */
    kMLogErrorCode_LogPortOutputFunctionAlreadyRegistered,
    /** @brief Error indicating that the input function is already registered to the log port. */
    kMLogErrorCode_LogPortInputFunctionAlreadyRegistered,
    /** @brief Error indicating a failure to unregister the input function from the log port. */
    kMLogErrorCode_LogPortInputFunctionUnregisteredFailed,
    /** @brief Error indicating that the maximum number of log ports has been reached. */
    kMLogErrorCode_LogPortFull
};

}//error_code
}//zengine

namespace zengine {
namespace log {

/** @brief Enumeration representing the different levels or types of logs. */
enum class LogTypeEnum : SizeType {
    /** @brief Minimum value for log type iteration. */
    kMin = 0,
    /** @brief Log type representing an error condition. */
    kError = kMin,
    /** @brief Log type representing a trace or debug message. */
    kTrace,
    /** @brief Log type representing general information. */
    kInfo,
    /** @brief Maximum value for log type iteration. */
    kMax
};

/** @brief Enumeration representing specific categories for info-level logs. */
enum class InfoLogTypeEnum : SizeType {
    /** @brief Minimum value for info log type iteration. */
    kMin = 0,
    /** @brief Represents a standard message log. */
    kMessage = kMin,
    /** @brief Represents the start of a process or operation. */
    kStart,
    /** @brief Represents an ongoing process or operation. */
    kProcess,
    /** @brief Represents the conclusion of a process or operation. */
    kFinish,
    /** @brief Represents a successful operation. */
    kSuccess,
    /** @brief Represents a failed operation. */
    kFailure,
    /** @brief Maximum value for info log type iteration. */
    kMax
};

/** @brief The maximum number of supported log ports. */
inline constexpr UInt32 kLogMaxPortNum = 8U;

/** @brief The number of log ports currently defined for use (Error, Trace, Info). */
inline constexpr UInt32 kLogUsedPortNum = 3U;

/** @brief The maximum number of output functions that can be registered per port. */
inline constexpr UInt32 kOutputFunctionMaxNum = 8U;

/** @brief The port ID assigned for error logging. */
inline constexpr UInt32 kErrorLogPortID = kLogMaxPortNum - 1U;

/** @brief The port ID assigned for trace logging. */
inline constexpr UInt32 kTraceLogPortID = kLogMaxPortNum - 2U;

/** @brief The port ID assigned for info logging. */
inline constexpr UInt32 kInfoLogPortID = kLogMaxPortNum - 3U;

/** @brief The root directory path where log files are stored. */
inline constexpr ZStringView kLogFileRootPathDir = "Log";

/** @brief The maximum size of a single log message string (4096 - 8 bytes for memory block header). */
inline constexpr UInt32 kLogStringMaxSize = 4096U;
/** @brief The size to extend the log string pool when more memory is needed. */
inline constexpr UInt32 kLogStringPoolExtendSize = 512U * kKB;

/** @brief The maximum size of the final formatted output log string. */
inline constexpr UInt32 kOutputStringMaxSize = 8192U;

/** @brief The size of log file output buffer size(65536 - 16 bytes for memory block header). */
inline constexpr UInt32 kFileOutputBufferSize = 65520U;

/** @brief The maximum number of log files to retain before rotation or overwriting. */
inline constexpr UInt32 kLogFileMaxNum = 10;

}//log
}//zengine