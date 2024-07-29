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
#ifndef Z_GUI_Z_WIDGET_OBJECT_H_
#define Z_GUI_Z_WIDGET_OBJECT_H_

#include "internal/z_drive.h"

#include "z_gui_object.h"
#include "../z_core/z_string.h"

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
class GUI_DLLAPI ZWidgetObject : public ZGuiObject {
public:
    enum WidgetTypeEnum {
        kWidgetTypeFrame
    };

    FORCEINLINE Void SetPriority(Int32 _priority) noexcept { priority_ = _priority; }

    NODISCARD FORCEINLINE Int32 Priority() const noexcept { return priority_; }
    NODISCARD FORCEINLINE Bool Visiable() const noexcept { return visiable_; }
    
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
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;

    virtual Void SetName(const Char* _name) noexcept;

    NODISCARD virtual Int32 Width() const noexcept;
    NODISCARD virtual Int32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Int32 XPos() const noexcept;
    NODISCARD virtual Int32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;

    NODISCARD virtual const Char* Name() const noexcept;
    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept = 0;

    //Base trigger functions.

    virtual Void OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept;
    virtual Void OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;
    virtual Void OnAdd(ZGuiObject* _owner_ptr) noexcept;

protected:
    using SuperType_ = ZGuiObject;

    ZWidgetObject() noexcept;
    ZWidgetObject(ZWidgetObject&& _obj) noexcept;
    ZWidgetObject(
        const Char* _name, 
        GuiSize _size, 
        GuiPos _pos, 
        Bool _visiable = true, 
        Int32 _priority = 0, 
        Bool _if_tick = true, 
        Bool _enabled = true
    ) noexcept;

    ZWidgetObject& operator=(ZWidgetObject&& _obj) noexcept;

    FORCEINLINE ~ZWidgetObject() {}

private:
    ZWidgetObject(const ZWidgetObject&) = delete;
    ZWidgetObject& operator=(const ZWidgetObject&) = delete;

    Void MoveP(ZWidgetObject&& _obj) noexcept;

    GuiSize size_;
    GuiPos pos_;
    GuiSize pre_size_;
    GuiPos pre_pos_;
    Bool size_changed_;
    Bool pos_changed_;
    Bool visiable_;
    Int32 priority_;
    ZString name_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_WIDGET_OBJECT_H_
