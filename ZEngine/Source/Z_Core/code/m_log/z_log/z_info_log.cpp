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
#include "f_log_file.h"
#include "m_log.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

Void ZInfoLog::FileOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    thread_local ZFile& file = InfoLogFile();
    ReturnType link_code = kOK;

    if (file.IsOpen()) {
        link_code = file.Print("{}\n", _output_str);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Print() link error!");
            file.Close();
            return;
        }
    }
}

Void ZInfoLog::ConsoleOutputLog(const ZLog* _log_ptr, ZStringView _output_str) noexcept {
    ZInfoLog& info_log = *(ZInfoLog*)_log_ptr;
    static constexpr Colour kMessageFrontColour = Colour(204, 204, 204, 255);
    static constexpr Colour kStartFrontColour = Colour(255, 255, 0, 255);
    static constexpr Colour kProcessFrontColour = Colour(255, 128, 0, 255);
    static constexpr Colour kFinishFrontColour = Colour(0, 255, 0, 255);
    static constexpr Colour kSuccessFrontColour = Colour(0, 204, 0, 255);
    static constexpr Colour kFailureFrontColour = Colour(204, 0, 0, 255);
    static constexpr Colour kBackColour = console::kNoChangeColour;
    switch (info_log.info_type_) {
    case InfoLogTypeEnum::kMessage:
        console::Print(
            kMessageFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    case InfoLogTypeEnum::kStart:
        console::Print(
            kStartFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    case InfoLogTypeEnum::kProcess:
        console::Print(
            kProcessFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    case InfoLogTypeEnum::kFinish:
        console::Print(
            kFinishFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    case InfoLogTypeEnum::kSuccess:
        console::Print(
            kSuccessFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    case InfoLogTypeEnum::kFailure:
        console::Print(
            kFailureFrontColour,
            kBackColour,
            console::kTextStyle_None,
            "{}\n",
            _output_str
        );
        break;
    }
}

ZInfoLog::ZInfoLog(TimeType _log_time, InfoLogTypeEnum _info_type) noexcept 
    : SuperType_(LogTypeEnum::kInfo, _log_time)
    , info_type_(_info_type) 
{}

ZInfoLog::~ZInfoLog() noexcept {}

NODISCARD Void* ZInfoLog::operator new(SizeType _size) noexcept {
    return memory_pool::ApplyInfoLogMemory();
}

NODISCARD Void ZInfoLog::operator delete(Void* _memory_ptr) noexcept {
    memory_pool::ReleaseInfoLogMemory(_memory_ptr);
}

NODISCARD ZLog::OutputFunctionArray_& ZInfoLog::OutputFunctionArrayInstance() noexcept {
    static OutputFunctionArray_ output_func_array_ptr;
    return output_func_array_ptr;
}

NODISCARD ZLog::OutputFunctionArray_& ZInfoLog::OutputFunctionArrayP() noexcept {
    return OutputFunctionArrayInstance();
}

Void ZInfoLog::GenerateOutputStringP(OutputString_* _output_str_ptr) noexcept {
    static ZSystemTime& system_time = ZSystemTime::Instance();
    system_time.UpdateTimeFast(LogTime());
  
    _output_str_ptr->size_ = _output_str_ptr->output_str_.AssignNoEnd(
        "{:04d}/{:02d}/{:02d}-{:02d}:{:02d}:{:02d}-{:03d}.{:03d}.{:03d} | {} | {}",
        system_time.Year(), system_time.Month(), system_time.Day(),
        system_time.Hour(), system_time.Min(), system_time.Sec(),
        system_time.Ms(), system_time.Us(), system_time.Ns(),
        kInfoLogType_String[static_cast<SizeType>(info_type_)], 
        ZStringView(LogStringPtr()->log_str_ptr_->GetDataPtr(), LogStringPtr()->size_)
    );
}



}//log
}//zengine