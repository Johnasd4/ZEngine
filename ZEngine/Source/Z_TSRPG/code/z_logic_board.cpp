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

#include "z_world_board.h"
#include "z_logic_tile.h"

namespace zengine {
namespace tsrpg {

ZLogicBoard::ZLogicBoard() noexcept 
    : SuperType_()
    , sub_board_ptr_set_()
    , owner_board_ptr_(nullptr)
    , world_board_ptr_(nullptr)
    , teleport_info_list_()
    , relevant_board_ptr_set_()
    , pos_offset_()
{}

ZLogicBoard::~ZLogicBoard() noexcept {}

ReturnType ZLogicBoard::SetPosOffset(const LogicVector3D& _offset) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    pos_offset_ = _offset;
    if (world_board_ptr_ != nullptr) {
        link_code = world_board_ptr_->SetPosOffset(_offset);
        if (link_code != kOK) {
            ret_val = error_code::kZLogicTileErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZWoardBoard::SetPosOffset() link error!");
            return ret_val;
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZLogicBoard::AddTeleportInfo(
    ZLogicTile* _source_tile_ptr,
    ZLogicBoard* _target_board_ptr,
    ZLogicTile* _target_tile_ptr
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;

    Z_CHECK(
        _source_tile_ptr == nullptr, error_code::kZLogicBoardErrorCode_NullptrParams,
        L"_source_tile_ptr is nullptr!");
    Z_CHECK(
        _target_board_ptr == nullptr, error_code::kZLogicBoardErrorCode_NullptrParams,
        L"_target_board_ptr is nullptr!");
    Z_CHECK(
        _target_tile_ptr == nullptr, error_code::kZLogicBoardErrorCode_NullptrParams,
        L"_target_tile_ptr is nullptr!");

    teleport_info_list_.PushBack(TeleportInfo_(_source_tile_ptr, _target_board_ptr, _target_tile_ptr));

    if (_target_board_ptr != this) {
        _target_board_ptr->relevant_board_ptr_set_.Insert(this);
    }

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
        _board_ptr == nullptr, error_code::kZLogicBoardErrorCode_NullptrParams,
        L"_board_ptr is nullptr!");
    Z_CHECK(
        _board_ptr->owner_board_ptr_ != nullptr, error_code::kZLogicBoardErrorCode_OwnerBoardAlreadyExists,
        L"Owner board already exists!");

    sub_board_ptr_set_.Insert(_board_ptr);
    _board_ptr->owner_board_ptr_ = this;
    _board_ptr->pos_offset_ = _logic_pos_offset;
    if (_board_ptr->world_board_ptr_ != nullptr) {
        link_code = _board_ptr->world_board_ptr_->SetPosOffset(_logic_pos_offset);
        if (link_code != kOK) {
            ret_val = error_code::kZLogicBoardErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZWoardBoard::SetPosOffset() link error!");
            return ret_val;
        }
    }
    return ret_val;
}

NODISCARD Void ZLogicBoard::RemoveTeleportInfo(
    ZLogicTile* _source_tile_ptr,
    ZLogicBoard* _target_board_ptr,
    ZLogicTile* _target_tile_ptr
) noexcept {
    teleport_info_list_.Remove(TeleportInfo_(_source_tile_ptr, _target_board_ptr, _target_tile_ptr));
}

NODISCARD Void ZLogicBoard::RemoveSubBoard(
    ZLogicBoard* _board_ptr
) noexcept {
    if (_board_ptr != nullptr && _board_ptr->owner_board_ptr_ == this) {
        sub_board_ptr_set_.Erase(_board_ptr);
        _board_ptr->owner_board_ptr_ = nullptr;
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

    ClearReleventInfoP();

    //clear the sub boards
    for (auto board_ptr = sub_board_ptr_set_.Begin(); board_ptr != sub_board_ptr_set_.End(); ++board_ptr) {
        (*board_ptr)->Destroy();
    }
    sub_board_ptr_set_.Clear();

    //clear the owner board link
    if (owner_board_ptr_ != nullptr) {
        owner_board_ptr_->sub_board_ptr_set_.Erase(this);
        owner_board_ptr_ = nullptr;
    }

    //destroy the world board
    if (world_board_ptr_ != nullptr) {
        world_board_ptr_->Destroy();
        delete world_board_ptr_;
        world_board_ptr_ = nullptr;
    }

    //clear position offset
    pos_offset_ = LogicVector3D(0, 0, 0);
}

/*
    Initialize the board to the given size.
*/
NODISCARD ReturnType ZLogicBoard::Initialize(const LogicVector2D& _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZBoard::Initialize() link error!");
        return ret_val;
    }
    
    return ret_val;
}

/*
    Fill the board with the tile template.
*/
NODISCARD ReturnType ZLogicBoard::Fill(const ZLogicTile& _tile_template) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    for (IndexType x = 0; x < XLength(); ++x) {
        for (IndexType y = 0; y < YLength(); ++y) {
            if ((*this)(x, y) != nullptr) {
                (*this)(x, y)->Destroy();
                delete (*this)(x, y);
            }
            ZLogicTile* logic_tile_ptr = static_cast<ZLogicTile*>(_tile_template.CreateCopy());
            logic_tile_ptr->pos_ = LogicVector3D(x, y, _tile_template.Z());
            (*this)(x, y) = logic_tile_ptr;
        }
    }

    //clear all the relevant info
    ClearReleventInfoP();

    //destroy the world board if exist
    if (world_board_ptr_ != nullptr) {
        world_board_ptr_->Destroy();
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
    for (auto board_ptr = sub_board_ptr_set_.Begin(); board_ptr != sub_board_ptr_set_.End(); ++board_ptr) {
        if ((*board_ptr)->CalculateRelativeBoardLogicPosOffsetP(_target_board_ptr, _offset_ptr)) {
            *_offset_ptr += (*board_ptr)->pos_offset_;
            return true;
        }
    }
    return false;
}

NODISCARD Void ZLogicBoard::ClearReleventInfoP() noexcept {
    //clear the relevant info
    teleport_info_list_.Clear();
    for (auto board_ptr = relevant_board_ptr_set_.Begin(); board_ptr != relevant_board_ptr_set_.End(); ++board_ptr) {
        //teleport info
        for (auto teleport_info = teleport_info_list_.Begin(); teleport_info != teleport_info_list_.End();) {
            if (teleport_info->target_board_ptr_ == this) {
                teleport_info = teleport_info_list_.Erase(teleport_info);
                continue;
            }
            ++teleport_info;
        }
    }
    relevant_board_ptr_set_.Clear();
}

}//tsrpg
}//zengine