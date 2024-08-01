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
ReturnType ZWindowManager::AddWindowToCurrentApplication(ZWindow* window_ptr) noexcept {
    ReturnType ret_val = kOK;
    if (instance_ptr_ == nullptr) {
        ret_val = error_code::kZApplicationErrorCodeApplicationNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Application not exist!");
        return ret_val;
    }
    instance_ptr_->window_ptr_vec_.PushBack(window_ptr);
    return ret_val;
}

ZWindowManager::ZWindowManager() noexcept 
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

ZWindowManager::~ZWindowManager() noexcept {
    glfwTerminate();
    instance_ptr_ = nullptr;
}

NODISCARD ReturnType ZWindowManager::Execute() noexcept {

}

Void ZWindowManager::AddWindow(ZWindow* window_ptr) noexcept {
    window_ptr_vec_.PushBack(window_ptr);
    window_ptr->OnAdd(window_ptr);
    if (active_window_ptr_ == nullptr) {
        active_window_ptr_ = window_ptr;
    }
}

ZWindowManager* ZWindowManager::instance_ptr_ = nullptr;

}//gui
}//zengine