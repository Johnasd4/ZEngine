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

#include "../z_core/z_string_view.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZTextErrorCodeEnum : ReturnType {
    kZTextErrorCode_LinkError = kErrorCodeBase_ZText,
    kZTextErrorCode_SystemError,
    kZTextErrorCode_NullptrParam,
    kZTextErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace gui {

/*
    Text class.
*/
class GUI_DLLAPI ZText : public ZWidgetObject {
public:
    static inline constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };

    ZText() noexcept;
    ZText(ZText&& _text) noexcept;
    ZText(ZStringView _text, GuiPos _pos = kBasePos) noexcept;

    virtual ~ZText() noexcept;

    ZText& operator=(ZText&& _text) noexcept;

    FORCEINLINE Void SetIfWarp(Bool _if_warp) noexcept { if_wrap_ = _if_warp; }

    NODISCARD FORCEINLINE Bool IfWarp() const noexcept { return if_wrap_; }

    /*
        Called before objects first tick.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    virtual Void SetText(ZStringView _text) noexcept;
    virtual Void SetTextColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual const ZString& Text() const noexcept;
    NODISCARD virtual GuiColour TextColour() const noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZText(const ZText&) = delete;
    ZText& operator=(const ZText&) = delete;

    Void MoveP(ZText&& _text) noexcept;

    GuiColour text_colour_;
    Bool if_wrap_;
};

}//gui
}//zengine