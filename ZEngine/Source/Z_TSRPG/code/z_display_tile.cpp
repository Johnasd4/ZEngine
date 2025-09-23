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

#include "z_display_tile.h"

#include "z_core/m_log.h"

#include "z_display_board.h"
#include "z_logic_tile.h"

namespace zengine {
namespace tsrpg {

ZDisplayTile::ZDisplayTile() noexcept
    : SuperType_()
    , pos_()
    , index_()
    , logic_tile_ptr_(nullptr)
    , next_display_tile_ptr_(nullptr)
    , pre_display_tile_ptr_(nullptr)
{}

ZDisplayTile::~ZDisplayTile() noexcept {}

NODISCARD ReturnType ZDisplayTile::SetPos(const DisplayVector3D& _pos) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (pos_ != _pos) {
        DisplayVector3D old_pos = pos_;
        pos_ = _pos;
        link_code = OnPosChanged(old_pos, _pos);
        if (link_code != kOK) {
            ret_val = error_code::kZDisplayTileErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZDisplayTile::OnPosChanged() link error!");
            return ret_val;
        }    
    }
    return ret_val;
}

NODISCARD ReturnType ZDisplayTile::SetPos(const LogicVector3D& _pos) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    ZDisplayBoard* owner_board_ptr = static_cast<ZDisplayBoard*>(owner_board_ptr_);
    link_code = SetPos(owner_board_ptr->CalculateDisplayVectorByLogicVector(_pos));
    if (link_code != kOK) {
        ret_val = error_code::kZDisplayTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZDisplayTile::SetPos() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD LogicVector2D ZDisplayTile::Index() const noexcept { return index_; }

Void ZDisplayTile::Destroy() noexcept {
    SuperType_::Destroy();
    if (logic_tile_ptr_ != nullptr) {
        if (pre_display_tile_ptr_ != nullptr) {
            pre_display_tile_ptr_->next_display_tile_ptr_ = next_display_tile_ptr_;
        }
        //if head node
        else {
            logic_tile_ptr_->display_tile_head_ptr_ = next_display_tile_ptr_;
        }
        if (next_display_tile_ptr_ != nullptr) {
            next_display_tile_ptr_->pre_display_tile_ptr_ = pre_display_tile_ptr_;
        }
    }
    next_display_tile_ptr_ = nullptr;
    pre_display_tile_ptr_ = nullptr;
    logic_tile_ptr_ = nullptr;
}

ReturnType ZDisplayTile::Initialize(
    ZBoard* _owner_board_ptr,
    ZLogicTile* _logic_tile_ptr,
    const LogicVector2D& _index
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::InitializeP(_owner_board_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZDisplayTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZDisplayTile::Initialize() link error!");
        return ret_val;
    }
    Z_CHECK(
        link_code != kOK, error_code::kZDisplayTileErrorCode_LinkError,
        L"_logic_tile_ptr is nullptr!"
    );
    Z_CHECK(
        _logic_tile_ptr == nullptr, error_code::kZDisplayTileErrorCode_LogicTileNotExists,
        L"_logic_tile_ptr is nullptr!"
    );
    if (_logic_tile_ptr->display_tile_head_ptr_ != nullptr) {
        _logic_tile_ptr->display_tile_head_ptr_->pre_display_tile_ptr_ = this;
    }
    next_display_tile_ptr_ = _logic_tile_ptr->display_tile_head_ptr_;
    _logic_tile_ptr->display_tile_head_ptr_ = this;
    logic_tile_ptr_ = _logic_tile_ptr;

    pos_ = static_cast<ZDisplayBoard*>(owner_board_ptr_)->CalculateDisplayVectorByLogicVector(_logic_tile_ptr->pos_);
    index_ = _index;

    return ret_val;
}

}//tsrpg
}//zengine