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
#include "../z_core/t_queue.h"
#include "../z_core/z_task.h"

#include "z_gui_object.h"
#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZWindowErrorCodeEnum : ReturnType {
    kZWindowErrorCode_LinkError = kErrorCodeBase_ZWindow,
    kZWindowErrorCode_SystemError,
    kZWindowErrorCode_NullptrParam,
    kZWindowErrorCode_ParamOutOfRange,
    kZWindowErrorCode_StateError,
    kZWindowErrorCode_WindowAreadyCreated,
    kZWindowErrorCode_WindowNotExist,
    kZWindowErrorCode_FontLanguageNotExist,
    kZWindowErrorCode_FontLanguageLoadFailed,
    kZWindowErrorCode_INIFileDirNotValid,
    kZWindowErrorCode_INIFileDirNotExist,
};
}//error_code
}//zengine

struct GLFWwindow;
struct ImGuiContext;
struct ImGuiIO;
struct ImFont;

namespace zengine {
namespace gui {

/*
    Window class. Only creates the window, before adding widgets, a ZFrame must be added first. 
    The window contain's the frames and the frame's contains the widgets.
    Call AddTask() to operate gui, task will be called when window ticks.
*/
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    /*
        The window screen mode enum.
    */
    enum class WindowScreenModeEnum_ : Int32 {
        kWindow,
        kFullScreenCustomSize,
        kFullScreenDefaultSize,
    };

    /*
        The window state enum.
    */
    enum class WindowStateEnum_ : Int32 {
        kTerminated,
        kInitialized,
        kOpened,
        kHidden
    };

    /*
        Font type enum.
    */
    enum class FontLanguageEnum_ : Int32 {
        kSimplifiedChineseCommon,
        kSimplifiedChineseAll
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
    NODISCARD FORCEINLINE static Int32 ActiveWindowNum() noexcept { return active_context_num_; }

    ZWindow() noexcept;
    ZWindow(ZWindow&& _window) noexcept;
    ZWindow(
        const Char* _name,
        GuiSize _size,
        GuiPos _pos = kBasePos,
        WindowScreenModeEnum_ _screen_mode = WindowScreenModeEnum_::kWindow
    ) noexcept;
    
    virtual ~ZWindow() noexcept;

    ZWindow& operator=(ZWindow&& _window) noexcept;

    /*
        Starts ticking the window, returns when the window is closed.
    */
    NODISCARD ReturnType Execute() noexcept;

    NODISCARD FORCEINLINE WindowStateEnum_ WindowState() const noexcept { return window_state_; }

    /*
        Called before objects first tick.
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
        Create window.
    */
    NODISCARD virtual ReturnType Create(
        const Char* _name,
        GuiSize _size,
        GuiPos _pos = kBasePos,
        WindowScreenModeEnum_ _screen_mode = WindowScreenModeEnum_::kWindow
    ) noexcept;
    /*
        Destroy window, release the resourses.
    */
    virtual Void Destroy() noexcept;

    /*
        Adds a frame to the window.
    */
    NODISCARD virtual ReturnType Add(ZFrame* _frame_ptr) noexcept;

    /*
        Adds task to run. Task will only run once. Task will be called when ticks.
    */
    Void AddTask(ZTask&& _task) noexcept;

    /*
        Remove a frame from the window.
    */
    virtual ReturnType Remove(ZFrame* _frame_ptr) noexcept;
    virtual Void RemoveAll() noexcept;

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

    NODISCARD virtual ReturnType LoadFontFromFileTTF(
        const Char* _file_dir,
        Float32 _font_size,
        FontLanguageEnum_ _font_language
    ) noexcept;

    NODISCARD virtual ReturnType SaveSettings(const Char* _file_dir) noexcept;

    NODISCARD virtual ReturnType LoadSettings(const Char* _file_dir) noexcept;

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
    ZWindow(const ZWindow&) = delete;
    ZWindow& operator=(const ZWindow&) = delete;

    Void MoveP(ZWindow&& _window) noexcept;

    /*
        Creates the window,.
    */
    NODISCARD ReturnType CreateWindowP(
        const Char* _name, GuiSize _size, WindowScreenModeEnum_ _screen_mode
    ) noexcept;

    /*
        Create context, will initialize opengl if not initialized.
    */
    NODISCARD ReturnType CreateContextP() noexcept;

    static Int32 tick_pur_window_tick_;
    static Int32 active_context_num_;
    
    GLFWwindow* window_handle_;
    ImGuiContext* imgui_context_ptr_;
    ImGuiIO* imgui_io_ptr_;
    TMultipleSet<ZFrame*, ZWidgetObjectCompare> frame_ptr_set_;
    TQueueSafe<ZTask> task_queue_;
    WindowStateEnum_ window_state_;
};

}//gui
}//zengine