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

#include "m_log/z_log/z_log.h"

#include "f_file_system.h"
#include "f_memory_pool.h"
#include "m_log.h"
#include "z_file.h"

namespace zengine {
namespace log {

NODISCARD ZStringView ZLog::CreateAndGetLogPath() noexcept {
    // log file path.
    static ZStringView path_str = std::invoke([]() -> ZStringView {
        static TFixedString<kMaxFileDirLength> log_path_str;
        const ZSystemTime& system_time = ZSystemTime::GetProcessStartTimeInstance();
        TFixedString<kMaxFileDirLength> log_root_path_str;
        TFixedString<kMaxFileDirLength> execute_path_str;

        SizeType execute_path_str_len = kMaxFileDirLength;
        ReturnType link_code = file_system::GetExecuteDirectoryPath(
            execute_path_str.GetDataPtr(), 
            &execute_path_str_len
        );
        if(link_code != kOK) {
            return ZStringView();
        }

        SizeType log_root_path_str_len = log_root_path_str.AssignNoEnd(
            "{}/{}",
            ZStringView(execute_path_str.GetDataPtr(), execute_path_str_len),
            kLogFileRootPathDir
        );

        SizeType log_path_str_len = log_path_str.AssignNoEnd(
            "{}/{:04}{:02}{:02}{:02}{:02}{:02}",
            ZStringView(log_root_path_str.GetDataPtr(), log_root_path_str_len),
            system_time.GetYear(),
            system_time.GetMonth(),
            system_time.GetDay(),
            system_time.GetHour(),
            system_time.GetMin(),
            system_time.GetSec()
        );

        // create log path.
        link_code = file_system::CreateDirectoryByPath(
            ZStringView(log_path_str.GetDataPtr(), log_path_str_len)
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code, 
                "file_system::CreateDirectoryByPath() link error!"
            );
            ASSERT(false, "file_system::CreateDirectoryByPath() link error!");
            Exit(error_code::kMLogErrorCode_LinkError);
        }

        // clear the expired log files.
        auto result = file_system::GetDirectoriesByPath(kLogFileRootPathDir);
        if (!result) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code,
                "file_system::GetDirectoriesByPath() link error!"
            );
            ASSERT(false, "file_system::GetDirectoriesByPath() link error!");
            Exit(error_code::kMLogErrorCode_LinkError);
        }
        while (result->GetSize() > log::kLogFileMaxNum) {
            file_system::DeleteDirectoryByPath(result->Front());
            result->PopFront();
        }

        return ZStringView(log_path_str.GetDataPtr(), log_path_str_len);
    });
    return path_str;
}

Void ZLog::OutputLog() noexcept {
    static OutputString_ output_string;
    OutputFunctionArray_& output_function_array = OutputFunctionArrayP();
    GenerateOutputStringP(&output_string);
    SizeType index = 0ULL;
    while (output_function_array[index]) {
        output_function_array[index](this, ZStringView(output_string.output_str_.GetDataPtr(), output_string.size_));
        ++index;
    }
}

ZLog::ZLog(
    LogTypeEnum _log_type,
    const TimeType& _log_time
) noexcept
    : SuperType_()
    , log_head_(_log_type, _log_time)
{}

} // log
} // zengine