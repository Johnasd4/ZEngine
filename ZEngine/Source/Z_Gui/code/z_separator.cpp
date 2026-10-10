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

#include "z_separator.h"

namespace zengine {
namespace gui {

ZSeparator::ZSeparator() noexcept 
    : SuperType_() 
    , colour_(kDefaultColour)
{}

NODISCARD ZSeparator::WidgetTypeEnum_ ZSeparator::GetWidgetType() const noexcept {
    return WidgetTypeEnum_::kSeparator;
}

Void ZSeparator::OnTick(Float32 _delta_sec) noexcept {
    if (!visible_) {
        return;
    }

    SuperType_::OnTick(_delta_sec);

    if (pos_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetCursorPos(ImVec2(pos_.x_, pos_.y_));
    }

    // push style colours
    ImGui::PushStyleColor(
        ImGuiCol_Separator,
        ImVec4(
            colour_.red_,
            colour_.green_,
            colour_.blue_,
            colour_.alpha_
        )
    );

    if (!enabled_) {
        ImGui::BeginDisabled();
    }

    ImGui::Separator();

    if (!enabled_) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleColor();

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

} // gui
} // zengine