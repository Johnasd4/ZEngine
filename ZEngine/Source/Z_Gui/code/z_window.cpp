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

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

ZWindow::ZWindow() noexcept 
    : SuperType_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated)
    , frame_ptr_set_(){}

ZWindow::ZWindow(ZWindow&& _window) noexcept 
    : SuperType_(std::forward<ZWindow>(_window)), frame_ptr_set_()
{
    MoveP(std::forward<ZWindow>(_window));
}

ZWindow::ZWindow(const Char* _name, GuiSize _size, GuiPos _pos, WindowScreenModeEnum_ _screen_mode) noexcept
    : SuperType_(_size, _pos, true)
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowStateTerminated)
    , frame_ptr_set_()
{
    ReturnType link_code = kOK;
    link_code = CreateP(_name, _size, _pos, _screen_mode);
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
    //begin frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Begin();
    }
}

Void ZWindow::Tick(Float32 _delta_sec) noexcept {
    GuiSize cur_size = Size();
    GuiPos cur_pos = Pos();

    //update size
    if (SizeChanged()) {
        glfwSetWindowSize(
            static_cast<GLFWwindow*>(window_handle_), 
            static_cast<Int32>(cur_size.width_), 
            static_cast<Int32>(cur_size.height_));
    }
    else {
        Int32 temp_width = 0, temp_height = 0;
        glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &temp_width, &temp_height);
        GuiSize temp_size = { static_cast<Float32>(temp_width), static_cast<Float32>(temp_height) };
        if (temp_size != cur_size) {
            SetSize(temp_size);
        }
    }

    //update pos
    if (PosChanged()) {
        glfwSetWindowPos(
            static_cast<GLFWwindow*>(window_handle_), 
            static_cast<Int32>(cur_pos.x_), 
            static_cast<Int32>(cur_pos.y_));
    }
    else {
        Int32 temp_pos_x = 0, temp_pos_y = 0;
        glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &temp_pos_x, &temp_pos_y);
        GuiPos temp_pos = { static_cast<Float32>(temp_pos_x), static_cast<Float32>(temp_pos_y) };
        if (temp_pos != cur_pos) {
            SetPos(temp_pos);
        }
    }

    SuperType_::Tick(_delta_sec);

    //tick frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        if (frame_ptr->Visiable()) {
            //push background colour
            GuiColour bg_colour = frame_ptr->BackgruondColour();
            ImGui::PushStyleColor(ImGuiCol_WindowBg, *reinterpret_cast<ImVec4*>(&bg_colour));
            //begin base frame
            ImGui::Begin(frame_ptr->Name(), nullptr, frame_ptr->FrameFlag());
            
            if (frame_ptr->Enabled()) {
                frame_ptr->Tick(_delta_sec);
            }
            //end base frame
            ImGui::End();
            //pop background colour
            ImGui::PopStyleColor();
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

Void ZWindow::Reset() noexcept {
    SuperType_::Reset();
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Reset();
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
    frame_ptr_set_.Clear();
    window_state_ = kWindowStateTerminated;
}

Void ZWindow::Add(ZFrame* _frame) noexcept {
    frame_ptr_set_.Insert(_frame);
    _frame->OnAdd(this);
}

Void ZWindow::SetBackgruondColour(GuiColour _colour) noexcept {
    glClearColor(_colour.red_, _colour.green_, _colour.blue_, _colour.alpha_);
}

Void ZWindow::SetName(const Char* _name) noexcept {
    glfwSetWindowTitle(static_cast<GLFWwindow*>(window_handle_), _name);
}

Void ZWindow::SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept {
    Int32 width = 0, height = 0, pos_x = 0, pos_y = 0;
    glfwGetWindowSize(static_cast<GLFWwindow*>(window_handle_), &width, &height);
    glfwGetWindowPos(static_cast<GLFWwindow*>(window_handle_), &pos_x, &pos_y);

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
            static_cast<GLFWwindow*>(window_handle_), nullptr, pos_x, pos_y, width, height, 0);
        break;
    case kWindowScreenModeFullScreenCustomSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, width, height, 0);
        break;
    case kWindowScreenModeFullScreenDefaultSize:
        glfwSetWindowMonitor(
            static_cast<GLFWwindow*>(window_handle_), main_monitor, 0, 0, video_mode->width, video_mode->height, 0);
        break;
    default:
        break;
    }
}

NODISCARD GuiColour ZWindow::BackgruondColour() const noexcept {
    GuiColour colour = { 0, 0, 0, 0 };
    glGetFloatv(GL_COLOR_CLEAR_VALUE, reinterpret_cast<Float32*>(&colour));
    return colour;
}

NODISCARD GuiPos ZWindow::AbsPos() const noexcept { return GuiPos(0.0f, 0.0f); }

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
    const Char* _name, GuiSize _size, GuiPos _pos, WindowScreenModeEnum_ _screen_mode
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
        window_handle_ = static_cast<Void*>(glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_),
            _name,
            nullptr,
            nullptr));
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    case kWindowScreenModeFullScreenCustomSize:
        //create window
        window_handle_ = static_cast<Void*>(glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_), 
            _name, 
            glfwGetPrimaryMonitor(), 
            nullptr));
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
    glfwSetWindowSize(
        static_cast<GLFWwindow*>(window_handle_), 
        static_cast<Int32>(_size.width_),
        static_cast<Int32>(_size.height_));
    glfwSetWindowPos(
        static_cast<GLFWwindow*>(window_handle_), 
        static_cast<Int32>(_pos.x_), 
        static_cast<Int32>(_pos.y_));

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