///*
//    Copyright (c) YuLin Zhu
//
//    This code file is licensed under the Creative Commons
//    Attribution-NonCommercial 4.0 International License.
//
//    You may obtain a copy of the License at
//    https://creativecommons.org/licenses/by-nc/4.0/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    Author: YuLin Zhu
//    Contact: 1152325286@qq.com
//*/
//#pragma once
//
//#include "drive.h"
//
//#include "../z_core/t_hash_map.h"
//
//#include "z_udp_socket.h"
//
//namespace zengine {
//namespace socket {
//
//struct ZUDPPacket;
//
//}//socket
//}//zengine
//
//namespace zengine {
//namespace socket {
//
///*
//    UDP peer for communication. Can only connect to one endpoint.
//    Open -> BindEndpoint -> Connect -> Send/Receive -> Close
//*/
//class SOCKET_DLLAPI ZUDPPeer : public ZObject {
//public:
//    static inline constexpr Int32 kDefaultConnectTimeMs = 30000;
//
//    enum class StateEnum_ : Int32 {
//        Uninitialized,
//        Closed,
//        Opened,
//        EndpointBind,
//        Connecting,
//        Connected,
//        Paused,
//        Stopped,
//        Error
//    };
//
//    ZUDPPeer(ZIOContext* _io_context_ptr) noexcept;
//    ~ZUDPPeer() noexcept;
//
//    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_.Value(); }
//    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
//    template<typename _ObjectType>
//    NODISCARD FORCEINLINE _ObjectType* LinkObjectPtr() const noexcept { return socket_.LinkObjectPtr(); }
//    NODISCARD FORCEINLINE ZUDPSocket* SocketPtr() noexcept { return &socket_; }
//
//    /*
//        Open the peer socket.
//    */
//    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;
//
//    /*
//        Bind endpoint. Call before connected.
//    */
//    NODISCARD ReturnType BindEndpoint(const ZUDPEndpoint& _tcp_endpoint) noexcept;
//
//    /*
//        Connect to peer or server. Return after connected or timeout.
//    */
//    NODISCARD ReturnType ConnectP2P(
//        const ZTCPEndpoint& _tcp_endpoint,
//        TimeType _max_connect_time_ms = kDefaultConnectTimeMs
//    ) noexcept;
//
//    /*
//        Sets os write buffer size. Call after connected.
//    */
//    NODISCARD ReturnType SetMaxPacketQuqueSize(SizeType _size) noexcept;
//    /*
//        Sets os read buffer size. Call after connected.
//    */
//    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;
//
//    /*
//        Close the connection.
//    */
//    NODISCARD ReturnType Close() noexcept;
//
//
//
//    /*
//        Stops connecting.
//    */
//    NODISCARD Void StopConnect() noexcept;
//
//    /*
//        Get socket ptr.
//        WARNING: Moving the socket data might cause fatal errors.
//    */
//    NODISCARD FORCEINLINE ZUDPSocket& GetSocket() noexcept { return socket_; }
//
//    /*
//        Read data. Will suspend the current thread until data read.
//    */
//    NODISCARD ReturnType Read(
//        ZBuffer _buffer,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Read data. Will not suspend the current thread.
//        _handle_func only needs to handle the read data.
//        _handle_func(ZUDPSocket* _socket_ptr, const ZConstBuffer _buffer)
//    */
//    NODISCARD ReturnType AsyncRead(
//        ZBuffer _buffer,
//        const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
//    ) noexcept;
//
//    /*
//        Read data until match char. Will suspend the current thread until data read.
//    */
//    NODISCARD ReturnType ReadUntil(
//        ZSocketBufferStream* _buffer_ptr,
//        Char _match_char,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Read data until match string. Will suspend the current thread until data read.
//    */
//    NODISCARD ReturnType ReadUntil(
//        ZSocketBufferStream* _buffer_ptr,
//        const Char* _match_str,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Read data until match char. Will not suspend the current thread.
//        _handle_func only needs to handle the read data.
//        _handle_func(ZUDPSocket* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
//    */
//    NODISCARD ReturnType AsyncReadUntil(
//        ZSocketBufferStream* _buffer_ptr,
//        Char _match_char,
//        const TFunction<Void(ZUDPSocket*, ZSocketBufferStream*)>& _handle_func
//    ) noexcept;
//
//    /*
//        Read data until match string. Will not suspend the current thread.
//        _handle_func only needs to handle the read data.
//        _handle_func(ZUDPSocket* _socket_ptr, ZSocketBufferStream* _buffer_stream_ptr)
//    */
//    NODISCARD ReturnType AsyncReadUntil(
//        ZSocketBufferStream* _buffer_ptr,
//        const Char* _match_str,
//        const TFunction<Void(ZUDPSocket*, ZSocketBufferStream*)>& _handle_func
//    ) noexcept;
//
//    /*
//        Read data until close. Will suspend the current thread until close.
//    */
//    NODISCARD ReturnType ReadUntilClose(
//        ZSocketBufferStream* _buffer_ptr,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Write data. Will suspend the current thread until data write.
//    */
//    NODISCARD ReturnType Write(
//        ZConstBuffer _buffer
//    ) noexcept;
//
//    /*
//        Write data. Will not suspend the current thread.
//        _handle_func will be called after the data send.
//        _handle_func(ZUDPSocket* _socket_ptr, const ZConstBuffer _buffer)
//    */
//    NODISCARD ReturnType AsyncWrite(
//        ZConstBuffer _buffer,
//        const TFunction<Void(ZUDPSocket*, const ZConstBuffer)>& _handle_func
//    ) noexcept;
//
//    /*
//        Return the size of the data can be read.
//    */
//    NODISCARD FORCEINLINE SizeType ReadableDataSize() noexcept {
//        return socket_.ReadableDataSize();
//    }
//
//protected:
//    using SuperType_ = ZObject;
//
//private:
//    ZUDPPeer(const ZUDPPeer&) = delete;
//    ZUDPPeer(ZUDPPeer&&) = delete;
//    ZUDPPeer& operator=(const ZUDPPeer&) = delete;
//    ZUDPPeer& operator=(ZUDPPeer&&) = delete;
//
//private:
//    THashMap<UInt32, ZUDPPacket*> receive_check_packet_map_;
//    TQueue
//    TAtom<StateEnum_> state_;
//    ZUDPSocket socket_;
//    ZIOContext* io_context_ptr_;
//};
//
//}//socket
//}//zengine