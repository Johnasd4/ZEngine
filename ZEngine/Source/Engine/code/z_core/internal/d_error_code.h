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
inline constexpr ReturnType kErrorCodeSolutionMask = 0x10000000u;
inline constexpr ReturnType kErrorCodeProjcetMask = 0x100000u;
inline constexpr ReturnType kErrorCodeTypeMask = 0x100u;

namespace error_code {

inline constexpr ReturnType kErrorCodeBase_SZEngine = 0x0u * kErrorCodeSolutionMask;
inline constexpr ReturnType kErrorCodeBase_PCore = 0x0u * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_ZString = 0x0u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZFile = 0x1u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZPrintManager = 0x2u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_MLog = 0x3u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_TUniqueLock = 0x4u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZThreadPool = 0x5u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FMemoryPool = 0x6u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZTask = 0x7u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_FFileSystem = 0x8u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZJson = 0x9u * kErrorCodeTypeMask + kErrorCodeBase_PCore;
inline constexpr ReturnType kErrorCodeBase_ZTimer = 0xAu * kErrorCodeTypeMask + kErrorCodeBase_PCore;

}//error_code
}//zengine