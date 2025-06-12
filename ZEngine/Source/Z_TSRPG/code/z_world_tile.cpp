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

#include "z_world_tile.h"

#include "z_core/m_log.h"

#include "z_world_board.h"
#include "z_logic_tile.h"

namespace zengine {
namespace tsrpg {

ZWorldTile::ZWorldTile() noexcept
    : SuperType_()
    , pos_()
    , index_()
{}

ZWorldTile::~ZWorldTile() noexcept {}

NODISCARD ReturnType ZWorldTile::SetPos(const WorldVector3D& _pos) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    if (pos_ != _pos) {
        WorldVector3D old_pos = pos_;
        pos_ = _pos;
        OnPosChanged(old_pos, _pos);
    }
    return ret_val;
}

NODISCARD ReturnType ZWorldTile::SetPos(const LogicVector3D& _pos) noexcept {
    ZWorldBoard* owner_board_ptr = static_cast<ZWorldBoard*>(OwnerBoardPtr());
    return SetPos(owner_board_ptr->CalculateWorldVectorByLogicVector(_pos));
}

NODISCARD UInt64 ZWorldTile::Type() const noexcept { return kTileType_WorldTile; }

Void ZWorldTile::Destroy() noexcept {
    SuperType_::Destroy();
}

ReturnType ZWorldTile::Initialize(ZBoard* _owner_board_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_owner_board_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZWorldTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZWorldTile::Initialize() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZWorldTile::RegisterLogicTile(
    ZLogicTile* _logic_tile_ptr
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;

    Z_CHECK(_logic_tile_ptr == nullptr, error_code::kZWorldTileErrorCode_LogicTileNotExists, L"Logic tile not exist!");

    pos_ = static_cast<ZWorldBoard*>(owner_board_ptr_)->CalculateWorldVectorByLogicVector(_logic_tile_ptr->pos_);
    index_ = _logic_tile_ptr->Index();
    return ret_val;
}

Void ZWorldTile::OnPosChanged(
    const WorldVector3D& _old_pos,
    const WorldVector3D& _new_pos
) noexcept {}

}//tsrpg
}//zengine