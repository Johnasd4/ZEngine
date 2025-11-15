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

#include "z_check_box.h"

namespace zengine {
namespace gui {

ZCheckBox::ZCheckBox() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , check_mark_colour_(kDefaultCheckMarkColour)
    , check_box_colour_(kDefaultCheckBoxColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , click_func_() 
    , if_checked_(false)
    , pre_if_checked_(false)
{}

ZCheckBox::ZCheckBox(ZCheckBox&& _button) noexcept
    : SuperType_(std::forward<ZCheckBox>(_button))
{
    MoveP(std::forward<ZCheckBox>(_button));
}

ZCheckBox::ZCheckBox(const Char* _name, GuiPos _pos) noexcept
    : SuperType_(_name, kBaseSize, _pos)
    , text_colour_(kDefaultTextColour)
    , check_mark_colour_(kDefaultCheckMarkColour)
    , check_box_colour_(kDefaultCheckBoxColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , click_func_()
    , if_checked_(false)
    , pre_if_checked_(false)
{}

ZCheckBox::~ZCheckBox() noexcept {}

ZCheckBox& ZCheckBox::operator=(ZCheckBox&& _button) noexcept {
    SuperType_::operator=(std::forward<ZCheckBox>(_button));
    MoveP(std::forward<ZCheckBox>(_button));
    return *this;
}

Void ZCheckBox::Begin() noexcept {
    SuperType_::Begin();
}

Void ZCheckBox::Tick(Float32 _delta_sec) noexcept {
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
            ImVec4(text_colour_.red_, 
                text_colour_.green_, 
                text_colour_.blue_, 
                text_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_CheckMark,
            ImVec4(
                check_mark_colour_.red_, 
                check_mark_colour_.green_, 
                check_mark_colour_.blue_, 
                check_mark_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBg,
            ImVec4(
                check_box_colour_.red_, 
                check_box_colour_.green_, 
                check_box_colour_.blue_, 
                check_box_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBgHovered,
            ImVec4(
                hovered_colour_.red_, 
                hovered_colour_.green_, 
                hovered_colour_.blue_, 
                hovered_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBgActive,
            ImVec4(
                active_colour_.red_, 
                active_colour_.green_, 
                active_colour_.blue_, 
                active_colour_.alpha_
            )
        );
    }
    else {
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            ImVec4(text_colour_.red_ * kDisableColourFactor,
                text_colour_.green_ * kDisableColourFactor,
                text_colour_.blue_ * kDisableColourFactor,
                text_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_CheckMark,
            ImVec4(
                check_mark_colour_.red_ * kDisableColourFactor,
                check_mark_colour_.green_ * kDisableColourFactor,
                check_mark_colour_.blue_ * kDisableColourFactor,
                check_mark_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBg,
            ImVec4(
                check_box_colour_.red_ * kDisableColourFactor,
                check_box_colour_.green_ * kDisableColourFactor,
                check_box_colour_.blue_ * kDisableColourFactor,
                check_box_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBgHovered,
            ImVec4(
                hovered_colour_.red_ * kDisableColourFactor,
                hovered_colour_.green_ * kDisableColourFactor,
                hovered_colour_.blue_ * kDisableColourFactor,
                hovered_colour_.alpha_
            )
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBgActive,
            ImVec4(
                active_colour_.red_ * kDisableColourFactor,
                active_colour_.green_ * kDisableColourFactor,
                active_colour_.blue_ * kDisableColourFactor,
                active_colour_.alpha_
            )
        );
    }

    ImGui::Checkbox(Name(), &if_checked_);
    if (if_checked_ != pre_if_checked_) {
        OnCheckBoxClicked(if_checked_);
        pre_if_checked_ = if_checked_;
    }

    ImVec2 temp_pos = ImGui::GetItemRectMin();
    ImVec2 temp_size = ImGui::GetItemRectSize();

    if (OwnerPtr() != nullptr) {
        ZGuiObject::SetPos(GuiPos(temp_pos.x, temp_pos.y) - OwnerPtr()->AbsPos());
    }
    ZGuiObject::SetSize(GuiSize(temp_size.x, temp_size.y));

    ImGui::PopStyleColor(5);
}

Void ZCheckBox::Reset() noexcept {
    SuperType_::Reset();
}

Void ZCheckBox::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

Void ZCheckBox::SetCheckMarkColour(GuiColour _colour) noexcept {
    check_mark_colour_ = _colour;
}

Void ZCheckBox::SetCheckBoxColour(GuiColour _colour) noexcept {
    check_box_colour_ = _colour;
}

Void ZCheckBox::SetHoveredColour(GuiColour _colour) noexcept {
    hovered_colour_ = _colour;
}

Void ZCheckBox::SetActiveColour(GuiColour _colour) noexcept {
    active_colour_ = _colour;
}

NODISCARD ZCheckBox::WidgetTypeEnum_ ZCheckBox::WidgetType() const noexcept {
    return WidgetTypeEnum_::kWidgetType_CheckBox;
}

NODISCARD GuiColour ZCheckBox::TextColour() const noexcept {
    return text_colour_;
}

NODISCARD GuiColour ZCheckBox::CheckMarkColour() const noexcept {
    return check_mark_colour_;
}

NODISCARD GuiColour ZCheckBox::CheckBoxColour() const noexcept {
    return check_box_colour_;
}

NODISCARD GuiColour ZCheckBox::HoveredColour() const noexcept {
    return hovered_colour_;
}

NODISCARD GuiColour ZCheckBox::ActiveColour() const noexcept {
    return active_colour_;
}

Void ZCheckBox::OnCheckBoxClicked(Bool _if_checked) noexcept {
    if (click_func_) {
        click_func_(_if_checked);
    }
}

Void ZCheckBox::MoveP(ZCheckBox&& _button) noexcept {
    click_func_ = std::move(_button.click_func_);
    text_colour_ = _button.text_colour_;
    check_mark_colour_ = _button.check_mark_colour_;
    check_box_colour_ = _button.check_box_colour_;
    hovered_colour_ = _button.hovered_colour_;
    active_colour_ = _button.active_colour_;
    if_checked_ = _button.if_checked_;
    pre_if_checked_ = _button.pre_if_checked_;
    _button.text_colour_ = kDefaultTextColour;
    _button.check_mark_colour_ = kDefaultCheckMarkColour;
    _button.check_box_colour_ = kDefaultCheckBoxColour;
    _button.hovered_colour_ = kDefaultHoveredColour;
    _button.active_colour_ = kDefaultActiveColour;
    _button.if_checked_ = false;
    _button.pre_if_checked_ = false;
}

}//gui
}//zengine