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
//#include "../z_core/t_function.h"
//#include "../z_core/t_pool_list.h"
//#include "../z_core/t_smart_pointer.h"
//#include "../z_core/t_unordered_map.h"
//#include "../z_core/z_mutex.h"
//#include "../z_core/z_object.h"
//#include "../z_core/z_string.h"
//#include "../z_core/z_string_view.h"
//#include "../z_core/z_thread.h"
//
//#include "z_buffer.h"
//#include "z_UDP_socket.h"
//
//namespace zengine {
//namespace socket {
//namespace internal {
//
//struct ZUDPSingleSessionClientData;
//struct ZUDPMultipleSessionClientData;
//
//}//internal
//}//socket
//}//zengine
//
//namespace zengine {
//namespace socket {
//
//using ZUDPClient = ZUDPSingleSessionClient;
//
///*
//    Single session UDP client. Can connect one server at a time.
//    Connect -> Read/Write -> Close
//*/
//class SOCKET_DLLAPI ZUDPSingleSessionClient : public ZObject {
//public:
//    static constexpr Int32 kConnectRetryForever = kInt32Max;
//
//    enum State_ : Int32 {
//        ZUDPSingleSessionClientState_Uninitialized,
//        ZUDPSingleSessionClientState_Idle,
//        ZUDPSingleSessionClientState_Connected,
//        ZUDPSingleSessionClientState_Error
//    };
//
//    ZUDPSingleSessionClient(ZIOContext* _io_context_ptr) noexcept;
//
//    ~ZUDPSingleSessionClient() noexcept;
//
//    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }
//    NODISCARD FORCEINLINE ZIOContext* IOContextPtr() const noexcept { return io_context_ptr_; }
//
//    /*
//        Bind endpoint by address and port. Call before connected.
//    */
//    NODISCARD ReturnType BindEndpoint(const Char* _address_str, Int32 _port) noexcept;
//
//    /*
//        Sets os write buffer size. Call after connected.
//    */
//    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;
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
//    /*
//        Connect to server. Will suspend the current thread.
//    */
//    NODISCARD ReturnType Connect(
//        ZStringView _address_str,
//        ZStringView _port_str,
//        Int32 _repeat_times = kConnectRetryForever
//    ) noexcept;
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
//        ZBufferStream* _buffer_ptr,
//        Char _match_char,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Read data until match string. Will suspend the current thread until data read.
//    */
//    NODISCARD ReturnType ReadUntil(
//        ZBufferStream* _buffer_ptr,
//        const Char* _match_str,
//        SizeType* _data_size_ptr = nullptr
//    ) noexcept;
//
//    /*
//        Read data until match char. Will not suspend the current thread.
//        _handle_func only needs to handle the read data.
//        _handle_func(ZUDPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
//    */
//    NODISCARD ReturnType AsyncReadUntil(
//        ZBufferStream* _buffer_ptr,
//        Char _match_char,
//        const TFunction<Void(ZUDPSocket*, ZBufferStream*)>& _handle_func
//    ) noexcept;
//
//    /*
//        Read data until match string. Will not suspend the current thread.
//        _handle_func only needs to handle the read data.
//        _handle_func(ZUDPSocket* _socket_ptr, ZBufferStream* _buffer_stream_ptr)
//    */
//    NODISCARD ReturnType AsyncReadUntil(
//        ZBufferStream* _buffer_ptr,
//        const Char* _match_str,
//        const TFunction<Void(ZUDPSocket*, ZBufferStream*)>& _handle_func
//    ) noexcept;
//
//    /*
//        Read data until close. Will suspend the current thread until close.
//    */
//    NODISCARD ReturnType ReadUntilClose(
//        ZBufferStream* _buffer_ptr,
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
//    ZUDPSingleSessionClient(const ZUDPSingleSessionClient&) = delete;
//    ZUDPSingleSessionClient(ZUDPSingleSessionClient&&) = delete;
//    ZUDPSingleSessionClient& operator=(const ZUDPSingleSessionClient&) = delete;
//    ZUDPSingleSessionClient& operator=(ZUDPSingleSessionClient&&) = delete;
//
//private:
//    TUniquePointer<internal::ZUDPSingleSessionClientData> data_ptr_;
//    ZUDPSocket socket_;
//    ZIOContext* io_context_ptr_;
//    State_ state_;
//
//    SizeType client_packet_index_;
//    SizeType server_packet_index_;
//};
//
//}//socket
//}//zengine