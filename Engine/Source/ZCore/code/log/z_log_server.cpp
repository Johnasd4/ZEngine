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

#include "z_log_server.h"

#include "m_log.h"

namespace zengine {
namespace log {

ZLogServer::ZLogServer() noexcept {
    port_array_.FillZero();
}

Void ZLogServer::OutputLog(IndexType port_id, const ZLog* log_ptr) noexcept {
    static ZLog::OutputString output_str;
    
    if (port_array_[port_id].input_func == nullptr) {
        return;
    }

    //generate output string
    port_array_[port_id].input_func(log_ptr, &output_str);

    //call the output functions
    for (IndexType func_index = 0; func_index < port_array_[port_id].output_func_array.Capacity(); ++func_index) {
        if (port_array_[port_id].output_func_array[func_index] != nullptr) {
            port_array_[port_id].output_func_array[func_index](log_ptr, output_str);
        }
    }
}

NODISCARD ReturnType ZLogServer::RegisterInputFunction(IndexType port_id,
                                                       Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    Bool registered = false;

    if (port_array_[port_id].input_func != nullptr) {
        ret_val = error_code::kMLogErrorCodeLogPortInputFunctionAlreadyRegistered;
        Z_LOG_ERROR(ret_val, 0, "Register failed, port %d input function already registered!", port_id);

        return ret_val;
    }

    port_array_[port_id].input_func = input_func;

    return ret_val;
}

NODISCARD ReturnType ZLogServer::UnregisterInputFunction(IndexType port_id,
                                         Void(*input_func)(const ZLog*, ZLog::OutputString*)) noexcept {
    ReturnType ret_val = kOK;
    if (port_array_[port_id].input_func != input_func) {
        ret_val = error_code::kMLogErrorCodeLogPortInputFunctionUnregisteredFailed;
        Z_LOG_ERROR(ret_val, 0, "Unregister failed, port %d function does't match!", port_id);

        return ret_val;
    }

    port_array_[port_id].input_func = nullptr;
    
    return ret_val;
}

NODISCARD ReturnType ZLogServer::RegisterOutputFunction(
        IndexType port_id, Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept {
    ReturnType ret_val = kOK;
    Bool registered = false;

    for (IndexType index = 0; index < port_array_[port_id].output_func_array.Capacity(); ++index) {
        if (port_array_[port_id].output_func_array[index] != nullptr) {
            //Check for same output.
            if (port_array_[port_id].output_func_array[index] == output_func) {
                ret_val = error_code::kMLogErrorCodeLogPortOutputFunctionAlreadyRegistered;
                Z_LOG_ERROR(ret_val, 0, "Register failed, port %d output function already registered!", port_id);

                return ret_val;
            }
        }
        else {
            //find empty output.
            if (!registered) {
                port_array_[port_id].output_func_array[index] = output_func;
                registered = true;
            }
        }
    }

    if (!registered) {
        ret_val = error_code::kMLogErrorCodeLogPortOutputFunctionFull;
        Z_LOG_ERROR(ret_val, 0, "Register failed, port %d output function array full!", port_id);
        return ret_val;
    }

    return ret_val;
}

Void ZLogServer::UnregisterOutputFunction(Void(*output_func)(const ZLog*, const ZLog::OutputString&)) noexcept {
    for (IndexType port_id = 0; port_id < port_array_.Capacity(); ++port_id) {
        for (IndexType func_index = 0; func_index < port_array_[port_id].output_func_array.Capacity(); ++func_index) {
            if (port_array_[port_id].output_func_array[func_index] == output_func) {
                port_array_[port_id].output_func_array[func_index] = nullptr;
            }
        }
    }
}

}//log
}//zengine