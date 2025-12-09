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
#include "m_log.h"
#include "t_pool.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

Void ZErrorLog::FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    static constexpr ZStringView log_head_end =
        "--------------------------------------------------------------------------------\n";
    static ZFile file = std::invoke([]() ->ZFile {
        ZFile file;
        ReturnType link_code = kOK;
        TFixedString<kMaxFileDirLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.AssignNoEnd(
            "{}\\{:04}{:02}{:02}{:02}{:02}{:02}_Error.log",
            ZLog::CreateAndGetLogPath(),
            system_time.Year(),
            system_time.Month(),
            system_time.Day(),
            system_time.Hour(),
            system_time.Min(),
            system_time.Sec()
        );
        link_code = file.Open(file_dir.DataPtr(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Open() link error!");
        }
        return file;
    });
    ReturnType link_code = kOK;

    if (file.IfOpen()) {
        link_code = file.Print(
            "{0}{1}\n{0}", 
            log_head_end, 
            _output_str
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Print() link error!");
            link_code = file.Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Close() link error!");
            }
            return;
        }
    }
}

Void ZErrorLog::ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    static constexpr ZStringView log_head_end =
        "--------------------------------------------------------------------------------\n";
    console::Print(
        "{0}{1}\n{0}", 
        log_head_end, 
        _output_str
    );
}

ZErrorLog::ZErrorLog(
    TimeType _log_time,
    ZStringView _proj_name,
    ZStringView _file_dir,
    ZStringView _func_name,
    Int32 _err_line,
    ReturnType _err_code,
    ReturnType _link_code
) noexcept 
    : SuperType_(LogTypeEnum::kError, _log_time)
    , proj_name_(_proj_name)
    , file_dir_(_file_dir)
    , func_name_(_func_name)
    , err_line_(_err_line)
    , err_code_(_err_code)
    , link_code_(_link_code)
{}

ZErrorLog::~ZErrorLog() noexcept {}

static TPoolSafe<ZErrorLog, false>& LogPoolInstanceP() noexcept {
    static TPoolSafe<ZErrorLog, false> error_log_pool;
    return error_log_pool;
}

NODISCARD Void* ZErrorLog::operator new(SizeType _size) noexcept {
    static TPoolSafe<ZErrorLog, false>& error_log_pool = LogPoolInstanceP();
    return error_log_pool.Apply();
}

NODISCARD Void ZErrorLog::operator delete(Void* _memory_ptr) noexcept {
    static TPoolSafe<ZErrorLog, false>& error_log_pool = LogPoolInstanceP();
    error_log_pool.Release(reinterpret_cast<ZErrorLog*>(_memory_ptr));
}
NODISCARD ZLog::OutputFunctionArray_& ZErrorLog::OutputFunctionArrayInstance() noexcept {
    static OutputFunctionArray_ output_func_array_ptr;
    return output_func_array_ptr;
}

NODISCARD ZLog::OutputFunctionArray_& ZErrorLog::OutputFunctionArray() noexcept {
    return OutputFunctionArrayInstance();
}

Void ZErrorLog::GenerateOutputString(OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    system_time.UpdateTimeFast(LogTime());
    _output_str_ptr->size_ = _output_str_ptr->output_str_.AssignNoEnd(
        "Time: {:04d}/{:02d}/{:02d}-{:02d}:{:02d}:{:02d}\n"
        "Project: {}\n"
        "File: {}\n"
        "Function: {}\n"
        "Line: {}\n"
        "Error Code: 0x{:x}\n"
        "Link Code: 0x{:x}\n"
        "Message: {}",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        proj_name_, 
        file_dir_,
        func_name_, 
        err_line_,
        err_code_, 
        link_code_, 
        ZStringView(LogStringPtr()->log_str_ptr_->DataPtr(), LogStringPtr()->size_)
    );
}

}//log
}//zengine