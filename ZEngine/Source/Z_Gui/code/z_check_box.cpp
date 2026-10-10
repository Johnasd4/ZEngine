/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE
#include "drive/d_pch.h"

#include "z_check_box.h"

namespace zengine {
namespace gui {

ZCheckBox::ZCheckBox() noexcept
    : SuperType_()
    , name_(string::GenerateString("{}", reinterpret_cast<SizeType>(this)))
    , text_colour_(kDefaultTextColour)
    , check_mark_colour_(kDefaultCheckMarkColour)
    , check_box_colour_(kDefaultCheckBoxColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , click_func_()
    , is_checked_(false)
{}

ZCheckBox::ZCheckBox(ZStringView _name) noexcept
    : SuperType_()
    , name_(_name)
    , text_colour_(kDefaultTextColour)
    , check_mark_colour_(kDefaultCheckMarkColour)
    , check_box_colour_(kDefaultCheckBoxColour)
    , hovered_colour_(kDefaultHoveredColour)
    , active_colour_(kDefaultActiveColour)
    , click_func_()
    , is_checked_(false)
{}

NODISCARD ZWidgetObject::WidgetTypeEnum_ ZCheckBox::GetWidgetType() const noexcept {
    return WidgetTypeEnum_::kCheckBox;
}

Void ZCheckBox::OnTick(Float32 _delta_sec) noexcept {
    if (!visible_) {
        return;
    }

    SuperType_::OnTick(_delta_sec);

    if (pos_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetCursorPos(ImVec2(pos_.x_, pos_.y_));
    }

    ImGui::SetWindowFontScale(GetFontScale());

    // push style colours
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

    if (!enabled_) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Checkbox(name_.GetDataPtr(), &is_checked_)) {
        OnClicked(is_checked_);
    }

    if (!enabled_) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleColor(5);

    // update size
    if (size_state_ == PosSizeStateEnum_::kGet) {
        ImVec2 frame_size = ImGui::GetItemRectSize();
        size_ = GuiSize(frame_size.x, frame_size.y);
        size_state_ = PosSizeStateEnum_::kNone;
    }

    // update pos
    if (pos_state_ == PosSizeStateEnum_::kGet) {
        ImVec2 frame_pos = ImGui::GetItemRectMin();
        ZGuiObject::SetPos(GuiPos(frame_pos.x, frame_pos.y) - owner_ptr_->GetAbsPos());
        pos_state_ = PosSizeStateEnum_::kNone;
    }
}

Void ZCheckBox::OnClicked(Bool _if_checked) noexcept {
    if (click_func_) {
        click_func_(_if_checked);
    }
}

} // gui
} // zengine