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
Void ZGuiObject::Tick(Float32 _delta_sec) noexcept {}
Void ZGuiObject::Hide() noexcept { OnHide(); }
Void ZGuiObject::Show() noexcept { OnShow(); }

Void ZGuiObject::SetWidth(Int32 _width) noexcept {}
Void ZGuiObject::SetHeight(Int32 _height) noexcept {}
Void ZGuiObject::SetXPos(Int32 _x_pos) noexcept {}
Void ZGuiObject::SetYPos(Int32 _y_pos) noexcept {}
Void ZGuiObject::SetSize(Int32 _width, Int32 _height) noexcept {}
Void ZGuiObject::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {}
Void ZGuiObject::SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept {}
Void ZGuiObject::SetName(const Char* _name) noexcept {}

NODISCARD GuiSize ZGuiObject::Size() const noexcept { return GuiSize(0, 0); }
NODISCARD GuiPos ZGuiObject::Pos() const noexcept { return GuiPos(0, 0); }
NODISCARD Int32 ZGuiObject::Width() const noexcept { return 0; }
NODISCARD Int32 ZGuiObject::Height() const noexcept { return 0; }
NODISCARD Int32 ZGuiObject::XPos() const noexcept { return 0; }
NODISCARD Int32 ZGuiObject::YPos() const noexcept { return 0; }
NODISCARD GuiColour ZGuiObject::BackgruondColour() const noexcept { return GuiColour(0.0f, 0.0f, 0.0f, 0.0f); }
NODISCARD const Char* ZGuiObject::Name() const noexcept { return ""; }

Void ZGuiObject::OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept {}
Void ZGuiObject::OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept {}
Void ZGuiObject::OnHide() noexcept {}
Void ZGuiObject::OnShow() noexcept {}

ZGuiObject::ZGuiObject() noexcept : SuperType_(), owner_ptr_(nullptr), if_tick_(false), enabled_(false) {}

ZGuiObject::ZGuiObject(Bool _if_tick, Bool _enabled) noexcept
    : SuperType_(), owner_ptr_(nullptr), if_tick_(_if_tick), enabled_(_enabled) {}

ZGuiObject::ZGuiObject(ZGuiObject&& _obj) noexcept 
    : SuperType_(std::move(_obj)) 
{ 
    MoveP(std::forward<ZGuiObject>(_obj)); 
}

ZGuiObject& ZGuiObject::operator=(ZGuiObject&& _obj) noexcept {
    SuperType_::operator=(std::move(_obj));
    MoveP(std::forward<ZGuiObject>(_obj));
    return *this;
}

Void ZGuiObject::MoveP(ZGuiObject&& _obj) noexcept {
    owner_ptr_ = _obj.owner_ptr_;
    if_tick_ = _obj.if_tick_;
    enabled_ = _obj.enabled_;
    _obj.owner_ptr_ = nullptr;
    _obj.if_tick_ = false;
    _obj.enabled_ = false;
}

}//gui
}//zengine