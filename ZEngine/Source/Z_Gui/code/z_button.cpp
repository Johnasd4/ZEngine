/*
    Copyright (c) YuLin Zhu (朱雨林)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (朱雨林)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "z_button.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            

ZButton::ZButton() noexcept 
    : SuperType_()
    , button_colour_(kDefaultButtonColour) 
    , button_hovered_colour_(kDefaultButtonHoveredColour)
    , button_active_colour_(kDefaultButtonActiveColour) 
    , click_event_ptr_(nullptr) {}

ZButton::ZButton(ZButton&& _button) noexcept 
    : SuperType_(std::forward<ZButton>(_button)) 
{
    MoveP(std::forward<ZButton>(_button));
}

ZButton::ZButton(const Char* _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , button_colour_(kDefaultButtonColour)
    , button_hovered_colour_(kDefaultButtonHoveredColour)
    , button_active_colour_(kDefaultButtonActiveColour)
    , click_event_ptr_(nullptr) {}

ZButton::~ZButton() noexcept {}

ZButton& ZButton::operator=(ZButton&& _button) noexcept {
    SuperType_::operator=(std::forward<ZButton>(_button));
    MoveP(std::forward<ZButton>(_button));
    return *this;
}

Void ZButton::Tick(Float32 _delta_sec) noexcept {

    if (!Visiable()) {
        return;
    }

    if (Enabled()) {
        SuperType_::Tick(_delta_sec);
    }

    GuiSize cur_size = Size();
    GuiPos cur_pos = Pos();
    ImGui::SetCursorPos(*reinterpret_cast<ImVec2*>(&cur_pos));

    //push background colour
    ImGui::PushStyleColor(ImGuiCol_Button, *reinterpret_cast<ImVec4*>(&button_colour_));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, *reinterpret_cast<ImVec4*>(&button_hovered_colour_));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, *reinterpret_cast<ImVec4*>(&button_active_colour_));
    if (Enabled()) {
        if (ImGui::Button(Name(), *reinterpret_cast<ImVec2*>(&cur_size))) {
            OnClick();
        }
    }
    else {
        ImGui::BeginDisabled();
        ImGui::Button(Name(), *reinterpret_cast<ImVec2*>(&cur_size));
        ImGui::EndDisabled();
    }

    //pop background colour
    ImGui::PopStyleColor(3);
}

Void ZButton::SetButtonColour(GuiColour _colour) noexcept {
    button_colour_ = _colour;
}
Void ZButton::SetButtonHoveredColour(GuiColour _colour) noexcept {
    button_hovered_colour_ = _colour;
}
Void ZButton::SetButtonActiveColour(GuiColour _colour) noexcept {
    button_active_colour_ = _colour;
}

NODISCARD ZButton::TypeEnum_ ZButton::WidgetType() const noexcept {
    return TypeEnum_::kTypeButton;
}

NODISCARD GuiColour ZButton::ButtonColour() const noexcept {
    return button_colour_;
}
NODISCARD GuiColour ZButton::ButtonHoveredColour() const noexcept {
    return button_hovered_colour_;
}
NODISCARD GuiColour ZButton::ButtonActiveColour() const noexcept {
    return button_active_colour_;
}

Void ZButton::OnClick() noexcept {
    if (click_event_ptr_ != nullptr) {
        click_event_ptr_(this);
    }
}

Void ZButton::BindCkickEvent(Void(*_click_event_ptr)(ZButton* _this_ptr)) noexcept {
    click_event_ptr_ = _click_event_ptr;
}

Void ZButton::MoveP(ZButton&& _button) noexcept {
    button_colour_ = _button.button_colour_;
    button_hovered_colour_ = _button.button_hovered_colour_;
    button_active_colour_ = _button.button_active_colour_;
    click_event_ptr_ = _button.click_event_ptr_;
    _button.button_colour_ = kDefaultButtonColour;
    _button.button_hovered_colour_ = kDefaultButtonHoveredColour;
    _button.button_active_colour_ = kDefaultButtonActiveColour;
    _button.click_event_ptr_ = nullptr;
}

}//gui
}//zengine