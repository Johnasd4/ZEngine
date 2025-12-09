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

#include "z_tile.h"

#include "z_board_object.h"

namespace zengine {
namespace tsrpg {

ZTile::ZTile() noexcept : SuperType_(), owner_board_ptr_(nullptr), board_object_head_ptr_(nullptr) {}

ZTile::~ZTile() noexcept {}

Void ZTile::Destroy() noexcept {
    SuperType_::Destroy();
    //destroy all the objects on the tile
    while (board_object_head_ptr_ != nullptr) {
        board_object_head_ptr_->Destroy();
        delete board_object_head_ptr_;
        board_object_head_ptr_ = board_object_head_ptr_->next_object_ptr_;
    }
}

NODISCARD ReturnType ZTile::InitializeP(ZBoard* _owner_board_ptr) noexcept {
    ReturnType ret_val = kOK;
    SuperType_::InitializeP();
    Z_CHECK(
        _owner_board_ptr == nullptr, error_code::kZTileErrorCode_NullptrParam,
        "_owner_board_ptr is nullptr!"
    );
    owner_board_ptr_ = _owner_board_ptr;
    return ret_val;
}

}//tsrpg
}//zengine