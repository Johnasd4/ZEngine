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

#include "internal/z_drive.h"

#include "z_widget_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZButtonErrorCode : ReturnType {
    kZButtonErrorCode_LinkError = kErrorCodeBase_ZButton
};

}//error_code

/*
    Button class.
*/
class GUI_DLLAPI ZButton : public ZWidgetObject {
public:
    /*
        The input text display mode enum.
    */
    enum ButtonFlagEnum_ {
        kButtonFlag_None = 0,
        // React on left mouse button
        kButtonFlag_Left = 1 << 0,
        // React on right mouse button
        kButtonFlag_Right = 1 << 1,
        // React on center mouse button
        kButtonFlag_Middle = 1 << 2,
        // React on all mouse button
        kButtonFlag_All = kButtonFlag_Left | kButtonFlag_Right | kButtonFlag_Middle
    };

    static constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr GuiColour kDefaultButtonColour = { 0.3f, 0.3f, 0.3f, 1.0f };
    static constexpr GuiColour kDefaultHoveredColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr GuiColour kDefaultActiveColour = { 0.4f, 0.4f, 0.4f, 1.0f };
    static constexpr Float32 kDefaultFontScale = 1.0f;
    static constexpr Int32 kDefaultButtonFlag = kButtonFlag_Left;

    ZButton() noexcept;
    ZButton(ZButton&& _button) noexcept;
    /*
        Add ## at the start of the name if you want an empty input text.
    */
    ZButton(const Char* _name) noexcept;
    /*
        Add ## at the start of the name if you want an empty input text.
    */
    ZButton(const Char* _name, GuiSize _size, GuiPos _pos) noexcept;

    ~ZButton() noexcept;

    ZButton& operator=(ZButton&& _button) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    FORCEINLINE Void SetButtonFlag(Int32 _button_flag) noexcept { button_flag_ = _button_flag; }
    /*
        Can only set one flag at a time.
    */
    FORCEINLINE Void SetButtonFlag(ButtonFlagEnum_ _button_flag, Bool _if_active) noexcept {
        _if_active ? button_flag_ |= _button_flag : button_flag_ &= ~_button_flag;
    }

    NODISCARD FORCEINLINE Int32 ButtonFlag() const noexcept { return button_flag_; }

    /*
        Will be called when the application executes.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every text, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    virtual Void SetWidth(Float32 _width) noexcept;
    virtual Void SetHeight(Float32 _height) noexcept;
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetXPos(Float32 _x_pos) noexcept;
    virtual Void SetYPos(Float32 _y_pos) noexcept;
    virtual Void SetPos(GuiPos _pos) noexcept;
    virtual Void SetTextColour(GuiColour _colour) noexcept;
    virtual Void SetButtonColour(GuiColour _colour) noexcept;
    virtual Void SetHoveredColour(GuiColour _colour) noexcept;
    virtual Void SetActiveColour(GuiColour _colour) noexcept;
    virtual Void SetFontScale(Float32 _scale) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual GuiColour TextColour() const noexcept;
    NODISCARD virtual GuiColour ButtonColour() const noexcept;
    NODISCARD virtual GuiColour HoveredColour() const noexcept;
    NODISCARD virtual GuiColour ActiveColour() const noexcept;
    NODISCARD virtual Float32 FontScale() const noexcept;

    virtual Void OnButtonClicked() noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZButton(const ZButton&) = delete;
    ZButton& operator=(const ZButton&) = delete;

    Void MoveP(ZButton&& _text) noexcept;

    GuiColour text_colour_;
    GuiColour button_colour_;
    GuiColour hovered_colour_;
    GuiColour active_colour_;
    Float32 font_scale_;
    Int32 button_flag_;
    Bool if_pos_set_;
    Bool if_size_set_;
};

}//gui
}//zengine