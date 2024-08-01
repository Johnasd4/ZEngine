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
#ifndef Z_GUI_APPLICATION_H_
#define Z_GUI_APPLICATION_H_

#include "internal/z_drive.h"

#include "../z_core/z_mutex.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZApplicationErrorCode : ReturnType {
    kZApplicationErrorCodeLinkError = kErrorCodeBaseZApplication,
    kZApplicationErrorCodeWindowAreadyCreated,
    kZApplicationErrorCodeApplicationNotExist
};

}//error_code

/*
    One main function can only run one application at a time.
    Before creating any gui objects, make an instance of this class to initialize opengl.
*/
class GUI_DLLAPI ZWindowManager : public ZObject {
public:
    ZWindowManager() noexcept;

    ~ZWindowManager() noexcept;

    /*
        Adds the window to the application.
    */
    static ReturnType AddWindowToCurrentApplication(ZWindow* window_ptr) noexcept;

    /*
        Starts the application, returns when the application ends.
    */
    NODISCARD ReturnType Execute() noexcept;

    /*
        Adds the window to the application.
    */
    Void AddWindow(ZWindow* window_ptr) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZWindowManager(const ZWindowManager&) = delete;
    ZWindowManager(ZWindowManager&&) = delete;
    ZWindowManager& operator=(const ZWindowManager&) = delete;
    ZWindowManager& operator=(ZWindowManager&&) = delete;

    static ZWindowManager* instance_ptr_;

    ZWindow* active_window_ptr_;
    ZMutex;
};

}//gui
}//zengine

#endif // !Z_GUI_APPLICATION_H_
