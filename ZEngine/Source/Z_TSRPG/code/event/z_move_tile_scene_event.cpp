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
#include "Z_world_board.h"
#include "z_world_tile.h"

namespace zengine {
namespace tsrpg {

ZMoveTileSceneEvent::ZMoveTileSceneEvent() noexcept
    : SuperType_()
    , tile_ptr_() 
    , start_pos_()
    , move_offset_()
{}

ZMoveTileSceneEvent::~ZMoveTileSceneEvent() noexcept {}

Void ZMoveTileSceneEvent::Destroy() noexcept {
    SuperType_::Destroy();
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
    ZLogicBoard* logic_board_ptr = static_cast<ZLogicBoard*>(tile_ptr->OwnerBoardPtr());
    ZWorldBoard* world_board_ptr = logic_board_ptr->world_board_ptr_;
    if (world_board_ptr == nullptr) {
        ret_val = error_code::kZSceneEventErrorCode_WorldBoardNotExist;
        Z_LOG_ERROR(
            ret_val, 0,
            L"World board not exist!");
        return ret_val;
    }

    tile_ptr_ = (*world_board_ptr)(logic_tile_ptr->Index());
    ZWorldTile* world_tile_ptr = static_cast<ZWorldTile*>(tile_ptr_);
    start_pos_ = world_tile_ptr->Pos();
    move_offset_ = world_board_ptr->CalculateWorldVectorByLogicVector(target_logic_vector - logic_tile_ptr->pos_);
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
    if (WaitingP()) {
        return ret_val;
    }
    ZWorldTile* world_tile_ptr = static_cast<ZWorldTile*>(tile_ptr_);
    link_code = world_tile_ptr->SetPos(start_pos_ + move_offset_ * ExecutePercentP());
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZWorldTile::SetPos() link error!");
        return ret_val;
    }
    return ret_val;
}

Void ZMoveTileSceneEvent::MoveP(ZMoveTileSceneEvent&& _event) noexcept {
    tile_ptr_ = _event.tile_ptr_;
    start_pos_ = _event.start_pos_;
    move_offset_ = _event.move_offset_;
    _event.tile_ptr_ = nullptr;
}

}//tsrpg
}//zengine