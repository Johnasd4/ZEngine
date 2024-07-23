/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "z_gui_object.h"

namespace zengine {
namespace gui {

Void ZGuiObject::Begin() noexcept {
    for (auto obj_ptr_iter = sub_obj_ptr_vec_.Begin(); obj_ptr_iter != sub_obj_ptr_vec_.End(); ++obj_ptr_iter) {
        ZGuiObject* obj = *obj_ptr_iter;
        obj->Begin();
    }
}
Void ZGuiObject::Tick(Float32 _delta_sec) noexcept {
    for (auto obj_ptr_iter = sub_obj_ptr_vec_.Begin(); obj_ptr_iter != sub_obj_ptr_vec_.End(); ++obj_ptr_iter) {
        ZGuiObject* obj = *obj_ptr_iter;
        if (obj->Enabled() && obj->IfTick()) {
            obj->Tick(_delta_sec);
        }
    }
}
Void ZGuiObject::Hide() noexcept { OnHide(); }
Void ZGuiObject::Show() noexcept { OnShow(); }
Void ZGuiObject::Add(ZGuiObject* _obj_ptr) noexcept {
    _obj_ptr->owner_ptr_ = this;
    sub_obj_ptr_vec_.PushBack(_obj_ptr);
    OnAdd();
}

Void ZGuiObject::SetWidth(Int32 _width) noexcept {}
Void ZGuiObject::SetHeight(Int32 _height) noexcept {}
Void ZGuiObject::SetXPos(Int32 _x_pos) noexcept {}
Void ZGuiObject::SetYPos(Int32 _y_pos) noexcept {}
Void ZGuiObject::SetSize(Int32 _width, Int32 _height) noexcept {}
Void ZGuiObject::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {}
Void ZGuiObject::SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept {}

NODISCARD GuiSize ZGuiObject::Size() noexcept { return GuiSize(0, 0); }
NODISCARD GuiPos ZGuiObject::Pos() noexcept { return GuiPos(0, 0); }
NODISCARD Int32 ZGuiObject::Width() noexcept { return 0; }
NODISCARD Int32 ZGuiObject::Height() noexcept { return 0; }
NODISCARD Int32 ZGuiObject::XPos() noexcept { return 0; }
NODISCARD Int32 ZGuiObject::YPos() noexcept { return 0; }
NODISCARD GuiColour ZGuiObject::BackgruondColour() noexcept { return GuiColour(0.0f, 0.0f, 0.0f, 0.0f); }

Void ZGuiObject::OnMouseClick(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept {}
Void ZGuiObject::OnMouseUp(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept {}
Void ZGuiObject::OnMouseDown(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept {}
Void ZGuiObject::OnKeyPress(KeyEnum _key) noexcept {}
Void ZGuiObject::OnKeyUp(KeyEnum _key) noexcept {}
Void ZGuiObject::OnKeyDown(KeyEnum _key) noexcept {}
Void ZGuiObject::OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept {}
Void ZGuiObject::OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept {}
Void ZGuiObject::OnHide() noexcept {}
Void ZGuiObject::OnShow() noexcept {}
Void ZGuiObject::OnAdd() noexcept {}

}//gui
}//zengine