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
#define TSRPG_DLLFILE
#include "drive/d_pch.h"

#include "z_logic_hex_board.h"

#include "z_logic_hex_tile.h"

namespace zengine {
namespace tsrpg {

ZLogicHexBoard::ZLogicHexBoard() noexcept : SuperType_() {}

ZLogicHexBoard::~ZLogicHexBoard() noexcept {}

NODISCARD UInt64 ZLogicHexBoard::Type() const noexcept {
    return kBoardType_LogicBoard | kBoardType_HexBoard; 
}

Void ZLogicHexBoard::Destroy() noexcept {
    SuperType_::Destroy();
}

NODISCARD ReturnType ZLogicHexBoard::Initialize(const LogicVector2D& _board_size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_board_size);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicHexBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoard::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ZLogicTile* ZLogicHexBoard::CreateLogicTileP() const noexcept {
    ZLogicTile* logic_tile_ptr = new ZLogicHexTile();
    return logic_tile_ptr;
}

}//tsrpg
}//zengine