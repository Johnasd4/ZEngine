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

#include "z_gui_object.h"

namespace zengine {
namespace gui {

NODISCARD ZMutex& ZGuiObject::OpenGLMutex() noexcept {
    static ZMutex opengl_mutex;
    return opengl_mutex;
}

NODISCARD ZGuiObject*& ZGuiObject::CallbackGuiObjectPtr() noexcept {
    thread_local ZGuiObject* gui_object_ptr;
    return gui_object_ptr;
}

Void ZGuiObject::Begin() noexcept {}
Void ZGuiObject::Tick(Float32 _delta_sec) noexcept {
    if (if_first_tick_) {
        if_first_tick_ = false;
        Begin();
    }

    if (size_changed_) {
        OnResize(pre_size_, size_);
        size_changed_ = false;
    }
    if (pos_changed_) {
        OnMove(pre_pos_, pos_);
        pos_changed_ = false;
    }
    pre_size_ = size_;
    pre_pos_ = pos_;
}
Void ZGuiObject::Hide() noexcept { 
    OnHide(); 
}
Void ZGuiObject::Show() noexcept { 
    OnShow(); 
}
Void ZGuiObject::Reset() noexcept {}

Void ZGuiObject::SetWidth(Float32 _width) noexcept {
    size_changed_ = (pre_size_.width_ != _width);
    size_.width_ = _width;
}
Void ZGuiObject::SetHeight(Float32 _height) noexcept {
    size_changed_ = (pre_size_.height_ != _height);
    size_.height_ = _height;
}
Void ZGuiObject::SetSize(GuiSize _size) noexcept {
    size_changed_ = (pre_size_ != _size);
    size_ = _size;
}
Void ZGuiObject::SetPosX(Float32 _pos_x) noexcept {
    pos_changed_ = (pre_pos_.x_ != _pos_x);
    pos_.x_ = _pos_x;
}
Void ZGuiObject::SetPosY(Float32 _pos_y) noexcept {
    pos_changed_ = (pre_pos_.y_ != _pos_y);
    pos_.y_ = _pos_y;
}
Void ZGuiObject::SetPos(GuiPos _pos) noexcept {
    pos_changed_ = (pre_pos_ != _pos);
    pos_ = _pos;
}
Void ZGuiObject::SetAbsPos(GuiPos _pos) noexcept {
    if (owner_ptr_ == nullptr) {
        SetPos(_pos);
    }
    else {
        SetPos(_pos - owner_ptr_->AbsPos());
    }
}
Void ZGuiObject::SetFontScale(Float32 _scale) noexcept { font_scale_ = _scale; }

NODISCARD Float32 ZGuiObject::Width() const noexcept { return size_.width_; }
NODISCARD Float32 ZGuiObject::Height() const noexcept { return size_.height_; }
NODISCARD GuiSize ZGuiObject::GetSize() const noexcept { return size_; }
NODISCARD Float32 ZGuiObject::PosX() const noexcept { return pos_.x_; }
NODISCARD Float32 ZGuiObject::PosY() const noexcept { return pos_.y_; }
NODISCARD GuiPos ZGuiObject::Pos() const noexcept { return pos_; }
NODISCARD GuiPos ZGuiObject::AbsPos() const noexcept { return pos_ + owner_ptr_->AbsPos(); }
NODISCARD Float32 ZGuiObject::FontScale() const noexcept {
    return owner_ptr_ == nullptr ? font_scale_ : font_scale_ * owner_ptr_->FontScale();
}

Void ZGuiObject::OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept {}
Void ZGuiObject::OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {}
Void ZGuiObject::OnHide() noexcept {}
Void ZGuiObject::OnShow() noexcept {}
Void ZGuiObject::OnAdd(ZGuiObject* _owner_ptr) noexcept {
    owner_ptr_ = _owner_ptr;
    if_first_tick_ = true;
}
Void ZGuiObject::OnRemove(ZGuiObject* _owner_ptr) noexcept {
    owner_ptr_ = nullptr;
}

Void ZGuiObject::OnKeyDown(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {}
Void ZGuiObject::OnKeyUp(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {}
Void ZGuiObject::OnKeyPress(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {}
Void ZGuiObject::OnMouseDown(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {}
Void ZGuiObject::OnMouseUp(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {}
Void ZGuiObject::OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept {}
Void ZGuiObject::OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {}

ZGuiObject::ZGuiObject() noexcept 
    : SuperType_()
    , size_(kBaseSize)
    , pos_(kBasePos)
    , pre_size_(kBaseSize)
    , pre_pos_(kBasePos)
    , size_changed_(false)
    , pos_changed_(false)
    , enabled_(true)
    , if_first_tick_(true)
    , font_scale_(true)
    , owner_ptr_(nullptr) {}

ZGuiObject::ZGuiObject(ZGuiObject&& _obj) noexcept 
    : SuperType_(std::move(_obj)) 
{ 
    MoveP(std::forward<ZGuiObject>(_obj)); 
}

ZGuiObject::ZGuiObject(
    GuiSize _size,
    GuiPos _pos,
    Bool _enabled
) noexcept
    : SuperType_()
    , size_(_size)
    , pos_(_pos)
    , pre_size_(kBaseSize)
    , pre_pos_(kBasePos)
    , size_changed_(_size != kBaseSize)
    , pos_changed_(_pos != kBasePos)
    , enabled_(_enabled)
    , if_first_tick_(true)
    , font_scale_(true)
    , owner_ptr_(nullptr) {}

ZGuiObject::~ZGuiObject() noexcept {}

ZGuiObject& ZGuiObject::operator=(ZGuiObject&& _obj) noexcept {
    SuperType_::operator=(std::move(_obj));
    MoveP(std::forward<ZGuiObject>(_obj));
    return *this;
}

Void ZGuiObject::MoveP(ZGuiObject&& _obj) noexcept {
    size_ = _obj.size_;
    pos_ = _obj.pos_;
    pre_size_ = _obj.pre_size_;
    pre_pos_ = _obj.pre_pos_;
    size_changed_ = _obj.size_changed_;
    pos_changed_ = _obj.pos_changed_;
    enabled_ = _obj.enabled_;
    if_first_tick_ = _obj.if_first_tick_;
    font_scale_ = _obj.font_scale_;
    owner_ptr_ = _obj.owner_ptr_;
    _obj.size_ = { 0,0 };
    _obj.pos_ = { 0,0 };
    _obj.pre_size_ = { 0,0 };
    _obj.pre_pos_ = { 0,0 };
    _obj.size_changed_ = false;
    _obj.pos_changed_ = false;
    _obj.enabled_ = false;
    _obj.if_first_tick_ = true;
    _obj.font_scale_ = kBaseFontScale;
    _obj.owner_ptr_ = nullptr;
}

}//gui
}//zengine