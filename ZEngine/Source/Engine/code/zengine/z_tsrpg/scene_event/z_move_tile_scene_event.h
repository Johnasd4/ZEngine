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

#include "z_scene_event.h"

#include "../../z_core/t_array.h"

namespace zengine {
namespace tsrpg {

/*
    The scene event base class.
*/
class TSRPG_DLLAPI ZMoveTileSceneEvent : public ZSceneEvent {
public:
    ZMoveTileSceneEvent() noexcept;

    virtual ~ZMoveTileSceneEvent() noexcept;

    virtual Void Destroy() noexcept;

    NODISCARD virtual ReturnType Initialize(
        Float32 _delay_time, 
        Float32 _execute_total_time, 
        const lua::LuaTable& _event_params
    ) noexcept;

    NODISCARD virtual ReturnType Execute(Float32 _delta_time) noexcept;

protected:
    using SuperType_ = ZSceneEvent;

private:    
    struct MoveParams_ {
        ZDisplayTile* tile_ptr_;
        DisplayVector3D start_pos_;
        DisplayVector3D move_offset_;
    };

    ZMoveTileSceneEvent(const ZMoveTileSceneEvent&) = delete;
    ZMoveTileSceneEvent(ZMoveTileSceneEvent&&) = delete;
    ZMoveTileSceneEvent& operator=(const ZMoveTileSceneEvent&) = delete;
    ZMoveTileSceneEvent& operator=(ZMoveTileSceneEvent&&) = delete;

    Void MoveP(ZMoveTileSceneEvent&& _event) noexcept;

    TArray<MoveParams_> move_params_array_;
};

}//tsrpg
}//zengine