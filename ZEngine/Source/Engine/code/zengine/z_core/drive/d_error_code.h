/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

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

inline constexpr ReturnType kErrorCodeBase_ZString = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZFile = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FConsole = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PCore;
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
inline constexpr ReturnType kErrorCodeBase_ZStringView = 0xEU * kErrorCodeTypeMask + kErrorCodeBase_PCore;


}//error_code
}//zengine