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
    FORCEINLINE Void Add(ZGuiObject* _obj_ptr) noexcept { _obj_ptr->owner_ptr_ = this; }

    FORCEINLINE Void SetIfTick(Bool _if_tick) noexcept { if_tick_ = _if_tick; }
    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }

    NODISCARD FORCEINLINE Bool IfTick() const noexcept { return if_tick_; }
    NODISCARD FORCEINLINE Bool Enabled() const noexcept { return enabled_; }

    virtual Void Initialize() noexcept;
    virtual Void Tick(Float32 _delta_time) noexcept;
    virtual Void Reset() noexcept;
    virtual Void Hide() noexcept;
    virtual Void Show() noexcept;
    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;
    //virtual Void SetBackgruondColour() noexcept;

    virtual ZSize Size() noexcept;
    virtual ZPos Pos() noexcept;
    virtual Int32 Width() noexcept;
    virtual Int32 Height() noexcept;
    virtual Int32 XPos() noexcept;
    virtual Int32 YPos() noexcept;

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept : SuperType_(), owner_ptr_(nullptr), if_tick_(true), enabled_(true) {}
    ZGuiObject(const ZGuiObject& _obj) noexcept : SuperType_(_obj) { CopyP(_obj); }
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
        _obj.owner_ptr_ = nullptr;
    }

    ZGuiObject* owner_ptr_;
    Bool if_tick_;
    Bool enabled_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
