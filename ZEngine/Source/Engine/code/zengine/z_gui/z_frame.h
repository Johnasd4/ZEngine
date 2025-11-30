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

#include "../z_core/t_set.h"
#include "../z_core/z_string_view.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZFrameErrorCodeEnum : ReturnType {
    kZFrameErrorCode_LinkError = kErrorCodeBase_ZFrame,
    kZFrameErrorCode_SystemError,
    kZFrameErrorCode_NullptrParam,
    kZFrameErrorCode_ParamOutOfRange,
    kZFrameErrorCode_FrameAreadyCreated,
    kZFrameErrorCode_FrameNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace gui {

/*
    Frame class.
*/
class GUI_DLLAPI ZFrame : public ZWidgetObject {
public:
    /*
        The frame screen mode enum.
    */
    enum FrameFlagEnum_ : Int32 {
        kFrameFlag_None = 0,
        // Disable title-bar
        kFrameFlag_NoTitleBar = 1 << 0,   
        // Disable user resizing with the lower-right grip
        kFrameFlag_NoResize = 1 << 1,   
        // Disable user moving the window
        kFrameFlag_NoMove = 1 << 2,   
        // Disable scrollbars (window can still scroll with mouse or programmatically)
        kFrameFlag_NoScrollbar = 1 << 3,   
        // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the
        // parent unless NoScrollbar is also set.
        kFrameFlag_NoScrollWithMouse = 1 << 4,   
        // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button 
        // (e.g. within a docking node).
        kFrameFlag_NoCollapse = 1 << 5,   
        // Resize every window to its content every frame
        kFrameFlag_AlwaysAutoResize = 1 << 6,  
        // Disable drawing background color (WindowBg, etc.) and outside border. 
        // Similar as using SetNextWindowBgAlpha(0.0f).
        kFrameFlag_NoBackground = 1 << 7,   
        // Never load/save settings in .ini file
        kFrameFlag_NoSavedSettings = 1 << 8,   
        // Disable catching mouse, hovering test with pass through.
        kFrameFlag_NoMouseInputs = 1 << 9,   
        // Has a menu-bar
        kFrameFlag_MenuBar = 1 << 10,  
        // Allow horizontal scrollbar to appear (off by default). 
        // You may use SetNextWindowContentSize(ImVec2(width,0.0f)); 
        // prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        kFrameFlag_HorizontalScrollbar = 1 << 11,  
        // Disable taking focus when transitioning from hidden to visible state
        kFrameFlag_NoFocusOnAppearing = 1 << 12,  
        // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        kFrameFlag_NoBringToFrontOnFocus = 1 << 13,  
        // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        kFrameFlag_AlwaysVerticalScrollbar = 1 << 14,  
        // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        kFrameFlag_AlwaysHorizontalScrollbar = 1 << 15, 
        // No gamepad/keyboard navigation within the window
        kFrameFlag_NoNavInputs = 1 << 16,  
        // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        kFrameFlag_NoNavFocus = 1 << 17,  
        // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the 
        // X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed 
        // when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        kFrameFlag_UnsavedDocument = 1 << 18,  
        kFrameFlag_NoNav = kFrameFlag_NoNavInputs | kFrameFlag_NoNavFocus,
        kFrameFlag_NoDecoration = kFrameFlag_NoTitleBar | kFrameFlag_NoResize | kFrameFlag_NoScrollbar | kFrameFlag_NoCollapse,
        kFrameFlag_NoInputs = kFrameFlag_NoMouseInputs | kFrameFlag_NoNavInputs | kFrameFlag_NoNavFocus,
    };

    static inline constexpr Int32 kBaseFrameLevel = 0;

    static inline constexpr Int32 kDefaultFrameFlag = kFrameFlag_None;
    static inline constexpr GuiColour kDefaultFrameBackgroundColour = { 0.2f, 0.2f, 0.2f, 1.0f };

    ZFrame() noexcept;
    ZFrame(ZFrame&& _frame) noexcept;
    /*
        Add ## at the start of the name if you want an empty title.
    */
    ZFrame(ZStringView _name, GuiSize _size = kBaseSize, GuiPos _pos = kBasePos) noexcept;

    virtual ~ZFrame() noexcept;

    ZFrame& operator=(ZFrame&& _frame) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    FORCEINLINE Void SetFrameFlag(Int32 _frame_flag) noexcept { frame_flag_ = _frame_flag; }
    /*
        Can only set one flag at a time.
    */
    FORCEINLINE Void SetFrameFlag(FrameFlagEnum_ _frame_flag, Bool _if_active) noexcept { 
        _if_active ? frame_flag_ |= _frame_flag : frame_flag_ &= ~_frame_flag;
    }

    NODISCARD FORCEINLINE Int32 FrameFlag() const noexcept { return frame_flag_; }
    NODISCARD FORCEINLINE Int32 FrameLevel() const noexcept { return frame_flag_; }

    /*
        Called before objects first tick.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Ticks every frame, called after added widgets.
        Do gui changes in this funcion to keep in sync.
        Override this funciton to tick widgets not added to the frame.
    */
    virtual Void TickWidget(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    /*
        Adds a widget to the frame.
    */
    NODISCARD virtual ReturnType Add(ZWidgetObject* _widget_obj_ptr) noexcept;

    /*
        Remove a widget from the frame.
    */
    NODISCARD virtual ReturnType Remove(ZWidgetObject* _widget_obj_ptr) noexcept;
    virtual Void RemoveAll() noexcept;

    virtual Void SetBackgruondColour(GuiColour _colour) noexcept;
    virtual Void SetScrollX(Float32 _scroll_x) noexcept;
    virtual Void SetScrollY(Float32 _scroll_y) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;
    NODISCARD virtual Float32 ScrollX() const noexcept;
    NODISCARD virtual Float32 ScrollY() const noexcept;
    NODISCARD virtual Float32 ScrollMaxX() const noexcept;
    NODISCARD virtual Float32 ScrollMaxY() const noexcept;

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

    /*
        Called when scroll changed.
    */
    virtual Void OnScrollChanged(
        Float32 _pre_scroll_x,
        Float32 _pre_scroll_y,
        Float32 _cur_scroll_x,
        Float32 _cur_scroll_y
    ) noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZFrame(const ZFrame&) = delete;
    ZFrame& operator=(const ZFrame&) = delete;

    Void MoveP(ZFrame&& _frame) noexcept;

    Void UpdateFrameLevelP(Int32 _super_frame_level) noexcept;

    Int32 frame_flag_;
    Int32 frame_level_;
    GuiColour frame_background_colour_;
    Float32 scroll_pre_x_;
    Float32 scroll_pre_y_;
    Float32 scroll_x_;
    Float32 scroll_y_;
    Float32 scroll_max_x_;
    Float32 scroll_max_y_;
    Bool scroll_x_set_;
    Bool scroll_y_set_;
    TMultipleSet<ZWidgetObject*, ZWidgetObjectCompare> widget_ptr_set_;
    TMultipleSet<ZFrame*, ZWidgetObjectCompare> frame_ptr_set_;
};

}//gui
}//zengine
