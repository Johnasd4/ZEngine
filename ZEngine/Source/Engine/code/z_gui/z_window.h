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

#include "z_gui_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZWindowErrorCode : ReturnType {
    kZWindowErrorCodeLinkError = kErrorCodeBaseZWindow,
    kZWindowErrorCodeScreenMode
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
        Calls every tick.
    */
    virtual Void Tick(Float32 _tick) noexcept = 0;

    /*
        Sets the title at runtime.
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
    Void SetFullScreen(Bool _if_full_screen) noexcept;

private:

    ZWindow(const ZWindow&) = delete;

    ZWindow& operator=(const ZWindow&) = delete;
    
    /*
        Creates the window, will initialize opengl if not initialized.
    */
    NODISCARD ReturnType CreateP(
        Int32 _width, Int32 _height, const Char* _title_str, WindowScreenModeEnum_ _screen_type, ZWindow* _share_window
    ) noexcept;
    /*
        Destroy the window, will terminate opengl if the last window destroyed.
    */
    NODISCARD ReturnType DestroyP() noexcept;

    static TAtom<Int32> window_num_;

    Handle handle_;
    WindowStateEnum_ window_state_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_WINDOW_H_
