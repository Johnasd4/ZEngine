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
#define GUI_DLLFILE
#include "drive/d_pch.h"

#include "drive/d_function.h"

namespace zengine {
namespace gui {

GUI_DLLAPI NODISCARD GuiSize ScreenSize() noexcept {
    GLFWmonitor* screen = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(screen);
    return GuiSize((Float32)mode->width, (Float32)mode->height);
}

GUI_DLLAPI NODISCARD GuiPos ScreenCenterPos(GuiSize _gui_obj_size) noexcept {
    GuiSize screen_size = ScreenSize();
    return GuiPos(
        screen_size.width_ * 0.5f - _gui_obj_size.width_ * 0.5f, 
        screen_size.height_ * 0.5f - _gui_obj_size.height_ * 0.5f
    );
}

GUI_DLLAPI NODISCARD Float32 TextLineHeight() noexcept {
    return ImGui::GetTextLineHeight();
}

GUI_DLLAPI NODISCARD Float32 TextLineHeightWithSpacing() noexcept {
    return ImGui::GetTextLineHeightWithSpacing();
}


}//gui
}//zengine