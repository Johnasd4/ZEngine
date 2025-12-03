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

#include "f_scene_control.h"

#include "f_scene_control/z_move_tile_scene_event.h"
#include "z_scene_event_manager.h"

namespace zengine {
namespace tsrpg {

TSRPG_DLLAPI Void AddSceneEvent(
    ZSceneEventEnum _event_id,
    Float32 _delay_time,
    Float32 _execute_total_time,
    const lua::LuaTable& _event_params
) noexcept {
    ReturnType link_code = kOK;
    ZSceneEvent* scene_event_ptr = nullptr;
    switch (_event_id) {
    case ZSceneEventEnum::kZSceneEventEnum_MoveTile:
        scene_event_ptr = new ZMoveTileSceneEvent();
        break;
    default:
        Z_LOG_ERROR(
            error_code::kFSceneControlErrorCode_EventIdNotExist, 0,
            L"Event id not exist! event_id: %d", _event_id);
        return;
    }
    link_code = scene_event_ptr->Initialize(_delay_time, _execute_total_time, _event_params);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFSceneControlErrorCode_LinkError, link_code,
            L"ZSceneEvent::Initialize() link error! event_id: %d", _event_id);
        return;
    }
    ZSceneEventManager::Instance().AddSceneEvent(scene_event_ptr);
}

}//tsrpg
}//zengine