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

#include "z_udp_packet_factory.h"

namespace zengine {
namespace socket {

ZUDPPacketFactory::ZUDPPacketFactory() noexcept
    : SuperType_()
{
    message_packet_pool_.ExtendPool(kMessagePacketPoolDefaultSize);
    command_packet_pool_.ExtendPool(kCommandPacketPoolDefaultSize);
}

ZUDPPacketFactory::~ZUDPPacketFactory() noexcept {}

}//socket
}//zengine