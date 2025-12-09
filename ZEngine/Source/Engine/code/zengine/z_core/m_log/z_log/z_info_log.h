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

/*
    Error log info.
*/
class CORE_DLLAPI ZInfoLog : public ZLog {
public:
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

    static Void FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    static Void ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    ZInfoLog(
        TimeType _log_time, 
        InfoLogTypeEnum _info_type
    ) noexcept;

    virtual ~ZInfoLog() noexcept;

    NODISCARD static Void* operator new(SizeType _size) noexcept;

    NODISCARD static Void operator delete(Void* _memory_ptr) noexcept;

    NODISCARD FORCEINLINE const InfoLogTypeEnum InfoType() const noexcept { return info_type_; }

    NODISCARD static OutputFunctionArray_& OutputFunctionArrayInstance() noexcept;
    NODISCARD virtual OutputFunctionArray_& OutputFunctionArray() noexcept;

protected:
    using SuperType_ = ZLog;

    virtual Void GenerateOutputString(OutputString_* _output_str_ptr) noexcept;

private:
    InfoLogTypeEnum info_type_;
};

}//log
}//zengine