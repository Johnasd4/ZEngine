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

#include "../z_core/m_log.h"
#include "../z_core/t_atom.h"
#include "../z_core/z_string.h"

#include "z_gui_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZWindowErrorCode : ReturnType {
    kZWindowErrorCodeLinkError = kErrorCodeBaseZWindow,
    kZWindowErrorCodeWindowAreadyCreated
};

}//error_code

/*
    Window class.
*/
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    /*
        Starts the main loop of the window.
    */
    NODISCARD ReturnType Execute() noexcept;
    /*
        Starts the main loop of the window.
    */
    NODISCARD Void ExecuteInNewThread() noexcept;

    /*
        Window states.
    */
    enum WindowStateEnum_ {
        kWindowStateTerminated,
        kWindowStateExecuted
    };

    /*
        The window screen mode.
    */
    enum WindowScreenModeEnum_ {
        kWindowScreenModeWindow,
        kWindowScreenModeFullScreenCustomSize,
        kWindowScreenModeFullScreenDefaultSize,
    };

protected:
    using SuperType_ = ZObject;

    ZWindow() noexcept;
    ZWindow(ZWindow&& _window) noexcept;
    ~ZWindow() noexcept;
    ZWindow& operator=(ZWindow&& _window) noexcept;

    /*
        Called before executing.
    */
    virtual ReturnType Initialize() = 0;
    /*
        Called every tick
    */
    virtual ReturnType Tick(Float32 _delta_time) = 0;

    /*
        Sets the title of the window.
    */
    Void SetTitle(const Char* _title_str) noexcept;
    /*
        Sets the size of the window.
    */
    Void SetWidth(Int32 _width) noexcept;
    /*
        Sets the size of the window.
    */
    Void SetHeight(Int32 _height) noexcept;
    /*
        Sets the size of the window.
    */
    Void SetWindowSize(Int32 _width, Int32 _height) noexcept;
    /*
        Sets if full screen.
    */
    Void SetScreenMode(WindowScreenModeEnum_ screen_mode) noexcept;
    /*
        Sets Vertical synchronization.
        _tick_pur_window_tick: Set 0 to not use vertiacl synchronization.
    */
    Void SetVerticalSynchronization(Bool _tick_pur_window_tick) noexcept;


private:

    ZWindow(const ZWindow&) = delete;

    ZWindow& operator=(const ZWindow&) = delete;
    
    /*
        Creates the window, will initialize opengl if not initialized.
    */
    NODISCARD ReturnType CreateP() noexcept;
    /*
        Destroy the window, will terminate opengl if the last window destroyed.
    */
    NODISCARD ReturnType DestroyP() noexcept;

    static TAtom<Int32> window_num_;

    Handle handle_;
    WindowStateEnum_ window_state_;
    WindowScreenModeEnum_ screen_mode_;
    ZString title_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_WINDOW_H_
