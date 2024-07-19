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
#ifndef Z_GUI_WINDOW_Z_WINDOW_MANAGER_H_
#define Z_GUI_WINDOW_Z_WINDOW_MANAGER_H_

#include "internal/z_drive.h"

#include "../z_core/t_list.h"
#include "../z_core/z_object.h"
#include "../z_core/z_thread.h"

#include "z_window.h"

namespace zengine {
namespace gui {



/*
    The window manager, controls the window's execution.
*/
class ZWindowManager : public ZObject {
public:
    static Void LogError(
        TimeType _raw_time,
        const WChar* _proj_name,
        const Char* _file_dir,
        const Char* _func_name,
        Int32 _err_line,
        ReturnType _err_code,
        ReturnType _link_code,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    static Void LogTrace(
        TimeType _raw_time,
        const WChar* _proj_name,
        const Char* _file_dir,
        const Char* _func_name,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

    static Void LogInfo(
        TimeType _raw_time,
        LogInfoEnum _info_type,
        const WChar* _format,
        ArgListType _args
    ) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    static ZWindowManager& InstanceP() noexcept;

    static Void TickThread() noexcept;

    ZWindowManager(const ZWindowManager&) = delete;
    ZWindowManager(ZWindowManager&&) = delete;
    ZWindowManager& operator=(const ZWindowManager&) = delete;
    ZWindowManager& operator=(ZWindowManager&&) = delete;

    ZWindowManager() noexcept;

    ~ZWindowManager() noexcept; 

    TList<ZWindow*> window_ptr_list_;
    Bool tick_thread_finished_;
    ZThread tick_thread_;
};

}//gui
}//zengine

#endif // !Z_GUI_WINDOW_Z_WINDOW_MANAGER_H_
