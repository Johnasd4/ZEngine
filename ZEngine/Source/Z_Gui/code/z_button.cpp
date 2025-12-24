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
#include "drive/d_pch.h"

#include "z_button.h"

namespace zengine {
namespace gui {

ZButton::ZButton() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , button_colour_(kDefaultButtonColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , button_flag_(kDefaultButtonFlag)
    , click_func_() {}

ZButton::ZButton(ZButton&& _button) noexcept
    : SuperType_(std::forward<ZButton>(_button))
{
    MoveP(std::forward<ZButton>(_button));
}

ZButton::ZButton(ZStringView _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , text_colour_(kDefaultTextColour)
    , button_colour_(kDefaultButtonColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , button_flag_(kDefaultButtonFlag)
    , click_func_() {}

ZButton::~ZButton() noexcept {}

ZButton& ZButton::operator=(ZButton&& _button) noexcept {
    if (this == &_button) {
        return *this;
    }
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

    if (PosSet() && PosChanged()) {
        GuiPos pos = Pos();
        ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));
    }

    SuperType_::Tick(_delta_sec);

    ImGui::SetWindowFontScale(FontScale());

    if (Enabled()) {
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
    }
    else {
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            ImVec4(
                text_colour_.red_ * kDisableColourFactor, 
                text_colour_.green_ * kDisableColourFactor, 
                text_colour_.blue_ * kDisableColourFactor, 
                text_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_Button,
            ImVec4(
                button_colour_.red_ * kDisableColourFactor, 
                button_colour_.green_ * kDisableColourFactor, 
                button_colour_.blue_ * kDisableColourFactor, 
                button_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered,
            ImVec4(
                button_colour_.red_ * kDisableColourFactor, 
                button_colour_.green_ * kDisableColourFactor, 
                button_colour_.blue_ * kDisableColourFactor, 
                button_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive,
            ImVec4(
                button_colour_.red_ * kDisableColourFactor, 
                button_colour_.green_ * kDisableColourFactor, 
                button_colour_.blue_ * kDisableColourFactor, 
                button_colour_.alpha_
            )
        );
    }

    GuiSize size = GetSize();
    if (SizeSet() ? ImGui::Button(Name().GetDataPtr(), ImVec2(size.width_, size.height_)) : ImGui::Button(Name().GetDataPtr())) {
        if (Enabled()) {
            OnButtonClicked();
        }
    }

    ImVec2 temp_pos = ImGui::GetItemRectMin();
    ImVec2 temp_size = ImGui::GetItemRectSize();

    if (OwnerPtr() != nullptr) {
        ZGuiObject::SetPos(GuiPos(temp_pos.x, temp_pos.y) - OwnerPtr()->AbsPos());
    }
    ZGuiObject::SetSize(GuiSize(temp_size.x, temp_size.y));

    ImGui::PopStyleColor(4);
}

Void ZButton::Reset() noexcept {
    SuperType_::Reset();
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

NODISCARD ZButton::WidgetTypeEnum_ ZButton::WidgetType() const noexcept {
    return WidgetTypeEnum_::kButton;
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

Void ZButton::OnButtonClicked() noexcept {
    if (click_func_) {
        click_func_();
    }
}

Void ZButton::MoveP(ZButton&& _button) noexcept {
    click_func_ = std::move(_button.click_func_);
    text_colour_ = _button.text_colour_;
    button_colour_ = _button.button_colour_;
    hovered_colour_ = _button.hovered_colour_;
    active_colour_ = _button.active_colour_;
    button_flag_ = _button.button_flag_;
    _button.text_colour_ = kDefaultTextColour;
    _button.button_colour_ = kDefaultButtonColour;
    _button.hovered_colour_ = kDefaultHoveredColour;
    _button.active_colour_ = kDefaultActiveColour;
    _button.button_flag_ = kDefaultButtonFlag;
}

}//gui
}//zengine