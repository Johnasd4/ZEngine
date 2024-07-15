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
ZWindow::ZWindow(
    Int32 _width, Int32 _height, const Char* _title_str, WindowScreenModeEnum _screen_mode, ZWindow* _share_window
) noexcept : handle_(nullptr), window_state_(kWindowStateTerminated) {
    ReturnType link_code = kOK;
    link_code = CreateP(_width, _height, _title_str, _screen_mode, _share_window);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kZWindowErrorCodeLinkError, link_code, L"ZWindow::CreateP() link error!");
    }
}

NODISCARD ReturnType ZWindow::CreateP(
    Int32 _width, Int32 _height, const Char* _title_str, WindowScreenModeEnum _screen_mode, ZWindow* _share_window
) noexcept {
    ReturnType ret_val = kOK;

    //init opengl
    if (glfwInit() != GLFW_TRUE) {
        ret_val = error_code::kZWindowErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, 0, L"glfwInit() link error!");
        return ret_val;
    }

    //create a window
    switch (_screen_mode) {
        case kWindowScreenModeWindow:
            //create window
            handle_ = (Void*)glfwCreateWindow(
                _width, _height, _title_str, nullptr, reinterpret_cast<GLFWwindow*>(_share_window->handle_));
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        case kWindowScreenModeFullScreenCustomSize:
            //create window
            handle_ = (Void*)glfwCreateWindow(
                _width, _height, _title_str, 
                glfwGetPrimaryMonitor(), reinterpret_cast<GLFWwindow*>(_share_window->handle_));
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
            handle_ = (Void*)glfwCreateWindow(
                video_mode->width, video_mode->height, _title_str, nullptr,
                reinterpret_cast<GLFWwindow*>(_share_window->handle_));
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