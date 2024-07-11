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
#define GUI_DLLFILE

#include "z_window.h"

namespace zengine {
namespace gui {

namespace internal {

/*
    Controls the global resourse initialze an terminate. Use the instance to create and destroy windows.
*/
class ZWindowManager {
    ZWindowManager() noexcept;

    ~ZWindowManager() noexcept;


};

}//internal

/*
    Creates an empty window.
*/
NODISCARD static ReturnType CreateWindowP(
    Int32 _width, Int32 _height, const Char* _title_str, Bool _full_screen, ZWindow* _share_window
) noexcept {

}

ZWindow::ZWindow() noexcept : handle_(nullptr) {}
ZWindow::ZWindow(ZWindow&& _window) noexcept : handle_(_window.handle_) {
    _window.handle_ == nullptr;
}
ZWindow::ZWindow(
    Int32 _width, Int32 _height, const Char* _title_str, Bool _full_screen, ZWindow* _share_window
) noexcept : handle_(nullptr) {
    ++window_num_;

}

NODISCARD static ReturnType CreateWindowP(
    Int32 _width, Int32 _height, const Char* _title_str, Bool _full_screen, ZWindow* _share_window
) noexcept {

}


}//gui
}//zengine