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

#include "../../z_core/z_buffer.h"
#include "../../z_core/z_object.h"

namespace zengine {
namespace socket {

enum class UDPPacketTypeEnum : UInt8 {
    //Message packet for at least one complete message.
    kMessagePackage,
    //Message first slice data. Contains both the message info and data slice.
    //Conbime all slice data to a complete message.
    kMessageFirstSlice,
    //Message slice UDP packet. Conbime all slice data to a complete message.
    kMessageSlice,
    //Message last slice data. Contains the size of the last data slice.
    //Conbime all slice data to a complete message.
    kMessageLastSlice,
    //Discard message by the given message id.
    kDiscardMessageByID,
    //Pause sending message. Will not discard unsent message.
    kPauseSendingMessage,
    //Continue sending messages after a pause sending message packet.
    kContinueSendingMessage,
    //Stop sending messages until a start sending message packet is received. Will discard all unsent messages.
    kStopSendingMessage,
    //Start sending message after a stop sending message packet.
    kStartSendingMessage,
    //Response for a received packet.
    kReceiveResponse,
    //Request to resend a lost packet.
    kResendRequest,
    //Keep alive packet for P2P connection.
    kP2PKeepAlive,
    //P2P connection request packet.
    kP2PConnect,
    //Shutdown P2P connection packet.
    kP2PShutdown,
    //Set the transfer speed limit for the remote side.
    kSetTransferSpeedLimit,
    //Echo packet for latency test.
    kEcho,
};

/*
    Packet base structure for UDP communication.
    Structure:
        0 - 0: packet_type_(UDPPacketTypeEnum, UInt8)
        1 - 1: session_id_(UInt8)
        2 - 3: time_ms_stamp_(UInt16)
        4 - 7: packet_id_(UInt32)
*/
#pragma pack(push, 1)
struct ZUDPPacket : public ZObject {
public:
    static constexpr SizeType kMaxPacketSize = 548;
    static constexpr SizeType kPacketHeadSize = 8;
    static constexpr SizeType kMaxPacketBodySize = kMaxPacketSize - kPacketHeadSize;

    static constexpr SizeType kPacketBodySize = 0;

    NODISCARD FORCEINLINE constexpr SizeType PacketBodySize() const noexcept {
        return kPacketBodySize;
    }

protected:
    using SuperType_ = ZObject;

    FORCEINLINE ZUDPPacket(UDPPacketTypeEnum _packet_type) noexcept 
        : SuperType_()
        , packet_type_(_packet_type)
    {}
    FORCEINLINE ~ZUDPPacket() noexcept {}

public:
    UDPPacketTypeEnum packet_type_;
    //max 255 sessions, use server to distinguish different clients.
    UInt8 session_id_;
    //max 66.535sec for one message time cycle.
    UInt16 time_ms_stamp_;
    UInt32 packet_id_;
};
#pragma pack(pop) 

template<typename _UDPPacketType>
NODISCARD FORCEINLINE ZConstBuffer UDPPacketToConstBuffer(const _UDPPacketType& _udp_packet) noexcept {
    return ZConstBuffer(&_udp_packet, _udp_packet.PacketBodySize() + ZUDPPacket::kPacketHeadSize);
}

}//socket
}//zengine