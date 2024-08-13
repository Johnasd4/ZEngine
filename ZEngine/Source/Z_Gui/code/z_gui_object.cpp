/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "z_gui_object.h"

namespace zengine {
namespace gui {

NODISCARD ZMutex& ZGuiObject::OpenGLMutex() noexcept {
    static ZMutex opengl_mutex;
    return opengl_mutex;
}

Void ZGuiObject::Begin() noexcept {}
Void ZGuiObject::Tick(Float32 _delta_sec) noexcept {}
Void ZGuiObject::Reset() noexcept {}
Void ZGuiObject::Hide() noexcept { 
    visiable_ = false;
    OnHide(); 
}
Void ZGuiObject::Show() noexcept { 
    visiable_ = true;
    OnShow(); 
}

Void ZGuiObject::SetName(const Char* _name) noexcept { name_ = _name; }

NODISCARD const Char* ZGuiObject::Name() const noexcept { return name_.String(); }


Void ZGuiObject::OnAdd(ZGuiObject* _owner_ptr) noexcept { owner_ptr_ = _owner_ptr; }
Void ZGuiObject::OnHide() noexcept {
    if (hide_event_ptr_ != nullptr) {
        hide_event_ptr_();
    }
    
}
Void ZGuiObject::OnShow() noexcept {
    if (show_event_ptr_ != nullptr) {
        show_event_ptr_();
    }
}
Void ZGuiObject::OnKeyDown(KeyEnum _clicked_button, Int32 _mods) noexcept {
    if (key_down_event_ptr_ != nullptr) {
        key_down_event_ptr_(this, _clicked_button, _mods);
    }
}
Void ZGuiObject::OnKeyUp(KeyEnum _clicked_button, Int32 _mods) noexcept {
    if (key_up_event_ptr_ != nullptr) {
        key_up_event_ptr_(this, _clicked_button, _mods);
    }
}
Void ZGuiObject::OnKeyPress(KeyEnum _clicked_button, Int32 _mods) noexcept {
    if (key_press_event_ptr_ != nullptr) {
        key_press_event_ptr_(this, _clicked_button, _mods);
    }
}
Void ZGuiObject::OnMouseDown(MouseButtonEnum _clicked_button, Int32 _mods) noexcept {
    if (mouse_down_event_ptr_ != nullptr) {
        mouse_down_event_ptr_(this, _clicked_button, _mods);
    }
}
Void ZGuiObject::OnMouseUp(MouseButtonEnum _clicked_button, Int32 _mods) noexcept {
    if (mouse_up_event_ptr_ != nullptr) {
        mouse_up_event_ptr_(this, _clicked_button, _mods);
    }
}
Void ZGuiObject::OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept {
    if (scroll_move_event_ptr_ != nullptr) {
        scroll_move_event_ptr_(this, _x_offset, _y_offset);
    }
}
Void ZGuiObject::OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {
    if (mouse_move_event_ptr_ != nullptr) {
        mouse_move_event_ptr_(this, _pre_pos, _cur_pos);
    }
}

Void ZGuiObject::BindAddEvent(
    Void(*_add_event_ptr)(ZGuiObject* _this_ptr, ZGuiObject* _owner_ptr)
) noexcept {
    add_event_ptr_ = _add_event_ptr;
}
Void ZGuiObject::BindHideEvent(
    Void(*_hide_event_ptr)(ZGuiObject* _this_ptr)
) noexcept {
    hide_event_ptr_ = _hide_event_ptr;
}
Void ZGuiObject::BindShowEvent(
    Void(*_show_event_ptr)(ZGuiObject* _this_ptr)
) noexcept {
    show_event_ptr_ = _show_event_ptr;
}
Void ZGuiObject::BindKeyDownEvent(
    Void(*_key_down_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
) noexcept {
    key_down_event_ptr_ = _key_down_event_ptr;
}
Void ZGuiObject::BindKeyUpEvent(
    Void(*_key_up_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
) noexcept {
    key_up_event_ptr_ = _key_up_event_ptr;
}
Void ZGuiObject::BindKeyPressEvent(
    Void(*_key_press_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
) noexcept {
    key_press_event_ptr_ = _key_press_event_ptr;
}
Void ZGuiObject::BindMouseDownEvent(
    Void(*_mouse_down_event_ptr)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods)
) noexcept {
    mouse_down_event_ptr_ = _mouse_down_event_ptr;
}
Void ZGuiObject::BindMouseUpEvent(
    Void(*_mouse_up_event_ptr)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods)
) noexcept {
    mouse_up_event_ptr_ = _mouse_up_event_ptr;
}
Void ZGuiObject::BindScrollMoveEvent(
    Void(*_scroll_move_event_ptr)(ZGuiObject* _this_ptr, Float32 _x_offset, Float32 _y_offset)
) noexcept {
    scroll_move_event_ptr_ = _scroll_move_event_ptr;
}
Void ZGuiObject::BindMouseMoveEvent(
    Void(*_mouse_move_event_ptr)(ZGuiObject* _this_ptr, GuiPos _pre_pos, GuiPos _cur_pos)
) noexcept {
    mouse_move_event_ptr_ = _mouse_move_event_ptr;
}

ZGuiObject::ZGuiObject() noexcept 
    : SuperType_()
    , owner_ptr_(nullptr)
    , name_()
    , enabled_(false)
    , visiable_(true)
    , priority_(0)
    , add_event_ptr_(nullptr)
    , hide_event_ptr_(nullptr)
    , show_event_ptr_(nullptr)
    , key_down_event_ptr_(nullptr)
    , key_up_event_ptr_(nullptr)
    , key_press_event_ptr_(nullptr)
    , mouse_down_event_ptr_(nullptr)
    , mouse_up_event_ptr_(nullptr)
    , scroll_move_event_ptr_(nullptr)
    , mouse_move_event_ptr_(nullptr) {}

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
    , pre_size_()
    , pre_pos_()
    , size_changed_(true)
    , pos_changed_(true)
    , owner_ptr_(nullptr)
    , enabled_(_enabled)
    , resize_event_ptr_(nullptr)
    , move_event_ptr_(nullptr)
    , hide_event_ptr_(nullptr)
    , show_event_ptr_(nullptr)
    , add_event_ptr_(nullptr)
    , key_down_event_ptr_(nullptr)
    , key_up_event_ptr_(nullptr)
    , key_press_event_ptr_(nullptr)
    , mouse_down_event_ptr_(nullptr)
    , mouse_up_event_ptr_(nullptr)
    , scroll_move_event_ptr_(nullptr)
    , mouse_move_event_ptr_(nullptr) {}

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
    owner_ptr_ = _obj.owner_ptr_;
    enabled_ = _obj.enabled_;
    resize_event_ptr_ = _obj.resize_event_ptr_;
    move_event_ptr_ = _obj.move_event_ptr_;
    hide_event_ptr_ = _obj.hide_event_ptr_;
    show_event_ptr_ = _obj.show_event_ptr_;
    add_event_ptr_ = _obj.add_event_ptr_;
    key_down_event_ptr_ = _obj.key_down_event_ptr_;
    key_up_event_ptr_ = _obj.key_up_event_ptr_;
    key_press_event_ptr_ = _obj.key_press_event_ptr_;
    mouse_down_event_ptr_ = _obj.mouse_down_event_ptr_;
    mouse_up_event_ptr_ = _obj.mouse_up_event_ptr_;
    scroll_move_event_ptr_ = _obj.scroll_move_event_ptr_;
    mouse_move_event_ptr_ = _obj.mouse_move_event_ptr_;
    _obj.size_ = { 0,0 };
    _obj.pos_ = { 0,0 };
    _obj.pre_size_ = { 0,0 };
    _obj.pre_pos_ = { 0,0 };
    _obj.size_changed_ = false;
    _obj.pos_changed_ = false;
    _obj.owner_ptr_ = nullptr;
    _obj.enabled_ = false;
    _obj.resize_event_ptr_ = nullptr;
    _obj.move_event_ptr_ = nullptr;
    _obj.hide_event_ptr_ = nullptr;
    _obj.show_event_ptr_ = nullptr;
    _obj.add_event_ptr_ = nullptr;
    _obj.key_down_event_ptr_ = nullptr;
    _obj.key_up_event_ptr_ = nullptr;
    _obj.key_press_event_ptr_ = nullptr;
    _obj.mouse_down_event_ptr_ = nullptr;
    _obj.mouse_up_event_ptr_ = nullptr;
    _obj.scroll_move_event_ptr_ = nullptr;
    _obj.mouse_move_event_ptr_ = nullptr;
}
Void ZGuiObject::Tick(Float32 _delta_sec) noexcept {
    pre_size_ = size_;
    pre_pos_ = pos_;
    if (size_changed_) {
        OnResize(pre_size_, size_);
        size_changed_ = false;
    }
    if (pos_changed_) {
        OnMove(pre_pos_, pos_);
        pos_changed_ = false;
    }
}


Void ZGuiObject::SetWidth(Float32 _width) noexcept {
    size_.width_ = _width;
    size_changed_ = true;
}
Void ZGuiObject::SetHeight(Float32 _height) noexcept {
    size_.height_ = _height;
    size_changed_ = true;
}
Void ZGuiObject::SetSize(GuiSize _size) noexcept {
    size_ = _size;
    size_changed_ = true;
}
Void ZGuiObject::SetXPos(Float32 _x_pos) noexcept {
    pos_.x_ = _x_pos;
    pos_changed_ = true;
}
Void ZGuiObject::SetYPos(Float32 _y_pos) noexcept {
    pos_.y_ = _y_pos;
    pos_changed_ = true;
}
Void ZGuiObject::SetPos(GuiPos _pos) noexcept {
    pos_ = _pos;
    pos_changed_ = true;
}

NODISCARD Float32 ZGuiObject::Width() const noexcept { return size_.width_; }
NODISCARD Float32 ZGuiObject::Height() const noexcept { return size_.height_; }
NODISCARD GuiSize ZGuiObject::Size() const noexcept { return size_; }
NODISCARD Float32 ZGuiObject::XPos() const noexcept { return pos_.x_; }
NODISCARD Float32 ZGuiObject::YPos() const noexcept { return pos_.y_; }
NODISCARD GuiPos ZGuiObject::Pos() const noexcept { return pos_; }
NODISCARD GuiPos ZGuiObject::AbsPos() const noexcept { return pos_ + owner_ptr_->AbsPos(); }
Void ZGuiObject::OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept {
    if (resize_event_ptr_ != nullptr) {
        resize_event_ptr_(_pre_size, _cur_size);
    }
}
Void ZGuiObject::OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {
    if (move_event_ptr_ != nullptr) {
        move_event_ptr_(_pre_pos, _cur_pos);
    }
}
}//gui
}//zengine