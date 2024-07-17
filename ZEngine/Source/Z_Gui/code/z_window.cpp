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

#include "glfw/glfw3.h" 

namespace zengine {
namespace gui {

ZWindow::ZWindow() noexcept : handle_(nullptr), window_state_(kWindowStateTerminated) {}
ZWindow::ZWindow(ZWindow&& _window) noexcept : handle_(_window.handle_), window_state_(_window.window_state_) {
    _window.handle_ = nullptr;
    _window.window_state_ = kWindowStateTerminated;
}

NODISCARD ReturnType ZWindow::CreateP() noexcept {
    ReturnType ret_val = kOK;

    if (handle_ != nullptr) {
        ret_val = error_code::kZWindowErrorCodeWindowAreadyCreated;
        Z_LOG_ERROR(ret_val, 0, L"Window aready created, can not create new window!");
        return ret_val;
    }

    //init opengl
    if (glfwInit() != GLFW_TRUE) {
        ret_val = error_code::kZWindowErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, 0, L"glfwInit() link error!");
        return ret_val;
    }

    //create a window
    switch (screen_mode_) {
        case kWindowScreenModeWindow:
            //create window
            handle_ = (Void*)glfwCreateWindow(Width(), Height(), title_.String(), nullptr, nullptr);
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        case kWindowScreenModeFullScreenCustomSize:
            //create window
            handle_ = (Void*)glfwCreateWindow(Width(), Height(), title_.String(), glfwGetPrimaryMonitor(), nullptr);
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        case kWindowScreenModeFullScreenDefaultSize:
            //get the main monitor
            GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
            if (main_monitor == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwGetPrimaryMonitor() link error!");
                return ret_val;
            }
            //get the main monitor
            const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
            if (video_mode == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwGetVideoMode() link error!");
                return ret_val;
            }
            //create window
            handle_ = (Void*)glfwCreateWindow(Width(), Height(), title_.String(), main_monitor, nullptr);
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        default:
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetVideoMode() link error!");
            return ret_val;
    }

    //add window num
    ++window_num_;

    return ret_val;
}

NODISCARD ReturnType ZWindow::DestroyP() noexcept {
    ReturnType ret_val = kOK;
    return ret_val;
}

TAtom<Int32> ZWindow::window_num_(0);

}//gui
}//zengine