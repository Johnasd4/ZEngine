/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "z_application.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

/*
    Adds the window to the application.
*/
ReturnType ZApplication::AddWindowToCurrentApplication(ZWindow* window_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (instance_ptr_ == nullptr) {
        ret_val = error_code::kZApplicationErrorCodeApplicationNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Application not exist!");
        return ret_val;
    }
    instance_ptr_->window_ptr_vec_.PushBack(window_ptr);
    return ret_val;
}

ZApplication::ZApplication() noexcept 
    : SuperType_()
    , active_window_ptr_(nullptr)
    , window_ptr_vec_()
{
    //init opengl
    if (glfwInit() != GLFW_TRUE) {
        Z_LOG_ERROR(error_code::kZApplicationErrorCodeLinkError, 0, L"glfwInit() link error!");
        return; 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    instance_ptr_ = this;
}

ZApplication::~ZApplication() noexcept {
    glfwTerminate();
    instance_ptr_ = nullptr;
}

NODISCARD ReturnType ZApplication::Execute() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    UInt32 pre_time = clock();

    //begin window
    for (
        auto window_ptr_iter = window_ptr_vec_.Begin();
        window_ptr_iter != window_ptr_vec_.End();
        ++window_ptr_iter
    ) {
        ZWindow* window_ptr = *window_ptr_iter;
        window_ptr->Begin();
    }

    while (ZWindow::ActiveWindowNum() > 0) {
        //update delta time
        UInt32 current_time = clock();
        Float32 delta_time = static_cast<Float32>(current_time - pre_time) * 0.001f;
        pre_time = current_time;

        //update window
        for (
            auto window_ptr_iter = window_ptr_vec_.Begin(); 
            window_ptr_iter != window_ptr_vec_.End(); 
            ++window_ptr_iter
        ) {
            ZWindow* window_ptr = *window_ptr_iter;
            GLFWwindow* window_handle = static_cast<GLFWwindow*>(window_ptr->WindowHandle());
            if (glfwGetWindowAttrib(window_handle, GLFW_FOCUSED)) {
                active_window_ptr_ = window_ptr;
            }

            if (active_window_ptr_ != window_ptr) {
                continue;
            }

            glfwMakeContextCurrent(window_handle);
            ImGui::SetCurrentContext(static_cast<ImGuiContext*>(window_ptr->WindowContext()));

            if (window_ptr->WindowState() != ZWindow::kWindowStateOpened) {
                continue;
            }

            //remove the closed window
            if (glfwWindowShouldClose(window_handle)) {
                window_ptr->Close();
                continue;
            }
            if (!window_ptr->Enabled()) {
                continue;
            }
            //Imgui frame start
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            window_ptr->Tick(delta_time);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            //Imgui frame end
            glfwSwapBuffers(window_handle);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwPollEvents();
        }
    }

    //release imgui resourses
    for (
        auto window_ptr_iter = window_ptr_vec_.Begin();
        window_ptr_iter != window_ptr_vec_.End();
        ++window_ptr_iter
    ) {
        ZWindow* window_ptr = *window_ptr_iter;
        ImGui::SetCurrentContext(static_cast<ImGuiContext*>(window_ptr->WindowContext()));
        //clean up imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

    }

    //release opengl resourses
    for (
        auto window_ptr_iter = window_ptr_vec_.Begin();
        window_ptr_iter != window_ptr_vec_.End();
        ++window_ptr_iter
    ) {
        ZWindow* window_ptr = *window_ptr_iter;
        GLFWwindow* window_handle = static_cast<GLFWwindow*>(window_ptr->WindowHandle());
        glfwMakeContextCurrent(window_handle);
        glfwDestroyWindow(window_handle);
        window_ptr->Destroy();
    }

    return ret_val;
}

Void ZApplication::AddWindow(ZWindow* window_ptr) noexcept {
    window_ptr_vec_.PushBack(window_ptr);
    window_ptr->OnAdd(window_ptr);
    if (active_window_ptr_ == nullptr) {
        active_window_ptr_ = window_ptr;
    }
}

ZApplication* ZApplication::instance_ptr_ = nullptr;

}//gui
}//zengine