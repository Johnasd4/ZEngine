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
#include "../../z_string_view.h"

#include "z_log.h"

namespace zengine {
namespace log {

/**
 * @brief Error log info.
 *
 * This class specializes the ZLog base class to handle error-specific logging information,
 * including project name, file path, function name, line number, and error codes.
 */
class CORE_DLLAPI CACHE_ALIGN ZErrorLog : public ZLog {
public:
    /** @brief Base class alias. */
    using SuperType_ = ZLog;

    /**
     * @brief Error log head information structure.
     */
    struct WORD_ALIGN ErrorLogHead_ {
        /** @brief The name of the project. */
        ZStringView proj_name_;
        /** @brief The file path. */
        ZStringView file_path_;
        /** @brief The function name. */
        ZStringView func_name_;
        /** @brief The line number of the error. */
        Int32 err_line_;
        /** @brief The error code. */
        ReturnType err_code_;
        /** @brief The link code or secondary error code. */
        ReturnType link_code_;
    };

    /** @brief Maximum size for the log string within the error log. */
    static inline constexpr SizeType kLogStringMaxSize =
        static_cast<SizeType>(kLogMaxSize) - sizeof(ErrorLogHead_) - sizeof(ZLog);

    /**
     * @brief Structure to hold the raw log message string.
     */
    struct LogString_ {
        /** @brief The current size of the log string. */
        SizeType size_;
        /** @brief Pointer to the fixed-size string buffer for the log message. */
        TFixedString<kLogStringMaxSize - sizeof(SizeType)> string_;
    };

public:
    /**
     * @brief Outputs the log content to a file.
     * @param _log_ptr Pointer to the log object.
     * @param _output_str The formatted log string to output.
     */
    static Void FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    /**
     * @brief Outputs the log content to the console.
     * @param _log_ptr Pointer to the log object.
     * @param _output_str The formatted log string to output.
     */
    static Void ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    /**
     * @brief Gets the static instance of the output function array.
     * @return OutputFunctionArray_& Reference to the fixed array of output functions.
     */
    NODISCARD static OutputFunctionArray_& GetOutputFunctionArrayInstance() noexcept;

    /**
     * @brief Custom memory allocation operator.
     * @param _size The size of memory to allocate.
     * @return Void* Pointer to the allocated memory.
     */
    NODISCARD static Void* operator new(SizeType _size) noexcept;

    /**
     * @brief Custom memory deallocation operator.
     * @param _memory_ptr Pointer to the memory to deallocate.
     */
    NODISCARD static Void operator delete(Void* _memory_ptr) noexcept;

    /**
     * @brief Constructs an ZErrorLog object.
     * @param _log_time The time the log occurred.
     * @param _proj_name The name of the project where the error occurred.
     * @param _file_path The file path where the error occurred.
     * @param _func_name The name of the function where the error occurred.
     * @param _err_line The line number where the error occurred.
     * @param _err_code The specific error code.
     * @param _link_code The associated link or secondary error code.
     */
    ZErrorLog(
        TimeType _log_time,
        ZStringView _proj_name,
        ZStringView _file_path,
        ZStringView _func_name,
        Int32 _err_line,
        ReturnType _err_code,
        ReturnType _link_code
    ) noexcept;

    /** @brief Deleted copy constructor. */
    ZErrorLog(const ZErrorLog&) = delete;

    /** @brief Deleted move constructor. */
    ZErrorLog(ZErrorLog&&) = delete;

    /**
     * @brief Virtual destructor.
     */
    virtual ~ZErrorLog() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZErrorLog& operator=(const ZErrorLog&) = delete;

    /** @brief Deleted move assignment operator. */
    ZErrorLog& operator=(ZErrorLog&&) = delete;

    /**
     * @brief Gets the error log head information.
     * @return const ErrorLogHead_& Reference to the error log head structure.
     */
    NODISCARD FORCEINLINE LogString_& LogString() noexcept { return log_string_; }

private:
    /**
     * @brief Generates the formatted output string for the error log.
     * @param _output_str_ptr Pointer to the output string structure to be filled.
     */
    virtual Void GenerateOutputStringP(OutputString_* _output_str_ptr) noexcept;

    /**
     * @brief Retrieves the registered output functions for this specific log type.
     * @return OutputFunctionArray_& Reference to the fixed array of output functions.
     */
    NODISCARD virtual OutputFunctionArray_& OutputFunctionArrayP() noexcept;

private:
    /** @brief The error log head information. */
    ErrorLogHead_ error_log_head_;
    /** @brief The log message string. */
    LogString_ log_string_;
};

} // log
} // zengine