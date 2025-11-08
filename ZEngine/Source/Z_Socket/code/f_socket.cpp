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

#include "f_tcp_log_output.h"

#include "../z_core/log/type/z_error_log.h"
#include "../z_core/log/type/z_trace_log.h"
#include "../z_core/log/type/z_info_log.h"
#include "../z_core/m_log.h"
#include "../z_core/t_atom.h"
#include "../z_core/t_fixed_memory.h"
#include "../z_core/t_queue.h"
#include "../z_core/z_mutex.h"
#include "../z_core/z_object.h"
#include "../z_core/z_thread.h"

#include "z_io_context.h"
#include "z_tcp_client.h"
#include "z_tcp_server.h"

namespace zengine {
namespace socket {

/*
    Get current public ip.
*/
SOCKET_DLLAPI ReturnType GetPublicIP(
    ZString* public_ip_str_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    return ret_val;
}

}//socket
}//zengine