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

#include "z_gui_object.h"

namespace zengine {
namespace gui {

/*
    Window class.
*/
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    NODISCARD FORCEINLINE static Int32 WindowNum() noexcept { return window_num_; }

    ZWindow() noexcept;
    ZWindow(ZWindow&& _window) noexcept;
    ZWindow(Int32 _width, Int32 _height, const Char* _title_str, Bool _full_screen, ZWindow* _share_window) noexcept;
    ~ZWindow() noexcept;

    ZWindow& operator=(ZWindow&& _window) noexcept;

    NODISCARD ReturnType Create(
        Int32 _width, Int32 _height, const Char* _title_str, Bool _full_screen, ZWindow* _share_window
    ) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZWindow(const ZWindow&) = delete;

    ZWindow& operator=(const ZWindow&) = delete;

    //The num of the windows, will call glfwTerminate when window_num_ = 0.
    inline static Int32 window_num_ = 0;

    Handle handle_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_WINDOW_H_
