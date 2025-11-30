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

#include "z_logic_board_object.h"

#include "z_display_board_object.h"

namespace zengine {
namespace tsrpg {

ZLogicBoardObject::ZLogicBoardObject() noexcept
    : SuperType_()
    , visible_(false)
    , attackable_(false)
    , if_obstacle_(false)
    , display_board_object_head_ptr_(nullptr)
{}

ZLogicBoardObject::~ZLogicBoardObject() noexcept {}

NODISCARD UInt64 ZLogicBoardObject::Type() const noexcept { return kBoardObjectType_LogicBoardObject; }

Void ZLogicBoardObject::Destroy() noexcept {
    SuperType_::Destroy();
    visible_ = false;
    attackable_ = false;
    if_obstacle_ = false;

    while (display_board_object_head_ptr_ != nullptr) {
        display_board_object_head_ptr_->Destroy();
    }
}

NODISCARD ReturnType ZLogicBoardObject::Initialize(ZTile* _owner_tile_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::InitializeP(_owner_tile_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicBoardObjectErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZBoard::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

}//tsrpg
}//zengine