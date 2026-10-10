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

#include "m_log/z_log/f_log_file.h"

namespace zengine {
namespace log {

NODISCARD ZFile& ErrorLogFile() noexcept {
    static ZFile file = std::invoke([]() ->ZFile {
        ZFile file;
        ReturnType link_code = kOK;
        TFixedString<kMaxFileDirLength> file_path;
        const ZSystemTime& system_time = ZSystemTime::GetProcessStartTimeInstance();
        file_path.AssignNoEnd(
            "{}/Error.log",
            ZLog::CreateAndGetLogPath()
        );
        link_code = file.Open(
            file_path.GetDataPtr(),
            ZFile::kWriteOnly | ZFile::kCreate | ZFile::kAppend
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Open() link error!");
        }
        link_code = file.SetOutputBufferSize(kFileOutputBufferSize);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::SetOutputBufferSize() link error!");
        }
        return file;
    });
    return file;
}

NODISCARD ZFile& InfoLogFile() noexcept {
    static ZFile file = std::invoke([]() ->ZFile {
        ZFile file;
        ReturnType link_code = kOK;
        TFixedString<kMaxFileDirLength> file_path;
        const ZSystemTime& system_time = ZSystemTime::GetProcessStartTimeInstance();
        file_path.AssignNoEnd(
            "{}/Info.log",
            ZLog::CreateAndGetLogPath()
        );
        link_code = file.Open(
            file_path.GetDataPtr(),
            ZFile::kWriteOnly | ZFile::kCreate | ZFile::kAppend
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Open() link error!");
        }
        link_code = file.SetOutputBufferSize(kFileOutputBufferSize);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::SetOutputBufferSize() link error!");
        }
        return file;
    });
    return file;
}

NODISCARD ZFile& TraceLogFile() noexcept {
    static ZFile file = std::invoke([]() ->ZFile {
        ZFile file;
        ReturnType link_code = kOK;
        TFixedString<kMaxFileDirLength> file_path;
        const ZSystemTime& system_time = ZSystemTime::GetProcessStartTimeInstance();
        file_path.AssignNoEnd(
            "{}/Trace.log",
            ZLog::CreateAndGetLogPath()
        );
        link_code = file.Open(
            file_path.GetDataPtr(),
            ZFile::kWriteOnly | ZFile::kCreate | ZFile::kAppend
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::Open() link error!");
        }
        link_code = file.SetOutputBufferSize(kFileOutputBufferSize);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCode_LinkError, link_code, "ZFile::SetOutputBufferSize() link error!");
        }
        return file;
    });
    return file;
}

} // log
} // zengine