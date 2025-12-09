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

#include "z_display_board_object.h"

#include "z_logic_board_object.h"

namespace zengine {
namespace tsrpg {

ZDisplayBoardObject::ZDisplayBoardObject() noexcept
    : SuperType_()
    , logic_board_object_ptr_(nullptr)
    , next_display_board_object_ptr_(nullptr)
    , pre_display_board_object_ptr_(nullptr)
{}

ZDisplayBoardObject::~ZDisplayBoardObject() noexcept {}

NODISCARD UInt64 ZDisplayBoardObject::Type() const noexcept { return kBoardObjectType_DisplayBoardObject; }

Void ZDisplayBoardObject::Destroy() noexcept {
    SuperType_::Destroy();
    if (pre_display_board_object_ptr_ != nullptr) {
        pre_display_board_object_ptr_->next_display_board_object_ptr_ = next_display_board_object_ptr_;
    }
    //if head node
    else {
        logic_board_object_ptr_->display_board_object_head_ptr_ = next_display_board_object_ptr_;
    }
    if (next_display_board_object_ptr_ != nullptr) {
        next_display_board_object_ptr_->pre_display_board_object_ptr_ = pre_display_board_object_ptr_;
    }
    next_display_board_object_ptr_ = nullptr;
    pre_display_board_object_ptr_ = nullptr;
    logic_board_object_ptr_ = nullptr;
}

NODISCARD ReturnType ZDisplayBoardObject::Initialize(ZTile* _owner_tile_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::InitializeP(_owner_tile_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZDisplayBoardObjectErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZBoardObject::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZDisplayBoardObject::RegisterLogicBoardObject(ZLogicBoardObject* _logic_object_ptr) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _logic_object_ptr == nullptr, error_code::kZDisplayBoardObjectErrorCode_LogicTileNotExists,
        "_logic_object_ptr is nullptr!"
    );
    if (_logic_object_ptr->display_board_object_head_ptr_ != nullptr) {
        _logic_object_ptr->display_board_object_head_ptr_->pre_display_board_object_ptr_ = this;
    }
    next_display_board_object_ptr_ = _logic_object_ptr->display_board_object_head_ptr_;
    _logic_object_ptr->display_board_object_head_ptr_ = this;
    logic_board_object_ptr_ = _logic_object_ptr;
    return ret_val;
}

}//tsrpg
}//zengine