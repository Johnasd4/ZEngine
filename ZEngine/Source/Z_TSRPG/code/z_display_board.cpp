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

#include "z_display_board.h"

#include "z_logic_board_viewport.h"
#include "z_logic_tile.h"
#include "z_display_tile.h"

namespace zengine {
namespace tsrpg {

ZDisplayBoard::ZDisplayBoard() noexcept
    : SuperType_()
    , tile_size_(TileSizeType(0.0, 0.0))
    , pos_offset_(DisplayVector3D(0.0, 0.0, 0.0))
    , tile_mesh_name_()
    , logic_board_viewport_ptr_(nullptr)
{}

ZDisplayBoard::~ZDisplayBoard() noexcept {}

NODISCARD ReturnType ZDisplayBoard::SetTileSize(const TileSizeType& _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (tile_size_ != _size) {
        TileSizeType old_tile_size = tile_size_;
        tile_size_ = _size;
        if (Initialized()) {
            link_code = OnTileSizeChanged(old_tile_size, _size);
            if (link_code != kOK) {
                ret_val = error_code::kZDisplayBoardErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, "ZDisplayBoard::OnTileSizeChanged() link error!");
                return ret_val;
            }          
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZDisplayBoard::SetPosOffset(const DisplayVector3D& _offset) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (pos_offset_ != _offset) {
        DisplayVector3D old_pos_offset = pos_offset_;
        pos_offset_ = _offset;
        if (Initialized()) {
            link_code = OnPosOffsetChanged(old_pos_offset, _offset);
            if (link_code != kOK) {
                ret_val = error_code::kZDisplayBoardErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, "ZDisplayBoard::OnPosOffsetChanged() link error!");
                return ret_val;
            }
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZDisplayBoard::SetPosOffset(const LogicVector3D& _offset) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    DisplayVector3D offset = CalculateDisplayVectorByLogicVector(_offset);
    if (pos_offset_ != offset) {
        DisplayVector3D old_pos_offset = pos_offset_;
        pos_offset_ = offset;
        if (Initialized()) {          
            link_code = OnPosOffsetChanged(old_pos_offset, offset);
            if (link_code != kOK) {
                ret_val = error_code::kZDisplayBoardErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, "ZDisplayBoard::OnPosOffsetChanged() link error!");
                return ret_val;
            }
        }
    }
    return ret_val;
}

Void ZDisplayBoard::Destroy() noexcept {
    SuperType_::Destroy();
    tile_size_ = TileSizeType(0.0, 0.0);
    pos_offset_ = DisplayVector3D(0.0, 0.0, 0.0);
    tile_mesh_name_.Clear();
    if (logic_board_viewport_ptr_ != nullptr) {
        logic_board_viewport_ptr_->display_board_ptr_ = nullptr;
        logic_board_viewport_ptr_ = nullptr;
    }
}

NODISCARD ReturnType ZDisplayBoard::Initialize(
    ZLogicBoardViewport* _logic_board_viewport_ptr,
    const TileSizeType& _tile_size,
    const WChar* _tile_mesh_name
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _logic_board_viewport_ptr == nullptr, error_code::kZDisplayBoardErrorCode_NullptrParam,
        "_logic_board_viewport_ptr is nullptr!"
    );
    link_code = SuperType_::InitializeP(_logic_board_viewport_ptr->view_size_);
    if (link_code != kOK) {
        ret_val = error_code::kZDisplayBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZBoard::Initialize() link error!");
        return ret_val;
    }
    tile_size_ = _tile_size;
    pos_offset_ = CalculateDisplayVectorByLogicVector(_logic_board_viewport_ptr->CalculateLogicPosOffset());
    tile_mesh_name_ = _tile_mesh_name;
    _logic_board_viewport_ptr->display_board_ptr_ = this;
    logic_board_viewport_ptr_ = _logic_board_viewport_ptr;
    return ret_val;
}

}//tsrpg
}//zengine