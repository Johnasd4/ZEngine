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

#include "z_board_object.h"

#include "z_core/m_log.h"

#include "z_tile.h"

namespace zengine {
namespace tsrpg {

ZBoardObject::ZBoardObject() noexcept
    : SuperType_()
    , pre_object_ptr_(nullptr)
    , next_object_ptr_(nullptr)
    , owner_tile_ptr_(nullptr)
{}

ZBoardObject::~ZBoardObject() noexcept {}

Void ZBoardObject::Destroy() noexcept {
    SuperType_::Destroy();
    if (pre_object_ptr_ != nullptr) {
        pre_object_ptr_->next_object_ptr_ = next_object_ptr_;
        pre_object_ptr_ = nullptr;
    }
    if (next_object_ptr_ != nullptr) {
        next_object_ptr_->pre_object_ptr_ = pre_object_ptr_;
        next_object_ptr_ = nullptr;
    }
    owner_tile_ptr_ = nullptr;
}

NODISCARD ReturnType ZBoardObject::Initialize(ZTile* _owner_tile_ptr) noexcept {
    ReturnType ret_val = kOK;
    SuperType_::Initialize();
    if (_owner_tile_ptr == nullptr || _owner_tile_ptr->initialized_ == false) {
        initialized_ = false;
        ret_val = error_code::kZBoardObjectErrorCode_OnwerTileNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Owner tile not exist or not initialized!");
        return ret_val;
    }
    next_object_ptr_ = _owner_tile_ptr->board_object_ptr_;
    _owner_tile_ptr->board_object_ptr_->pre_object_ptr_ = this;
    _owner_tile_ptr->board_object_ptr_ = this;
    owner_tile_ptr_ = _owner_tile_ptr;
    return ret_val;
}

}//tsrpg
}//zengine