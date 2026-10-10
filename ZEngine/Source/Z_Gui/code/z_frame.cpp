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

#include "z_frame.h"

namespace zengine {
namespace gui {

ZFrame::ZFrame() noexcept
    : SuperType_()
    , name_(string::GenerateString("{}", reinterpret_cast<SizeType>(this)))
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , flag_(kDefaultFrameFlag)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
{}

ZFrame::ZFrame(ZStringView _name) noexcept
    : SuperType_()
    , name_(_name)
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , flag_(kDefaultFrameFlag)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
{}

ZFrame::ZFrame(
    ZStringView _name,
    GuiSize _size,
    GuiPos _pos
) noexcept
    : SuperType_(_size, _pos)
    , name_(_name)
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , flag_(kDefaultFrameFlag)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
{}

ZFrame::~ZFrame() noexcept {
    RemoveAllWidget();
}

NODISCARD ZWidgetObject::WidgetTypeEnum_ ZFrame::GetWidgetType() const noexcept {
    return ZWidgetObject::WidgetTypeEnum_::kFrame;
}

Void ZFrame::AddWidget(ZWidgetObject* _widget_ptr) noexcept {
    ASSERT(
        _widget_ptr != nullptr,
        "ZWindow::AddWidget: _widget_ptr is nullptr!"
    );

    widget_ptr_array_.PushBack(_widget_ptr);

    _widget_ptr->OnAdd(this);
}

Void ZFrame::RemoveWidget(ZWidgetObject* _widget_ptr) noexcept {
    for (auto iter = widget_ptr_array_.Begin(); iter != widget_ptr_array_.End(); ++iter) {
        if ((*iter) == _widget_ptr) {
            widget_ptr_array_.Erase(iter);
            break;
        }
    }

    _widget_ptr->OnRemove();
}

Void ZFrame::RemoveAllWidget() noexcept {
    for (auto iter = widget_ptr_array_.Begin(); iter != widget_ptr_array_.End(); ++iter) {
        (*iter)->OnRemove();
    }

    widget_ptr_array_.Clear();
}

Void ZFrame::OnTick(Float32 _delta_sec) noexcept {
    if (!visible_) {
        return;
    }

    SuperType_::OnTick(_delta_sec);

    // push id
    ImGui::PushID(reinterpret_cast<const Void*>(this));

    // update pos, must be called every tick to work
    if (pos_state_ == PosSizeStateEnum_::kSet) {    
        ImGui::SetCursorPos(ImVec2(pos_.x_, pos_.y_));
    }

    // push background colour
    ImGui::PushStyleColor(
        ImGuiCol_ChildBg,
        ImVec4(
            background_colour_.red_,
            background_colour_.green_,
            background_colour_.blue_,
            background_colour_.alpha_
        )
    );

    if (!enabled_) {
        ImGui::BeginDisabled();
    }

    // sub frame begin
    if (size_state_ == PosSizeStateEnum_::kSet) {
        ImGui::BeginChild(
            name_.GetDataPtr(),
            ImVec2(size_.width_, size_.height_), 
            true, 
            flag_
        );
    }
    else {
        ImGui::BeginChild(
            name_.GetDataPtr(),
            ImVec2(0.0F, 0.0F), 
            true, 
            flag_
        );
    }

    // tick widgets
    for (auto iter = widget_ptr_array_.Begin(); iter != widget_ptr_array_.End(); ++iter) {
        (*iter)->OnTick(_delta_sec);
    }

    // tick widgets not added
    OnWidgetTick(_delta_sec);

    scroll_pos_.x_ = scroll_pos_.x_ <= scroll_max_pos_.x_ ? scroll_pos_.x_ : scroll_max_pos_.x_;
    scroll_pos_.y_ = scroll_pos_.y_ <= scroll_max_pos_.y_ ? scroll_pos_.y_ : scroll_max_pos_.y_;
    scroll_pos_.x_ = scroll_pos_.x_ >= 0.0F ? scroll_pos_.x_ : 0.0F;
    scroll_pos_.y_ = scroll_pos_.y_ >= 0.0F ? scroll_pos_.y_ : 0.0F;

    if (scroll_x_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetScrollX(scroll_pos_.x_);
        scroll_x_state_ = PosSizeStateEnum_::kGet;
    }
    else if (scroll_x_state_ == PosSizeStateEnum_::kGet) {
        scroll_pos_.x_ = ImGui::GetScrollX();
    }

    if (scroll_y_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetScrollY(scroll_pos_.y_);
        scroll_y_state_ = PosSizeStateEnum_::kGet;
    }
    else if (scroll_y_state_ == PosSizeStateEnum_::kGet) {
        scroll_pos_.y_ = ImGui::GetScrollY();
    }

    scroll_max_pos_.x_ = ImGui::GetScrollMaxX();
    scroll_max_pos_.y_ = ImGui::GetScrollMaxY();

    // sub frame end
    ImGui::EndChild();

    if (!enabled_) {
        ImGui::EndDisabled();
    }

    // pop background colour
    ImGui::PopStyleColor();

    // update size
    if (size_state_ == PosSizeStateEnum_::kGet) {
        ImVec2 frame_size = ImGui::GetItemRectSize();
        size_ = GuiSize(frame_size.x, frame_size.y);
    }

    // update pos
    if (pos_state_ == PosSizeStateEnum_::kGet) {
        ImVec2 frame_pos = ImGui::GetItemRectMin();
        ZGuiObject::SetPos(GuiPos(frame_pos.x, frame_pos.y) - owner_ptr_->GetAbsPos());
    } 

    // pop id
    ImGui::PopID();
}

Void ZFrame::OnWidgetTick(Float32 _delta_sec) noexcept {}

} // gui
} // zengine