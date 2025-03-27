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
#ifndef Z_CORE_LOG_Z_LOG_SERVER_H_
#define Z_CORE_LOG_Z_LOG_SERVER_H_

#include "internal/z_drive.h"

#include "m_log.h"
#include "t_array.h"

namespace zengine {
namespace log {

/*
    Log server, controls what output functions will be called by different logs. 
*/
class ZLogServer : public ZObject {
public:
    static constexpr Int32 kMaxPortNum = 8;
    static constexpr Int32 kMaxOutputPurPort = 8;

    /*
        Single port.
    */
    struct ZLogPort {
        Void(*input_func_)(const ZLog*, ZLog::OutputString_*) = nullptr;
        TArray<Void(*)(const ZLog*, const ZLog::OutputString_&), kMaxOutputPurPort> output_func_array_;
    };

    ZLogServer() noexcept;

    /*
        Calls the output functions.
    */
    Void OutputLog(IndexType _port_id, const ZLog* _log_ptr) noexcept;

    /*
        Register the input function, the function will be called when log happens.
    */
    NODISCARD ReturnType RegisterInputFunction(
        IndexType _port_id,
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Removes the output function.
    */
    NODISCARD ReturnType UnregisterInputFunction(
        IndexType _port_id,
        Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
    ) noexcept;

    /*
        Register the output function, the function will be called when log happens.
    */
    NODISCARD ReturnType RegisterOutputFunction(
        IndexType _port_id,
        Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
    ) noexcept;

    /*
        Removes the output function.
    */
    Void UnregisterOutputFunction(Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)) noexcept;


protected:
    using SuperType_ = ZObject;

private:
    ZLogServer(const ZLogServer&) = delete;
    ZLogServer(ZLogServer&&) = delete;
    ZLogServer& operator=(const ZLogServer&) = delete;
    ZLogServer& operator=(ZLogServer&&) = delete;

    TArray<ZLogPort, kMaxPortNum> port_array_;
};

}//log
}//zengine

#endif // !Z_CORE_LOG_Z_LOG_PORT_H_
