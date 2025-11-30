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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "drive/d_function.h"

namespace zengine {
namespace socket {

SOCKET_DLLAPI ZString IP4ToString(UInt32 _ip4) noexcept {
    return string::GenerateString("%d.%d.%d.%d", 
        (_ip4 & 0xFF000000) >> 24,
        (_ip4 & 0x00FF0000) >> 16,
        (_ip4 & 0x0000FF00) >> 8,
        (_ip4 & 0x000000FF)
    );
}

SOCKET_DLLAPI ReturnType StringToIP4(const Char* _ip_string, UInt32* _ip4_ptr) noexcept {
    ReturnType ret_val = kOK;
    UInt32 result = 0U;
    Int32 segment = 0;
    Int32 value = 0;
    //TODO:bug
    Z_CHECK(
        _ip4_ptr == nullptr,
        error_code::kPSocketErrorCode_NullptrParam,
        L"_ip4_ptr is nullptr!"
    );

    for (; *_ip_string; ++_ip_string) {
        Char c = *_ip_string;

        if (c >= '0' && c <= '9') {
            value = value * 10 + (c - '0');
            if (value > 255) {
                ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
                Z_LOG_ERROR(
                    ret_val, 0, 
                    L"ip out of range! _ip_string: %ls",
                    string::String2WString(_ip_string).String()
                );
                return ret_val;
            }
            continue;
        }

        if (c != '.') {
            ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
            Z_LOG_ERROR(
                ret_val, 0,
                L"Invaild char! _ip_string: %ls",
                string::String2WString(_ip_string).String()
            );
            return ret_val;
        }

        if (++segment > 3) {
            ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
            Z_LOG_ERROR(
                ret_val, 0,
                L"More then 3 segment! _ip_string: %ls",
                string::String2WString(_ip_string).String()
            );
            return ret_val;
        }
        result = (result << 8) | (UInt32)value;
        value = 0;
    }

    if (segment != 3) {
        ret_val = error_code::kPSocketErrorCode_AddressNotVaild;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Not 3 segment! _ip_string: %ls",
            string::String2WString(_ip_string).String()
        );
        return ret_val;
    }

    result = (result << 8) | (UInt32)value;
    
    *_ip4_ptr = result;
    
    return ret_val;
}

}//socket
}//zengine
