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

#include "../../drive.h"

#include "../../t_fixed_array.h"
#include "../../t_fixed_string.h"

#include "../c_log.h"

namespace zengine {
namespace log {

/**
 * @brief Base class of the log.
 *
 * This class provides the fundamental structure for logging, including message storage,
 * timestamping, and dispatching formatted logs to output handlers.
 */
class CORE_DLLAPI ZLog : public ZObject<false> {
public:
    /**
     * @brief Structure to hold the raw log message string.
     */
    struct LogString_ {
        /** @brief The current size of the log string. */
        SizeType size_;
        /** @brief Pointer to the fixed-size string buffer for the log message. */
        TFixedString<kLogStringMaxSize>* log_str_ptr_;
    }; 
    /**
     * @brief Structure to hold the formatted output string.
     */
    struct OutputString_ {
        /** @brief The current size of the output string. */
        SizeType size_;
        /** @brief Fixed-size string buffer for the formatted output. */
        TFixedString<kOutputStringMaxSize> output_str_;
    };
    /** @brief Function pointer type definition for log output handlers. */
    using OutputFunction_ = Void(*)(const ZLog*, ZStringView);
    /** @brief Array type definition for storing registered output functions. */
    using OutputFunctionArray_ = TFixedArray<OutputFunction_, kOutputFunctionMaxNum>;

    /**
     * @brief Create and get the log path.
     *
     * Creates the directory structure for logs based on the current system time
     * and returns the generated path.
     * @return ZStringView The path to the log directory.
     */
    NODISCARD static ZStringView CreateAndGetLogPath() noexcept;

    /**
     * @brief Virtual destructor.
     *
     * Cleans up resources, specifically releasing the memory allocated for the log string.
     */
    virtual ~ZLog() noexcept;

    /**
     * @brief Gets the type of the log.
     * @return const LogTypeEnum The log type enumeration value.
     */
    NODISCARD FORCEINLINE const LogTypeEnum LogType() const noexcept { return log_type_; }

    /**
     * @brief Gets the time when the log was created.
     * @return const TimeType& Reference to the time structure.
     */
    NODISCARD FORCEINLINE const TimeType& LogTime() const noexcept { return log_time_; }

    /**
     * @brief Gets the pointer to the internal log string structure.
     * @return LogString_* Pointer to the mutable log string structure.
     */
    NODISCARD FORCEINLINE LogString_* LogStringPtr() noexcept { return &log_str_; }

    /**
     * @brief Formats and outputs the log.
     *
     * Generates the final output string and iterates through all registered output functions
     * to dispatch the log message.
     */
    Void OutputLog() noexcept;

protected:
    /** @brief Base class alias. */
    using SuperType_ = ZObject;

    /**
     * @brief Protected constructor.
     * @param _log_type The type/severity of the log.
     * @param _log_time The timestamp when the log event occurred.
     */
    ZLog(
        LogTypeEnum _log_type,
        const TimeType& _log_time
    ) noexcept;

private:
    /** @brief Deleted copy constructor. */
    ZLog(const ZLog&) = delete;
    /** @brief Deleted move constructor. */
    ZLog(ZLog&&) = delete;
    /** @brief Deleted copy assignment operator. */
    ZLog& operator=(const ZLog&) = delete;
    /** @brief Deleted move assignment operator. */
    ZLog& operator=(ZLog&&) = delete;

    /**
     * @brief Generates the formatted output string.
     *
     * Pure virtual function to be implemented by derived classes to define specific log formatting.
     * @param _output_str_ptr Pointer to the output string structure to be filled.
     */
    virtual Void GenerateOutputStringP(OutputString_* _output_str_ptr) noexcept = 0;

    /**
     * @brief Retrieves the array of output functions.
     *
     * Pure virtual function to be implemented by derived classes to provide specific output handlers.
     * @return OutputFunctionArray_& Reference to the fixed array of output functions.
     */
    NODISCARD virtual OutputFunctionArray_& OutputFunctionArrayP() noexcept = 0;

private:
    /** @brief The type/severity of the log. */
    LogTypeEnum log_type_;
    /** @brief The timestamp of the log. */
    TimeType log_time_;
    /** @brief The raw log message content. */
    LogString_ log_str_;
};

}//log
}//zengine