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

#include "../../z_core/t_fixed_memory.h"

#include "z_udp_packet.h"

namespace zengine {
namespace socket {

/*
    Message last slice data. Contains the size of the last data slice.
    Conbime all slice data to a complete message.
    Structure:
        0 - 0: packet_type_(UDPPacketTypeEnum, UInt8)
        1 - 1: session_id_(UInt8)
        2 - 3: time_ms_stamp_(UInt16)
        4 - 7: packet_id_(UInt32)
        8 - 11: message_id_(UInt32)
        12 - 15: data_size_(UInt32)
        16 - END: data_(Byte[kDataSize])
*/
struct ZUDPPacketMessageLastSilce : public ZUDPPacket {
public:
    static constexpr SizeType kLastSliceDataHeadSize = 8ULL;
    static constexpr SizeType kPacketBodySize = ZUDPPacket::kMaxPacketBodySize;
    static constexpr SizeType kDataMaxSize = kPacketBodySize - kLastSliceDataHeadSize;

    FORCEINLINE ZUDPPacketMessageLastSilce() noexcept
        : SuperType_(UDPPacketTypeEnum::kMessageLastSlice)
    {}
    FORCEINLINE ~ZUDPPacketMessageLastSilce() noexcept {}

    NODISCARD FORCEINLINE SizeType PacketBodySize() const noexcept {
        return data_size_;
    }

protected:
    using SuperType_ = ZUDPPacket;

public:
    UInt32 message_id_;
    UInt32 data_size_;
    TFixedMemory<kDataMaxSize> data_;
};

}//socket
}//zengine