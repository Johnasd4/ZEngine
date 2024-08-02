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
#ifndef Z_GUI_Z_FRAME_H_
#define Z_GUI_Z_FRAME_H_

#include "internal/z_drive.h"

#include "../z_core/t_set.h"
#include "../z_core/z_string.h"

#include "z_widget_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZFrameErrorCode : ReturnType {
    kZFrameErrorCodeLinkError = kErrorCodeBaseZFrame,
    kZFrameErrorCodeFrameAreadyCreated,
    kZFrameErrorCodeFrameNotExist
};

}//error_code

/*
    Frame class.
*/
class GUI_DLLAPI ZFrame : public ZWidgetObject {
public:
    /*
        The frame screen mode enum.
    */
    enum FrameFlagEnum_ {
        kFrameFlagNone = 0,
        kFrameFlagNoTitleBar = 1 << 0,   // Disable title-bar
        kFrameFlagNoResize = 1 << 1,   // Disable user resizing with the lower-right grip
        kFrameFlagNoMove = 1 << 2,   // Disable user moving the window
        kFrameFlagNoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        kFrameFlagNoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        kFrameFlagNoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        kFrameFlagAlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
        kFrameFlagNoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        kFrameFlagNoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
        kFrameFlagNoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        kFrameFlagMenuBar = 1 << 10,  // Has a menu-bar
        kFrameFlagHorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        kFrameFlagNoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        kFrameFlagNoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        kFrameFlagAlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        kFrameFlagAlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        kFrameFlagNoNavInputs = 1 << 16,  // No gamepad/keyboard navigation within the window
        kFrameFlagNoNavFocus = 1 << 17,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        kFrameFlagUnsavedDocument = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        kFrameFlagNoNav = kFrameFlagNoNavInputs | kFrameFlagNoNavFocus,
        kFrameFlagNoDecoration = kFrameFlagNoTitleBar | kFrameFlagNoResize | kFrameFlagNoScrollbar | kFrameFlagNoCollapse,
        kFrameFlagNoInputs = kFrameFlagNoMouseInputs | kFrameFlagNoNavInputs | kFrameFlagNoNavFocus,
    };

    static constexpr Int32 kBaseFrameLevel = 0;

    static constexpr Int32 kDefaultFrameFlag = kFrameFlagNoNav | kFrameFlagNoDecoration | kFrameFlagNoInputs;
    static constexpr GuiColour kDefaultFrameBackGroundColour = { 0.5f, 0.5f, 0.5f, 1.0f };

    ZFrame() noexcept;
    ZFrame(ZFrame&& _frame) noexcept;
    ZFrame(const Char* _name, GuiSize _size, GuiPos _pos) noexcept;

    ~ZFrame() noexcept;

    ZFrame& operator=(ZFrame&& _frame) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    FORCEINLINE Void SetFrameFlag(Int32 _frame_flag) noexcept { frame_flag_ = _frame_flag; }

    NODISCARD FORCEINLINE Int32 FrameFlag() const noexcept { return frame_flag_; }
    NODISCARD FORCEINLINE Int32 FrameLevel() const noexcept { return frame_flag_; }

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
        Adds a widget to the frame.
    */
    virtual Void Add(ZWidgetObject* _widget_obj) noexcept;

    virtual Void SetBackgruondColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;

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
    TMultiset<ZWidgetObject*, ZWidgetObjectCompare> widget_ptr_set_;
    TMultiset<ZFrame*, ZWidgetObjectCompare> frame_ptr_set_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_FRAME_H_
