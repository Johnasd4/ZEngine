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

#include "z_scene_event_manager.h"

#include "f_scene_control/z_scene_event.h"

namespace zengine {
namespace tsrpg {

ZSceneEventManager& ZSceneEventManager::Instance() noexcept {
    static ZSceneEventManager manager;
    return manager;
}

Void ZSceneEventManager::AddSceneEvent(ZSceneEvent* _scene_event_ptr) noexcept {
    temp_scene_event_group_ptr_->scene_event_list_.PushBack(_scene_event_ptr);
}

Void ZSceneEventManager::SetSceneEventGroupInteractive(Bool _if_interactive) noexcept {
    temp_scene_event_group_ptr_->interactive_ = true;
}

Void ZSceneEventManager::AddCurrentSceneEventGroupToExecuteList() noexcept {
    scene_event_group_execute_list_.PushBack(temp_scene_event_group_ptr_);
    temp_scene_event_group_ptr_ = SceneEventGroup_::InstancePool().Apply();
}

Void ZSceneEventManager::FinishAllExecutingSceneEventGroup() noexcept {
    ReturnType link_code = kOK;
    while(!scene_event_group_execute_list_.IsEmpty()) {
        SceneEventGroup_* event_group_ptr = scene_event_group_execute_list_.Front();
        TList<ZSceneEvent*>& event_list = event_group_ptr->scene_event_list_;
        while (!event_list.IsEmpty()) {
            ZSceneEvent* event_ptr = event_list.Front();
            link_code = event_ptr->FinishImmediately();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSceneEventErrorCode_LinkError, link_code,
                    "ZSceneEvent::FinishImmediately() link error!");
            }
            delete event_ptr;
            event_list.PopFront();
        }
        event_list.Clear();
        event_group_ptr->interactive_ = false;
        scene_event_group_execute_list_.PopFront();
        SceneEventGroup_::InstancePool().Release(event_group_ptr);
    }
}

Void ZSceneEventManager::Tick(Float32 _delta_time) noexcept {
    if (scene_event_group_execute_list_.IsEmpty() || if_pause_) {
        return;
    }
    Float32 tick_time = _delta_time * speed_mult_;
    ReturnType link_code = kOK;
    auto event_group_ptr = scene_event_group_execute_list_.Begin();
    Bool if_finish = (*event_group_ptr)->interactive_;
    do {
        TList<ZSceneEvent*>& event_list = (*event_group_ptr)->scene_event_list_;
        auto event_ptr = event_list.Begin();
        while (event_ptr != event_list.End()) {
            link_code = (*event_ptr)->Execute(tick_time);
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZSceneEventErrorCode_LinkError, link_code,
                    "ZSceneEvent::Execute() link error!");
                (*event_ptr)->Destroy();
            }
            if ((*event_ptr)->Finished()) {
                delete (*event_ptr);
                event_ptr = event_list.Erase(event_ptr);
            }
            else {
                ++event_ptr;
            }
        }
        if (event_list.IsEmpty()) {
            SceneEventGroup_::InstancePool().Release(*event_group_ptr);
            event_group_ptr = scene_event_group_execute_list_.Erase(event_group_ptr);
        }
        else {
            ++event_group_ptr;
        }
        if (event_group_ptr == scene_event_group_execute_list_.End()) {
            if_finish = true;
        }
        else {
            if_finish = (*event_group_ptr)->interactive_;
        }
    } while (if_finish);
}

ZSceneEventManager::ZSceneEventManager() noexcept 
    : SuperType_()
    , scene_event_group_execute_list_()
    , speed_mult_(kDefaultSpeedMult)
    , if_pause_(false)
{   
    SceneEventGroup_::InstancePool().ExtendPool(SceneEventGroup_::kDefaultInstancePoolDefaultSize);
    temp_scene_event_group_ptr_ = SceneEventGroup_::InstancePool().Apply();
}

ZSceneEventManager::~ZSceneEventManager() noexcept {}

ZSceneEventManager::SceneEventGroup_::SceneEventGroup_() noexcept
    : scene_event_list_()
    , interactive_(false) {}

NODISCARD TPool<ZSceneEventManager::SceneEventGroup_>& ZSceneEventManager::SceneEventGroup_::InstancePool() noexcept {
    static TPool<ZSceneEventManager::SceneEventGroup_> pool;
    return pool;
}

}//tsrpg
}//zengine