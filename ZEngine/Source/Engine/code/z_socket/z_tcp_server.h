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

class ZTCPServerData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Simple one-on-one socket tcp server. Only one client is allowed at a time.
    SetEndpoint -> Listen -> Accept -> Read/Write -> Close
*/
class SOCKET_DLLAPI ZTCPServer : public ZObject {
public:
    enum State_ {
        ZTCPServerState_Idle,
        ZTCPServerState_Listen,
        ZTCPServerState_Connect,
        ZTCPServerState_Error
    };

    ZTCPServer() noexcept;

    ~ZTCPServer() noexcept;

    NODISCARD State_ State() noexcept;

    /*
        Sets the address and port. Call before Listen() or after Close().
    */
    NODISCARD ReturnType SetEndpoint(const Char* _address_str, Int32 _port) noexcept;

    /*
        Sets the socket buffer size. Call after a client is connected.
    */
    NODISCARD ReturnType SetSocketBufferSize(Int32 _size) noexcept;

    /*
        Starts the server.
    */
    NODISCARD ReturnType Listen() noexcept;

    /*
        Suspend the current thread until a client is connected.
    */
    NODISCARD ReturnType Accept() noexcept;

    /*
        Close the server.
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
    ZTCPServer(const ZTCPServer&) = delete;
    ZTCPServer(ZTCPServer&&) = delete;
    ZTCPServer& operator=(const ZTCPServer&) = delete;
    ZTCPServer& operator=(ZTCPServer&&) = delete;

private:
    State_ state_;

    internal::ZTCPServerData* data_ptr_;
};

}//socket
}//zengine