/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_GUI_Z_BUTTON_H_
#define Z_GUI_Z_BUTTON_H_

#include "internal/z_drive.h"

#include "../z_core/z_string.h"

#include "z_widget_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZButtonErrorCode : ReturnType {
    kZButtonErrorCodeLinkError = kErrorCodeBaseZButton,
    kZButtonErrorCodeFrameAreadyCreated,
    kZButtonErrorCodeFrameNotExist
};

}//error_code

/*
    Frame class.
*/
class GUI_DLLAPI ZButton : public ZWidgetObject {
public:
    static constexpr GuiColour kDefaultButtonColour = { 0.4f, 0.4f, 0.4f, 1.0f };
    static constexpr GuiColour kDefaultButtonHoveredColour = { 0.2f, 0.2f, 0.2f, 1.0f };
    static constexpr GuiColour kDefaultButtonActiveColour = { 0.3f, 0.3f, 0.3f, 1.0f };

    ZButton() noexcept;
    ZButton(ZButton&& _button) noexcept;
    ZButton(const Char* _name, GuiSize _size, GuiPos _pos) noexcept;

    ~ZButton() noexcept;

    ZButton& operator=(ZButton&& _button) noexcept;

    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;

    virtual Void SetButtonColour(GuiColour _colour) noexcept;
    virtual Void SetButtonHoveredColour(GuiColour _colour) noexcept;
    virtual Void SetButtonActiveColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept;

    NODISCARD virtual GuiColour ButtonColour() const noexcept;
    NODISCARD virtual GuiColour ButtonHoveredColour() const noexcept;
    NODISCARD virtual GuiColour ButtonActiveColour() const noexcept;

    virtual Void OnClick() noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZButton(const ZButton&) = delete;
    ZButton& operator=(const ZButton&) = delete;

    Void MoveP(ZButton&& _button) noexcept;

    GuiColour button_colour_;
    GuiColour button_hovered_colour_;
    GuiColour button_active_colour_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_BUTTON_H_