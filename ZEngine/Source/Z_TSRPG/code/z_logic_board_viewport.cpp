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

#include "z_logic_board_viewport.h"

#include "z_logic_board.h"
#include "z_logic_tile.h"
#include "z_display_board.h"

#include "z_core/m_log.h"

namespace zengine {
namespace tsrpg {

ZLogicBoardViewport::ZLogicBoardViewport() noexcept
    : SuperType_()
    , base_index_(0, 0)
    , view_size_(0, 0)
    , logic_board_ptr_(nullptr)
    , display_board_ptr_(nullptr)
    , next_logic_board_viewport_ptr_(nullptr)
    , pre_logic_board_viewport_ptr_(nullptr)
{}

ZLogicBoardViewport::~ZLogicBoardViewport() noexcept {}

NODISCARD ZLogicTile* ZLogicBoardViewport::operator()(IndexType _x, IndexType _y) noexcept {
    IndexType x = base_index_.x_ + _x;
    IndexType y = base_index_.y_ + _y;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* const ZLogicBoardViewport::operator()(IndexType _x, IndexType _y) const noexcept {
    IndexType x = base_index_.x_ + _x;
    IndexType y = base_index_.y_ + _y;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* ZLogicBoardViewport::operator()(LogicVector2D _index) noexcept {
    IndexType x = base_index_.x_ + _index.x_;
    IndexType y = base_index_.y_ + _index.y_;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* const ZLogicBoardViewport::operator()(LogicVector2D _index) const noexcept {
    IndexType x = base_index_.x_ + _index.x_;
    IndexType y = base_index_.y_ + _index.y_;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* ZLogicBoardViewport::At(IndexType _x, IndexType _y) noexcept {
    IndexType x = base_index_.x_ + _x;
    IndexType y = base_index_.y_ + _y;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* const ZLogicBoardViewport::At(IndexType _x, IndexType _y) const noexcept {
    IndexType x = base_index_.x_ + _x;
    IndexType y = base_index_.y_ + _y;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* ZLogicBoardViewport::At(LogicVector2D _index) noexcept {
    IndexType x = base_index_.x_ + _index.x_;
    IndexType y = base_index_.y_ + _index.y_;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}
NODISCARD ZLogicTile* const ZLogicBoardViewport::At(LogicVector2D _index) const noexcept {
    IndexType x = base_index_.x_ + _index.x_;
    IndexType y = base_index_.y_ + _index.y_;
    if (logic_board_ptr_->IndexCheck(x, y)) {
        return static_cast<ZLogicTile*>(logic_board_ptr_->At(x, y));
    }
    else {
        return nullptr;
    }
}

NODISCARD RPGObjectType ZLogicBoardViewport::Type() const noexcept {
    return logic_board_ptr_ ? logic_board_ptr_->Type() : 0;
}

Void ZLogicBoardViewport::Destroy() noexcept {
    SuperType_::Destroy();
    base_index_ = LogicVector2D(0, 0);
    view_size_ = LogicVector2D(0, 0);

    //destroy display board
    if (display_board_ptr_ != nullptr) {
        display_board_ptr_->Destroy();
    }

    //remove form logic board
    if (logic_board_ptr_ != nullptr) {
        if (pre_logic_board_viewport_ptr_ != nullptr) {
            pre_logic_board_viewport_ptr_->next_logic_board_viewport_ptr_ = next_logic_board_viewport_ptr_;
        }
        //if head node
        else {
            logic_board_ptr_->logic_board_viewport_head_ptr_ = next_logic_board_viewport_ptr_;
        }
        if (next_logic_board_viewport_ptr_ != nullptr) {
            next_logic_board_viewport_ptr_->pre_logic_board_viewport_ptr_ = pre_logic_board_viewport_ptr_;
        }
    }

    logic_board_ptr_ = nullptr;
    display_board_ptr_ = nullptr;
    next_logic_board_viewport_ptr_ = nullptr;
    pre_logic_board_viewport_ptr_ = nullptr;
}

NODISCARD ReturnType ZLogicBoardViewport::Initialize(
    ZLogicBoard* _logic_board_ptr,
    const LogicVector2D& _base_index,
    const LogicVector2D& _view_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    SuperType_::InitializeP();

    Z_CHECK(
        _logic_board_ptr == nullptr, error_code::kZLogicBoardViewerErrorCode_NullptrParams,
        L"_logic_board_ptr is nullptr!"
    );

    base_index_= _base_index;
    view_size_ = (_view_size.x_ < 0 || _view_size.y_ < 0) ? _logic_board_ptr->Size() : _view_size;

    if (_logic_board_ptr->logic_board_viewport_head_ptr_ != nullptr) {
        _logic_board_ptr->logic_board_viewport_head_ptr_->pre_logic_board_viewport_ptr_ = this;
    }
    next_logic_board_viewport_ptr_ = _logic_board_ptr->logic_board_viewport_head_ptr_;
    _logic_board_ptr->logic_board_viewport_head_ptr_ = this;
    logic_board_ptr_ = _logic_board_ptr;

    return ret_val;
}

NODISCARD ReturnType ZLogicBoardViewport::SetBaseIndex(const LogicVector2D& _base_index) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (base_index_ != _base_index) {
        LogicVector2D old_base_index = base_index_;
        base_index_ = _base_index;
        if (display_board_ptr_ != nullptr) {
            link_code = display_board_ptr_->OnLogicBoardViewportBaseIndexChanged(old_base_index, _base_index);
            if (link_code != kOK) {
                ret_val = error_code::kZLogicBoardViewerErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, L"ZDisplayBoard::OnLogicBoardViewerChanged() link error!");
                return ret_val;
            }
            
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZLogicBoardViewport::SetViewSize(const LogicVector2D& _view_size) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (view_size_ != _view_size) {
        LogicVector2D old_view_size = view_size_;
        view_size_ = _view_size;
        if (display_board_ptr_ != nullptr) {
            link_code = display_board_ptr_->OnLogicBoardViewportViewSizeChanged(old_view_size, view_size_);
            if (link_code != kOK) {
                ret_val = error_code::kZLogicBoardViewerErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, L"ZDisplayBoard::OnLogicBoardViewerChanged() link error!");
                return ret_val;
            }           
        }
    }
    return ret_val;
}

/*
    Gets the tile's surrounded tile by the given inner and outer radius(distance to the center tile).
    Includes the inner circle and outer circle.
*/
NODISCARD ReturnType ZLogicBoardViewport::GetSurroundTile(
    TVector<ZTile*>* _tile_list_ptr,
    const LogicVector2D& _center_index,
    Int32 _inner_radius,
    Int32 _outer_radius
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    LogicVector2D index(_center_index.x_ + base_index_.x_, _center_index.y_ + base_index_.y_);
    link_code = logic_board_ptr_->GetSurroundTile(_tile_list_ptr, index, _inner_radius, _outer_radius);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicBoardViewerErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoard::GetSurroundTile() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD LogicVector3D ZLogicBoardViewport::CalculateLogicPosOffset() const noexcept {
    if (logic_board_ptr_ == nullptr) {
        return LogicVector3D(0, 0, 0);
    }
    return LogicVector3D(
        logic_board_ptr_->pos_offset_.x_ + base_index_.x_,
        logic_board_ptr_->pos_offset_.y_ + base_index_.y_,
        logic_board_ptr_->pos_offset_.z_
    );
}

NODISCARD ReturnType ZLogicBoardViewport::OnLogicBoardPosOffsetChanged(const LogicVector3D& _offset) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    LogicVector3D offset(_offset.x_ + base_index_.x_, _offset.y_ + base_index_.y_, _offset.z_);
    link_code = display_board_ptr_->SetPosOffset(offset);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicBoardViewerErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZDisplay::SetPosOffset() link error!");
        return ret_val;
    }

    return ret_val;
}

}//tsrpg
}//zengine