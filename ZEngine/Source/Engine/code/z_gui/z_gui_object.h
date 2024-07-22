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

#include "../z_core/t_vector.h"
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

    NODISCARD FORCEINLINE Bool IfTick() const noexcept { return if_tick_; }
    NODISCARD FORCEINLINE Bool Enabled() const noexcept { return enabled_; }

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
        Adds another object to this object.
    */
    virtual Void Add(ZGuiObject* _obj_ptr) noexcept;


    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;
    virtual Void SetBackgruondColour(Int32 _red, Int32 _green, Int32 _blue, Int32 _alpha) noexcept;

    FORCEINLINE Void SetSize(GuiSize _size) noexcept { SetSize(_size.width_, _size.height_); }
    FORCEINLINE Void SetPos(GuiPos _pos) noexcept { SetPos(_pos.x_, _pos.y_); }
    FORCEINLINE Void SetBackgruondColour(ColourRGBA _colour) noexcept {
        SetBackgruondColour(_colour.red_, _colour.green_, _colour.blue_, _colour.alpha_);
    }

    NODISCARD virtual GuiSize Size() noexcept;
    NODISCARD virtual GuiPos Pos() noexcept;
    NODISCARD virtual Int32 Width() noexcept;
    NODISCARD virtual Int32 Height() noexcept;
    NODISCARD virtual Int32 XPos() noexcept;
    NODISCARD virtual Int32 YPos() noexcept;
    NODISCARD virtual ColourRGBA BackgruondColour() noexcept;

    virtual Void OnMouseClick(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept;
    virtual Void OnMouseUp(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept;
    virtual Void OnMouseDown(MouseKeyEnum _key, Int32 _pos_x, Int32 _pos_y) noexcept;
    virtual Void OnKeyPress(KeyEnum _key) noexcept;
    virtual Void OnKeyUp(KeyEnum _key) noexcept;
    virtual Void OnKeyDown(KeyEnum _key) noexcept;
    virtual Void OnMove(Int32 _pre_x, Int32 _pre_y, Int32 _cur_x, Int32 _cur_y) noexcept;
    virtual Void OnResize(Int32 _pre_width, Int32 _pre_height, Int32 _cur_width, Int32 _cur_height) noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;
    virtual Void OnAdd() noexcept;

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept : SuperType_(), owner_ptr_(nullptr), sub_obj_ptr_vec_(), if_tick_(true), enabled_(true) {}
    ZGuiObject(const ZGuiObject& _obj) noexcept : SuperType_(_obj), sub_obj_ptr_vec_() { CopyP(_obj); }
    ZGuiObject(ZGuiObject&& _obj) noexcept : SuperType_(std::move(_obj)) { MoveP(std::forward<ZGuiObject>(_obj)); }

    const ZGuiObject& operator=(const ZGuiObject& _obj) noexcept {
        SuperType_::operator=(_obj);
        CopyP(_obj);
        return *this;
    }
    const ZGuiObject& operator=(ZGuiObject&& _obj) noexcept {
        SuperType_::operator=(std::move(_obj));
        MoveP(std::forward<ZGuiObject>(_obj));
        return *this;
    }

    FORCEINLINE ~ZGuiObject() {}

private:
    FORCEINLINE Void CopyP(const ZGuiObject& _obj) noexcept {
        owner_ptr_ = _obj.owner_ptr_;
    }

    FORCEINLINE Void MoveP(ZGuiObject&& _obj) noexcept {
        owner_ptr_ = _obj.owner_ptr_;
        sub_obj_ptr_vec_ = std::move(_obj.sub_obj_ptr_vec_);
        _obj.owner_ptr_ = nullptr;
    }

    ZGuiObject* owner_ptr_;
    TVector<ZGuiObject*> sub_obj_ptr_vec_;
    Bool if_tick_;
    Bool enabled_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
