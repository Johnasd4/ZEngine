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

using RPGObjectType = UInt64;

enum BoardTypeEnum : RPGObjectType {
    kBoardType_LogicBoard = 1 << 0,
    kBoardType_DisplayBoard = 1 << 1,
    kBoardType_HexBoard = 1 << 2,
    kBoardType_SquareBoard = 1 << 3
};

enum TileTypeEnum : RPGObjectType {
    kTileType_LogicTile = 1 << 0,
    kTileType_DisplayTile = 1 << 1,
    kTileType_HexTile = 1 << 2,
    kTileType_SquareTile = 1 << 3
};

enum BoardObjectTypeEnum : RPGObjectType {
    kBoardObjectType_LogicBoardObject = 1 << 0,
    kBoardObjectType_DisplayBoardObject = 1 << 1
};

//the config path
inline constexpr WChar kTSRPGConfigPathDir[] = L".\\config\\tsrpg_config.json";

}//tsrpg
}//zengine