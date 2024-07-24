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
#ifndef Z_GUI_Z_GUI_OBJECT_H_
#define Z_GUI_Z_GUI_OBJECT_H_

#include "internal/z_drive.h"

#include "../z_core/z_object.h"

namespace zengine {
namespace gui {

/*
    The base class of the gui classes.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
    Object members:
    Int32 width_: The width of the gui object.
    Int32 height_: The height of the gui object.
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    FORCEINLINE Void SetIfTick(Bool _if_tick) noexcept { if_tick_ = _if_tick; }
    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }
    FORCEINLINE Void SetOwnerPtr(ZGuiObject* _owner_ptr) noexcept { owner_ptr_ = _owner_ptr; }

    NODISCARD FORCEINLINE Bool IfTick() const noexcept { return if_tick_; }
    NODISCARD FORCEINLINE Bool Enabled() const noexcept { return enabled_; }
    NODISCARD FORCEINLINE ZGuiObject* OwnerPtr() const noexcept { return owner_ptr_; }

    /*
        Will be called when the object is added to another object.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Hides the object.
    */
    virtual Void Hide() noexcept;
    /*
        Shows the object.
    */
    virtual Void Show() noexcept;

    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;
    virtual Void SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept;
    virtual Void SetName(const Char* _name) noexcept;

    FORCEINLINE Void SetSize(GuiSize _size) noexcept { SetSize(_size.width_, _size.height_); }
    FORCEINLINE Void SetPos(GuiPos _pos) noexcept { SetPos(_pos.x_, _pos.y_); }
    FORCEINLINE Void SetBackgruondColour(GuiColour _colour) noexcept {
        SetBackgruondColour(_colour.red_, _colour.green_, _colour.blue_, _colour.alpha_);
    }

    NODISCARD virtual Int32 Width() const noexcept;
    NODISCARD virtual Int32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Int32 XPos() const noexcept;
    NODISCARD virtual Int32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;
    NODISCARD virtual GuiColour BackgruondColour() const noexcept;
    NODISCARD virtual const Char* Name() const noexcept;

    //Base trigger functions.

    virtual Void OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept;
    virtual Void OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept;
    ZGuiObject(Bool _if_tick, Bool _enabled = true) noexcept;
    ZGuiObject(ZGuiObject&& _obj) noexcept;

    ZGuiObject& operator=(ZGuiObject&& _obj) noexcept;

    FORCEINLINE ~ZGuiObject() {}

private:
    ZGuiObject(const ZGuiObject&) = delete;
    ZGuiObject& operator=(const ZGuiObject&) = delete;

    Void MoveP(ZGuiObject&& _obj) noexcept;

    ZGuiObject* owner_ptr_;
    Bool if_tick_;
    Bool enabled_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
