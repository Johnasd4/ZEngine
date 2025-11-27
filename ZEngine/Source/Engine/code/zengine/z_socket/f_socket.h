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
#pragma once

#include "drive.h"

#include "../z_core/z_string.h"

#include "z_udp_endpoint.h"

namespace zengine {
namespace error_code {

enum FSocketErrorCode : ReturnType {
    kFSocketErrorCode_LinkError = kErrorCodeBase_FSocket,
    kFSocketErrorCode_SystemError,
    kFSocketErrorCode_NullptrParam,
    kFSocketErrorCode_ParamOutOfRange,
    kFSocketErrorCode_LogServerStateError,
    kFSocketErrorCode_CommandIDNotExist,
    kFSocketErrorCode_ReplyIDNotExist,
};

}//zengine
}//error_code

namespace zengine {
namespace socket {

/*
    Get current public ip.
*/
SOCKET_DLLAPI ReturnType GetUDPPublicIP4AndPort(
    const ZUDPEndpoint& _local_udp_endpoint,
    UInt32* _public_udp_ip_ptr,
    UInt16* _public_udp_port_ptr
) noexcept;

}//socket
}//zengine