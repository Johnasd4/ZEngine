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

#include "internal/z_drive.h"

#include "../z_core/t_atom.h"
#include "../z_core/t_set.h"

#include "z_gui_object.h"
#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZWindowErrorCode : ReturnType {
    kZWindowErrorCode_LinkError = kErrorCodeBase_ZWindow,
    kZWindowErrorCode_SystemError,
    kZWindowErrorCode_NullptrParam,
    kZWindowErrorCode_ParamOutOfRange,
    kZWindowErrorCode_WindowAreadyCreated,
    kZWindowErrorCode_WindowNotExist
};
}//error_code
}//zengine

struct GLFWwindow;
struct ImGuiContext;

namespace zengine {
namespace gui {

/*
    Window class. Only creates the window, before adding widgets, a ZFrame must be added first. 
    The window contain's the frames and the frame's contains the widgets.
*/
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    /*
        The window screen mode enum.
    */
    enum WindowScreenModeEnum_ {
        kWindowScreenMode_Window,
        kWindowScreenMode_FullScreenCustomSize,
        kWindowScreenMode_FullScreenDefaultSize,
    };

    /*
        The window state enum.
    */
    enum WindowStateEnum_ {
        kWindowState_Terminated,
        kWindowState_Opened,
        kWindowState_Hidden
    };

    /*
        Sets Vertical synchronization.
        _tick_pur_window_tick: Set 0 to not use vertiacl synchronization, default 0.
    */
    FORCEINLINE static Void SetVerticalSynchronization(Int32 _tick_pur_window_tick) noexcept {
        tick_pur_window_tick_ = _tick_pur_window_tick; 
    }
    /*
        Gets the active window num.
    */
    NODISCARD FORCEINLINE static Int32 ActiveWindowNum() noexcept { return active_window_num_.Value(); }

    ZWindow() noexcept;
    ZWindow(ZWindow&& _window) noexcept;
    ZWindow(
        const Char* _name,
        GuiSize _size,
        GuiPos _pos = { 0, 0 }, 
        WindowScreenModeEnum_ _screen_mode = kWindowScreenMode_Window
    ) noexcept;
    
    ~ZWindow() noexcept;

    ZWindow& operator=(ZWindow&& _window) noexcept;

    /*
        Starts ticking the window.
    */
    NODISCARD ReturnType Execute() noexcept;

    NODISCARD FORCEINLINE WindowStateEnum_ WindowState() const noexcept { return window_state_; }

    /*
        Will be called when the application executes, returns when the window is closed.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Hides the window.
    */
    virtual Void Hide() noexcept;
    /*
        Shows the window, if the window was closed, will call begin().
    */
    virtual Void Show() noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;
    /*
        Destroy the window, release the resourses.
    */
    virtual Void Destroy() noexcept;

    /*
        Adds a frame to the window.
    */
    virtual Void Add(ZFrame* _frame) noexcept;

    virtual Void SetName(const Char* _name) noexcept;

    virtual Void SetBackgruondColour(GuiColour _colour) noexcept;
    virtual Void SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept;

    /*
        Sets the window pos at the screen center.
        Based on the current window size and scrren size.
    */
    Void SetScreenCenter() noexcept;

    /*
        Returns the position that actually uses by the library.
        This class is the base position.
    */
    NODISCARD virtual GuiPos AbsPos() const noexcept;
    NODISCARD virtual const Char* Name() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;


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
        Called before the window destroyed.
    */
    virtual Void OnDestroy() noexcept;

protected:
    using SuperType_ = ZGuiObject;

private:
    friend class ZWindowManager;

    ZWindow(const ZWindow&) = delete;
    ZWindow& operator=(const ZWindow&) = delete;

    Void MoveP(ZWindow&& _window) noexcept;

    /*
        Creates the window, will initialize opengl if not initialized.
    */
    NODISCARD virtual ReturnType CreateP(
        const Char* _name, GuiSize _size, GuiPos _pos, WindowScreenModeEnum_ _screen_mode
    ) noexcept;

    static Int32 tick_pur_window_tick_;
    static TAtom<Int32> active_window_num_;

    GLFWwindow* window_handle_;
    ImGuiContext* window_context_;
    TMultiset<ZFrame*, ZWidgetObjectCompare> frame_ptr_set_;
    WindowStateEnum_ window_state_;
};

}//gui
}//zengine