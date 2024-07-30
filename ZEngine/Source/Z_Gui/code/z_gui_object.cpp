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

Void ZGuiObject::Begin() noexcept {}
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
Void ZGuiObject::Hide() noexcept { 
    OnHide(); 
}
Void ZGuiObject::Show() noexcept { 
    OnShow(); 
}
Void ZGuiObject::Reset() noexcept {}

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
Void ZGuiObject::SetName(const Char* _name) noexcept {}

NODISCARD Float32 ZGuiObject::Width() const noexcept { return size_.width_; }
NODISCARD Float32 ZGuiObject::Height() const noexcept { return size_.height_; }
NODISCARD GuiSize ZGuiObject::Size() const noexcept { return size_; }
NODISCARD Float32 ZGuiObject::XPos() const noexcept { return pos_.x_; }
NODISCARD Float32 ZGuiObject::YPos() const noexcept { return pos_.y_; }
NODISCARD GuiPos ZGuiObject::Pos() const noexcept { return pos_; }
NODISCARD const Char* ZGuiObject::Name() const noexcept { return ""; }

Void ZGuiObject::OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept {}
Void ZGuiObject::OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {}
Void ZGuiObject::OnHide() noexcept {}
Void ZGuiObject::OnShow() noexcept {}
Void ZGuiObject::OnAdd(ZGuiObject* _owner_ptr) noexcept {
    owner_ptr_ = _owner_ptr;
}

ZGuiObject::ZGuiObject() noexcept 
    : SuperType_()
    , size_()
    , pos_()
    , pre_size_()
    , pre_pos_()
    , size_changed_(false)
    , pos_changed_(false)
    , owner_ptr_(nullptr)
    , enabled_(false) {}

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
    , enabled_(_enabled) {}

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
    _obj.size_ = { 0,0 };
    _obj.pos_ = { 0,0 };
    _obj.pre_size_ = { 0,0 };
    _obj.pre_pos_ = { 0,0 };
    _obj.size_changed_ = false;
    _obj.pos_changed_ = false;
    _obj.owner_ptr_ = nullptr;
    _obj.enabled_ = false;
}

}//gui
}//zengine