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

#include "../drive.h"

#ifndef CORE_DLLFILE
#define FMT_HEADER_ONLY 
#endif//CORE_DLLFILE

#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/os.h"
#include "fmt/xchar.h"

#include "../z_string_view.h"

//ZStringView
template <>
struct fmt::formatter<zengine::ZStringView> : fmt::formatter<std::string_view> {
    auto format(const zengine::ZStringView& _string_view, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_string_view.STDStringView(), _context);
    }
};

//ZString
template <>
struct fmt::formatter<zengine::ZString> : fmt::formatter<std::string_view> {
    auto format(const zengine::ZString& _string, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(std::string_view(_string.GetDataPtr(), _string.GetSize()), _context);
    }
};

//ZString
template <>
struct fmt::formatter<zengine::ZStringLocal> : fmt::formatter<std::string_view> {
    auto format(const zengine::ZStringLocal& _string, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(std::string_view(_string.GetDataPtr(), _string.GetSize()), _context);
    }
};

//Char*
template <>
struct fmt::formatter<const zengine::Char*> : fmt::formatter<std::string_view> {
    auto format(const zengine::Char* _string, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(std::string_view(_string), _context);
    }
};