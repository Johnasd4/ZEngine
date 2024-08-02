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

#include "z_opengl_manager.h"

#include "../z_core/t_lock_guard.h"

#include "z_window.h"

namespace zengine {
namespace gui {

NODISCARD ReturnType ZOpenGLManager::InitializeOpenGL() noexcept {
    ReturnType ret_val = kOK;
    TLockGuard<ZMutex> lock_guard(ZOpenGLManager::InstanceP().opengl_mutex_);
    //init opengl
    if (glfwInit() != GLFW_TRUE) {
        ret_val = error_code::kZWindowErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, 0, L"glfwInit() link error!");
        return ret_val;
    }
    return ret_val;
}

Void ZOpenGLManager::TerminateOpenGL() noexcept {
    TLockGuard<ZMutex> lock_guard(ZOpenGLManager::InstanceP().opengl_mutex_);
    glfwTerminate();
}

NODISCARD ZOpenGLManager& ZOpenGLManager::InstanceP() noexcept {
    static ZOpenGLManager opengl_manager;
    return opengl_manager;
}

ZOpenGLManager::ZOpenGLManager() noexcept : SuperType_(), opengl_mutex_() {}

}//gui
}//zengine