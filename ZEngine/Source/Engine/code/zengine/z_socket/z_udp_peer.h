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

#include "../z_core/t_function.h"
#include "../z_core/t_hash_map.h"
#include "../z_core/t_loop_queue.h"
#include "../z_core/t_unique_lock.h"
#include "../z_core/z_mutex.h"

#include "z_udp_socket.h"

namespace zengine {
namespace socket {

struct ZUDPPacket;

}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    UDP peer for communication. Can only connect to one endpoint.
    Open -> BindEndpoint -> SetReceiveMessageHandleFunction -> Connect -> Send -> Close
*/
class SOCKET_DLLAPI ZUDPPeer : public ZObject {
public:
    static inline constexpr Int32 kDefaultConnectTimeMs = 30000;

    enum class StateEnum_ : Int32 {
        Uninitialized,
        Closed,
        Opened,
        EndpointBind,
        Connecting,
        Connected,
        Paused,
        Stopped,
        Error
    };

    ZUDPPeer(ZIOContext* _io_context_ptr) noexcept;
    ~ZUDPPeer() noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_.Value(); }
    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return socket_.IOContextPtr(); }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return socket_.LinkObjectPtr(); }
    template<typename _ObjectType>
    NODISCARD Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        socket_.SetLinkObjectPtr(_obj_ptr);
    }
    NODISCARD FORCEINLINE ZUDPSocket* SocketPtr() noexcept { return &socket_; }

    /*
        Open the peer socket.
    */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /*
        Bind endpoint. Call before connected.
    */
    NODISCARD ReturnType BindEndpoint(const ZUDPEndpoint& _tcp_endpoint) noexcept;

    /*
        Connect to peer or server. Return after connected or timeout.
    */
    NODISCARD ReturnType ConnectP2P(
        const ZTCPEndpoint& _tcp_endpoint,
        TimeType _max_connect_time_ms = kDefaultConnectTimeMs
    ) noexcept;

    /*
        Sets os read buffer size. Call after connected.
    */
    NODISCARD FORCEINLINE ReturnType SetReceiveMessageHandleFunction(
        
    ) noexcept;
    
    /*
        Stops connecting.
    */
    NODISCARD Void Send() noexcept;

    /*
        Close the connection.
    */
    NODISCARD ReturnType Close() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZUDPPeer(const ZUDPPeer&) = delete;
    ZUDPPeer(ZUDPPeer&&) = delete;
    ZUDPPeer& operator=(const ZUDPPeer&) = delete;
    ZUDPPeer& operator=(ZUDPPeer&&) = delete;

private:
    ZUDPSocket socket_;
    ZUDPPacketFactory packet_factory_;
    THashMap<UInt32, TUniquePointer<ZUDPPacket>> receive_unorder_packet_map_;
    THashMap<UInt32, TUniquePointer<ZUDPPacket>> send_uncheck_packet_map_;
    TLoopQueue<TUniquePointer<ZUDPPacket>> receive_packet_queue_;
    TLoopQueue<TUniquePointer<ZUDPPacket>> send_packet_queue_;
    UInt32 receive_packet_index_;
    UInt32 send_packet_index_;
    TFunction<Void(ReturnType, ZUDPPeer*, const ZConstBuffer)> receive_message_handle_func_;
    //SizeType reveive_unhandle_packet_max_size_;
    //SizeType reveive_unhandle_packet_size_;
    //SizeType unsend_packet_max_size_;
    //SizeType unsend_packet_size_;
    //SizeType send_speed_limit_pur_sec_;
    TimeType send_message_max_interval_;
    ZMutex packet_mutex_;
    TAtom<StateEnum_> state_;
};

}//socket
}//zengine