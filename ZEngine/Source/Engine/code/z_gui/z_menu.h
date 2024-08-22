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
#ifndef Z_GUI_Z_MENU_H_
#define Z_GUI_Z_MENU_H_

#include "internal/z_drive.h"

#include "../z_core/z_string.h"

#include "z_gui_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZMenuErrorCode : ReturnType {
    kZMenuErrorCodeLinkError = kErrorCodeBaseZMenu,
    kZMenuErrorCodeFrameAreadyCreated,
    kZMenuErrorCodeFrameNotExist
};

}//error_code

/*
    Frame class.
*/
class GUI_DLLAPI ZMenu : public ZGuiObject {
public:
    static constexpr GuiColour kDefaultMenuColour = { 0.4f, 0.4f, 0.4f, 1.0f };
    static constexpr GuiColour kDefaultMenuHoveredColour = { 0.2f, 0.2f, 0.2f, 1.0f };
    static constexpr GuiColour kDefaultMenuActiveColour = { 0.3f, 0.3f, 0.3f, 1.0f };

    ZMenu() noexcept;
    ZMenu(ZMenu&& _button) noexcept;
    ZMenu(const Char* _name, GuiSize _size, GuiPos _pos) noexcept;

    ~ZMenu() noexcept;

    ZMenu& operator=(ZMenu&& _button) noexcept;

    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;

    virtual Void SetMenuColour(GuiColour _colour) noexcept;
    virtual Void SetMenuHoveredColour(GuiColour _colour) noexcept;
    virtual Void SetMenuActiveColour(GuiColour _colour) noexcept;

    NODISCARD virtual TypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual GuiColour MenuColour() const noexcept;
    NODISCARD virtual GuiColour MenuHoveredColour() const noexcept;
    NODISCARD virtual GuiColour MenuActiveColour() const noexcept;

    virtual Void OnClick() noexcept;

    Void BindCkickEvent(Void(*_click_event_ptr)(ZMenu* _this_ptr)) noexcept;  

protected:
    using SuperType_ = ZGuiAdjustableObject;

private:
    ZMenu(const ZMenu&) = delete;
    ZMenu& operator=(const ZMenu&) = delete;

    Void MoveP(ZMenu&& _button) noexcept;

    Void(*click_event_ptr_)(ZMenu* _this_ptr);

    GuiColour button_colour_;
    GuiColour button_hovered_colour_;
    GuiColour button_active_colour_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_MENU_H_