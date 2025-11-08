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

#include "z_core/f_file_system.h"
#include "z_core/m_log.h"
#include "z_core/z_object.h"

#include "z_frame.h"

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
    , imgui_context_ptr_(nullptr)
    , imgui_io_ptr_(nullptr)
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
    , imgui_context_ptr_(nullptr)
    , imgui_io_ptr_(nullptr)
    , window_state_(kWindowState_Terminated)
    , frame_ptr_set_()
{
    ReturnType link_code = kOK;
    link_code = CreateWindowP(_name, _size, _screen_mode);
    if (link_code != kOK) {
        Destroy();
        Z_LOG_ERROR(error_code::kZWindowErrorCode_LinkError, link_code, L"ZWindow::CreateWindowP() link error!");
        return;
    }

    if (_pos != kBasePos) {
        Int32 pos_x = 0, pos_y = 0;
        glfwGetWindowPos(window_handle_, &pos_x, &pos_y);
        _pos = GuiPos(static_cast<Float32>(pos_x), static_cast<Float32>(pos_y));
        SetPos(_pos);
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

    Z_CHECK(
        window_state_ != kWindowState_Initialized,
        error_code::kZWindowErrorCode_StateError,
        L"Window state error! state: %d expect state: %d",
        window_state_, kWindowState_Initialized
    );

    window_state_ = kWindowState_Opened;

    UInt32 pre_time = clock();

    //render loop
    while (!glfwWindowShouldClose(window_handle_)) {
        glfwMakeContextCurrent(window_handle_);
        
        //clear buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Imgui frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //calculate delta time
        UInt32 current_time = clock();
        Float32 delta_time = static_cast<Float32>(current_time - pre_time) * 0.001f;
        pre_time = current_time;

        //call tasks
        while (task_queue_.Size() > 0ULL) {
            link_code = task_queue_.Front().Run();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZWindowErrorCode_LinkError, 
                    link_code, 
                    L"ZTask::Run() link error!"
                );
            }
            task_queue_.Pop();
        }

        //ticks window
        Tick(delta_time);

        //render frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //Imgui frame end
        glfwSwapBuffers(window_handle_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
    }

    //destroys window
    Destroy();

    return ret_val;
}

Void ZWindow::Begin() noexcept {
    SuperType_::Begin();
}

Void ZWindow::Tick(Float32 _delta_sec) noexcept {
    if (!Enabled() || window_state_ != ZWindow::kWindowState_Opened) {
        Sleep(1);
        return;
    }

    GuiSize cur_size = Size();
    GuiPos cur_pos = Pos();

    //update size
    if (SizeChanged()) {
        glfwSetWindowSize(
            window_handle_,
            static_cast<Int32>(cur_size.width_), 
            static_cast<Int32>(cur_size.height_)
        );
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
}

Void ZWindow::Hide() noexcept {
    if (window_state_ != kWindowState_Opened) {
        return;
    }
    SuperType_::Hide();
    glfwHideWindow(window_handle_);
    window_state_ = kWindowState_Hidden;
}

Void ZWindow::Show() noexcept {
    if (window_state_ != kWindowState_Hidden) {
        return;
    }
    glfwShowWindow(window_handle_);
    window_state_ = kWindowState_Opened;
}

Void ZWindow::Reset() noexcept {
    SuperType_::Reset();
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Reset();
    }
}

NODISCARD ReturnType ZWindow::Create(
    const Char* _name,
    GuiSize _size,
    GuiPos _pos,
    WindowScreenModeEnum_ _screen_mode
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    link_code = CreateWindowP(_name, _size, _screen_mode);
    if (link_code != kOK) {
        Destroy();
        ret_val = error_code::kZWindowErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZWindow::CreateWindowP() link error!");
        return ret_val;
    }

    if (_pos != kBasePos) {
        Int32 pos_x = 0, pos_y = 0;
        glfwGetWindowPos(window_handle_, &pos_x, &pos_y);
        _pos = GuiPos(static_cast<Float32>(pos_x), static_cast<Float32>(pos_y));
        SetPos(_pos);
    }

    internal::ZWindowCallback::SetActiveWindowPtr(this);

    return ret_val;
}

Void ZWindow::Destroy() noexcept {
    OnDestroy();

    if (imgui_context_ptr_ != nullptr) {
        //release imgui resourses
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    //release opengl resourses
    if (window_handle_ != nullptr) {
        glfwDestroyWindow(window_handle_);
    }

    window_handle_ = nullptr;
    imgui_context_ptr_ = nullptr;
    imgui_io_ptr_ = nullptr;
    frame_ptr_set_.Clear();
    window_state_ = kWindowState_Terminated;

    {
        TLockGuard lock_guard(OpenGLMutex());
        --active_context_num_;
        if (active_context_num_ == 0) {
            glfwTerminate();
        }
    }
}

NODISCARD ReturnType ZWindow::Add(ZFrame* _frame_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        _frame_ptr == nullptr,
        error_code::kZFrameErrorCode_NullptrParam,
        L"_widget_obj is nullptr!"
    );

    frame_ptr_set_.Insert(_frame_ptr);
    _frame_ptr->OnAdd(this);

    return ret_val;
}

Void ZWindow::AddTask(ZTask&& _task) noexcept {
    task_queue_.Push(std::forward<ZTask>(_task));
}

ReturnType ZWindow::Remove(ZFrame* _frame_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        _frame_ptr == nullptr,
        error_code::kZFrameErrorCode_NullptrParam,
        L"_widget_obj is nullptr!"
    );

    frame_ptr_set_.Erase(_frame_ptr);
    _frame_ptr->OnRemove(this);

    return ret_val;
}

Void ZWindow::RemoveAll() noexcept {
    ReturnType ret_val = kOK;

    TVector<ZFrame*> remove_frame_ptr_vector;
    for (auto iter = frame_ptr_set_.Begin(); iter != frame_ptr_set_.End(); ++iter) {
        remove_frame_ptr_vector.PushBack(*iter);
    }
    frame_ptr_set_.Clear();
    for (auto iter = remove_frame_ptr_vector.Begin(); iter != remove_frame_ptr_vector.End(); ++iter) {
        (*iter)->OnRemove(this);
    }
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
        Z_LOG_ERROR(error_code::kZWindowErrorCode_SystemError, 0, L"glfwGetPrimaryMonitor() system error!");
        return;
    }
    //get the main monitor
    const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
    if (video_mode == nullptr) {
        Z_LOG_ERROR(error_code::kZWindowErrorCode_SystemError, 0, L"glfwGetVideoMode() system error!");
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
    SetPos(ScreenCenterPos(Size()));
}

NODISCARD GuiPos ZWindow::AbsPos() const noexcept { return GuiPos(0.0f, 0.0f); }

NODISCARD const Char* ZWindow::Name() const noexcept {
    return glfwGetWindowTitle(window_handle_);
}

NODISCARD GuiColour ZWindow::BackgruondColour() const noexcept {
    GuiColour colour = { 0, 0, 0, 0 };
    glGetFloatv(GL_COLOR_CLEAR_VALUE, reinterpret_cast<Float32*>(&colour));
    return colour;
}

NODISCARD ReturnType ZWindow::LoadFontFromFileTTF(
    const Char* _file_dir,
    Float32 _font_size,
    FontLanguageEnum_ _font_language
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (imgui_context_ptr_ == nullptr) {
        link_code = CreateContextP();
        if (link_code != kOK) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZWindow::CreateContextP() link error!");
            return ret_val;
        }
    }

    ImFont* font_ptr = nullptr;

    switch (_font_language) {
    case kFontLanguage_SimplifiedChineseCommon:
        font_ptr = imgui_io_ptr_->Fonts->AddFontFromFileTTF(
            _file_dir, 
            _font_size,
            nullptr, 
            imgui_io_ptr_->Fonts->GetGlyphRangesChineseSimplifiedCommon()
        );
        break;
    case kFontLanguage_SimplifiedChineseAll:
        font_ptr = imgui_io_ptr_->Fonts->AddFontFromFileTTF(
            _file_dir,
            _font_size,
            nullptr,
            imgui_io_ptr_->Fonts->GetGlyphRangesChineseFull()
        );
        break;
    default:
        ret_val = error_code::kZWindowErrorCode_FontLanguageNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Font language not exist! _font_language: %d", _font_language);
        return ret_val;
    }

    if (font_ptr == nullptr) {
        ret_val = error_code::kZWindowErrorCode_FontLanguageLoadFailed;
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Font language load failed! _file_dir: %ls", 
            string::String2WString(_file_dir).String()
        );
        return ret_val;
    }

    imgui_io_ptr_->FontDefault = font_ptr;

    //build font
    imgui_io_ptr_->Fonts->Build();

    //build texture
    ImGui_ImplOpenGL3_CreateFontsTexture();

    return ret_val;
}

NODISCARD ReturnType ZWindow::SaveSettings(const Char* _file_dir) noexcept {
    ReturnType ret_val = kOK;

    ZWString file_dir = string::String2WString(_file_dir);
    if (!file_system::PathValid(file_dir.String())) {
        ret_val = error_code::kZWindowErrorCode_INIFileDirNotValid;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Save settings failed, file dir not valid! _file_dir: %ls",
            file_dir.String()
        );
        return ret_val;
    }

    ImGui::SaveIniSettingsToDisk(_file_dir);

    return ret_val;
}

NODISCARD ReturnType ZWindow::LoadSettings(const Char* _file_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZWString file_dir = string::String2WString(_file_dir);
    if (!file_system::PathExist(file_dir.String())) {
        ret_val = error_code::kZWindowErrorCode_INIFileDirNotExist;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Load settings failed, file dir not exist! _file_dir: %ls",
            file_dir.String()
        );
        return ret_val;
    }

    ImGui::LoadIniSettingsFromDisk(_file_dir);

    return ret_val;
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
    imgui_context_ptr_ = _window.imgui_context_ptr_;
    window_state_ = _window.window_state_;
    _window.window_handle_ = nullptr;
    _window.imgui_context_ptr_ = nullptr;
    _window.window_state_ = kWindowState_Terminated;
    internal::ZWindowCallback::SetActiveWindowPtr(this);
}

NODISCARD ReturnType ZWindow::CreateWindowP(
    const Char* _name, GuiSize _size, WindowScreenModeEnum_ _screen_mode
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    static ZMutex create_window_mutex;

    Z_CHECK(
        window_state_ != kWindowState_Terminated,
        error_code::kZWindowErrorCode_StateError,
        L"Window state error! state: %d expect state: %d",
        window_state_, kWindowState_Terminated
    );

    if (imgui_context_ptr_ == nullptr) {
        link_code = CreateContextP();
        if (link_code != kOK) {
            ret_val = error_code::kZWindowErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZWindow::CreateContextP() link error!");
            return ret_val;
        }
    }

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
            nullptr
        );
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() system error!");
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
            nullptr
        );
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() system error!");
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
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetPrimaryMonitor() system error!");
            return ret_val;
        }
        //get the main monitor
        const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
        if (video_mode == nullptr) {
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwGetVideoMode() system error!");
            return ret_val;
        }
        //create window
        window_handle_ = glfwCreateWindow(video_mode->width, video_mode->height, _name, main_monitor, nullptr);
        if (window_handle_ == nullptr) {
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwCreateWindow() system error!");
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
    //bind callback functions
    glfwSetKeyCallback(window_handle_, internal::ZWindowCallback::KeyCallBack);
    glfwSetMouseButtonCallback(window_handle_, internal::ZWindowCallback::MouseButtonCallback);
    glfwSetCursorPosCallback(window_handle_, internal::ZWindowCallback::MousePositionCallback);
    glfwSetScrollCallback(window_handle_, internal::ZWindowCallback::MouseScrollCallback);

    //bind imgui to window
    ImGui_ImplGlfw_InitForOpenGL(window_handle_, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    window_state_ = kWindowState_Initialized;
    internal::ZWindowCallback::SetActiveWindowPtr(this);

    return ret_val;
}

NODISCARD ReturnType ZWindow::CreateContextP() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    {
        TLockGuard lock_guard(OpenGLMutex());
        //init opengl
        if (glfwInit() != GLFW_TRUE) {
            ret_val = error_code::kZWindowErrorCode_SystemError;
            Z_LOG_ERROR(ret_val, 0, L"glfwInit() system error!");
            return ret_val;
        }

        ++active_context_num_;
    }

    //choose opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //imgui
    IMGUI_CHECKVERSION();
    imgui_context_ptr_ = ImGui::CreateContext();
    imgui_io_ptr_ = &ImGui::GetIO();
    imgui_io_ptr_->IniFilename = kImguiINIFileDir;
    //set current context
    ImGui::SetCurrentContext(imgui_context_ptr_);
    return ret_val;
}

Int32 ZWindow::tick_pur_window_tick_ = 0;

Int32 ZWindow::active_context_num_ = 0;

}//gui
}//zengine