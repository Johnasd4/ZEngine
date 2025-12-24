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

#include "drive.h"

#include "m_log.h"
#include "t_fixed_string.h"
#include "z_file.h"
#include "z_system_time.h"

namespace zengine {
namespace log {

/**
 * @brief Retrieves the singleton instance of the error log file.
 * @return Reference to the error log ZFile instance.
 */
NODISCARD ZFile& ErrorLogFile() noexcept;

/**
 * @brief Retrieves the singleton instance of the info log file.
 * @return Reference to the info log ZFile instance.
 */
NODISCARD ZFile& InfoLogFile() noexcept;

/**
 * @brief Retrieves the singleton instance of the trace log file.
 * @return Reference to the trace log ZFile instance.
 */
NODISCARD ZFile& TraceLogFile() noexcept;

}//log
}//zengine