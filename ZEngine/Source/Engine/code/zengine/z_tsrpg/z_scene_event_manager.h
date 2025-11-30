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

#include "../z_core/z_object.h"
#include "../z_core/t_list.h"
#include "../z_core/t_pool.h"

namespace zengine {
namespace tsrpg {

/*
    The manager for the scene events.
*/
class TSRPG_DLLAPI ZSceneEventManager : public ZObject {
private:
    static inline constexpr Float32 kDefaultSpeedMult = 1.0F;
    static inline constexpr SizeType kDefaultSceneEventGroupListSize = 126ULL;

public:
    static ZSceneEventManager& Instance() noexcept;
    
    NODISCARD FORCEINLINE Void SetSpeedMult(Bool _speed_mult) noexcept { speed_mult_ = _speed_mult; };
    NODISCARD FORCEINLINE Void SetPause(Bool _if_pause) noexcept { if_pause_ = _if_pause; };

    Void AddSceneEvent(ZSceneEvent* _scene_event_ptr) noexcept;
    Void SetSceneEventGroupInteractive(Bool _if_interactive) noexcept;
    Void AddCurrentSceneEventGroupToExecuteList() noexcept;
    Void FinishAllExecutingSceneEventGroup() noexcept;
    Void Tick(Float32 _delta_time) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    struct SceneEventGroup_ {
        static inline constexpr SizeType kDefaultInstancePoolDefaultSize = 30;

        NODISCARD static TPool<SceneEventGroup_>& InstancePool() noexcept;

        SceneEventGroup_() noexcept;

        TList<ZSceneEvent*> scene_event_list_;
        Bool interactive_;
    };

    ZSceneEventManager(const ZSceneEventManager&) = delete;
    ZSceneEventManager(ZSceneEventManager&&) = delete;
    ZSceneEventManager& operator=(const ZSceneEventManager&) = delete;
    ZSceneEventManager& operator=(ZSceneEventManager&&) = delete;

    ZSceneEventManager() noexcept;
    ~ZSceneEventManager() noexcept;

    TList<SceneEventGroup_*> scene_event_group_execute_list_;
    SceneEventGroup_* temp_scene_event_group_ptr_;
    Float32 speed_mult_;
    Bool if_pause_;
};

}//tsrpg
}//zengine