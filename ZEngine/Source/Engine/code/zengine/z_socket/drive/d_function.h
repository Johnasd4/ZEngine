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

#include "d_error_code.h"
#include "d_lib.h"
#include "d_type.h"

#include "../../z_core/z_string.h"

namespace zengine {
namespace socket {

/*
    Change UInt32 ip to string ip.
*/
SOCKET_DLLAPI ZString IP4ToString(UInt32 _ip4) noexcept;

/*
    Change string ip to UInt32 ip.
*/
SOCKET_DLLAPI ReturnType StringToIP4(const Char* _ip_string, UInt32* _ip4_ptr) noexcept;

}//socket
}//zengine