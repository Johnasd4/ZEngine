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
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }

    NODISCARD FORCEINLINE Bool Enabled() const noexcept { return enabled_; }
    /*
        Returns if the size changed last tick.
    */
    NODISCARD FORCEINLINE Bool SizeChanged() const noexcept { return size_changed_; }
    /*
        Returns if the position changed last tick.
    */
    NODISCARD FORCEINLINE Bool PosChanged() const noexcept { return pos_changed_; }
    NODISCARD FORCEINLINE ZGuiObject* OwnerPtr() const noexcept { return owner_ptr_; }

    /*
        Will be called when the application executes.
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
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(GuiPos _pos) noexcept;
    virtual Void SetName(const Char* _name) noexcept;

    NODISCARD virtual Int32 Width() const noexcept;
    NODISCARD virtual Int32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Int32 XPos() const noexcept;
    NODISCARD virtual Int32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;
    NODISCARD virtual const Char* Name() const noexcept;

    //Base trigger functions.

    virtual Void OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept;
    virtual Void OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;
    virtual Void OnAdd(ZGuiObject* _owner_ptr) noexcept;

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept;
    ZGuiObject(ZGuiObject&& _obj) noexcept;
    ZGuiObject(
        GuiSize _size,
        GuiPos _pos,
        Bool _enabled = true
    ) noexcept;

    ZGuiObject& operator=(ZGuiObject&& _obj) noexcept;

    FORCEINLINE ~ZGuiObject() {}

private:
    ZGuiObject(const ZGuiObject&) = delete;
    ZGuiObject& operator=(const ZGuiObject&) = delete;

    Void MoveP(ZGuiObject&& _obj) noexcept;

    GuiSize size_;
    GuiPos pos_;
    GuiSize pre_size_;
    GuiPos pre_pos_;
    Bool size_changed_;
    Bool pos_changed_;
    ZGuiObject* owner_ptr_;
    Bool enabled_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
