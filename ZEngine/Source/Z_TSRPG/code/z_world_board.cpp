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

#include "z_world_board.h"

#include "z_core/m_log.h"

#include "z_logic_board.h"
#include "z_logic_tile.h"
#include "z_world_tile.h"

namespace zengine {
namespace tsrpg {

ZWorldBoard::ZWorldBoard(const TileSizeType& _tile_size) noexcept
    : SuperType_()
    , logic_board_ptr_(nullptr)
    , tile_size_(_tile_size)
    , pos_offset_()
{}

ZWorldBoard::~ZWorldBoard() noexcept {}

NODISCARD ReturnType ZWorldBoard::SetTileSize(const TileSizeType& _size) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        logic_board_ptr_ == nullptr, error_code::kZWorldBoardErrorCode_LogicBoardNotExist, 
        L"Logic board not exist!");

    if (tile_size_ != _size) {
        TileSizeType old_tile_size = tile_size_;
        tile_size_ = _size;
        if (initialized_) {
            OnTileSizeChanged(old_tile_size, _size);
        }
    }

    return ret_val;
}

NODISCARD ReturnType ZWorldBoard::SetPosOffset(const WorldVector3D& _offset) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;

    Z_CHECK(
        logic_board_ptr_ == nullptr, error_code::kZWorldBoardErrorCode_LogicBoardNotExist,
        L"Logic board not exist!");

    if (pos_offset_ != _offset) {
        WorldVector3D old_pos_offset = pos_offset_;
        pos_offset_ = _offset;
        if (initialized_) {
            OnPosOffsetChanged(old_pos_offset, _offset);
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZWorldBoard::SetPosOffset(const LogicVector3D& _offset) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;

    Z_CHECK(
        logic_board_ptr_ == nullptr, error_code::kZWorldBoardErrorCode_LogicBoardNotExist,
        L"Logic board not exist!");

    WorldVector3D offset = CalculateWorldVectorByLogicVector(_offset);
    if (pos_offset_ != offset) {
        WorldVector3D old_pos_offset = pos_offset_;
        pos_offset_ = offset;
        if (initialized_) {
            OnPosOffsetChanged(old_pos_offset, offset);
        }
    }
    return ret_val;
}

NODISCARD UInt64 ZWorldBoard::Type() const noexcept { return kBoardType_WorldBoard; }

Void ZWorldBoard::Destroy() noexcept {
    SuperType_::Destroy();
}

NODISCARD ReturnType ZWorldBoard::Initialize(const LogicVector2D& _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kZWorldBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZBoard::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZWorldBoard::RegisterLogicBoard(
    ZLogicBoard* _logic_board_ptr,
    const ZWorldTile& _tile_template
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    logic_board_ptr_ = _logic_board_ptr;

    //create the tile by the logic board
    for (IndexType x = 0; x < logic_board_ptr_->XLength(); ++x) {
        for (IndexType y = 0; y < logic_board_ptr_->YLength(); ++y) {
            ZLogicTile* logic_tile_ptr = static_cast<ZLogicTile*>((*logic_board_ptr_)(x, y));
            if (logic_tile_ptr == nullptr) {
                continue;
            }
            ZWorldTile* world_tile_ptr = static_cast<ZWorldTile*>(_tile_template.CreateCopy());
            link_code = world_tile_ptr->Initialize(this);
            if (link_code != kOK) {
                delete world_tile_ptr;
                Z_LOG_ERROR(
                    error_code::kZWorldBoardErrorCode_LinkError, link_code,
                    L"ZWorldTile::Initialize() link error! (x, y) = (%d, %d)", x, y);
                return ret_val;
            }
            link_code = world_tile_ptr->RegisterLogicTile(logic_tile_ptr);
            if (link_code != kOK) {
                delete world_tile_ptr;
                Z_LOG_ERROR(
                    error_code::kZWorldBoardErrorCode_LinkError, link_code,
                    L"ZWorldTile::RegisterLogicTile() link error! (x, y) = (%d, %d)", x, y);
                return ret_val;
            }
            tile_matrix_(x, y) = world_tile_ptr;
        }
    }
    return ret_val;
}

Void ZWorldBoard::OnTileSizeChanged(
    const TileSizeType& _old_tile_size,
    const TileSizeType& _new_tile_size
) noexcept {
    ReturnType link_code = kOK;
    for (IndexType x = 0; x < logic_board_ptr_->XLength(); ++x) {
        for (IndexType y = 0; y < logic_board_ptr_->YLength(); ++y) {
            ZWorldTile* tile_ptr = static_cast<ZWorldTile*>(tile_matrix_(x, y));
            if (tile_ptr != nullptr) {
                link_code = tile_ptr->SetPos(static_cast<ZLogicTile*>((*logic_board_ptr_)(x, y))->Pos());
                if (link_code != kOK) {
                    Z_LOG_ERROR(
                        error_code::kZWorldBoardErrorCode_LinkError, link_code,
                        L"ZWorldTile::SetPos() link error! (x, y) = (%d, %d)", x, y);
                }
            }
        }
    }
}

Void ZWorldBoard::OnPosOffsetChanged(
    const WorldVector3D& _old_offset,
    const WorldVector3D& _new_offset
) noexcept {}

}//tsrpg
}//zengine