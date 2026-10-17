/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE
#include "drive/d_pch.h"

#include "z_application.h"

#include "z_window.h"

namespace zengine {
namespace gui {
namespace internal {

class ZApplicationCallback {
public:
    static Void KeyCallBack(
        GLFWwindow* _window_handle, 
        Int32 _key, 
        Int32 scancode, 
        Int32 _action, 
        Int32 _mods
    ) noexcept {
        ZApplication* app_ptr = static_cast<ZApplication*>(glfwGetWindowUserPointer(_window_handle));
        if (_action == GLFW_PRESS && app_ptr->key_down_callback_func_) {
            app_ptr->key_down_callback_func_(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0
            );
        }
        else if (_action == GLFW_RELEASE && app_ptr->key_up_callback_func_) {
            app_ptr->key_up_callback_func_(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0
            );
        }
        else if (_action == GLFW_REPEAT && app_ptr->key_press_callback_func_) {
            app_ptr->key_press_callback_func_(
                static_cast<KeyEnum>(_key),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0
            );
        }
    }

    static Void MouseButtonCallback(
        GLFWwindow* _window_handle, 
        Int32 _button, 
        Int32 _action, 
        Int32 _mods
    ) noexcept {
        ZApplication* app_ptr = static_cast<ZApplication*>(glfwGetWindowUserPointer(_window_handle));
        if (_action == GLFW_PRESS && app_ptr->mouse_down_callback_func_) {
            app_ptr->mouse_down_callback_func_(
                static_cast<MouseButtonEnum>(_button),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0
            );
        }
        else if (_action == GLFW_RELEASE && app_ptr->mouse_up_callback_func_) {
            app_ptr->mouse_up_callback_func_(
                static_cast<MouseButtonEnum>(_button),
                (_mods & GLFW_MOD_SHIFT) != 0,
                (_mods & GLFW_MOD_CONTROL) != 0,
                (_mods & GLFW_MOD_ALT) != 0
            );
        }
    }

    static Void MouseScrollCallback(
        GLFWwindow* _window_handle, 
        Float64 _x_offset, 
        Float64 _y_offset
    ) {
        ZApplication* app_ptr = static_cast<ZApplication*>(glfwGetWindowUserPointer(_window_handle));
        if (app_ptr->scroll_move_callback_func_) {
            app_ptr->scroll_move_callback_func_(
                static_cast<Float32>(_x_offset),
                static_cast<Float32>(_y_offset)
            );
        }
    }

    static Void MouseMoveCallback(
        GLFWwindow* _window_handle, 
        Float64 _pos_x, 
        Float64 _pos_y
    ) noexcept {
        ZApplication* app_ptr = static_cast<ZApplication*>(glfwGetWindowUserPointer(_window_handle));
        if (app_ptr->mouse_move_callback_func_) {
            thread_local GuiPos pre_pos = { static_cast<Float32>(_pos_x), static_cast<Float32>(_pos_y) };
            GuiPos cur_pos = { static_cast<Float32>(_pos_x), static_cast<Float32>(_pos_y) };
            app_ptr->mouse_move_callback_func_(pre_pos, cur_pos);
            pre_pos = cur_pos;
        }
    }

private:
    ZApplicationCallback() = delete;
    ZApplicationCallback(const ZApplicationCallback&) = delete;
    ZApplicationCallback(ZApplicationCallback&&) = delete;
    ZApplicationCallback& operator=(const ZApplicationCallback&) = delete;
    ZApplicationCallback& operator=(ZApplicationCallback&&) = delete;
};

static Void* ImguiAlloc(SizeType _size, Void*) {
    return memory_pool::ApplyThreadLocalMemory(_size);
}

static Void ImguiFree(Void* _memory_ptr, Void*) {
    memory_pool::ReleaseThreadLocalMemory(_memory_ptr);
}

static Void* RLFWAlloc(SizeType _size, Void*) {
    return memory_pool::ApplyThreadLocalMemory(_size);
}

static Void* RLFWRealloc(Void* _memory_ptr, SizeType _size, Void*) {
    return memory_pool::ReapplyThreadLocalMemory(_memory_ptr, _size);
}

static Void RLFWFree(Void* _memory_ptr, Void*) {
    memory_pool::ReleaseThreadLocalMemory(_memory_ptr);
}


} // internal
} // gui
} // zengine


namespace zengine {
namespace gui {

ZApplication::ZApplication(const Char* _name, GuiSize _size, GuiPos _pos, ScreenModeEnum_ _screen_mode) noexcept
    : SuperType_()
    , window_ptr_array_()
    , task_queue_()
    , application_handle_(nullptr) 
    , imgui_context_ptr_(nullptr)
    , imgui_io_ptr_(nullptr)
    , state_(StateEnum_::kTerminated)
{
    ASSERT(ZThread::GetCurrentThreadID() == ZThread::GetMainThreadID(), "ZApplication: Not on main thread!");

    ReturnType link_code = kOK;
    link_code = CreateP(_name, _size, _pos, _screen_mode);
    if (link_code != kOK) {
        DestroyP();
        Z_LOG_ERROR(error_code::kZApplicationErrorCode_LinkError, link_code, "ZApplication::CreateP() link error!");
        return;
    }
}

ZApplication::~ZApplication() noexcept {
    if (state_ != StateEnum_::kTerminated) {
        DestroyP();
    }
}

NODISCARD Bool ZApplication::IsFullScreen() const noexcept {
    return glfwGetWindowMonitor(application_handle_) != nullptr;
}

NODISCARD GuiSize ZApplication::GetSize() const noexcept {
    Int32 width = 0, height = 0;
    glfwGetWindowSize(application_handle_, &width, &height);
    return GuiSize(static_cast<Float32>(width), static_cast<Float32>(height));
}

NODISCARD GuiPos ZApplication::GetPos() const noexcept {
    Int32 x = 0, y = 0;
    glfwGetWindowPos(application_handle_, &x, &y);
    return GuiPos(static_cast<Float32>(x), static_cast<Float32>(y));
}

NODISCARD GuiPos ZApplication::GetAbsPos() const noexcept {
    return GuiPos(0.0F, 0.0F);
}

NODISCARD const Char* ZApplication::GetName() const noexcept {
    const Char* title_str = glfwGetWindowTitle(application_handle_);
    return title_str == nullptr ? "" : title_str;
}

NODISCARD GuiColour ZApplication::GetBackgroundColour() const noexcept {
    GLfloat colour[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, colour);
    return GuiColour(colour[0], colour[1], colour[2], colour[3]);
}

Void ZApplication::SetVisible(Bool _visible) noexcept {
    if (_visible == true) {
        glfwShowWindow(application_handle_);
    }
    else if (_visible == false){
        glfwHideWindow(application_handle_);
    }
}

Void ZApplication::SetSize(GuiSize _size) noexcept {
    glfwSetWindowSize(
        application_handle_, 
        static_cast<Int32>(_size.width_), 
        static_cast<Int32>(_size.height_)
    );
}

Void ZApplication::SetPos(GuiPos _pos) noexcept {
    glfwSetWindowPos(
        application_handle_,
        static_cast<Int32>(_pos.x_),
        static_cast<Int32>(_pos.y_)
    );
}

Void ZApplication::SetAbsPos(GuiPos _pos) noexcept {
    SetPos(_pos);
}

Void ZApplication::SetName(const Char* _name) noexcept {
    glfwSetWindowTitle(application_handle_, _name);
}

Void ZApplication::SetBackgroundColour(GuiColour _colour) noexcept {
    glClearColor(_colour.red_, _colour.green_, _colour.blue_, _colour.alpha_);
}

Void ZApplication::SetScreenMode(ScreenModeEnum_ _screen_mode) noexcept {
    Int32 width = 0, height = 0, pos_x = 0, pos_y = 0;
    glfwGetWindowSize(application_handle_, &width, &height);
    glfwGetWindowPos(application_handle_, &pos_x, &pos_y);

    //get the main monitor
    GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
    if (main_monitor == nullptr) {
        Z_LOG_ERROR(error_code::kZApplicationErrorCode_SystemOrLibraryError, 0, "glfwGetPrimaryMonitor() system error!");
        return;
    }
    //get the main monitor
    const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
    if (video_mode == nullptr) {
        Z_LOG_ERROR(error_code::kZApplicationErrorCode_SystemOrLibraryError, 0, "glfwGetVideoMode() system error!");
        return;
    }

    switch (_screen_mode) {
    case ScreenModeEnum_::kWindow:
        glfwSetWindowMonitor(
            application_handle_, nullptr, pos_x, pos_y, width, height, 0);
        break;
    case ScreenModeEnum_::kFullScreenCustomSize:
        glfwSetWindowMonitor(
            application_handle_, main_monitor, 0, 0, width, height, 0);
        break;
    case ScreenModeEnum_::kFullScreenDefaultSize:
        glfwSetWindowMonitor(
            application_handle_, main_monitor, 0, 0, video_mode->width, video_mode->height, 0);
        break;
    default:
        break;
    }
}

Void ZApplication::SetPosToScreenCenter() noexcept {
    if(!IsFullScreen()) {
        SetPos(ScreenCenterPos(GetSize()));
    }
}

Void ZApplication::SetVerticalSynchronization(Int32 _tick_per_app_tick) noexcept {
    glfwSwapInterval(_tick_per_app_tick);
}

NODISCARD ReturnType ZApplication::Execute() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ == StateEnum_::kInitialized,
        error_code::kZApplicationErrorCode_StateError,
        "Window state error! state: {} expect state: {}",
        static_cast<Int32>(state_), static_cast<Int32>(StateEnum_::kInitialized)
    );

    state_ = StateEnum_::kExecuted;

    TimeType pre_time_ms = ZSystemTime::GetTimeMs();

    // render loop
    while (!glfwWindowShouldClose(application_handle_)) {     

        // calculate delta time
        TimeType current_time_ms = ZSystemTime::GetTimeMs();
        Float32 delta_time = static_cast<Float32>(current_time_ms - pre_time_ms) * 0.001f;
        pre_time_ms = current_time_ms;

        // clear buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Imgui frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // call tasks
        ZTask task;
        while (task_queue_.TryPopFront(task)) {
            link_code = task.Run();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZApplicationErrorCode_LinkError,
                    link_code, 
                    "ZTask::Run() link error!"
                );
            }
        }

        // input events
        glfwPollEvents();

        // ticks window
        OnTick(delta_time);

        // render frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Imgui frame end
        glfwSwapBuffers(application_handle_);
    }

    // destroys window
    DestroyP();

    return ret_val;
}

Void ZApplication::AddWindow(ZWindow* _window_ptr) noexcept {
    ASSERT(
        _window_ptr != nullptr,
        "ZWindow::AddWindow: _window_ptr is nullptr!"
    );

    window_ptr_array_.PushBack(_window_ptr);
}

Void ZApplication::RemoveWindow(ZWindow* _window_ptr) noexcept {
    window_ptr_array_.EraseFirst(_window_ptr);
}

Void ZApplication::RemoveAllWindow() noexcept {
    window_ptr_array_.Clear();
}

Void ZApplication::AddTask(ZTask&& _task) noexcept {
    task_queue_.PushBack(std::forward<ZTask>(_task));
}

NODISCARD ReturnType ZApplication::LoadFontFromFileTTF(
    const Char* _file_path,
    Float32 _font_size,
    FontLanguageEnum_ _font_language
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (imgui_context_ptr_ == nullptr) {
        link_code = CreateContextP();
        if (link_code != kOK) {
            ret_val = error_code::kZApplicationErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZApplication::CreateContextP() link error!");
            return ret_val;
        }
    }

    ImFont* font_ptr = nullptr;

    switch (_font_language) {
    case FontLanguageEnum_::kSimplifiedChineseCommon:
        font_ptr = imgui_io_ptr_->Fonts->AddFontFromFileTTF(
            _file_path,
            _font_size,
            nullptr,
            imgui_io_ptr_->Fonts->GetGlyphRangesChineseSimplifiedCommon()
        );
        break;
    case FontLanguageEnum_::kSimplifiedChineseAll:
        font_ptr = imgui_io_ptr_->Fonts->AddFontFromFileTTF(
            _file_path,
            _font_size,
            nullptr,
            imgui_io_ptr_->Fonts->GetGlyphRangesChineseFull()
        );
        break;
    default:
        ret_val = error_code::kZApplicationErrorCode_FontLanguageNotExist;
        Z_LOG_ERROR(ret_val, 0, "Font language not exist! _font_language: {}", static_cast<Int32>(_font_language));
        return ret_val;
    }

    if (font_ptr == nullptr) {
        ret_val = error_code::kZApplicationErrorCode_FontLanguageLoadFailed;
        Z_LOG_ERROR(
            ret_val, 0,
            "Font language load failed! _file_path: {}",
            _file_path
        );
        return ret_val;
    }

    imgui_io_ptr_->FontDefault = font_ptr;

    // build font
    imgui_io_ptr_->Fonts->Build();

    // build texture
    ImGui_ImplOpenGL3_CreateFontsTexture();

    return ret_val;
}

NODISCARD ReturnType ZApplication::SaveSettings(const Char* _file_dir) noexcept {
    ReturnType ret_val = kOK;

    ImGui::SaveIniSettingsToDisk(_file_dir);
    if (!file_system::IsPathExist(_file_dir)) {
        ret_val = error_code::kZApplicationErrorCode_INIFileDirNotValid;
        Z_LOG_ERROR(
            ret_val, 0,
            "Save settings failed, file dir not valid! _file_dir: {}",
            _file_dir
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZApplication::LoadSettings(const Char* _file_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!file_system::IsPathExist(_file_dir)) {
        ret_val = error_code::kZApplicationErrorCode_INIFileDirNotExist;
        Z_LOG_ERROR(
            ret_val, 0,
            "Load settings failed, file dir not exist! _file_dir: {}",
            _file_dir
        );
        return ret_val;
    }

    ImGui::LoadIniSettingsFromDisk(_file_dir);

    return ret_val;
}


Void ZApplication::OnTick(Float32 _delta_sec) noexcept {
    // tick windows
    for (auto& iter : window_ptr_array_) {
        iter->OnTick(_delta_sec);
    }
}

Void ZApplication::OnDestroy() noexcept {}

NODISCARD ReturnType ZApplication::CreateP(
    const Char* _name,
    GuiSize _size,
    GuiPos _pos,
    ScreenModeEnum_ _screen_mode
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    static ZMutex create_application_mutex;

    Z_CHECK(
        state_ == StateEnum_::kTerminated,
        error_code::kZApplicationErrorCode_StateError,
        "Window state error! state: {} expect state: {}",
        static_cast<Int32>(state_), static_cast<Int32>(StateEnum_::kTerminated)
    );

    if (imgui_context_ptr_ == nullptr) {
        link_code = CreateContextP();
        if (link_code != kOK) {
            ret_val = error_code::kZApplicationErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZApplication::CreateContextP() link error!");
            return ret_val;
        }
    }

    // create a application
    switch (_screen_mode) {
    case ScreenModeEnum_::kWindow:
    {
        // create application
        application_handle_ = glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_),
            _name,
            nullptr,
            nullptr
        );
        if (application_handle_ == nullptr) {
            ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "glfwCreateWindow() system error!");
            return ret_val;
        }
        break;
    }
    case ScreenModeEnum_::kFullScreenCustomSize:
    {
        // create application
        application_handle_ = glfwCreateWindow(
            static_cast<Int32>(_size.width_),
            static_cast<Int32>(_size.height_),
            _name,
            glfwGetPrimaryMonitor(),
            nullptr
        );
        if (application_handle_ == nullptr) {
            ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "glfwCreateWindow() system error!");
            return ret_val;
        }
        break;
    }
    case ScreenModeEnum_::kFullScreenDefaultSize:
    {
        // get the main monitor
        GLFWmonitor* main_monitor = glfwGetPrimaryMonitor();
        if (main_monitor == nullptr) {
            ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "glfwGetPrimaryMonitor() system error!");
            return ret_val;
        }
        // get the main monitor
        const GLFWvidmode* video_mode = glfwGetVideoMode(main_monitor);
        if (video_mode == nullptr) {
            ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "glfwGetVideoMode() system error!");
            return ret_val;
        }
        // create application
        application_handle_ = glfwCreateWindow(video_mode->width, video_mode->height, _name, main_monitor, nullptr);
        if (application_handle_ == nullptr) {
            ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(ret_val, 0, "glfwCreateWindow() system error!");
            return ret_val;
        }
        break;
    }
    default:
        break;
    }

    // opengl
    glfwMakeContextCurrent(application_handle_);

    // set application position
    glfwSetWindowUserPointer(application_handle_, this);
    glfwSetKeyCallback(application_handle_, internal::ZApplicationCallback::KeyCallBack);
    glfwSetMouseButtonCallback(application_handle_, internal::ZApplicationCallback::MouseButtonCallback);
    glfwSetScrollCallback(application_handle_, internal::ZApplicationCallback::MouseScrollCallback);
    glfwSetCursorPosCallback(application_handle_, internal::ZApplicationCallback::MouseMoveCallback);

    // bind imgui to application
    ImGui_ImplGlfw_InitForOpenGL(application_handle_, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    state_ = StateEnum_::kInitialized;

    return ret_val;
}

Void ZApplication::DestroyP() noexcept {
    OnDestroy();

    if (imgui_context_ptr_ != nullptr) {
        //release imgui resourses
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    // release opengl resourses
    if (application_handle_ != nullptr) {
        glfwDestroyWindow(application_handle_);
    }

    application_handle_ = nullptr;
    imgui_context_ptr_ = nullptr;
    imgui_io_ptr_ = nullptr;
    window_ptr_array_.Clear();
    state_ = StateEnum_::kTerminated;

    glfwTerminate();
}

NODISCARD ReturnType ZApplication::CreateContextP() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // init glfw allocator
    GLFWallocator glfw_allocator;
    glfw_allocator.allocate = internal::RLFWAlloc;
    glfw_allocator.reallocate = internal::RLFWRealloc;
    glfw_allocator.deallocate = internal::RLFWFree;
    glfw_allocator.user = nullptr;
    glfwInitAllocator(&glfw_allocator);

    // init opengl
    if (glfwInit() != GLFW_TRUE) {
        ret_val = error_code::kZApplicationErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(ret_val, 0, "glfwInit() system error!");
        return ret_val;
    }

    // choose opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::SetAllocatorFunctions(internal::ImguiAlloc, internal::ImguiFree);
    imgui_context_ptr_ = ImGui::CreateContext();
    imgui_io_ptr_ = &ImGui::GetIO();
    imgui_io_ptr_->IniFilename = kDefaultImguiINIFilePath.GetDataPtr();
    // set current context
    ImGui::SetCurrentContext(imgui_context_ptr_);

    return ret_val;
}

} // gui
} // zengine