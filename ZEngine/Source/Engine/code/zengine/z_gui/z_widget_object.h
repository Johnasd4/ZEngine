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
#pragma once

#include "drive.h"

#include "../z_core/z_string.h"

#include "z_gui_object.h"

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
    static constexpr Int32 kDefaultPriority = 0;

    enum WidgetTypeEnum_ : Int32 {
        kWidgetType_Frame,
        kWidgetType_Text,
        kWidgetType_InputText,
        kWidgetType_Button,
        kWidgetType_Separator,
        kWidgetType_CheckBox,
    };

    FORCEINLINE Void SetPriority(Int32 _priority) noexcept { priority_ = _priority; }
    FORCEINLINE Void SetVisiable(Bool _visible) noexcept { _visible ? Show() : Hide(); }
    FORCEINLINE Void SetSameLine(Bool _same_line) noexcept { same_line_ = _same_line; }
    FORCEINLINE Void SetSameLinePosOffset(GuiPos _pos_offset) noexcept { same_line_pos_offset_ = _pos_offset; }

    NODISCARD FORCEINLINE Int32 Priority() const noexcept { return priority_; }
    NODISCARD FORCEINLINE Bool Visiable() const noexcept { return visiable_; }
    NODISCARD FORCEINLINE Bool SameLine() const noexcept { return same_line_; }
    NODISCARD FORCEINLINE GuiPos SameLinePosOffset() const noexcept { return same_line_pos_offset_; }
    NODISCARD FORCEINLINE Bool PosSet() const noexcept { return pos_set_; }
    NODISCARD FORCEINLINE Bool SizeSet() const noexcept { return size_set_; }
    
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;

    /*
        Release resource.
    */
    virtual Void ReleaseResource() noexcept;

    virtual Void SetWidth(Float32 _width) noexcept;
    virtual Void SetHeight(Float32 _height) noexcept;
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetPosX(Float32 _pos_x) noexcept;
    virtual Void SetPosY(Float32 _pos_y) noexcept;
    virtual Void SetPos(GuiPos _pos) noexcept;
    virtual Void SetName(const Char* _name) noexcept;

    NODISCARD virtual const Char* Name() const noexcept;


    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept = 0;

    //Base trigger functions.

    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;

protected:
    using SuperType_ = ZGuiObject;

    ZWidgetObject() noexcept;
    ZWidgetObject(ZWidgetObject&& _obj) noexcept;
    ZWidgetObject(
        const Char* _name, 
        GuiSize _size = kBaseSize,
        GuiPos _pos = kBasePos, 
        Int32 _priority = kDefaultPriority,
        Bool _visiable = true, 
        Bool _enabled = true
    ) noexcept;

    ZWidgetObject& operator=(ZWidgetObject&& _obj) noexcept;

    FORCEINLINE ~ZWidgetObject() {}

private:
    ZWidgetObject(const ZWidgetObject&) = delete;
    ZWidgetObject& operator=(const ZWidgetObject&) = delete;

    Void MoveP(ZWidgetObject&& _obj) noexcept;

    Int32 priority_;
    Bool visiable_;
    Bool pos_set_;
    Bool size_set_;
    Bool same_line_;
    GuiPos same_line_pos_offset_;
    ZString name_;
};

struct ZWidgetObjectCompare {
    NODISCARD FORCEINLINE Bool operator()(
        const ZWidgetObject* _left_obj_ptr, const ZWidgetObject* _right_obj_ptr
    ) const noexcept {
        return _left_obj_ptr->Priority() < _right_obj_ptr->Priority();
    }
};

}//gui
}//zengine
