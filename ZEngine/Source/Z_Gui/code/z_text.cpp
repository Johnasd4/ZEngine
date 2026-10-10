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

#include "z_text.h"

namespace zengine {
namespace gui {

ZText::ZText() noexcept 
    : SuperType_() 
    , text_("")
    , text_colour_(kDefaultTextColour)
    , is_wrapped_(true) 
{}

ZText::ZText(
    ZStringView _text,
    Bool _is_wrapped
) noexcept
    : SuperType_()
    , text_(_text)
    , text_colour_(kDefaultTextColour)
    , is_wrapped_(_is_wrapped)
{}

NODISCARD ZWidgetObject::WidgetTypeEnum_ ZText::GetWidgetType() const noexcept {
    return WidgetTypeEnum_::kText;
}

Void ZText::OnTick(Float32 _delta_sec) noexcept {
    if (!visible_) {
        return;
    }

    SuperType_::OnTick(_delta_sec);

    if (pos_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetCursorPos(ImVec2(pos_.x_, pos_.y_));
    }

    ImGui::SetWindowFontScale(GetFontScale());

    // push style colour
    ImGui::PushStyleColor(
        ImGuiCol_Text,
        ImVec4(
            text_colour_.red_,
            text_colour_.green_,
            text_colour_.blue_,
            text_colour_.alpha_
        )
    );

    if (!enabled_) {
        ImGui::BeginDisabled();
    }

    if (is_wrapped_) {
        if (size_.width_ > 0.0f) {
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + size_.width_);
            ImGui::Text("%s", text_.GetDataPtr());
            ImGui::PopTextWrapPos();
        }
        else {
            ImGui::TextWrapped("%s", text_.GetDataPtr());
        }
    }
    else {
        ImGui::Text("%s", text_.GetDataPtr());
    }

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