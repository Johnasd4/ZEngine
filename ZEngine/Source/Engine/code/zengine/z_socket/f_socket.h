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

#include "internal/z_drive.h"

#include "z_core/z_string.h"

/*
    This module allows logs over tcp. 
    After a client is connected to the server, when a log is being output, 
    the module will copy the output and store it in a queue. In a new thread, 
    logs will be popped out and send to the client.
    When the client disconnects over a certain time, the log queue will be emptied.
*/

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
SOCKET_DLLAPI ReturnType GetPublicIP(
    ZString* public_ip_str_ptr
) noexcept;

}//socket
}//zengine