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
#ifndef Z_GUI_OPENGL_Z_OPENGL_MANAGER_H_
#define Z_GUI_OPENGL_Z_OPENGL_MANAGER_H_

#include "internal/z_drive.h"

#include "../z_core/z_mutex.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace gui {

/*
    Controls the OpenGL resourse initialize and terminate.
*/
class ZOpenGLManager : public ZObject {
public:
    NODISCARD static ReturnType InitializeOpenGL() noexcept;
    static Void TerminateOpenGL() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    NODISCARD static ZOpenGLManager& InstanceP() noexcept;

    ZOpenGLManager() noexcept;

    ZOpenGLManager(const ZOpenGLManager&) = delete;
    ZOpenGLManager(ZOpenGLManager&&) = delete;
    ZOpenGLManager& operator=(const ZOpenGLManager&) = delete;
    ZOpenGLManager& operator=(ZOpenGLManager&&) = delete;

    ZMutex opengl_mutex_;
};

}//gui
}//zengine

#endif // !Z_GUI_OPENGL_Z_OPENGL_MANAGER_H_