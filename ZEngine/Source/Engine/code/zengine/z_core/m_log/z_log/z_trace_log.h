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

/*
    Error log info.
*/
class CORE_DLLAPI ZTraceLog : public ZLog {
public:
    static Void FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    static Void ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept;

    ZTraceLog(
        TimeType _log_time, 
        ZStringView _proj_name,
        ZStringView _file_dir,
        ZStringView _func_name,
        Int32 _trace_line
    ) noexcept;

    virtual ~ZTraceLog() noexcept;

    NODISCARD static Void* operator new(SizeType _size) noexcept;

    NODISCARD static Void operator delete(Void* _memory_ptr) noexcept;

    NODISCARD static OutputFunctionArray_& OutputFunctionArrayInstance() noexcept;
    NODISCARD virtual OutputFunctionArray_& OutputFunctionArray() noexcept;

protected:
    using SuperType_ = ZLog;

private:
    ZTraceLog(const ZTraceLog&) = delete;
    ZTraceLog(ZTraceLog&&) = delete;
    ZTraceLog& operator=(const ZTraceLog&) = delete;
    ZTraceLog& operator=(ZTraceLog&&) = delete;

    virtual Void GenerateOutputString(OutputString_* _output_str_ptr) noexcept;

private:
    ZStringView proj_name_;
    ZStringView file_dir_;
    ZStringView func_name_;
    Int32 trace_line_;
};

}//log
}//zengine