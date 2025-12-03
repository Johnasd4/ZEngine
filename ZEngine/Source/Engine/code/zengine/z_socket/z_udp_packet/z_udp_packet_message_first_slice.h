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
    Message first slice data. Contains both the message info and data slice.
    Conbime all slice data to a complete message.
    Structure:
        0 - 0: packet_type_(UDPPacketTypeEnum, UInt8)
        1 - 1: session_id_(UInt8)
        2 - 3: time_ms_stamp_(UInt16)
        4 - 7: packet_id_(UInt32)
        8 - 11: slice_num_(UInt32)
        12 - 15: message_id_(UInt32)
        16 - 23: message_size_(SizeType)
        24 - END: data_(Byte[kDataSize])
*/
struct ZUDPPacketMessageFirstSilce : public ZUDPPacket {
public:
    static constexpr SizeType kHeadSliceDataHeadSize = 16ULL;
    static constexpr SizeType kPacketBodySize = ZUDPPacket::kMaxPacketBodySize;
    static constexpr SizeType kDataSize = kPacketBodySize - kHeadSliceDataHeadSize;

    FORCEINLINE ZUDPPacketMessageFirstSilce() noexcept
        : SuperType_(UDPPacketTypeEnum::kMessageFirstSlice)
    {}
    FORCEINLINE ~ZUDPPacketMessageFirstSilce() noexcept {}

    NODISCARD FORCEINLINE constexpr SizeType PacketBodySize() const noexcept {
        return kPacketBodySize;
    }

protected:
    using SuperType_ = ZUDPPacket;

public:
    UInt32 message_id_;
    UInt32 slice_num_;
    SizeType message_size_;
    TFixedMemory<kDataSize> data_;
};

}//socket
}//zengine