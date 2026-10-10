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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "m_log/z_log/z_error_log.h"

#include "f_console.h"
#include "f_log_file.h"
#include "m_log.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

Void ZErrorLog::FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    static constexpr ZStringView kLogHeadEnd =
        "--------------------------------------------------------------------------------\n";
    static ZFile& file = ErrorLogFile();
    ReturnType link_code = kOK;

    if (file.IsOpen()) {
        link_code = file.Print(
            "{0}{1}\n{0}", 
            kLogHeadEnd, 
            _output_str
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Print() link error!");
            file.Close();
            return;
        }
    }
}

Void ZErrorLog::ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    static constexpr ZStringView kLogHeadEnd =
        "--------------------------------------------------------------------------------\n";
    static constexpr Colour kFrontColour = Colour(204, 0, 204, 255);
    static constexpr Colour kBackColour = console::kNoChangeColour;
    console::Print(
        kFrontColour,
        kBackColour,
        console::kTextStyle_None,
        "{0}{1}\n{0}", 
        kLogHeadEnd, 
        _output_str
    );
}

NODISCARD ZLog::OutputFunctionArray_& ZErrorLog::GetOutputFunctionArrayInstance() noexcept {
    static OutputFunctionArray_ output_func_array_ptr;
    return output_func_array_ptr;
}

NODISCARD Void* ZErrorLog::operator new(SizeType _size) noexcept {
    return memory_pool::ApplyErrorLogMemory();
}

NODISCARD Void ZErrorLog::operator delete(Void* _memory_ptr) noexcept {
    memory_pool::ReleaseErrorLogMemory(_memory_ptr);
}

ZErrorLog::ZErrorLog(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_path,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code
) noexcept 
    : SuperType_(LogTypeEnum::kError, _log_time)
    , error_log_head_(_proj_name, _file_path, _func_name, _err_line, _err_code, _link_code)
{}

NODISCARD ZLog::OutputFunctionArray_& ZErrorLog::OutputFunctionArrayP() noexcept {
    return GetOutputFunctionArrayInstance();
}

Void ZErrorLog::GenerateOutputStringP(OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::GetInstance();
    system_time.UpdateTimeFast(LogTime());
    _output_str_ptr->size_ = _output_str_ptr->output_str_.AssignNoEnd(
        "TimeNs: {:04d}/{:02d}/{:02d}-{:02d}:{:02d}:{:02d}-{:03d}.{:03d}.{:03d}\n"
        "Project: {}\n"
        "File: {}\n"
        "Function: {}\n"
        "Line: {}\n"
        "Error Code: 0x{:x}\n"
        "Link Code: 0x{:x}\n"
        "Message: {}",
        system_time.GetYear(), system_time.GetMonth(), system_time.GetDay(),
        system_time.GetHour(), system_time.GetMin(), system_time.GetSec(),
        system_time.GetMs(), system_time.GetUs(), system_time.GetNs(),
        error_log_head_.proj_name_,
        error_log_head_.file_path_,
        error_log_head_.func_name_,
        error_log_head_.err_line_,
        error_log_head_.err_code_,
        error_log_head_.link_code_,
        ZStringView(log_string_.string_.GetDataPtr(), log_string_.size_)
    );
}

} // log
} // zengine