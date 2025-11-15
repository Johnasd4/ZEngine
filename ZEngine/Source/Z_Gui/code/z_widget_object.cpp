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

#include "z_widget_object.h"

namespace zengine {
namespace gui {

Void ZWidgetObject::Tick(Float32 _delta_sec) noexcept {
    SuperType_::Tick(_delta_sec);
    if (SameLine()) {
        if (same_line_pos_offset_ == kBasePos) {
            ImGui::SameLine();
        }
        else {
            ImGui::SameLine(same_line_pos_offset_.x_, same_line_pos_offset_.y_);
        }
    }
}

Void ZWidgetObject::ReleaseResource() noexcept {
    name_.Clear();
    name_.ShrinkToFit();
}

Void ZWidgetObject::SetWidth(Float32 _width) noexcept {
    SuperType_::SetWidth(_width);
    size_set_ = true;
}
Void ZWidgetObject::SetHeight(Float32 _height) noexcept {
    SuperType_::SetHeight(_height);
    size_set_ = true;
}
Void ZWidgetObject::SetSize(GuiSize _size) noexcept {
    SuperType_::SetSize(_size);
    size_set_ = true;
}
Void ZWidgetObject::SetPosX(Float32 _pos_x) noexcept {
    SuperType_::SetPosX(_pos_x);
    pos_set_ = true;
}
Void ZWidgetObject::SetPosY(Float32 _pos_y) noexcept {
    SuperType_::SetPosY(_pos_y);
    pos_set_ = true;
}
Void ZWidgetObject::SetPos(GuiPos _pos) noexcept {
    SuperType_::SetPos(_pos);
    pos_set_ = true;
}

Void ZWidgetObject::SetName(const Char* _name) noexcept { name_ = _name; }

NODISCARD const Char* ZWidgetObject::Name() const noexcept { return name_.String(); }

Void ZWidgetObject::OnHide() noexcept {
    SuperType_::OnHide();
    visiable_ = false;
}
Void ZWidgetObject::OnShow() noexcept {
    SuperType_::OnShow();
    visiable_ = true;
}

ZWidgetObject::ZWidgetObject() noexcept 
    : SuperType_()
    , priority_(kDefaultPriority)
    , visiable_(true)
    , size_set_(false)
    , pos_set_(false)
    , same_line_(false)
    , same_line_pos_offset_(kBasePos)
    , name_("")
{}

ZWidgetObject::ZWidgetObject(
    const Char* _name,
    GuiSize _size,
    GuiPos _pos,
    Int32 _priority,
    Bool _visiable,
    Bool _enabled
) noexcept
    : SuperType_(_size, _pos, _enabled)
    , priority_(_priority)
    , visiable_(_visiable)
    , size_set_(_size != kBaseSize)
    , pos_set_(_pos != kBasePos)
    , same_line_(false)
    , same_line_pos_offset_(kBasePos)
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
    priority_ = _obj.priority_;
    visiable_ = _obj.visiable_;
    size_set_ = _obj.size_set_;
    pos_set_ = _obj.pos_set_;
    same_line_ = _obj.same_line_;
    same_line_pos_offset_ = _obj.same_line_pos_offset_;
    name_ = std::move(_obj.name_);
    _obj.priority_ = kDefaultPriority;
    _obj.visiable_ = false;
    _obj.size_set_ = false;
    _obj.pos_set_ = false;
    _obj.same_line_ = false;
    _obj.same_line_pos_offset_ = kBasePos;
}

}//gui
}//zengine