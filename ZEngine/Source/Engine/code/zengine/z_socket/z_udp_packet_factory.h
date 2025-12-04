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

#include "../z_core/t_fixed_memory.h"
#include "../z_core/t_pool.h"

#include "z_udp_packet.h"

namespace zengine {
namespace socket {

/*
    UDP packet factory. Use TPool to manage UDP packet memory.
*/
class SOCKET_DLLAPI ZUDPPacketFactory : public ZObject {
private:
    static constexpr SizeType kMessagePacketSize = ZUDPPacket::kMaxPacketSize;
    static constexpr SizeType kCommandPacketSize = ZUDPPacket::kPacketHeadSize + 8ULL;
    static constexpr SizeType kMessagePacketPoolDefaultSize = 100ULL;
    static constexpr SizeType kCommandPacketPoolDefaultSize = 50ULL;

public:
    ZUDPPacketFactory() noexcept;
    ~ZUDPPacketFactory() noexcept;

    template<typename _PacketType>
    NODISCARD FORCEINLINE _PacketType* Apply() noexcept {
        if constexpr (
            kSameType<_PacketType, ZUDPPacketMessagePackage> ||
            kSameType<_PacketType, ZUDPPacketMessageFirstSilce> ||
            kSameType<_PacketType, ZUDPPacketMessageSlice> ||
            kSameType<_PacketType, ZUDPPacketMessageLastSilce>
        ) {
            return new(message_packet_pool_.Apply()->DataPtr<Void>()) _PacketType();
        }
        else if constexpr (
            kSameType<_PacketType, ZUDPPacketDiscardMessageByID> ||
            kSameType<_PacketType, ZUDPPacketPauseSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketContinueSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketStopSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketStartSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketReceiveResponse> ||
            kSameType<_PacketType, ZUDPPacketResendRequest> ||
            kSameType<_PacketType, ZUDPPacketP2PKeepAlive> ||
            kSameType<_PacketType, ZUDPPacketP2PConnect> ||
            kSameType<_PacketType, ZUDPPacketP2PShutdown> ||
            kSameType<_PacketType, ZUDPPacketSetTransferSpeedLimit> ||
            kSameType<_PacketType, ZUDPPacketEcho>
        ) {
            return new(command_packet_pool_.Apply()->DataPtr<Void>()) _PacketType();
        }
        else {
            static_assert(
                "ZUDPPacketFactory::Apply(): Unsupported packet type!"
            );
            return nullptr;
        }
    }

    template<typename _PacketType>
    FORCEINLINE Void Release(_PacketType* _packet_ptr) noexcept {
        if constexpr (
            kSameType<_PacketType, ZUDPPacketMessagePackage> ||
            kSameType<_PacketType, ZUDPPacketMessageFirstSilce> ||
            kSameType<_PacketType, ZUDPPacketMessageSlice> ||
            kSameType<_PacketType, ZUDPPacketMessageLastSilce>
        ) {
            if (_packet_ptr != nullptr) {
                message_packet_pool_.Release(reinterpret_cast<TFixedMemory<kMessagePacketSize>*>(_packet_ptr));
            }
        }
        else if constexpr (
            kSameType<_PacketType, ZUDPPacketDiscardMessageByID> ||
            kSameType<_PacketType, ZUDPPacketPauseSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketContinueSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketStopSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketStartSendingMessage> ||
            kSameType<_PacketType, ZUDPPacketReceiveResponse> ||
            kSameType<_PacketType, ZUDPPacketResendRequest> ||
            kSameType<_PacketType, ZUDPPacketP2PKeepAlive> ||
            kSameType<_PacketType, ZUDPPacketP2PConnect> ||
            kSameType<_PacketType, ZUDPPacketP2PShutdown> ||
            kSameType<_PacketType, ZUDPPacketSetTransferSpeedLimit> ||
            kSameType<_PacketType, ZUDPPacketEcho>
        ) {
            if (_packet_ptr != nullptr) {
                command_packet_pool_.Release(reinterpret_cast<TFixedMemory<kCommandPacketSize>*>(_packet_ptr));
            }
        }
        else {
            static_assert(
                "ZUDPPacketFactory::Release(): Unsupported packet type!"
            );
        }
    }

protected:
    using SuperType_ = ZObject;

private:
    ZUDPPacketFactory(const ZUDPPacketFactory&) = delete;
    ZUDPPacketFactory(ZUDPPacketFactory&&) = delete;
    ZUDPPacketFactory& operator=(const ZUDPPacketFactory&) = delete;
    ZUDPPacketFactory& operator=(ZUDPPacketFactory&&) = delete;

private:
    TPool<TFixedMemory<kMessagePacketSize>> message_packet_pool_;
    TPool<TFixedMemory<kCommandPacketSize>> command_packet_pool_;
};

}//socket
}//zengine