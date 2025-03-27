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

#include "z_window.h"

#include "../z_core/m_log.h"
#include "../z_core/t_lock_guard.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace gui {

namespace internal {

class ZWindowCallback : public ZObject {
public:
    /*
        Set the current active window.
    */
    static Void SetActiveWindowPtr(ZWindow* window_ptr) noexcept {
        InstanceP().active_window_ptr_ = window_ptr;
    }

    /*
        Key callback function.
    */
    static Void KeyCallBack(GLFWwindow* window, Int32 _key, Int32 scancode, Int32 _action, Int32 _mods) noexcept {
        ZWindow* window_ptr = InstanceP().active_window_ptr_;
        if (_action == GLFW_PRESS) {
            window_ptr->OnKeyPress(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0);
            window_ptr->OnKeyDown(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0);
        }
        else if (_action == GLFW_RELEASE) {
            window_ptr->OnKeyUp(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0);
        }
        else if (_action == GLFW_REPEAT) {
            window_ptr->OnKeyPress(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0);
        }
    }

    /*
        Mouse button callback function.
    */
    static Void MouseButtonCallback(GLFWwindow* _window, Int32 _button, Int32 _action, Int32 _mods) noexcept {
        ZWindow* window_ptr = InstanceP().active_window_ptr_;
        if (_action == GLFW_PRESS) {
            window_ptr->OnMouseDown(
                static_cast<MouseButtonEnum>(_button), 
                (_mods & GLFW_MOD_SHIFT) != 0, 
                (_mods & GLFW_MOD_CONTROL) != 0, 
                (_mods & GLFW_MOD_ALT) != 0);
        }
        else if (_action == GLFW_RELEASE) {
            window_ptr->OnMouseUp(
                static_cast<MouseButtonEnum>(_button),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0);
        }
    }

    /*
        Mouse scroll callback function.
    */
    static Void MouseScrollCallback(GLFWwindow* _window, Float64 _x_offset, Float64 _y_offset) {
        ZWindow* window_ptr = InstanceP().active_window_ptr_;
        window_ptr->OnScrollMove(static_cast<Float32>(_x_offset), static_cast<Float32>(_y_offset));
    }

    /*
        Mouse position callback function.
    */
    static Void MousePositionCallback(GLFWwindow* _window_handle, Float64 _x_pos, Float64 _y_pos) noexcept {
        thread_local GuiPos pre_pos = { static_cast<Float32>(_x_pos), static_cast<Float32>(_y_pos) };
        GuiPos cur_pos = { static_cast<Float32>(_x_pos), static_cast<Float32>(_y_pos) };
        ZWindow* window_ptr = InstanceP().active_window_ptr_;
        window_ptr->OnMouseMove(pre_pos, cur_pos);
        pre_pos = cur_pos;
    }

protected:
    using SuperType_ = ZObject;

private:
    ZWindowCallback(const ZWindowCallback&) = delete;
    ZWindowCallback(ZWindowCallback&&) = delete;
    ZWindowCallback& operator=(const ZWindowCallback&) = delete;
    ZWindowCallback& operator=(ZWindowCallback&&) = delete;

    FORCEINLINE NODISCARD static ZWindowCallback& InstanceP() noexcept {
        thread_local ZWindowCallback instance;
        return instance;
    } 

    ZWindowCallback() noexcept : SuperType_(), active_window_ptr_() {}

    ZWindow* active_window_ptr_;
};

}//internal

ZWindow::ZWindow() noexcept 
    : SuperType_()
    , window_handle_(nullptr) 
    , window_context_(nullptr)
    , window_state_(kWindowState_Terminated)
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
    , window_state_(kWindowState_Terminated)
    , frame_ptr_set_()
{
    ReturnType link_code = kOK;
    link_code = CreateP(_name, _size, _pos, _screen_mode);
    if (link_code != kOK) {
        Z_LOG_ERROR(error_code::kZWindowErrorCode_LinkError, 0, L"ZWindow::Create() link error!");
    }
    internal::ZWindowCallback::SetActiveWindowPtr(this);
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

    UInt32 pre_time = clock();

    //begin window
    Begin();

    while (!glfwWindowShouldClose(window_handle_)) {
        //calculate delta time
        UInt32 current_time = clock();
        Float32 delta_time = static_cast<Float32>(current_time - pre_time) * 0.001f;
        pre_time = current_time;

        //ticks window
        Tick(delta_time);
    }

    //prepare to detroy the window
    OnDestroy();

    //release imgui resourses
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //release opengl resourses
    glfwDestroyWindow(window_handle_);

    //destroys the window
    Destroy();

    return ret_val;
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
    if (!Enabled() || window_state_ != ZWindow::kWindowState_Opened) {
        Sleep(1);
        return;
    }

    GuiSize cur_size = Size();
    GuiPos cur_pos = Pos();

    //Imgui frame start
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //update size
    if (SizeChanged()) {
        glfwSetWindowSize(
            window_handle_,
            static_cast<Int32>(cur_size.width_), 
            static_cast<Int32>(cur_size.height_));
    }
    else {
        Int32 temp_width = 0, temp_height = 0;
        glfwGetWindowSize(window_handle_, &temp_width, &temp_height);
        GuiSize temp_size = { static_cast<Float32>(temp_width), static_cast<Float32>(temp_height) };
        if (temp_size != cur_size) {
            SetSize(temp_size);
        }
    }

    //update pos
    if (PosChanged()) {
        glfwSetWindowPos(
            window_handle_,
            static_cast<Int32>(cur_pos.x_), 
            static_cast<Int32>(cur_pos.y_));
    }
    else {
        Int32 temp_pos_x = 0, temp_pos_y = 0;
        glfwGetWindowPos(window_handle_, &temp_pos_x, &temp_pos_y);
        GuiPos temp_pos = { static_cast<Float32>(temp_pos_x), static_cast<Float32>(temp_pos_y) };
        if (temp_pos != cur_pos) {
            SetPos(temp_pos);
        }
    }

    SuperType_::Tick(_delta_sec);

    //tick frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Tick(_delta_sec);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //Imgui frame end
    glfwSwapBuffers(window_handle_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
}

Void ZWindow::Hide() noexcept {
    SuperType_::Hide();
    glfwHideWindow(window_handle_);
    window_state_ = kWindowState_Hidden;
}

Void ZWindow::Show() noexcept {
    SuperType_::Show();
    switch (window_state_) {
    case kWindowState_Terminated:
        Z_LOG_ERROR(error_code::kZWindowErrorCode_WindowNotExist, 0, L"Window does not exist!");
        break;
    case kWindowState_Opened:
        break;
    case kWindowState_Hidden:
        glfwShowWindow(window_handle_);
        window_state_ = kWindowState_Opened;
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

Void ZWindow::Destroy() noexcept {
    OnDestroy();
    window_handle_ = nullptr;
    window_context_ = nullptr;
    frame_ptr_set_.Clear();
    window_state_ = kWindowState_Terminated;
    --active_window_num_;
    if (active_window_num_ == 0) {             
        TLockGuard<ZMutex> lock_guard(OpenGLMutex());
        glfwTerminate();
    }
}

Void ZWindow::Add(ZFrame* _frame) noexcept {
    frame_ptr_set_.Insert(_frame);
    _frame->OnAdd(this);
}

Void ZWindow::SetBackgruondColour(GuiColour _colour) noexcept {
    glClearColor(_colour.red_, _colour.green_, _colour.blue_, _colour.alpha_);
}

Void ZWindow::SetName(const Char* _name) noexcept {
    glfwSetWindowTitle(window_handle_, _name);
}

Void ZWindow::SetScreenMode(WindowScreenModeEnum_ _screen_mode) noexcept {
    Int32 width = 0, height = 0, pos_x = 0, pos_y = 0;
    glfwGetWindowSize(window_handle_, &width, &height);
    glfwGetWindowPos(window_handle_, &pos_x, &pos_y);

    //get the main monitor
    GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
    if (main_monitor == nullptr) {
        Z_LOG_ERROR(error_code::kZWindowErrorCode_LinkError, 0, L"glfwGetPrimaryMonitor() link error!");
        return;
    }
    //get the main monitor
    const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
    if (video_mode == nullptr) {
        Z_LOG_ERROR(error_code::kZWindowErrorCode_LinkError, 0, L"glfwGetVideoMode() link error!");
        return;
    }

    switch (_screen_mode) {
    case kWindowScreenMode_Window:
        glfwSetWindowMonitor(
            window_handle_, nullptr, pos_x, pos_y, width, height, 0);
        break;
    case kWindowScreenMode_FullScreenCustomSize:
        glfwSetWindowMonitor(
            window_handle_, main_monitor, 0, 0, width, height, 0);
        break;
    case kWindowScreenMode_FullScreenDefaultSize:
        glfwSetWindowMonitor(
            window_handle_, main_monitor, 0, 0, video_mode->width, video_mode->height, 0);
        break;
    default:
        break;
    }
}

Void ZWindow::SetScreenCenter() noexcept {
    GuiSize window_size = Size();
    GuiSize screen_size = ScreenSize();
    SetPos(GuiPos(screen_size.width_ * 0.5f - window_size.width_ * 0.5f,
                  screen_size.height_ * 0.5f - window_size.height_ * 0.5f));
}

NODISCARD GuiColour ZWindow::BackgruondColour() const noexcept {
    GuiColour colour = { 0, 0, 0, 0 };
    glGetFloatv(GL_COLOR_CLEAR_VALUE, reinterpret_cast<Float32*>(&colour));
    return colour;
}

NODISCARD GuiPos ZWindow::AbsPos() const noexcept { return GuiPos(0.0f, 0.0f); }

NODISCARD const Char* ZWindow::Name() const noexcept {
    return glfwGetWindowTitle(window_handle_);
}

Void ZWindow::OnKeyDown(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyDown(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyDown(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZWindow::OnKeyUp(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyUp(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyUp(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZWindow::OnKeyPress(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyPress(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyPress(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZWindow::OnMouseDown(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnMouseDown(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseDown(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZWindow::OnMouseUp(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnMouseUp(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseUp(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZWindow::OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept {
    SuperType_::OnScrollMove(_x_offset, _y_offset);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnScrollMove(_x_offset, _y_offset);
    }
}

Void ZWindow::OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {
    SuperType_::OnMouseMove(_pre_pos, _cur_pos);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseMove(_pre_pos, _cur_pos);
    }
}

Void ZWindow::OnDestroy() noexcept {}

Void ZWindow::MoveP(ZWindow&& _window) noexcept {
    window_handle_ = _window.window_handle_;
    window_context_ = _window.window_context_;
    window_state_ = _window.window_state_;
    _window.window_handle_ = nullptr;
    _window.window_context_ = nullptr;
    _window.window_state_ = kWindowState_Terminated;
    internal::ZWindowCallback::SetActiveWindowPtr(this);
}

NODISCARD ReturnType ZWindow::CreateP(
    const Char* _name, GuiSize _size, GuiPos _pos, WindowScreenModeEnum_ _screen_mode
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    static ZMutex create_window_mutex;
    if (window_state_ != kWindowState_Terminated) {
        ret_val = error_code::kZWindowErrorCode_WindowAreadyCreated;
        Z_LOG_ERROR(ret_val, 0, L"Window already created!");
        return ret_val;
    }

    {
        TLockGuard<ZMutex> lock_guard(OpenGLMutex());
        //init opengl
        if (glfwInit() != GLFW_TRUE) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwInit() link error!");
            return ret_val;
        }
    }

    if (link_code != kOK) {
        ret_val = error_code::kZWindowErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, 0, L"ZOpenGLManager::InitializeOpenGL() link error!");
        return ret_val;
    }

    //choose opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create a window
    switch (_screen_mode) {
    case kWindowScreenMode_Window:
    {
        TLockGuard<ZMutex> lock_guard(OpenGLMutex());
        //create window
        window_handle_ = glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_),
            _name,
            nullptr,
            nullptr);
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    }
    case kWindowScreenMode_FullScreenCustomSize:
    {
        TLockGuard<ZMutex> lock_guard(OpenGLMutex());
        //create window
        window_handle_ = glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_), 
            _name, 
            glfwGetPrimaryMonitor(), 
            nullptr);
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    }
    case kWindowScreenMode_FullScreenDefaultSize:
    {
        TLockGuard<ZMutex> lock_guard(OpenGLMutex());
        //get the main monitor
        GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
        if (main_monitor == nullptr) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetPrimaryMonitor() link error!");
            return ret_val;
        }
        //get the main monitor
        const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
        if (video_mode == nullptr) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetVideoMode() link error!");
            return ret_val;
        }
        //create window
        window_handle_ = glfwCreateWindow(video_mode->width, video_mode->height, _name, main_monitor, nullptr);
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() link error!");
            return ret_val;
        }
        break;
    }
    default:
        break;
    }

    //opengl
    glfwMakeContextCurrent(window_handle_) ;
    //set interval
    glfwSwapInterval(tick_pur_window_tick_);
    //set size and pos
    glfwSetWindowSize(window_handle_, static_cast<Int32>(_size.width_), static_cast<Int32>(_size.height_));
    glfwSetWindowPos(window_handle_, static_cast<Int32>(_pos.x_), static_cast<Int32>(_pos.y_));
    //bind callback functions
    glfwSetKeyCallback(window_handle_, internal::ZWindowCallback::KeyCallBack);
    glfwSetMouseButtonCallback(window_handle_, internal::ZWindowCallback::MouseButtonCallback);
    glfwSetCursorPosCallback(window_handle_, internal::ZWindowCallback::MousePositionCallback);
    glfwSetScrollCallback(window_handle_, internal::ZWindowCallback::MouseScrollCallback);

    //imgui
    IMGUI_CHECKVERSION();
    window_context_ = ImGui::CreateContext();
    ImGui::SetCurrentContext(window_context_);
    ImGui_ImplGlfw_InitForOpenGL(window_handle_, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    window_state_ = kWindowState_Opened;
    internal::ZWindowCallback::SetActiveWindowPtr(this);
    ++active_window_num_;

    return ret_val;
}

Int32 ZWindow::tick_pur_window_tick_ = 0;

TAtom<Int32> ZWindow::active_window_num_(0);

}//gui
}//zengine