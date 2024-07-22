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
#ifndef Z_GUI_Z_WINDOW_H_
#define Z_GUI_Z_WINDOW_H_

#include "internal/z_drive.h"

#include "../z_core/t_vector.h"

#include "z_gui_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZWindowErrorCode : ReturnType {
    kZWindowErrorCodeLinkError = kErrorCodeBaseZWindow,
    kZWindowErrorCodeWindowAreadyCreated,
    kZWindowErrorCodeWindowNotExist
};

}//error_code

/*
    Window class.
*/
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    /*
        The window screen mode.
    */
    enum WindowScreenModeEnum_ {
        kWindowScreenModeWindow,
        kWindowScreenModeFullScreenCustomSize,
        kWindowScreenModeFullScreenDefaultSize,
    };

    /*
        The window screen mode.
    */
    enum WindowStateEnum_ {
        kWindowStateTerminated,
        kWindowStateOpened,
        kWindowStateClosed,
        kWindowStateHidden
    };

    /*
        Sets Vertical synchronization.
        _tick_pur_window_tick: Set 0 to not use vertiacl synchronization, default 0.
    */
    FORCEINLINE static Void SetVerticalSynchronization(Int32 _tick_pur_window_tick) noexcept {
        tick_pur_window_tick_ = _tick_pur_window_tick; 
    }
    /*
        Sets Vertical synchronization.
        _tick_pur_window_tick: Set 0 to not use vertiacl synchronization, default 0.
    */
    NODISCARD FORCEINLINE static Int32 ActiveWindowNum() noexcept { return active_window_num_; }

    ZWindow() noexcept;
    ZWindow(Int32 _width, Int32 _height, const Char* _title, WindowScreenModeEnum_ _screen_mode) noexcept;
    ZWindow(ZWindow&& _window) noexcept;
    
    ~ZWindow() noexcept;

    ZWindow& operator=(ZWindow&& _window) noexcept;

    /*
        Creates the window, will initialize opengl if not initialized.
    */
    NODISCARD virtual ReturnType Create(
        Int32 _width, Int32 _height, const Char* _title, WindowScreenModeEnum_ _screen_mode
    ) noexcept;

    /*
        Destroy the window, release the resourses.
    */
    NODISCARD virtual ReturnType Destroy() noexcept;

    /*
        Close the window, calls reset when the window is opened again.
    */
    NODISCARD virtual ReturnType Close() noexcept;

    /*
        Hides the window.
    */
    virtual Void Hide() noexcept;

    /*
        Shows the window, if the window was closed, will reset the window.
    */
    virtual Void Show() noexcept;

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
    virtual Void SetBackgruondColour(ColourRGBA _colour) noexcept;
    virtual Void SetBackgruondColour(Int32 _red, Int32 _green, Int32 _blue, Int32 _alpha) noexcept;

    virtual GuiSize Size() noexcept;
    virtual GuiPos Pos() noexcept;
    virtual Int32 Width() noexcept;
    virtual Int32 Height() noexcept;
    virtual Int32 XPos() noexcept;
    virtual Int32 YPos() noexcept;
    virtual ColourRGBA BackgruondColour() noexcept;

    Void SetTitle(const Char* _title) noexcept;
    Void SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept;

    NODISCARD FORCEINLINE Handle WinowHandle() const noexcept { return window_handle_; }
    NODISCARD FORCEINLINE Handle WinowContext() const noexcept { return window_context_; }
    NODISCARD FORCEINLINE WindowStateEnum_ WinowState() const noexcept { return window_state_; }

protected:
    using SuperType_ = ZGuiObject;

private:
    friend class ZWindowManager;

    ZWindow(const ZWindow&) = delete;
    ZWindow& operator=(const ZWindow&) = delete;
    
    Void MoveP(ZWindow&& _window) noexcept;

    static Int32 tick_pur_window_tick_;

    static Int32 active_window_num_;

    Handle window_handle_;
    Handle window_context_;
    WindowStateEnum_ window_state_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_WINDOW_H_
