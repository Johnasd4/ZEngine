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
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

ZWindow::ZWindow() noexcept 
    : SuperType_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated) {}

ZWindow::ZWindow(ZWindow&& _window) noexcept 
    : SuperType_(std::forward<ZWindow>(_window))
{
    MoveP(std::forward<ZWindow>(_window));
}

ZWindow::ZWindow(Int32 _width, Int32 _height, const Char* _title, WindowScreenModeEnum_ _screen_mode) noexcept
    : SuperType_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated)
{
    ReturnType link_code = kOK;
    link_code = Create(_width, _height, _title, _screen_mode);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kZWindowErrorCodeLinkError, 0, L"ZWindow::Create() link error!");
    }
}


ZWindow::~ZWindow() noexcept {}

ZWindow& ZWindow::operator=(ZWindow&& _window) noexcept {
    SuperType_::operator=(std::forward<ZWindow>(_window));
    MoveP(std::forward<ZWindow>(_window));
    return *this;
}

Void ZWindow::Hide() noexcept {
    SuperType_::Hide();
    glfwHideWindow(static_cast<GLFWwindow*>(window_handle_));
    window_state_ = kWindowStateHidden;
}

Void ZWindow::Show() noexcept {
    SuperType_::Show();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    switch (window_state_) {
    case kWindowStateTerminated:
        Z_LOG_ERROR(error_code::kZWindowErrorCodeWindowNotExist, 0, L"Window does not exist!");
        break;
    case kWindowStateOpened:
        break;
    case kWindowStateClosed:
        Begin();
        glfwSetWindowShouldClose(static_cast<GLFWwindow*>(window_handle_), false);
        glfwShowWindow(static_cast<GLFWwindow*>(window_handle_));
        window_state_ = kWindowStateOpened;
        ++active_window_num_;
        break;
    case kWindowStateHidden:
        glfwShowWindow(static_cast<GLFWwindow*>(window_handle_));
        window_state_ = kWindowStateOpened;
        break;
    }
}

NODISCARD ReturnType ZWindow::Create(
    Int32 _width, Int32 _height, const Char* _title, WindowScreenModeEnum_ _screen_mode
) noexcept {
    ReturnType ret_val = kOK;

    if (window_state_ != kWindowStateTerminated) {
        ret_val = error_code::kZWindowErrorCodeWindowAreadyCreated;
        Z_LOG_ERROR(ret_val, 0, L"Window already created!");
        return ret_val;
    }

    //create a window
    switch (_screen_mode) {
    case kWindowScreenModeWindow:
        //create window
        window_handle_ = static_cast<Void*>(glfwCreateWindow(_width, _height, _title, nullptr, nullptr));
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    case kWindowScreenModeFullScreenCustomSize:
        //create window
        window_handle_ = static_cast<Void*>(
            glfwCreateWindow(_width, _height, _title, glfwGetPrimaryMonitor(), nullptr));
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    case kWindowScreenModeFullScreenDefaultSize:
    {
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
        window_handle_ = static_cast<Void*>(
            glfwCreateWindow(video_mode->width, video_mode->height, _title, main_monitor, nullptr));
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    }
    default:
        break;
    }

    //opengl
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(window_handle_));
    glfwSwapInterval(tick_pur_window_tick_);

    //imgui
    IMGUI_CHECKVERSION();
    window_context_ = ImGui::CreateContext();
    ImGui::SetCurrentContext(static_cast<ImGuiContext*>(window_context_));
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window_handle_), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    window_state_ = kWindowStateOpened;
    ++active_window_num_;

    return ret_val;
}

NODISCARD ReturnType ZWindow::Destroy() noexcept {
    ReturnType ret_val = kOK;
    window_handle_ = nullptr;
    window_context_ = nullptr;
    window_state_ = kWindowStateTerminated;
    return ret_val;
}

NODISCARD ReturnType ZWindow::Close() noexcept {
    ReturnType ret_val = kOK;
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(window_handle_), true);
    glfwHideWindow(static_cast<GLFWwindow*>(window_handle_));
    window_state_ = kWindowStateClosed;    
    --active_window_num_;
    return ret_val;
}

Void ZWindow::SetWidth(Int32 _width) noexcept {
    SuperType_::SetWidth(_width);
    Int32 width, height;
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &width, &height);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), _width, height);
}

Void ZWindow::SetHeight(Int32 _height) noexcept {
    SuperType_::SetWidth(_height);
    Int32 width, height;
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &width, &height);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), width, _height);
}

Void ZWindow::SetSize(Int32 _width, Int32 _height) noexcept {
    SuperType_::SetSize(_width, _height);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), _width, _height);
}

Void ZWindow::SetXPos(Int32 _x_pos) noexcept {
    //TODO
}
Void ZWindow::SetYPos(Int32 _y_pos) noexcept {
    //TODO
}

Void ZWindow::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {
    SuperType_::SetSize(_x_pos, _y_pos);
    glfwSetWindowPos(static_cast<GLFWwindow*>(window_handle_), _x_pos, _y_pos);
}

Void ZWindow::SetTitle(const Char* _title) noexcept {
    glfwSetWindowTitle(static_cast<GLFWwindow*>(window_handle_), _title);
}



Void ZWindow::SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept {
    Int32 window_width, window_height, x_pos, y_pos;
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &window_width, &window_height);
    glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &x_pos, &y_pos);

    //get the main monitor
    GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
    if (main_monitor == nullptr) {
        Z_LOG_ERROR(error_code::kZWindowErrorCodeLinkError, 0, L"glfwGetPrimaryMonitor() link error!");
        return;
    }
    //get the main monitor
    const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
    if (video_mode == nullptr) {
        Z_LOG_ERROR(error_code::kZWindowErrorCodeLinkError, 0, L"glfwGetVideoMode() link error!");
        return;
    }

    switch (_screen_mode) {
    case kWindowScreenModeWindow:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), nullptr, x_pos, y_pos, window_width, window_height, 0);
        break;
    case kWindowScreenModeFullScreenCustomSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, window_width, window_height, 0);
        break;
    case kWindowScreenModeFullScreenDefaultSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, video_mode->width, video_mode->height, 0);
        break;
    default:
        break;
    }
}

Void ZWindow::MoveP(ZWindow&& _window) noexcept {
    window_handle_ = _window.window_handle_;
    window_context_ = _window.window_context_;
    window_state_ = _window.window_state_;
    _window.window_handle_ = nullptr;
    _window.window_context_ = nullptr;
    _window.window_state_ = kWindowStateTerminated;
}

Int32 ZWindow::tick_pur_window_tick_ = 0;

Int32 ZWindow::active_window_num_ = 0;

}//gui
}//zengine