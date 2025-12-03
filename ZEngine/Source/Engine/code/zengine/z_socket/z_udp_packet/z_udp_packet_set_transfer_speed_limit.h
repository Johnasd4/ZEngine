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

#include "../drive.h"

#include "z_udp_packet.h"

namespace zengine {
namespace socket {

/*
    Set data transfer speed limit UDP packet.
    Structure:
        0 - 0: packet_type_(UDPPacketTypeEnum, UInt8)
        1 - 1: session_id_(UInt8)
        2 - 3: time_ms_stamp_(UInt16)
        4 - 7: packet_id_(UInt32)
        8 - 15: data_pur_sec_(SizeType)
*/
struct ZUDPPacketSetTransferSpeedLimit : public ZUDPPacket {
public:
    static constexpr SizeType kPacketBodySize = 8ULL;

    FORCEINLINE ZUDPPacketSetTransferSpeedLimit() noexcept
        : SuperType_(UDPPacketTypeEnum::kSetTransferSpeedLimit)
    {}
    FORCEINLINE ~ZUDPPacketSetTransferSpeedLimit() noexcept {}

    NODISCARD FORCEINLINE constexpr SizeType PacketBodySize() const noexcept {
        return kPacketBodySize;
    }

protected:
    using SuperType_ = ZUDPPacket;

public:
    SizeType data_pur_sec_;
};

}//socket
}//zengine