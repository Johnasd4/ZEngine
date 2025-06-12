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

#include "z_board.h"

#include "z_tile.h"

namespace zengine {
namespace tsrpg {

ZBoard::ZBoard() noexcept 
    : SuperType_()
    , tile_matrix_()
{}

ZBoard::~ZBoard() noexcept {}

Void ZBoard::Destroy() noexcept {
    SuperType_::Destroy();
    Bool initialized_ = false;
    //clear the old tiles
    for (auto tile_ptr = tile_matrix_.Begin(); tile_ptr != tile_matrix_.End(); ++tile_ptr) {
        if (*tile_ptr == nullptr) {
            continue;
        }
        (*tile_ptr)->Destroy();
        delete (*tile_ptr);
    }
}

NODISCARD ReturnType ZBoard::Initialize(const LogicVector2D& _size) noexcept {
    ReturnType ret_val = kOK;
    SuperType_::Initialize();
    tile_matrix_.Resize(_size.x_, _size.y_, nullptr);
    return ret_val;
}

}//tsrpg
}//zengine