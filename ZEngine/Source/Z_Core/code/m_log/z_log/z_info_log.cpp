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

#include "m_log/z_log/z_info_log.h"

#include "f_console.h"
#include "m_log.h"
#include "t_pool.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

Void ZInfoLog::FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    static ZFile& file = std::invoke([]() ->ZFile& {
        static ZFile file;
        ReturnType link_code = kOK;
        TFixedString<kMaxFileDirLength> file_dir;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();
        file_dir.AssignNoEnd(
            "{}\\{:04}{:02}{:02}{:02}{:02}{:02}_Info.log", ZLog::CreateAndGetLogPath(),
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec()
        );
        link_code = file.Open(file_dir.DataPtr(), ZFile::kOpenTypeAppend);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::OpenSafe() link error!");
        }
        return file;
    });
    ReturnType link_code = kOK;

    if (file.IfOpen()) {
        link_code = file.Print("{}\n", _output_str);
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

Void ZInfoLog::ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    ZInfoLog& info_log = *(ZInfoLog*)_log_ptr;
    switch (info_log.info_type_) {
    case InfoLogTypeEnum::kMessage:
        console::Print("{}\n", _output_str);
        break;
    case InfoLogTypeEnum::kStart:
        console::Print("{}\n", _output_str);
        break;
    case InfoLogTypeEnum::kProcess:
        console::Print("{}\n", _output_str);
        break;
    case InfoLogTypeEnum::kFinish:
        console::Print("{}\n", _output_str);
        break;
    case InfoLogTypeEnum::kSuccess:
        console::Print("{}\n", _output_str);
        break;
    case InfoLogTypeEnum::kFailure:
        console::Print("{}\n", _output_str);
        break;
    }
}

ZInfoLog::ZInfoLog(TimeType _log_time, InfoLogTypeEnum _info_type) noexcept 
    : SuperType_(LogTypeEnum::kInfo, _log_time)
    , info_type_(_info_type) 
{}

ZInfoLog::~ZInfoLog() noexcept {}

static TPoolSafe<ZInfoLog, false>& LogPoolInstanceP() noexcept {
    static TPoolSafe<ZInfoLog, false> error_log_pool;
    return error_log_pool;
}

NODISCARD Void* ZInfoLog::operator new(SizeType _size) noexcept {
    static TPoolSafe<ZInfoLog, false>& error_log_pool = LogPoolInstanceP();
    return error_log_pool.Apply();
}

NODISCARD Void ZInfoLog::operator delete(Void* _memory_ptr) noexcept {
    static TPoolSafe<ZInfoLog, false>& error_log_pool = LogPoolInstanceP();
    error_log_pool.Release(reinterpret_cast<ZInfoLog*>(_memory_ptr));
}

NODISCARD ZLog::OutputFunctionArray_& ZInfoLog::OutputFunctionArrayInstance() noexcept {
    static OutputFunctionArray_ output_func_array_ptr;
    return output_func_array_ptr;
}

NODISCARD ZLog::OutputFunctionArray_& ZInfoLog::OutputFunctionArray() noexcept {
    return OutputFunctionArrayInstance();
}

Void ZInfoLog::GenerateOutputString(OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    system_time.UpdateTimeFast(LogTime());
  
    _output_str_ptr->size_ = _output_str_ptr->output_str_.AssignNoEnd(
        "{:04d}/{:02d}/{:02d}-{:02d}:{:02d}:{:02d} | {} | {}",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        kInfoLogType_String[static_cast<SizeType>(info_type_)], 
        ZStringView(LogStringPtr()->log_str_ptr_->DataPtr(), LogStringPtr()->size_)
    );
}



}//log
}//zengine