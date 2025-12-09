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

/*
    Base class of the log.
*/
class CORE_DLLAPI ZLog : public ZObject {
public:
    struct LogString_ {
        SizeType size_;
        TFixedString<kLogStringMaxSize>* log_str_ptr_;
    }; 
    struct OutputString_ {
        SizeType size_;
        TFixedString<kOutputStringMaxSize> output_str_;
    };
    using OutputFunction_ = Void(*)(const ZLog*, ZStringView);
    using OutputFunctionArray_ = TFixedArray<OutputFunction_, kOutputFunctionMaxNum>;

    /*
        Create and get the log path.
    */
    NODISCARD static ZStringView CreateAndGetLogPath() noexcept;

    virtual ~ZLog() noexcept;

    NODISCARD FORCEINLINE const LogTypeEnum LogType() const noexcept { return log_type_; }
    NODISCARD FORCEINLINE const TimeType& LogTime() const noexcept { return log_time_; }
    NODISCARD FORCEINLINE LogString_* LogStringPtr() noexcept { return &log_str_; }

    Void OutputLog() noexcept;

    NODISCARD virtual OutputFunctionArray_& OutputFunctionArray() noexcept = 0;

protected:
    using SuperType_ = ZObject;

    ZLog(
        LogTypeEnum _log_type,
        const TimeType& _log_time
    ) noexcept;

private:
    ZLog(const ZLog&) = delete;
    ZLog(ZLog&&) = delete;
    ZLog& operator=(const ZLog&) = delete;
    ZLog& operator=(ZLog&&) = delete;

    virtual Void GenerateOutputString(OutputString_* _output_str_ptr) noexcept = 0;

private:
    LogTypeEnum log_type_;
    TimeType log_time_;
    LogString_ log_str_;
};

}//log
}//zengine