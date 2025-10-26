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

#include "scene_event/z_move_tile_scene_event.h"

#include "z_logic_board.h"
#include "z_logic_tile.h"
#include "Z_display_board.h"
#include "z_display_tile.h"

namespace zengine {
namespace tsrpg {

ZMoveTileSceneEvent::ZMoveTileSceneEvent() noexcept
    : SuperType_()
    , move_params_vector_()
{}

ZMoveTileSceneEvent::~ZMoveTileSceneEvent() noexcept {}

Void ZMoveTileSceneEvent::Destroy() noexcept {
    SuperType_::Destroy();
    move_params_vector_.Clear();
}

NODISCARD ReturnType ZMoveTileSceneEvent::Initialize(
    Float32 _delay_time,
    Float32 _execute_total_time, 
    const lua::LuaTable& _event_params
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK; 
    link_code = SuperType_::Initialize(_delay_time, _execute_total_time, _event_params);
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZSceneEvent::Initialize() link error!");
        return ret_val;
    }

    LogicVector3D target_logic_vector;
    ZTile* tile_ptr;
    link_code = lua::GetLuaObjectFormLuaTable(_event_params, 1, &tile_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"lua::GetLuaObjectFormLuaTable() link error!");
        return ret_val;
    }
    link_code = lua::GetLuaObjectFormLuaTable(_event_params, 2, &target_logic_vector);
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"lua::GetLuaObjectFormLuaTable() link error!");
        return ret_val;
    }

    if (tile_ptr == nullptr) {
        ret_val = error_code::kZSceneEventErrorCode_TileNotExist;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Tile not exist!");
        return ret_val;
    }

    ZLogicTile* logic_tile_ptr = static_cast<ZLogicTile*>(tile_ptr);
    ZDisplayTile* display_tile_ptr = logic_tile_ptr->display_tile_head_ptr_;
    while (display_tile_ptr != nullptr) {
        ZDisplayBoard* display_board_ptr = static_cast<ZDisplayBoard*>(display_tile_ptr->owner_board_ptr_);
        move_params_vector_.PushBack(MoveParams_(
            display_tile_ptr,
            display_tile_ptr->pos_,
            display_board_ptr->CalculateDisplayVectorByLogicVector(target_logic_vector - logic_tile_ptr->pos_)
        ));
    }
    return ret_val;
}

NODISCARD ReturnType ZMoveTileSceneEvent::Execute(Float32 _delta_time) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Execute(_delta_time);
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZSceneEvent::Execute() link error!");
        return ret_val;
    }
    //TODO: Tiles might be destroyed during the event, do something.
    if (move_params_vector_.Size() == 0) {
        link_code = FinishImmediately();
        return ret_val;
    }
    if (WaitingP()) {
        return ret_val;
    }
    //TODO: Change to tiles
    //ZDisplayTile* display_tile_ptr = static_cast<ZDisplayTile*>(tile_ptr_);
    //link_code = display_tile_ptr->SetPos(start_pos_ + move_offset_ * ExecutePercentP());
    //if (link_code != kOK) {
    //    ret_val = error_code::kZSceneEventErrorCode_LinkError;
    //    Z_LOG_ERROR(
    //        ret_val, link_code,
    //        L"ZDisplayTile::SetPos() link error!");
    //    return ret_val;
    //}
    return ret_val;
}

Void ZMoveTileSceneEvent::MoveP(ZMoveTileSceneEvent&& _event) noexcept {
    move_params_vector_ = std::move(_event.move_params_vector_);
    _event.move_params_vector_.Clear();
}

}//tsrpg
}//zengine