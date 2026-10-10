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

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PGui = 0x2U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_ZApplication = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZWindow = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZFrame = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZText = 0x3U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZInputText = 0x4U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZButton = 0x5U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZSeparator = 0x6U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZCheckBox = 0x7U * kErrorCodeTypeMask + kErrorCodeBase_PGui;

} // error_code
} // zengine