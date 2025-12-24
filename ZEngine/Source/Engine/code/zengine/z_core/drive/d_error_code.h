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

inline constexpr ReturnType kOK = 0;
inline constexpr ReturnType kErrorCodeSolutionMask = 0x10000000U;
inline constexpr ReturnType kErrorCodeProjcetMask = 0x100000U;
inline constexpr ReturnType kErrorCodeTypeMask = 0x100U;

namespace error_code {

inline constexpr ReturnType kErrorCodeBase_SZEngine = 0x0U * kErrorCodeSolutionMask;
inline constexpr ReturnType kErrorCodeBase_PCore = 0x0U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_FString = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZFile = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_TFixedString = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_MLog = 0x3U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_TUniqueLock = 0x4U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZThreadPool = 0x5U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FMemoryPool = 0x6U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZTask = 0x7U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FFileSystem = 0x8U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZJson = 0x9U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZTimer = 0xAU * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZConfig = 0xBU * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FSystem = 0xCU * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_TFunction = 0xDU * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZSerializer = 0xEU * kErrorCodeTypeMask + kErrorCodeBase_PCore;

}//error_code
}//zengine