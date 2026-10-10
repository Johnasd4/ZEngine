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

#include "z_gui_object.h"

namespace zengine {
namespace gui {

NODISCARD GuiSize ZGuiObject::GetSize() const noexcept { return size_; }

NODISCARD GuiPos ZGuiObject::GetPos() const noexcept { return pos_; }

NODISCARD GuiPos ZGuiObject::GetAbsPos() const noexcept {
    return owner_ptr_ == nullptr ? pos_ : pos_ + owner_ptr_->GetAbsPos();
}

NODISCARD Float32 ZGuiObject::GetFontScale() const noexcept {
    return owner_ptr_ == nullptr ? font_scale_ : font_scale_ * owner_ptr_->GetFontScale();
}

Void ZGuiObject::SetVisible(Bool _visible) noexcept { 
    visible_ = _visible; 
}

Void ZGuiObject::SetSize(GuiSize _size) noexcept {
    size_ = _size;
    size_state_ = _size != kBaseSize ? PosSizeStateEnum_::kSet : PosSizeStateEnum_::kGet;
}

Void ZGuiObject::SetPos(GuiPos _pos) noexcept {
    pos_ = _pos;
    pos_state_ = _pos != kBasePos ? PosSizeStateEnum_::kSet : PosSizeStateEnum_::kGet;
}

Void ZGuiObject::SetAbsPos(GuiPos _pos) noexcept {
    if (owner_ptr_ == nullptr) {
        SetPos(_pos);
    }
    else {
        SetPos(_pos - owner_ptr_->GetAbsPos());
    }
}

Void ZGuiObject::SetFontScale(Float32 _scale) noexcept { font_scale_ = _scale; }

Void ZGuiObject::OnAdd(ZGuiObject* _owner_ptr) noexcept {
    owner_ptr_ = _owner_ptr;
}

Void ZGuiObject::OnRemove() noexcept {
    owner_ptr_ = nullptr;
}

ZGuiObject::ZGuiObject() noexcept
    : SuperType_()
    , size_(kBaseSize)
    , pos_(kBasePos)
    , size_state_(PosSizeStateEnum_::kGet)
    , pos_state_(PosSizeStateEnum_::kGet)
    , enabled_(true)
    , visible_(true)
    , font_scale_(kBaseFontScale)
    , owner_ptr_(nullptr) 
{}

ZGuiObject::ZGuiObject(GuiSize _size, GuiPos _pos) noexcept
    : SuperType_()
    , size_(_size)
    , pos_(_pos)
    , size_state_(_size != kBaseSize ? PosSizeStateEnum_::kSet : PosSizeStateEnum_::kGet)
    , pos_state_(_pos != kBasePos ? PosSizeStateEnum_::kSet : PosSizeStateEnum_::kGet)
    , enabled_(true)
    , visible_(true)
    , font_scale_(kBaseFontScale)
    , owner_ptr_(nullptr) 
{}

} // gui
} // zengine