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

#include "internal/d_function.h"

namespace zengine {
namespace gui {

GUI_DLLAPI NODISCARD GuiSize ScreenSize() noexcept {
    GLFWmonitor* screen = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(screen);
    return GuiSize((Float32)mode->width, (Float32)mode->height);
}

}//gui
}//zengine