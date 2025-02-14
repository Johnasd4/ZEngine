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
    static constexpr Int32 kDefaultPriority = 0;

    enum WidgetTypeEnum {
        kWidgetType_Frame,
        kWidgetType_Text,
        kWidgetType_InputText,
    };

    FORCEINLINE Void SetPriority(Int32 _priority) noexcept { priority_ = _priority; }

    NODISCARD FORCEINLINE Int32 Priority() const noexcept { return priority_; }
    NODISCARD FORCEINLINE Bool Visiable() const noexcept { return visiable_; }
    
    virtual Void SetName(const Char* _name) noexcept;

    NODISCARD virtual const Char* Name() const noexcept;
    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept = 0;

    //Base trigger functions.

    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;

protected:
    using SuperType_ = ZGuiObject;

    ZWidgetObject() noexcept;
    ZWidgetObject(ZWidgetObject&& _obj) noexcept;
    ZWidgetObject(
        const Char* _name, 
        GuiSize _size, 
        GuiPos _pos, 
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

#endif // !Z_GUI_Z_WIDGET_OBJECT_H_
