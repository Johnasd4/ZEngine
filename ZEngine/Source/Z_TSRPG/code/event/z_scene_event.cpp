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

#include "f_scene_control/z_scene_event.h"

#include "z_math/f_basic_math.h"

namespace zengine {
namespace tsrpg {

ZSceneEvent::ZSceneEvent() noexcept 
    : SuperType_()
    , delay_time_(0.0f)
    , execute_total_time_(0.0f)
    , execute_total_time_rep_(0.0f)
    , execute_time_(0.0f) {}

ZSceneEvent::~ZSceneEvent() noexcept {}

NODISCARD Bool ZSceneEvent::Finished() noexcept {
    return execute_time_ >= execute_total_time_;
}

NODISCARD ReturnType ZSceneEvent::FinishImmediately() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    execute_time_ = execute_total_time_ * 2;    //execute_time_ * execute_total_time_rep_ >= 1
    link_code = Execute(0.0f);
    if (link_code != kOK) {
        ret_val = error_code::kZSceneEventErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZSceneEvent::Execute() link error!");
        return ret_val;
    }
    return ret_val;
}

Void ZSceneEvent::Destroy() noexcept {
    delay_time_ = 0.0f;
    execute_total_time_ = 0.0f;
    execute_total_time_rep_ = 0.0f;
    execute_time_ = 0.0f;
}

NODISCARD ReturnType ZSceneEvent::Initialize(
    Float32 _delay_time, Float32 _execute_total_time, const lua::LuaTable& _event_params) noexcept 
{
    ReturnType ret_val = kOK;
    delay_time_ = _delay_time;
    execute_total_time_ = _execute_total_time;
    execute_total_time_rep_ = execute_total_time_ == 0.0f ? 0.0f : (1.0f / execute_total_time_);
    return ret_val;
}

NODISCARD ReturnType ZSceneEvent::Execute(Float32 _delta_time) noexcept {
    ReturnType ret_val = kOK;
    if (delay_time_ > 0.0f) {
        delay_time_ -= _delta_time;
        if (delay_time_ < 0.0f) {
            execute_time_ -= delay_time_;
        }
    }
    else {
        execute_time_ += _delta_time;
    }
    return ret_val;
}

NODISCARD Float32 ZSceneEvent::ExecutePercentP() noexcept {
    return execute_total_time_rep_ == 0.0f ? 1.0f : math::Min(1.0f, execute_time_ * execute_total_time_rep_);
}

NODISCARD Bool ZSceneEvent::WaitingP() noexcept {
    return delay_time_ > 0.0f;
}

Void ZSceneEvent::MoveP(ZSceneEvent&& _event) noexcept {
    delay_time_ = _event.delay_time_;
    execute_total_time_ = _event.execute_total_time_;
    execute_total_time_rep_ = _event.execute_total_time_rep_;
    execute_time_ = _event.execute_time_;
    _event.delay_time_ = 0.0f;
    _event.execute_total_time_ = 0.0f;
    _event.execute_total_time_rep_ = 0.0f;
    _event.execute_time_ = 0.0f;
}

}//tsrpg
}//zengine