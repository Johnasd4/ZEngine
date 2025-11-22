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

#include "../z_core/z_mutex.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

namespace zengine {
namespace gui {

/*
    The base class of the gui classes.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    static constexpr GuiPos kBasePos = { -10000.0f, -10000.0f };
    static constexpr GuiSize kBaseSize = { -10000.0f, -10000.0f };
    static constexpr Float32 kBaseFontScale = 1.0f;
    static constexpr Float32 kDisableColourFactor = 0.5f;

    NODISCARD static ZMutex& OpenGLMutex() noexcept;
    NODISCARD static ZGuiObject*& CallbackGuiObjectPtr() noexcept;

    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }
    FORCEINLINE Void SetFirstTick(Bool _if_first_tick) noexcept { if_first_tick_ = _if_first_tick; }
    FORCEINLINE Void SetOwnerPtr(ZGuiObject* _owner_ptr) noexcept { owner_ptr_ = _owner_ptr; }

    NODISCARD FORCEINLINE Bool Enabled() const noexcept { return enabled_; }
    NODISCARD FORCEINLINE Bool IfFirstTick() const noexcept { return if_first_tick_; }
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
        Called before objects first tick.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
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

    virtual Void SetWidth(Float32 _width) noexcept;
    virtual Void SetHeight(Float32 _height) noexcept;
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetPosX(Float32 _pos_x) noexcept;
    virtual Void SetPosY(Float32 _pos_y) noexcept;
    virtual Void SetPos(GuiPos _pos) noexcept;
    virtual Void SetAbsPos(GuiPos _pos) noexcept;
    virtual Void SetFontScale(Float32 _scale) noexcept;

    NODISCARD virtual Float32 Width() const noexcept;
    NODISCARD virtual Float32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Float32 PosX() const noexcept;
    NODISCARD virtual Float32 PosY() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;
    /*
        Returns the position that actually uses by the library.
    */
    NODISCARD virtual GuiPos AbsPos() const noexcept;
    NODISCARD virtual Float32 FontScale() const noexcept;

    //Base trigger functions.

    virtual Void OnResize(GuiSize _pre_size, GuiSize _cur_size) noexcept;
    virtual Void OnMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept;
    virtual Void OnHide() noexcept;
    virtual Void OnShow() noexcept;
    virtual Void OnAdd(ZGuiObject* _owner_ptr) noexcept;
    virtual Void OnRemove(ZGuiObject* _owner_ptr) noexcept;

    virtual Void OnKeyDown(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept;
    virtual Void OnKeyUp(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept;
    virtual Void OnKeyPress(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept;
    virtual Void OnMouseDown(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept;
    virtual Void OnMouseUp(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept;
    /*
        Front and back is y, left and right is x. Front and Left is positive.
    */
    virtual Void OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept;
    virtual Void OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept;

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
    Bool enabled_;
    Bool if_first_tick_;
    Float32 font_scale_;
    ZGuiObject* owner_ptr_;
};

}//gui
}//zengine