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
#define GUI_DLLFILE

#include "z_button.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

ZButton::ZButton() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , button_colour_(kDefaultButtonColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , font_scale_(kDefaultFontScale)
    , button_flag_(kDefaultButtonFlag)
    , if_pos_set_(false)
    , if_size_set_(false) {}

ZButton::ZButton(ZButton&& _button) noexcept
    : SuperType_(std::forward<ZButton>(_button))
{
    MoveP(std::forward<ZButton>(_button));
}

ZButton::ZButton(const Char* _name) noexcept
    : SuperType_(_name, { 0.0f, 0.0f }, { 0.0f, 0.0f })
    , text_colour_(kDefaultTextColour)
    , button_colour_(kDefaultButtonColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , font_scale_(kDefaultFontScale)
    , button_flag_(kDefaultButtonFlag)
    , if_pos_set_(false)
    , if_size_set_(false) {}

ZButton::ZButton(const Char* _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , text_colour_(kDefaultTextColour)
    , button_colour_(kDefaultButtonColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , font_scale_(kDefaultFontScale)
    , button_flag_(kDefaultButtonFlag)
    , if_pos_set_(true)
    , if_size_set_(true) {}

ZButton::~ZButton() noexcept {}

ZButton& ZButton::operator=(ZButton&& _button) noexcept {
    SuperType_::operator=(std::forward<ZButton>(_button));
    MoveP(std::forward<ZButton>(_button));
    return *this;
}

Void ZButton::Begin() noexcept {
    SuperType_::Begin();
}

Void ZButton::Tick(Float32 _delta_sec) noexcept {
    if (!Visiable()) {
        return;
    }

    if (Enabled()) {
        if (if_pos_set_) {
            GuiPos pos = Pos();
            ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));
        }

        SuperType_::Tick(_delta_sec);

        ImGui::PushStyleColor(
            ImGuiCol_Text, 
            ImVec4(text_colour_.red_, text_colour_.green_, text_colour_.blue_, text_colour_.alpha_)
        );
        ImGui::PushStyleColor(
            ImGuiCol_Button, 
            ImVec4(button_colour_.red_, button_colour_.green_, button_colour_.blue_, button_colour_.alpha_)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered, 
            ImVec4(hovered_colour_.red_, hovered_colour_.green_, hovered_colour_.blue_, hovered_colour_.alpha_)
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive, 
            ImVec4(active_colour_.red_, active_colour_.green_, active_colour_.blue_, active_colour_.alpha_)
        );

        ImGui::SetWindowFontScale(font_scale_ * kFontScaleMultFactor);

        GuiSize size = Size();
        if (if_size_set_ ? ImGui::Button(Name(), ImVec2(size.width_, size.height_)) : ImGui::Button(Name())) {
            OnButtonClicked();
        }

        ImGui::PopStyleColor(4);
    }
}

Void ZButton::Reset() noexcept {
    SuperType_::Reset();
}

Void ZButton::SetWidth(Float32 _width) noexcept {
    ZGuiObject::SetWidth(_width);
    if_size_set_ = true;
}

Void ZButton::SetHeight(Float32 _height) noexcept {
    ZGuiObject::SetHeight(_height);
    if_size_set_ = true;
}

Void ZButton::SetSize(GuiSize _size) noexcept {
    ZGuiObject::SetSize(_size);
    if_size_set_ = true;
}

Void ZButton::SetXPos(Float32 _x_pos) noexcept {
    ZGuiObject::SetXPos(_x_pos);
    if_pos_set_ = true;
}

Void ZButton::SetYPos(Float32 _y_pos) noexcept {
    ZGuiObject::SetYPos(_y_pos);
    if_pos_set_ = true;
}

Void ZButton::SetPos(GuiPos _pos) noexcept {
    ZGuiObject::SetPos(_pos);
    if_pos_set_ = true;
}

Void ZButton::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

Void ZButton::SetButtonColour(GuiColour _colour) noexcept {
    button_colour_ = _colour;
}

Void ZButton::SetHoveredColour(GuiColour _colour) noexcept {
    hovered_colour_ = _colour;
}

Void ZButton::SetActiveColour(GuiColour _colour) noexcept {
    active_colour_ = _colour;
}

Void ZButton::SetFontScale(Float32 _scale) noexcept {
    font_scale_ = _scale;
}

NODISCARD ZButton::WidgetTypeEnum ZButton::WidgetType() const noexcept {
    return WidgetTypeEnum::kWidgetType_Button;
}

NODISCARD GuiColour ZButton::TextColour() const noexcept {
    return text_colour_;
}

NODISCARD GuiColour ZButton::ButtonColour() const noexcept {
    return button_colour_;
}

NODISCARD GuiColour ZButton::HoveredColour() const noexcept {
    return hovered_colour_;
}

NODISCARD GuiColour ZButton::ActiveColour() const noexcept {
    return active_colour_;
}

NODISCARD Float32 ZButton::FontScale() const noexcept {
    return font_scale_;
} 

Void ZButton::OnButtonClicked() noexcept {}

Void ZButton::MoveP(ZButton&& _button) noexcept {
    text_colour_ = _button.text_colour_;
    button_colour_ = _button.button_colour_;
    hovered_colour_ = _button.hovered_colour_;
    active_colour_ = _button.active_colour_;
    font_scale_ = _button.font_scale_;
    button_flag_ = _button.button_flag_;
    if_pos_set_ = _button.if_pos_set_;
    if_size_set_ = _button.if_size_set_;
    _button.text_colour_ = kDefaultTextColour;
    _button.button_colour_ = kDefaultButtonColour;
    _button.hovered_colour_ = kDefaultHoveredColour;
    _button.active_colour_ = kDefaultActiveColour;
    _button.font_scale_ = kDefaultFontScale;
    _button.button_flag_ = kDefaultButtonFlag;
    _button.if_pos_set_ = false;
    _button.if_size_set_ = false;
}

}//gui
}//zengine