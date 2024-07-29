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

#include "z_widget_object.h"

namespace zengine {
namespace gui {

Void ZWidgetObject::Begin() noexcept { SuperType_::Begin(); }
Void ZWidgetObject::Tick(Float32 _delta_sec) noexcept {
    SuperType_::Tick(_delta_sec);
    if (size_changed_) {
        OnResize(pre_size_.width_, pre_size_.height_, size_.width_, size_.height_);
        size_changed_ = false;
    }
    if (size_changed_) {
        OnMove(pre_pos_.x_, pre_pos_.y_, pos_.x_, pos_.y_);
        size_changed_ = false;
    }
}
Void ZWidgetObject::Hide() noexcept { 
    SuperType_::Hide();
    visiable_ = false;
}
Void ZWidgetObject::Show() noexcept { 
    SuperType_::Show();
    visiable_ = true;
}
Void ZWidgetObject::Reset() noexcept { SuperType_::Reset(); }

Void ZWidgetObject::SetWidth(Int32 _width) noexcept {
    pre_size_ = size_;
    size_.width_ = _width;
    size_changed_ = true;
}
Void ZWidgetObject::SetHeight(Int32 _height) noexcept {
    pre_size_ = size_;
    size_.height_ = _height;
    size_changed_ = true;
}
Void ZWidgetObject::SetSize(Int32 _width, Int32 _height) noexcept {
    pre_size_ = size_;
    size_.width_ = _width;
    size_.height_ = _height;
    size_changed_ = true;
}
Void ZWidgetObject::SetXPos(Int32 _x_pos) noexcept {
    pre_pos_ = pos_;
    pos_.x_ = _x_pos;
    pos_changed_ = true;
}
Void ZWidgetObject::SetYPos(Int32 _y_pos) noexcept {
    pre_pos_ = pos_;
    pos_.y_ = _y_pos;
    pos_changed_ = true;
}
Void ZWidgetObject::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {
    pre_pos_ = pos_;
    pos_.x_ = _x_pos;
    pos_.y_ = _y_pos;
    pos_changed_ = true;
}
Void ZWidgetObject::SetName(const Char* _name) noexcept { name_ = _name; }

NODISCARD Int32 ZWidgetObject::Width() const noexcept { return size_.width_; }
NODISCARD Int32 ZWidgetObject::Height() const noexcept { return size_.height_; }
NODISCARD GuiSize ZWidgetObject::Size() const noexcept { return size_; }
NODISCARD Int32 ZWidgetObject::XPos() const noexcept { return pos_.x_; }
NODISCARD Int32 ZWidgetObject::YPos() const noexcept { return pos_.y_; }
NODISCARD GuiPos ZWidgetObject::Pos() const noexcept { return pos_; }
NODISCARD const Char* ZWidgetObject::Name() const noexcept { return name_.String(); }

Void ZWidgetObject::OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept {
    SuperType_::OnMove(_pre_x, _pre_y, _cur_x, _cur_y);
}
Void ZWidgetObject::OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept {
    SuperType_::OnResize(_pre_width, _pre_height, _cur_width, _cur_height);
}
Void ZWidgetObject::OnHide() noexcept {}
Void ZWidgetObject::OnShow() noexcept {}
Void ZWidgetObject::OnAdd(ZGuiObject* _owner_ptr) noexcept { SuperType_::OnAdd(_owner_ptr); }

ZWidgetObject::ZWidgetObject() noexcept 
    : SuperType_()
    , size_()
    , pos_()
    , pre_size_()
    , pre_pos_()
    , size_changed_(false)
    , pos_changed_(false)
    , visiable_(false)
    , priority_(0)
    , name_("")
{}

ZWidgetObject::ZWidgetObject(
    const Char* _name,
    GuiSize _size,
    GuiPos _pos,
    Bool _visiable,
    Int32 _priority,
    Bool _if_tick,
    Bool _enabled
) noexcept
    : SuperType_(_if_tick, _enabled)
    , size_(_size)
    , pos_(_pos)
    , pre_size_()
    , pre_pos_()
    , size_changed_(false)
    , pos_changed_(false)
    , visiable_(_visiable)
    , priority_(_priority)
    , name_(_name)
{}

ZWidgetObject::ZWidgetObject(ZWidgetObject&& _obj) noexcept 
    : SuperType_(std::move(_obj)) 
{ 
    MoveP(std::forward<ZWidgetObject>(_obj)); 
}

ZWidgetObject& ZWidgetObject::operator=(ZWidgetObject&& _obj) noexcept {
    SuperType_::operator=(std::move(_obj));
    MoveP(std::forward<ZWidgetObject>(_obj));
    return *this;
}

Void ZWidgetObject::MoveP(ZWidgetObject&& _obj) noexcept {
    size_ = _obj.size_;
    pos_ = _obj.pos_;
    pre_size_ = _obj.pre_size_;
    pre_pos_ = _obj.pre_pos_;
    size_changed_ = _obj.size_changed_;
    pos_changed_ = _obj.pos_changed_;
    visiable_ = _obj.visiable_;
    priority_ = _obj.priority_;
    name_ = std::move(_obj.name_);
    _obj.size_ = { 0,0 };
    _obj.pos_ = { 0,0 };
    _obj.pre_size_ = { 0,0 };
    _obj.pre_pos_ = { 0,0 };
    _obj.size_changed_ = false;
    _obj.pos_changed_ = false;
    _obj.visiable_ = false;
    _obj.priority_ = 0;
}

}//gui
}//zengine