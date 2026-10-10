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
#include "../../z_string_view.h"

#include "z_log.h"

namespace zengine {
namespace log {

/**
 * @brief Information log info.
 *
 * This class specializes the ZLog base class to handle informational logging events,
 * categorization via InfoLogTypeEnum, and formatting.
 */
class CORE_DLLAPI CACHE_ALIGN ZInfoLog : public ZLog {
public:
    /** @brief Base class alias. */
    using SuperType_ = ZLog;

    /** 
     * @brief Info log head information structure. 
     */
    struct WORD_ALIGN InfoLogHead_ {
        /** @brief The specific type of information log. */
        InfoLogTypeEnum info_type_;
    };

    /** @brief Maximum size for the log string within the info log. */
    static inline constexpr SizeType kLogStringMaxSize =
        static_cast<SizeType>(kLogMaxSize) - sizeof(InfoLogHead_) - sizeof(ZLog);

    /**
     * @brief Structure to hold the raw log message string.
     */
    struct LogString_ {
        /** @brief The current size of the log string. */
        SizeType size_;
        /** @brief Pointer to the fixed-size string buffer for the log message. */
        TFixedString<kLogStringMaxSize - sizeof(SizeType)> string_;
    };

    /**
     * @brief String representation of information log types.
     *
     * A fixed array mapping InfoLogTypeEnum values to their corresponding string representations
     * (e.g., "Message", "Start", "Success").
     */
    static inline constexpr TFixedArray<ZStringView, static_cast<SizeType>(InfoLogTypeEnum::kMax)> kInfoLogType_String = 
    {
        "Message",
        "Start  ",
        "Process",
        "Finish ",
        "Success",
        "Failure"
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
     * @brief Constructs a ZInfoLog object.
     * @param _log_time The time the log occurred.
     * @param _info_type The specific type/category of the information log.
     */
    ZInfoLog(
        TimeType _log_time, 
        InfoLogTypeEnum _info_type
    ) noexcept;

    /** @brief Deleted copy constructor. */
    ZInfoLog(const ZInfoLog&) = delete;

    /** @brief Deleted move constructor. */
    ZInfoLog(ZInfoLog&&) = delete;

    /**
     * @brief Virtual destructor.
     */
    virtual ~ZInfoLog() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZInfoLog& operator=(const ZInfoLog&) = delete;

    /** @brief Deleted move assignment operator. */
    ZInfoLog& operator=(ZInfoLog&&) = delete;

    /**
     * @brief Gets the info log head information.
     * @return const InfoLogHead_& Reference to the info log head structure.
     */
    NODISCARD FORCEINLINE LogString_& LogString() noexcept { return log_string_; }

    /**
     * @brief Gets the specific information type of this log.
     * @return const InfoLogTypeEnum The info type enumeration value.
     */
    NODISCARD FORCEINLINE const InfoLogTypeEnum InfoType() const noexcept { return info_log_head_.info_type_; }

protected:
    /**
     * @brief Generates the formatted output string for the info log.
     * @param _output_str_ptr Pointer to the output string structure to be filled.
     */
    virtual Void GenerateOutputStringP(OutputString_* _output_str_ptr) noexcept;

    /**
     * @brief Retrieves the registered output functions for this specific log type.
     * @return OutputFunctionArray_& Reference to the fixed array of output functions.
     */
    NODISCARD virtual OutputFunctionArray_& OutputFunctionArrayP() noexcept;

private:
    /** @brief The info log head information. */
    InfoLogHead_ info_log_head_;
    /** @brief The log message string. */
    LogString_ log_string_;
};

} // log
} // zengine