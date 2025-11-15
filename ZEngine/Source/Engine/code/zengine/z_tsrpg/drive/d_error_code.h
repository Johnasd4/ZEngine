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
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PTSRPG = 0xF0U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_ZRPGObject = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;

inline constexpr ReturnType kErrorCodeBase_ZBoard = 0x10U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZDisplayBoard = 0x11U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicBoard = 0x12U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicBoardViewer = 0x13U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicHexBoard = 0x14U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;

inline constexpr ReturnType kErrorCodeBase_ZTile = 0x20U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicTile = 0x21U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZDisplayTile = 0x22U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicHexTile = 0x23U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;

inline constexpr ReturnType kErrorCodeBase_ZBoardObject = 0x30U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZLogicBoardObject = 0x31U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZDisplayBoardObject = 0x32U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;

inline constexpr ReturnType kErrorCodeBase_FSceneControl = 0x100U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;
inline constexpr ReturnType kErrorCodeBase_ZSceneEvent = 0x101U * kErrorCodeTypeMask + kErrorCodeBase_PTSRPG;

}//error_code
}//zengine