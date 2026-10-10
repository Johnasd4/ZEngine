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

#include "z_input_text.h"

namespace zengine {
namespace gui {

ZInputText::ZInputText() noexcept
    : SuperType_()
    , name_(string::GenerateString("{}", reinterpret_cast<SizeType>(this)))
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_changed_func_()
    , input_text_buffer_(kDefaultInputTextSize)
    , input_text_length_(0)
    , flag_(kDefaultInputTextFlag)
    , is_multiline_(false)
    , is_auto_extend_buffer_(true)
{
    input_text_buffer_.At<Char>(0) = '\0';
}

ZInputText::ZInputText(ZStringView _name) noexcept
    : SuperType_()
    , name_(_name)
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_changed_func_()
    , input_text_buffer_(kDefaultInputTextSize)
    , input_text_length_(0)
    , flag_(kDefaultInputTextFlag)
    , is_multiline_(false)
    , is_auto_extend_buffer_(true)
{
    input_text_buffer_.At<Char>(0) = '\0';
}

NODISCARD ZWidgetObject::WidgetTypeEnum_ ZInputText::GetWidgetType() const noexcept {
    return WidgetTypeEnum_::kInputText;
}

Void ZInputText::SetInputText(ZStringView _input_text) noexcept {
    if (input_text_buffer_.GetSize() <= _input_text.GetSize()) {
        input_text_buffer_.Resize(_input_text.GetSize() + 1ULL);
    }
    Copy(input_text_buffer_.GetDataPtr<Char>(), _input_text.GetDataPtr(), _input_text.GetSize());
    input_text_buffer_.At<Char>(_input_text.GetSize()) = '\0';
    input_text_length_ = static_cast<Int32>(_input_text.GetSize());
}

Void ZInputText::OnTick(Float32 _delta_sec) noexcept {
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
        ImVec4(
            text_colour_.red_,
            text_colour_.green_,
            text_colour_.blue_,
            text_colour_.alpha_
        )
    );
    ImGui::PushStyleColor(
        ImGuiCol_FrameBg,
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

    Int32 flag = flag_ | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackResize;
    if (is_multiline_) {
        ImGui::InputTextMultiline(
            name_.GetDataPtr(),
            input_text_buffer_.GetDataPtr<Char>(),
            input_text_buffer_.GetSize(),
            ImVec2(size_.width_, size_.height_),
            flag,
            InputTextCallbackFunc,
            this
        );
    }
    else {
        if (size_state_ == PosSizeStateEnum_::kSet) {
            ImGui::SetNextItemWidth(size_.width_);
        }
        ImGui::InputText(
            name_.GetDataPtr(),
            input_text_buffer_.GetDataPtr<Char>(),
            input_text_buffer_.GetSize(),
            flag,
            InputTextCallbackFunc,
            this
        );
    }

    if (!enabled_) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleColor(2);

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

Void ZInputText::OnInputTextChanged() noexcept {
    if (input_text_changed_func_) {
        input_text_changed_func_(ZStringView(input_text_buffer_.GetDataPtr<const Char>(), input_text_length_));
    }
}

Int32 ZInputText::InputTextCallbackFunc(ImGuiInputTextCallbackData* _data_ptr) noexcept {
    ZInputText* input_text_ptr = reinterpret_cast<ZInputText*>(_data_ptr->UserData);
    if (_data_ptr->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        // Resize callback
        input_text_ptr->input_text_buffer_.Resize(
            Max(input_text_ptr->input_text_length_ * kInputTextBufferExtendFactor, _data_ptr->BufSize)
        );
        _data_ptr->Buf = input_text_ptr->input_text_buffer_.GetDataPtr<Char>();
    }
    else if (_data_ptr->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        input_text_ptr->input_text_length_ = _data_ptr->BufTextLen;
        input_text_ptr->OnInputTextChanged();
    }
    return 0;
}

} // gui
} // zengine