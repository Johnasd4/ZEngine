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
class CORE_DLLAPI CACHE_LINE_ALIGN ZInfoLog : public ZLog {
public:
    /**
     * @brief String representation of information log types.
     *
     * A fixed array mapping InfoLogTypeEnum values to their corresponding string representations
     * (e.g., "Message", "Start", "Success").
     */
    static constexpr TFixedArray<ZStringView, static_cast<SizeType>(InfoLogTypeEnum::kMax)> kInfoLogType_String =
        TFixedArray<ZStringView, static_cast<SizeType>(InfoLogTypeEnum::kMax)>(
            [](TFixedArray<ZStringView, static_cast<SizeType>(InfoLogTypeEnum::kMax)>* _array_ptr) {
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kMessage)] = "Message";
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kStart)] = "Start  ";
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kProcess)] = "Process";
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kFinish)] = "Finish ";
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kSuccess)] = "Success";
                (*_array_ptr)[static_cast<SizeType>(InfoLogTypeEnum::kFailure)] = "Failure";
            });

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
     * @brief Constructs a ZInfoLog object.
     * @param _log_time The time the log occurred.
     * @param _info_type The specific type/category of the information log.
     */
    ZInfoLog(
        TimeType _log_time, 
        InfoLogTypeEnum _info_type
    ) noexcept;

    /**
     * @brief Virtual destructor.
     */
    virtual ~ZInfoLog() noexcept;

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
     * @brief Gets the specific information type of this log.
     * @return const InfoLogTypeEnum The info type enumeration value.
     */
    NODISCARD FORCEINLINE const InfoLogTypeEnum InfoType() const noexcept { return info_type_; }

    /**
     * @brief Gets the static instance of the output function array.
     * @return OutputFunctionArray_& Reference to the fixed array of output functions.
     */
    NODISCARD static OutputFunctionArray_& OutputFunctionArrayInstance() noexcept;

protected:
    /** @brief Base class alias. */
    using SuperType_ = ZLog;

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
    /** @brief The specific type of information log. */
    InfoLogTypeEnum info_type_;
};

}//log
}//zengine