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
    //clear the old tiles
    for (auto tile_ptr = tile_matrix_.Begin(); tile_ptr != tile_matrix_.End(); ++tile_ptr) {
        if (*tile_ptr == nullptr) {
            continue;
        }
        (*tile_ptr)->Destroy();
        delete (*tile_ptr);
    }
    tile_matrix_.Clear();
}

NODISCARD ReturnType ZBoard::GetSurroundTile(
    TArray<ZTile*>* _tile_list_ptr,
    const LogicVector2D& _center_index,
    Int32 _inner_radius,
    Int32 _outer_radius
) noexcept {
    Z_TSRPG_INITIALIZE_CHECK();
    ReturnType ret_val = kOK;
    Z_CHECK(
        _tile_list_ptr == nullptr, error_code::kZBoardErrorCode_NullptrParam,
        "_tile_list_ptr is nullptr!"
    );
    if (IsType(kBoardType_HexBoard)) {
        _tile_list_ptr->Clear();
        _tile_list_ptr->Reserve(_outer_radius * 12);

        Int32 dx_start = -_outer_radius;
        Int32 dx_end = _outer_radius;
        for (Int32 dx = -_outer_radius; dx <= _outer_radius; ++dx) {
            Int32 dy_start = math::Max(-_outer_radius, -dx - _outer_radius);
            Int32 dy_end = math::Min(_outer_radius, -dx + _outer_radius);
            for (Int32 dy = dy_start; dy <= dy_end; ++dy) {
                Int32 x = _center_index.x_ + dx;
                Int32 y = _center_index.y_ + dy;
                if (!IndexCheck(LogicVector2D(x, y))) {
                    continue;
                }
                //outer of inner circle
                if ((math::Abs(dx) + math::Abs(dx + dy) + math::Abs(dy)) / 2 >= _inner_radius) {
                    _tile_list_ptr->PushBack(At(x, y));
                }
            }
        }
    }
    //else if (IsType(kBoardType_SquareBoard)) {
    //}
    else {
        ret_val = error_code::kZBoardErrorCode_BoardTypeNotExist;
        Z_LOG_ERROR(ret_val, 0, "Board type not exist! Type: %d", Type());
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZBoard::InitializeP(const LogicVector2D& _board_size) noexcept {
    ReturnType ret_val = kOK;
    SuperType_::InitializeP();
    tile_matrix_.Resize(_board_size.x_, _board_size.y_, nullptr);
    return ret_val;
}

}//tsrpg
}//zengine