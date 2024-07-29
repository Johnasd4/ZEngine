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
    , size_()
    , pos_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated)
    , frame_ptr_set_(){}

ZWindow::ZWindow(ZWindow&& _window) noexcept 
    : SuperType_(std::forward<ZWindow>(_window)), frame_ptr_set_()
{
    MoveP(std::forward<ZWindow>(_window));
}

ZWindow::ZWindow(Int32 _width, Int32 _height, const Char* _name, WindowScreenModeEnum_ _screen_mode) noexcept
    : SuperType_(true)
    , size_()
    , pos_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated)
    , frame_ptr_set_()
{
    ReturnType link_code = kOK;
    link_code = CreateP(_width, _height, _name, _screen_mode);
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

Void ZWindow::Begin() noexcept {
    SuperType_::Begin();
    //update size and pos
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &size_.width_, &size_.height_);
    glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &pos_.x_, &pos_.y_);

    //begin frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Begin();
    }
}

Void ZWindow::Tick(Float32 _delta_sec) noexcept {
    SuperType_::Tick(_delta_sec);

    //get current size and pos
    GuiSize cur_size = { 0, 0 };
    GuiPos cur_pos = { 0, 0 };
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &cur_size.width_, &cur_size.height_);
    glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &cur_pos.x_, &cur_pos.y_);

    //check if resized or moved
    if (cur_size != size_) {
        OnResize(size_.width_, size_.height_, cur_size.width_, cur_size.height_);
        size_ = cur_size;
    }
    if (cur_pos != pos_) {
        OnMove(pos_.x_, pos_.y_, cur_pos.x_, cur_pos.y_);
        pos_ = cur_pos;
    }

    //tick frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        Bool if_open = false;
        if (frame_ptr->Enabled()) {
            ImGui::Begin(frame_ptr->Name(), &if_open, frame_ptr->FrameFlag());
            if (if_open) {
                if (frame_ptr->IfTick()) {
                    frame_ptr->Tick(_delta_sec);
                }
            }
            else {
                frame_ptr->Close();
            }
            ImGui::End();
        }
    }
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

Void ZWindow::Close() noexcept {
    OnClose();
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(window_handle_), true);
    glfwHideWindow(static_cast<GLFWwindow*>(window_handle_));
    window_state_ = kWindowStateClosed;
    --active_window_num_;
}

Void ZWindow::Destroy() noexcept {
    OnDestroy();
    window_handle_ = nullptr;
    window_context_ = nullptr;
    window_state_ = kWindowStateTerminated;
}

Void ZWindow::AddFrame(ZFrame* _frame) noexcept {
    frame_ptr_set_.Insert(_frame);
    _frame->OnAdd(this);
    _frame->Begin();
}

Void ZWindow::SetWidth(Int32 _width) noexcept {
    SuperType_::SetWidth(_width);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), _width, size_.height_);
}

Void ZWindow::SetHeight(Int32 _height) noexcept {
    SuperType_::SetWidth(_height);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), size_.width_, _height);
}

Void ZWindow::SetSize(Int32 _width, Int32 _height) noexcept {
    SuperType_::SetSize(_width, _height);
    glfwSetWindowSize(static_cast<GLFWwindow*>(window_handle_), _width, _height);
}

Void ZWindow::SetXPos(Int32 _x_pos) noexcept {
    SuperType_::SetXPos(_x_pos);
    glfwSetWindowPos(static_cast<GLFWwindow*>(window_handle_), _x_pos, pos_.y_);
}
Void ZWindow::SetYPos(Int32 _y_pos) noexcept {
    SuperType_::SetYPos(_y_pos);
    glfwSetWindowPos(static_cast<GLFWwindow*>(window_handle_), pos_.x_, _y_pos);
}

Void ZWindow::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {
    SuperType_::SetSize(_x_pos, _y_pos);
    glfwSetWindowPos(static_cast<GLFWwindow*>(window_handle_), _x_pos, _y_pos);
}

Void ZWindow::SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept {
    SuperType_::SetBackgruondColour(_red, _green, _blue, _alpha);
    glClearColor(_red, _green, _blue, _alpha);
}

Void ZWindow::SetName(const Char* _name) noexcept {
    glfwSetWindowTitle(static_cast<GLFWwindow*>(window_handle_), _name);
}

Void ZWindow::SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept {
    GuiSize size = { 0, 0 };
    GuiPos pos = { 0, 0 };
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &size.width_, &size.height_);
    glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &pos.x_, &pos.y_);

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
            static_cast<GLFWwindow*>(window_handle_), nullptr, pos.x_, pos.y_, size.width_, size.height_, 0);
        break;
    case kWindowScreenModeFullScreenCustomSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, size.width_, size.height_, 0);
        break;
    case kWindowScreenModeFullScreenDefaultSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, video_mode->width, video_mode->height, 0);
        break;
    default:
        break;
    }
}

NODISCARD Int32 ZWindow::Width() const noexcept {
    return size_.width_;
}

NODISCARD Int32 ZWindow::Height() const noexcept {
    return size_.height_;
}

NODISCARD GuiSize ZWindow::Size() const noexcept {
    return size_;
}

NODISCARD Int32 ZWindow::XPos() const noexcept {
    return pos_.x_;
}

NODISCARD Int32 ZWindow::YPos() const noexcept {
    return pos_.y_;
}

NODISCARD GuiPos ZWindow::Pos() const noexcept {
    return pos_;
}

NODISCARD GuiColour ZWindow::BackgruondColour() const noexcept {
    GuiColour colour = { 0, 0, 0, 0 };
    glGetFloatv(GL_COLOR_CLEAR_VALUE, reinterpret_cast<Float32*>(&colour));
    return colour;
}

NODISCARD const Char* ZWindow::Name() const noexcept {
    return glfwGetWindowTitle(static_cast<GLFWwindow*>(window_handle_));
}

Void ZWindow::OnClose() noexcept {}
Void ZWindow::OnDestroy() noexcept {}

Void ZWindow::MoveP(ZWindow&& _window) noexcept {
    window_handle_ = _window.window_handle_;
    window_context_ = _window.window_context_;
    window_state_ = _window.window_state_;
    _window.window_handle_ = nullptr;
    _window.window_context_ = nullptr;
    _window.window_state_ = kWindowStateTerminated;
}

NODISCARD ReturnType ZWindow::CreateP(
    Int32 _width, Int32 _height, const Char* _name, WindowScreenModeEnum_ _screen_mode
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
        window_handle_ = static_cast<Void*>(glfwCreateWindow(_width, _height, _name, nullptr, nullptr));
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    case kWindowScreenModeFullScreenCustomSize:
        //create window
        window_handle_ = static_cast<Void*>(
            glfwCreateWindow(_width, _height, _name, glfwGetPrimaryMonitor(), nullptr));
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
            glfwCreateWindow(video_mode->width, video_mode->height, _name, main_monitor, nullptr));
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

Int32 ZWindow::tick_pur_window_tick_ = 0;

Int32 ZWindow::active_window_num_ = 0;

}//gui
}//zengine