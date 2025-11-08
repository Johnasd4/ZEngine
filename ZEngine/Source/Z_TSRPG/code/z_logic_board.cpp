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

#include "z_logic_board.h"

#include "z_core/m_log.h"

#include "z_logic_board_viewport.h"
#include "z_display_board.h"
#include "z_logic_tile.h"

namespace zengine {
namespace tsrpg {

ZLogicBoard::ZLogicBoard() noexcept 
    : SuperType_()
    , pos_offset_()
    , logic_board_viewport_head_ptr_(nullptr)
    , owner_board_ptr_(nullptr)
    , sub_board_head_ptr_(nullptr)
    , next_sub_board_ptr_(nullptr)
    , pre_sub_board_ptr_(nullptr)
{}

ZLogicBoard::~ZLogicBoard() noexcept {}

ReturnType ZLogicBoard::SetPosOffset(const LogicVector3D& _offset) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    pos_offset_ = _offset;
    ZLogicBoardViewport* logic_board_viewport_ptr = logic_board_viewport_head_ptr_;
    while (logic_board_viewport_ptr != nullptr) {
        link_code = logic_board_viewport_ptr->OnLogicBoardPosOffsetChanged(_offset);
        if (link_code != kOK) {
            ret_val = error_code::kZLogicTileErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoardViewport::OnLogicBoardPosOffsetChanged() link error!");
        }
        logic_board_viewport_ptr = logic_board_viewport_ptr->next_logic_board_viewport_ptr_;
    }
    return ret_val;
}

ReturnType ZLogicBoard::SetBaseLayerTexture(const WChar* _texture_name) noexcept {
    ReturnType ret_val = kOK;
    const ZLogicTileTexture* texture_ptr = ZLogicTileTexture::GetLogicTileTextureByName(_texture_name);
    if (texture_ptr == nullptr) {
        ret_val = error_code::kZLogicBoardErrorCode_TileTextureNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Tile texture not exist! Name: %ls", _texture_name);
        return ret_val;
    }
    base_layer_texture_ptr_ = texture_ptr;
    return ret_val;
}

NODISCARD ReturnType ZLogicBoard::AddSubBoard(
    ZLogicBoard* _board_ptr,
    const LogicVector3D& _logic_pos_offset
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _board_ptr == nullptr, error_code::kZLogicBoardErrorCode_NullptrParam,
        L"_board_ptr is nullptr!"
    );
    Z_CHECK(
        _board_ptr->owner_board_ptr_ != nullptr, error_code::kZLogicBoardErrorCode_OwnerBoardAlreadyExists,
        L"Owner board already exists!"
    );

    if (sub_board_head_ptr_ != nullptr) {
        _board_ptr->pre_sub_board_ptr_ = nullptr;
    }
    sub_board_head_ptr_ = _board_ptr;
    _board_ptr->owner_board_ptr_ = this;
    link_code = _board_ptr->SetPosOffset(_logic_pos_offset);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoard::SetPosOffset() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD Void ZLogicBoard::RemoveSubBoard(
    ZLogicBoard* _board_ptr
) noexcept {
    if (_board_ptr != nullptr && _board_ptr->owner_board_ptr_ == this) {
        if (pre_sub_board_ptr_ != nullptr) {
            pre_sub_board_ptr_->next_sub_board_ptr_ = next_sub_board_ptr_;
        }
        //if head node
        else {
            sub_board_head_ptr_ = pre_sub_board_ptr_->next_sub_board_ptr_;
        }
        if (next_sub_board_ptr_ != nullptr) {
            next_sub_board_ptr_->pre_sub_board_ptr_ = pre_sub_board_ptr_;
        }
        _board_ptr->owner_board_ptr_ = nullptr;
        _board_ptr->next_sub_board_ptr_ = nullptr;
        _board_ptr->pre_sub_board_ptr_ = nullptr;
    }
}

NODISCARD Bool ZLogicBoard::CalculateRelativeBoardLogicPosOffset(
    const ZLogicBoard* _target_board_ptr,
    LogicVector3D* _offset_ptr
) const noexcept {
    ZLogicBoard const* root_board_ptr = this;
    while (owner_board_ptr_ != nullptr) {
        *_offset_ptr -= root_board_ptr->pos_offset_;
        root_board_ptr = owner_board_ptr_;
    }
    return root_board_ptr->CalculateRelativeBoardLogicPosOffsetP(_target_board_ptr, _offset_ptr);
}

NODISCARD UInt64 ZLogicBoard::Type() const noexcept { return kBoardType_LogicBoard; }

Void ZLogicBoard::Destroy() noexcept {
    SuperType_::Destroy();

    //destroy the logic board viewport
    while (logic_board_viewport_head_ptr_ != nullptr) {
        logic_board_viewport_head_ptr_->Destroy();
    }

    //clear the owner board link
    if (owner_board_ptr_ != nullptr) {
        owner_board_ptr_->RemoveSubBoard(this);
        owner_board_ptr_ = nullptr;
    }

    //destroy the sub boards
    while (sub_board_head_ptr_ != nullptr) {
        sub_board_head_ptr_->Destroy();
    }

    //clear position offset
    pos_offset_ = LogicVector3D(0, 0, 0);
}

NODISCARD ReturnType ZLogicBoard::Initialize(const LogicVector2D& _board_size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::InitializeP(_board_size);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZBoard::Initialize() link error!");
        return ret_val;
    }
    
    return ret_val;
}

NODISCARD ReturnType ZLogicBoard::Fill(
    Int32 _horizontal,
    const WChar* _texture_name,
    Int32 _texture_length
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    //destroy all the existing logic board viewport
    while (logic_board_viewport_head_ptr_ != nullptr) {
        logic_board_viewport_head_ptr_->Destroy();
    }

    const ZLogicTileTexture* tile_texture_ptr = nullptr;
    if (_texture_name != nullptr) {
        //get texture
        tile_texture_ptr = ZLogicTileTexture::GetLogicTileTextureByName(_texture_name);
        if (tile_texture_ptr == nullptr) {
            ret_val = error_code::kZLogicBoardErrorCode_TileTextureNotExist;
            Z_LOG_ERROR(ret_val, 0, L"Tile texture not exist! Name: %ls", _texture_name);
            return ret_val;
        }
    }

    //fill the board
    for (Int32 x = 0; x < XLength(); ++x) {
        for (Int32 y = 0; y < YLength(); ++y) {
            if ((*this)(x, y) != nullptr) {
                (*this)(x, y)->Destroy();
                delete (*this)(x, y);
                (*this)(x, y) = nullptr;
            }
            ZLogicTile* logic_tile_ptr = CreateLogicTileP();
            link_code = logic_tile_ptr->Initialize(
                this,
                LogicVector3D(x, y, _horizontal),
                tile_texture_ptr,
                _texture_length
            );
            if (link_code != kOK) {
                ret_val = error_code::kZLogicBoardErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, L"ZLogicTile::Initialize() link error!");
                return ret_val;
            }
            (*this)(x, y) = logic_tile_ptr;
        }
    }

    return ret_val;
}


NODISCARD Bool ZLogicBoard::CalculateRelativeBoardLogicPosOffsetP(
    const ZLogicBoard* _target_board_ptr,
    LogicVector3D* _offset_ptr
) const noexcept {
    if (this == _target_board_ptr) {
        return true;
    }
    ZLogicBoard* sub_board_ptr = sub_board_head_ptr_;
    while (sub_board_ptr != nullptr) {
        if (sub_board_ptr->CalculateRelativeBoardLogicPosOffsetP(_target_board_ptr, _offset_ptr)) {
            *_offset_ptr += sub_board_ptr->pos_offset_;
            return true;
        }
        sub_board_ptr = sub_board_ptr->next_sub_board_ptr_;
    }
    return false;
}

}//tsrpg
}//zengine