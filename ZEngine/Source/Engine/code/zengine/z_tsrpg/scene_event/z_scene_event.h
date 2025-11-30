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

#include "../drive.h"

#include "../../z_core/z_object.h"
#include "../../z_lua/f_lua.h"

namespace zengine {
namespace error_code {
enum ZSceneEventErrorCodeEnum : ReturnType {
    kZSceneEventErrorCode_LinkError = kErrorCodeBase_ZSceneEvent,
    kZSceneEventErrorCode_TileNotExist,
    kZSceneEventErrorCode_DisplayBoardNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The scene event base class.
*/
class TSRPG_DLLAPI ZSceneEvent : public ZObject {
public:
    static inline constexpr Float32 ExecuteFinishPercent = 1.0f;

    ZSceneEvent() noexcept;

    virtual ~ZSceneEvent() noexcept;
    
    NODISCARD Bool Finished() noexcept;
    NODISCARD ReturnType FinishImmediately() noexcept;

    virtual Void Destroy() noexcept;

    NODISCARD virtual ReturnType Initialize(
        Float32 _delay_time, 
        Float32 _execute_total_time, 
        const lua::LuaTable& _event_params
    ) noexcept;
    
    NODISCARD virtual ReturnType Execute(Float32 _delta_time) noexcept;

protected:
    using SuperType_ = ZObject;
     
    NODISCARD Float32 ExecutePercentP() noexcept;
    NODISCARD Bool WaitingP() noexcept;

private:
    ZSceneEvent(const ZSceneEvent&) = delete;
    ZSceneEvent(ZSceneEvent&&) = delete;
    ZSceneEvent& operator=(const ZSceneEvent&) = delete;
    ZSceneEvent& operator=(ZSceneEvent&&) = delete;

    Void MoveP(ZSceneEvent&& _event) noexcept;

    Float32 delay_time_;
    Float32 execute_total_time_;
    Float32 execute_total_time_rep_;
    Float32 execute_time_;
};

}//tsrpg
}//zengine