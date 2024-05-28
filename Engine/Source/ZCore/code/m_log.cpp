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
#define CORE_DLLFILE

#include "m_log.h"

#include "f_console.h"
#include "z_thread.h"

namespace zengine {
namespace internal {

struct LogErrorInfo {

};

/*
    The log manager, log's the error and info to the console, file and any place that needs to log.
*/
class ZLogManager : public ZObject {
private:
    static constexpr Int32 kTimeStringSize_YMDHMS = sizeof("YYYY/MM/DD-HH:MM:SS") - 1;

public:
    static ZLogManager& Instance() noexcept {
        static ZLogManager log_manager;
        return log_manager;
    }

    ~ZLogManager() noexcept {
        log_thread_finished_ = true;
        if (log_thread_.Joinable()) {
            log_thread_.Join();
        }
    }

protected:
    using SuperType = ZObject;

private:
    static Void LogThread() noexcept {
        static ZLogManager& log_manager = ZLogManager::Instance();
        while (log_manager.log_thread_finished_ == false) {

        }
    }

    ZLogManager() noexcept {
        log_thread_finished_ = false;
        log_thread_ = ZThread(&ZLogManager::LogThread);
    }

    

    Bool log_thread_finished_;
    ZThread log_thread_;
};

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogError(time_t raw_time,
                                 ReturnType err_val,
                                 ReturnType link_val,
                                 const CChar* error_file, 
                                 const CChar* error_funcion,
                                 Int32 error_line, 
                                 const CChar* format,
                                 ...) noexcept {



}

}//internal
}//zengine