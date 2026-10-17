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

#include "z_window.h"

#include "z_widget_object.h"

namespace zengine {
namespace gui {

ZWindow::ZWindow() noexcept
    : SuperType_()
    , name_(string::GenerateString("{}", reinterpret_cast<SizeType>(this)))
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , flag_(kDefaultWindowFlag)
{}

ZWindow::ZWindow(ZStringView _name) noexcept
    : SuperType_()
    , name_(_name)
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , flag_(kDefaultWindowFlag)
{}

ZWindow::ZWindow(
    ZStringView _name,
    GuiSize _size,
    GuiPos _pos
) noexcept
    : SuperType_(_size, _pos)
    , name_(_name)
    , widget_ptr_array_()
    , background_colour_(kDefaultBackgroundColour)
    , scroll_x_state_(PosSizeStateEnum_::kGet)
    , scroll_y_state_(PosSizeStateEnum_::kGet)
    , scroll_pos_(kBasePos)
    , scroll_max_pos_(kBasePos)
    , flag_(kDefaultWindowFlag)
{}

ZWindow::~ZWindow() noexcept {
    RemoveAllWidget();
}

Void ZWindow::AddWidget(ZWidgetObject* _widget_ptr) noexcept {
    ASSERT(
        _widget_ptr != nullptr,
        "ZWindow::AddWidget: _widget_ptr is nullptr!"
    );

    widget_ptr_array_.PushBack(_widget_ptr);

    _widget_ptr->OnAdd(this);
}

Void ZWindow::RemoveWidget(ZWidgetObject* _widget_ptr) noexcept {
    widget_ptr_array_.EraseFirst(_widget_ptr);

    _widget_ptr->OnRemove();
}

Void ZWindow::RemoveAllWidget() noexcept {
    for (auto& iter : widget_ptr_array_) {
        iter->OnRemove();
    }

    widget_ptr_array_.Clear();
}

Void ZWindow::OnTick(Float32 _delta_sec) noexcept {
    if (!visible_) {
        return;
    }

    // push id
    ImGui::PushID(reinterpret_cast<const Void*>(this));

    // set size
    if (size_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetNextWindowSize(ImVec2(size_.width_, size_.height_));
        size_state_ = PosSizeStateEnum_::kGet;
    }

    // set pos
    if (pos_state_ == PosSizeStateEnum_::kSet) {
        ImGui::SetNextWindowPos(ImVec2(pos_.x_, pos_.y_));
        pos_state_ = PosSizeStateEnum_::kGet;
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

    // begin base window
    if (ImGui::Begin(name_.GetDataPtr(), &visible_, flag_)) {
        // font scale
        ImGui::SetWindowFontScale(GetFontScale());

        // tick widgets
            for (auto& iter : widget_ptr_array_) {
                iter->OnTick(_delta_sec);
            }

        // tick widgets not added
        OnWidgetTick(_delta_sec);

        // update size
        if (size_state_ == PosSizeStateEnum_::kGet) {
            ImVec2 window_size = ImGui::GetWindowSize();
            size_ = GuiSize(window_size.x, window_size.y);
        }

        // update pos
        if (pos_state_ == PosSizeStateEnum_::kGet) {
            ImVec2 window_pos = ImGui::GetWindowPos();
            pos_ = GuiPos(window_pos.x, window_pos.y);
        }

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
    }

    // end base window
    ImGui::End();

    if (!enabled_) {
        // tick widgets not added
        ImGui::EndDisabled();
    }

    // pop background colour
    ImGui::PopStyleColor();

    // pop id
    ImGui::PopID();
}

Void ZWindow::OnWidgetTick(Float32 _delta_sec) noexcept {}

} // gui
} // zengine