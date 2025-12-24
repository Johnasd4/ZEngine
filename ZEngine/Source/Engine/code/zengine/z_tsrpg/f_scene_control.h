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
#pragma once

#include "drive.h"

#include "../z_lua/f_lua.h"

#include "f_scene_control/z_scene_event.h"

namespace zengine {
namespace error_code {
enum FSceneControlErrorCodeEnum : ReturnType {
    kFSceneControlErrorCode_LinkError = kErrorCodeBase_FSceneControl,
    kFSceneControlErrorCode_SystemOrLibraryError,
    kFSceneControlErrorCode_NullptrParam,
    kFSceneControlErrorCode_ParamOutOfRange,
    kFSceneControlErrorCode_LuaObjectNotValid,
    kFSceneControlErrorCode_EventIdNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

enum class ZSceneEventEnum : SizeType {
    kZSceneEventEnum_MoveTile,
};

TSRPG_DLLAPI Void AddSceneEvent(
    ZSceneEventEnum _event_id, 
    Float32 _delay_time, 
    Float32 _execute_total_time, 
    const lua::LuaTable& _event_params
) noexcept;
TSRPG_DLLAPI Void SetScenePause() noexcept;
TSRPG_DLLAPI Void SetSceneSpeedMult() noexcept;
TSRPG_DLLAPI Void FinishAllSceneEvent() noexcept;

}//tsrpg
}//zengine