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

#include "../z_core/z_mutex.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string.h"

namespace zengine {
namespace gui {

class ZGuiAdjustableObject;

/*
    The base class of the gui classes.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
    Object members:
    ZGuiObject* owner_ptr_
    ZString name_
    Bool visiable_
    Bool enabled_
    Int32 priority_
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    static constexpr Int32 kDefaultPriority = 0;
    static constexpr Float32 kDIsabledAlphaFactor = 0.5f;

    enum TypeEnum_ {
        kTypeWindow,
        kTypeFrame,
        kTypeButton
    };

    enum ModEnum_ {
        kModShift = 1 << 0,
        kModCtrl = 1 << 1,
        kModAlt = 1 << 2,
        kModWin = 1 << 3,
        kModCapsLock = 1 << 4,
        kModNumLock = 1 << 5
    };

    NODISCARD static ZMutex& OpenGLMutex() noexcept;

    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }
    FORCEINLINE Void SetPriority(Int32 _priority) noexcept { priority_ = _priority; }

    NODISCARD FORCEINLINE Bool Visiable() const noexcept { return visiable_; }
    NODISCARD FORCEINLINE Int32 Priority() const noexcept { return priority_; }
    NODISCARD FORCEINLINE ZGuiAdjustableObject* OwnerPtr() const noexcept { return owner_ptr_; }

    /*
        Will be called when the application executes.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;
    /*
        Hides the object.
    */
    virtual Void Hide() noexcept;
    /*
        Shows the object.
    */
    virtual Void Show() noexcept;

    virtual Void SetName(const Char* _name) noexcept;

    NODISCARD virtual const Char* Name() const noexcept;
    NODISCARD virtual Bool Enabled() const noexcept;

    NODISCARD virtual TypeEnum_ WidgetType() const noexcept = 0;

    //Base trigger functions.
    virtual Void OnAdd(ZGuiAdjustableObject* _owner_ptr) noexcept;
    virtual Void OnRemove() noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;
    virtual Void OnKeyDown(KeyEnum _clicked_button, Int32 _mods) noexcept;
    virtual Void OnKeyUp(KeyEnum _clicked_button, Int32 _mods) noexcept;
    virtual Void OnKeyPress(KeyEnum _clicked_button, Int32 _mods) noexcept;
    virtual Void OnMouseDown(MouseButtonEnum _clicked_button, Int32 _mods) noexcept;
    virtual Void OnMouseUp(MouseButtonEnum _clicked_button, Int32 _mods) noexcept;
    /*
        Front and back is y, left and right is x. Front and Left is positive.
    */
    virtual Void OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept;
    virtual Void OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept;  

    Void BindAddEvent(
        Void(*_add_event_ptr)(ZGuiObject* _this_ptr , ZGuiAdjustableObject* _owner_ptr)
    ) noexcept;
    Void BindRemoveEvent(
        Void(*_remove_event_ptr)(ZGuiObject* _this_ptr)
    ) noexcept;
    Void BindHideEvent(
        Void(*_hide_event_ptr)(ZGuiObject* _this_ptr)
    ) noexcept;
    Void BindShowEvent(
        Void(*_show_event_ptr)(ZGuiObject* _this_ptr)
    ) noexcept;
    Void BindKeyDownEvent(
        Void(*_key_down_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
    ) noexcept;
    Void BindKeyUpEvent(
        Void(*_key_up_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
    ) noexcept;
    Void BindKeyPressEvent(
        Void(*_key_press_event_ptr)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods)
    ) noexcept;
    Void BindMouseDownEvent(
        Void(*_mouse_down_event_ptr)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods)
    ) noexcept;
    Void BindMouseUpEvent(
        Void(*_mouse_up_event_ptr)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods)
    ) noexcept;
    Void BindScrollMoveEvent(
        Void(*_scroll_move_event_ptr)(ZGuiObject* _this_ptr, Float32 _x_offset, Float32 _y_offset)
    ) noexcept;
    Void BindMouseMoveEvent(
        Void(*_mouse_move_event_ptr)(ZGuiObject* _this_ptr, GuiPos _pre_pos, GuiPos _cur_pos)
    ) noexcept;

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept;
    ZGuiObject(ZGuiObject&& _obj) noexcept;
    ZGuiObject(
        ZString _name, 
        Bool _visiable = true, 
        Bool _enabled = true, 
        Int32 _priority = kDefaultPriority
    ) noexcept;

    ~ZGuiObject() noexcept;

    ZGuiObject& operator=(ZGuiObject&& _obj) noexcept;

private:
    ZGuiObject(const ZGuiObject&) = delete;
    ZGuiObject& operator=(const ZGuiObject&) = delete;

    Void MoveP(ZGuiObject&& _obj) noexcept;

    Void(*add_event_ptr_)(ZGuiObject* _this_ptr, ZGuiAdjustableObject* _owner_ptr);
    Void(*remove_event_ptr_)(ZGuiObject* _this_ptr);
    Void(*hide_event_ptr_)(ZGuiObject* _this_ptr);
    Void(*show_event_ptr_)(ZGuiObject* _this_ptr);
    Void(*key_down_event_ptr_)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods);
    Void(*key_up_event_ptr_)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods);
    Void(*key_press_event_ptr_)(ZGuiObject* _this_ptr, KeyEnum _clicked_button, Int32 _mods);
    Void(*mouse_down_event_ptr_)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods);
    Void(*mouse_up_event_ptr_)(ZGuiObject* _this_ptr, MouseButtonEnum _clicked_button, Int32 _mods);
    Void(*scroll_move_event_ptr_)(ZGuiObject* _this_ptr, Float32 _x_offset, Float32 _y_offset);
    Void(*mouse_move_event_ptr_)(ZGuiObject* _this_ptr, GuiPos _pre_pos, GuiPos _cur_pos);

    ZGuiAdjustableObject* owner_ptr_;
    ZString name_;
    Bool visiable_;
    Bool enabled_;
    Int32 priority_;
};

struct ZGuiObjectCompare {
    NODISCARD FORCEINLINE Bool operator()(
        const ZGuiObject* _left_obj_ptr, const ZGuiObject* _right_obj_ptr
        ) const noexcept {
        return _left_obj_ptr->Priority() < _right_obj_ptr->Priority();
    }
};

/*
    The base class of the gui widget adjustable classes. Different from ZGuiObject, this class can move and resize the
    object.
    Object members:
    GuiSize size_
    GuiPos pos_
*/
class GUI_DLLAPI ZGuiAdjustableObject : public ZGuiObject {
public:
    static constexpr Int32 kDefaultPriority = 0x1000;

    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;

    virtual Void SetWidth(Float32 _width) noexcept;
    virtual Void SetHeight(Float32 _height) noexcept;
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetXPos(Float32 _x_pos) noexcept;
    virtual Void SetYPos(Float32 _y_pos) noexcept;
    virtual Void SetPos(GuiPos _pos) noexcept;

    NODISCARD virtual Float32 Width() const noexcept;
    NODISCARD virtual Float32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Float32 XPos() const noexcept;
    NODISCARD virtual Float32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;
    /*
        Returns the position that actually uses by the library.
    */
    NODISCARD virtual GuiPos AbsPos() const noexcept;

    //Base trigger functions.
    virtual Void OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept;
    virtual Void OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept;

    Void BindResizeEvent(
        Void(*_resize_event_ptr)(ZGuiAdjustableObject* _this_ptr, GuiSize _pre_size, GuiSize _cur_size)
    ) noexcept;
    Void BindMoveEvent(
        Void(*_move_event_ptr)(ZGuiAdjustableObject* _this_ptr, GuiPos _pre_pos, GuiPos _cur_pos)
    ) noexcept;
protected:
    using SuperType_ = ZGuiObject;

    ZGuiAdjustableObject() noexcept;
    ZGuiAdjustableObject(ZGuiAdjustableObject&& _obj) noexcept;
    ZGuiAdjustableObject(
        const Char* _name,
        GuiSize _size,
        GuiPos _pos,
        Int32 _priority = kDefaultPriority,
        Bool _visiable = true,
        Bool _enabled = true
    ) noexcept;

    ZGuiAdjustableObject& operator=(ZGuiAdjustableObject&& _obj) noexcept;

    ~ZGuiAdjustableObject() noexcept;

    NODISCARD FORCEINLINE Bool SizeChanged() noexcept { return size_changed_; }
    NODISCARD FORCEINLINE Bool PosChanged() noexcept { return pos_changed_; }

private:
    ZGuiAdjustableObject(const ZGuiAdjustableObject&) = delete;
    ZGuiAdjustableObject& operator=(const ZGuiAdjustableObject&) = delete;

    Void MoveP(ZGuiAdjustableObject&& _obj) noexcept;

    Void(*resize_event_ptr_)(ZGuiAdjustableObject* _this_ptr, GuiSize _pre_size, GuiSize _cur_size);
    Void(*move_event_ptr_)(ZGuiAdjustableObject* _this_ptr, GuiPos _pre_pos, GuiPos _cur_pos);

    GuiSize pre_size_;
    GuiPos pre_pos_;
    Bool size_changed_;
    Bool pos_changed_;

    GuiSize size_;
    GuiPos pos_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
