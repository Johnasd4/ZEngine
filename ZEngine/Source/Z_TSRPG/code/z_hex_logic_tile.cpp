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

#include "z_hex_logic_tile.h"

#include "z_core/m_log.h"
#include "z_math/f_basic_math.h"

#include "z_logic_board.h"

namespace zengine {
namespace tsrpg {

ZHexLogicTile::ZHexLogicTile() noexcept : SuperType_() {}

ZHexLogicTile::~ZHexLogicTile() noexcept {}

NODISCARD UInt64 ZHexLogicTile::Type() const noexcept { return kTileType_LogicTile | kTileType_HexTile; }

Void ZHexLogicTile::Destroy() noexcept {
    SuperType_::Destroy();
}

NODISCARD ReturnType ZHexLogicTile::Initialize(ZBoard* _owner_board_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_owner_board_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZHexLogicTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicTile::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ZTile* ZHexLogicTile::CreateCopy() const noexcept {
    ZHexLogicTile* tile = new ZHexLogicTile();
    return tile;
}

NODISCARD Int32 ZHexLogicTile::CalculateDistanceWithHeight(const ZLogicTile& _tile) const noexcept {
    if (owner_board_ptr_ == _tile.owner_board_ptr_) {
        Int32 dx = pos_.x_ - _tile.pos_.x_;
        Int32 dy = pos_.y_ - _tile.pos_.y_;
        Int32 dz = pos_.z_ - _tile.pos_.z_;
        return (math::Abs(dx) + math::Abs(dx + dy) + math::Abs(dy)) / 2 + math::Abs(dz);
    }
    else {
        //Find the root board
        ZLogicBoard const* root_board_ptr = static_cast<ZLogicBoard const*>(owner_board_ptr_);
        LogicVector3D offset(0, 0, 0);
        if (root_board_ptr->CalculateRelativeBoardLogicPosOffset(root_board_ptr, &offset)) {
            Int32 dx = pos_.x_ - _tile.pos_.x_ + offset.x_;
            Int32 dy = pos_.y_ - _tile.pos_.y_ + offset.y_;
            Int32 dz = pos_.z_ - _tile.pos_.z_ + offset.z_;
            return (math::Abs(dx) + math::Abs(dx + dy) + math::Abs(dy)) / 2 + math::Abs(dz);
        }
        else {
            return kDistanceNotConnet;
        }
    }
}

NODISCARD Int32 ZHexLogicTile::CalculateDistanceWithoutHeight(const ZLogicTile& _tile) const noexcept {
    if (owner_board_ptr_ == _tile.owner_board_ptr_) {
        Int32 dx = pos_.x_ - _tile.pos_.x_;
        Int32 dy = pos_.y_ - _tile.pos_.y_;
        return (math::Abs(dx) + math::Abs(dx + dy) + math::Abs(dy)) / 2;
    }
    else {
        //Find the root board
        ZLogicBoard const* root_board_ptr = static_cast<ZLogicBoard const*>(owner_board_ptr_);
        LogicVector3D offset(0, 0, 0);
        if (root_board_ptr->CalculateRelativeBoardLogicPosOffset(root_board_ptr, &offset)) {
            Int32 dx = pos_.x_ - _tile.pos_.x_ + offset.x_;
            Int32 dy = pos_.y_ - _tile.pos_.y_ + offset.y_;
            return (math::Abs(dx) + math::Abs(dx + dy) + math::Abs(dy)) / 2;
        }
        else {
            return kDistanceNotConnet;
        }
    }
}

}//tsrpg
}//zengine