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

namespace zengine {
namespace gui {

ZWindow::ZWindow() noexcept 
    : SuperType_()
    , title_()
    , handle_(nullptr)
    , window_state_(kWindowStateTerminated)
    , screen_mode_(kWindowScreenModeWindow)
    , tick_pur_window_tick_(0) {}

ZWindow::ZWindow(ZWindow&& _window) noexcept 
    : SuperType_(std::forward<ZWindow>(_window))
{
    MoveP(std::forward<ZWindow>(_window));
}

ZWindow::~ZWindow() noexcept {}

ZWindow& ZWindow::operator=(ZWindow&& _window) noexcept {
    SuperType_::operator=(std::forward<ZWindow>(_window));
    MoveP(std::forward<ZWindow>(_window));
    return *this;
}

NODISCARD ReturnType ZWindow::Execute() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    
    link_code = CreateWindowP();
    if (link_code != kOK) {
        ret_val = error_code::kZWindowErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, 0, L"ZWindow::CreateP() link error!");
        return ret_val;
    }

    Initialize();

    while (!glfwWindowShouldClose(static_cast<GLFWwindow*>(handle_))) {
        //clear the buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        //exchange the buffer
        glfwSwapBuffers(static_cast<GLFWwindow*>(handle_));
        glfwPollEvents();
    }

    //all window released
    //clean up imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //clean up opengl3
    glfwDestroyWindow(static_cast<GLFWwindow*>(handle_));
    glfwTerminate();

    return ret_val;
}

NODISCARD ReturnType ZWindow::ExecuteInNewThread() noexcept {
    ReturnType ret_val = kOK;

    return ret_val;
}

Void ZWindow::SetTitle(const Char* _title) noexcept {
    title_ = _title;
}

Void ZWindow::SetWidth(Int32 _width) noexcept {
    SuperType_::SetWidth(_width);
}

Void ZWindow::SetHeight(Int32 _height) noexcept {
    SuperType_::SetHeight(_height);
}

Void ZWindow::SetSize(Int32 _width, Int32 _height) noexcept {
    SuperType_::SetSize(_width, _height);
}

Void ZWindow::SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept {
    screen_mode_ = _screen_mode;
}

Void ZWindow::SetVerticalSynchronization(Int32 _tick_pur_window_tick) noexcept {
    tick_pur_window_tick_ = _tick_pur_window_tick;
}

Void ZWindow::Initialize() noexcept {}

Void ZWindow::Tick(Float32 _delta_time) noexcept {}

Void ZWindow::MoveP(ZWindow&& _window) noexcept {
    title_ = std::move(_window.title_);
    handle_ = _window.handle_;
    window_state_ = _window.window_state_;
    screen_mode_ = _window.screen_mode_;
    tick_pur_window_tick_ = _window.tick_pur_window_tick_;
    _window.handle_ = nullptr;
    _window.window_state_ = kWindowStateTerminated;
    _window.screen_mode_ = kWindowScreenModeWindow;
    _window.tick_pur_window_tick_ = 0;
}

NODISCARD ReturnType ZWindow::CreateWindowP() noexcept {
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
            handle_ = static_cast<Void*>(glfwCreateWindow(Width(), Height(), title_.String(), nullptr, nullptr));
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        case kWindowScreenModeFullScreenCustomSize:
            //create window
            handle_ = static_cast<Void*>(
                glfwCreateWindow(Width(), Height(), title_.String(), glfwGetPrimaryMonitor(), nullptr));
            if (handle_ == nullptr) {
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
            handle_ = static_cast<Void*>(glfwCreateWindow(Width(), Height(), title_.String(), main_monitor, nullptr));
            if (handle_ == nullptr) {
                ret_val = error_code::kZWindowErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
                return ret_val;
            }
            break;
        }
        default:
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetVideoMode() link error!");
            return ret_val;
    }
    
    ZGuiObject::OpenGL3Mutex().Lock();
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(handle_));
    glfwSwapInterval(tick_pur_window_tick_);
    ZGuiObject::OpenGL3Mutex().Unlock();

    ZGuiObject::ImguiMutex().Lock();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(handle_), true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ZGuiObject::ImguiMutex().Unlock();

    return ret_val;
}

}//gui
}//zengine