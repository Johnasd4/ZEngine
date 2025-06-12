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
namespace tsrpg {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PTSRPG = 0x20000000u;

inline constexpr ReturnType kErrorCodeBase_ZRPGObject = kErrorCodeBase_PTSRPG + 0x000u;

inline constexpr ReturnType kErrorCodeBase_ZTile = kErrorCodeBase_PTSRPG + 0x000u;
inline constexpr ReturnType kErrorCodeBase_ZLogicTile = kErrorCodeBase_PTSRPG + 0x100u;
inline constexpr ReturnType kErrorCodeBase_ZWorldTile = kErrorCodeBase_PTSRPG + 0x200u;
inline constexpr ReturnType kErrorCodeBase_ZBoard = kErrorCodeBase_PTSRPG + 0x300u;
inline constexpr ReturnType kErrorCodeBase_ZWorldBoard = kErrorCodeBase_PTSRPG + 0x400u;
inline constexpr ReturnType kErrorCodeBase_ZLogicBoard = kErrorCodeBase_PTSRPG + 0x500u;
inline constexpr ReturnType kErrorCodeBase_ZBoardObject = kErrorCodeBase_PTSRPG + 0x600u;
inline constexpr ReturnType kErrorCodeBase_ZLogicBoardObject = kErrorCodeBase_PTSRPG + 0x700u;
inline constexpr ReturnType kErrorCodeBase_ZWorldBoardObject = kErrorCodeBase_PTSRPG + 0x800u;

inline constexpr ReturnType kErrorCodeBase_ZHexLogicTile = kErrorCodeBase_PTSRPG + 0x1000u;
inline constexpr ReturnType kErrorCodeBase_ZHexLogicBoard = kErrorCodeBase_PTSRPG + 0x1100u;

inline constexpr ReturnType kErrorCodeBase_FSceneControl = kErrorCodeBase_PTSRPG + 0x2000u;
inline constexpr ReturnType kErrorCodeBase_ZSceneEvent = kErrorCodeBase_PTSRPG + 0x2100u;


}//error_code
}//tsrpg
}//zengine