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
#define CORE_DLLFILE

#include "z_log_server.h"

#include "m_log.h"

namespace zengine {
namespace log {

ZLogServer::ZLogServer() noexcept : SuperType_() {
    //init ports
    port_array_.FillZero();
}

Void ZLogServer::OutputLog(SizeType _port_id, const ZLog* _log_ptr) noexcept {
    static ZLog::OutputString_ output_str;
    
    if (port_array_[_port_id].input_func_ == nullptr) {
        return;
    }

    //generate output string
    port_array_[_port_id].input_func_(_log_ptr, &output_str);

    //call the output functions
    for (SizeType func_index = 0; func_index < port_array_[_port_id].output_func_array_.Capacity(); ++func_index) {
        if (port_array_[_port_id].output_func_array_[func_index] != nullptr) {
            port_array_[_port_id].output_func_array_[func_index](_log_ptr, output_str);
        }
    }
}

NODISCARD ReturnType ZLogServer::RegisterInputFunction(
    SizeType _port_id,
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    Bool registered = false;

    if (port_array_[_port_id].input_func_ != nullptr) {
        ret_val = error_code::kMLogErrorCode_LogPortInputFunctionAlreadyRegistered;
        Z_LOG_ERROR(ret_val, 0, L"Register failed, port %d input function already registered!", _port_id);

        return ret_val;
    }

    port_array_[_port_id].input_func_ = _input_func;

    return ret_val;
}

NODISCARD ReturnType ZLogServer::UnregisterInputFunction(
    SizeType _port_id,
    Void(*_input_func)(const ZLog*, ZLog::OutputString_*)
) noexcept {
    ReturnType ret_val = kOK;
    if (port_array_[_port_id].input_func_ != _input_func) {
        ret_val = error_code::kMLogErrorCode_LogPortInputFunctionUnregisteredFailed;
        Z_LOG_ERROR(ret_val, 0, L"Unregister failed, port %d function does't match!", _port_id);

        return ret_val;
    }

    port_array_[_port_id].input_func_ = nullptr;
    
    return ret_val;
}

NODISCARD ReturnType ZLogServer::RegisterOutputFunction(
    SizeType _port_id, 
    Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)
) noexcept {
    ReturnType ret_val = kOK;
    Bool registered = false;

    for (SizeType index = 0; index < port_array_[_port_id].output_func_array_.Capacity(); ++index) {
        if (port_array_[_port_id].output_func_array_[index] != nullptr) {
            //Check for same output.
            if (port_array_[_port_id].output_func_array_[index] == _output_func) {
                ret_val = error_code::kMLogErrorCode_LogPortOutputFunctionAlreadyRegistered;
                Z_LOG_ERROR(ret_val, 0, L"Register failed, port %d output function already registered!", _port_id);

                return ret_val;
            }
        }
        else {
            //find empty output.
            if (!registered) {
                port_array_[_port_id].output_func_array_[index] = _output_func;
                registered = true;
            }
        }
    }

    if (!registered) {
        ret_val = error_code::kMLogErrorCode_LogPortOutputFunctionFull;
        Z_LOG_ERROR(ret_val, 0, L"Register failed, port %d output function array full!", _port_id);
        return ret_val;
    }

    return ret_val;
}

Void ZLogServer::UnregisterOutputFunction(Void(*_output_func)(const ZLog*, const ZLog::OutputString_&)) noexcept {
    for (SizeType port_id = 0; port_id < port_array_.Capacity(); ++port_id) {
        for (SizeType func_index = 0; func_index < port_array_[port_id].output_func_array_.Capacity(); ++func_index) {
            if (port_array_[port_id].output_func_array_[func_index] == _output_func) {
                port_array_[port_id].output_func_array_[func_index] = nullptr;
            }
        }
    }
}

}//log
}//zengine