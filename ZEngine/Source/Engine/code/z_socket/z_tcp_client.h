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

#include "internal/z_drive.h"

#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

class ZTCPClientData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Simple one-on-one socket tcp client.
    Resolve -> Connect -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPClient : public ZObject {
public:
    static constexpr Int32 kConnectRetryForever = kInt32Max;

    enum State_ {
        ZTCPClientState_Idle,
        ZTCPClientState_Connect,
        ZTCPClientState_Error
    };

    ZTCPClient() noexcept;

    ~ZTCPClient() noexcept;

    NODISCARD State_ State() noexcept;

    /*
        Sets the address and port. Call before Connect() or after Close().
    */
    NODISCARD ReturnType SetEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

    /*
        Coonect to server.
    */
    NODISCARD ReturnType Connect(Int32 _repeat_times = kConnectRetryForever) noexcept;

    /*
        Close the connection.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Read a message from the client. Will suspend the current thread until a message is received.
    */
    NODISCARD ReturnType Read(Void* _data_buffer, Int32 _buffer_size, SizeType* _message_size_ptr = nullptr) noexcept;

    /*
        Send a message to the client.
    */
    NODISCARD ReturnType Write(const Void* _data_ptr, SizeType _date_size) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZTCPClient(const ZTCPClient&) = delete;
    ZTCPClient(ZTCPClient&&) = delete;
    ZTCPClient& operator=(const ZTCPClient&) = delete;
    ZTCPClient& operator=(ZTCPClient&&) = delete;

private:
    State_ state_;

    internal::ZTCPClientData* data_ptr_;
};

}//socket
}//zengine