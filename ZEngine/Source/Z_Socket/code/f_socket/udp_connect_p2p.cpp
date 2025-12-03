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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "f_socket.h"

#include "z_io_context.h"
#include "z_udp_socket.h"

namespace zengine {
namespace socket {

SOCKET_DLLAPI ReturnType UDPConnectP2P(
    const ZUDPEndpoint& _local_udp_endpoint,
    const ZUDPEndpoint& _target_udp_endpoint,
    TimeType _max_connect_time,
    ZIOContext* _io_context_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZUDPSocket udp_socket(_io_context_ptr);

    link_code = udp_socket.Open(_local_udp_endpoint.IPType());
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZUDPSocket::Open() link error!"
        );
        return ret_val;
    }

    link_code = udp_socket.BindEndpoint(_local_udp_endpoint);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZUDPSocket::BindEndpoint() link error!"
        );
        return ret_val;
    }

    struct P2PMessage {
        Bool local_received_;
        Bool target_received_;
    };

    TimeType start_time = TimeSec();
    TimeType current_time = start_time;
    while (current_time - start_time < _max_connect_time) {
        // Prepare data to send
        const char* data_to_send = "Hello, P2P!";
        size_t data_size = strlen(data_to_send);

        //// Send data to the target UDP endpoint
        //if (!udp_socket.SendTo(_target_udp_endpoint, data_to_send, data_size)) {
        //    return ERROR_SEND_DATA_FAILED;
        //}

        //// Optionally: Receive response from the target UDP endpoint
        //char buffer[1024];
        //size_t bytes_received = udp_socket.ReceiveFrom(buffer, sizeof(buffer), _public_udp_endpoint);
        //if (bytes_received > 0) {
        //    // Process received data
        //    std::string response(buffer, bytes_received);
        //    if (response == "Hello back!") {
        //        return SUCCESSFUL_CONNECTION;
        //    }
        //}

        // Sleep for a short duration before sending the next packet
        SleepMs(100LL);
    }

    return ret_val;
}

}//socket
}//zengine