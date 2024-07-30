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
    , visiable_(false)
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
    name_ = std::move(_obj.name_);
    _obj.priority_ = kDefaultPriority;
    _obj.visiable_ = false;
}

}//gui
}//zengine