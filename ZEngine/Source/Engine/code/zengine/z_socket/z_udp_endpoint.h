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
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

namespace zengine {
namespace socket {

/*
    IO context.
*/
class SOCKET_DLLAPI ZUDPEndpoint : public ZObject {
public:
    static constexpr SizeType KEndpointSize = 32;
    static constexpr SizeType KIP6Size = 16;

    ZUDPEndpoint() noexcept;
    ZUDPEndpoint(const ZUDPEndpoint& _endpoint) noexcept;
    ZUDPEndpoint(ZUDPEndpoint&& _endpoint) noexcept;
    ZUDPEndpoint(const Char* _ip_str, UInt16 _port) noexcept;
    ZUDPEndpoint(UInt32 _ip, UInt16 _port) noexcept;

    NODISCARD ReturnType SetEndpoint(const Char* _ip_str, UInt16 _port) noexcept;
    Void SetEndpoint(UInt32 _ip, UInt16 _port) noexcept;

    NODISCARD ZString IPString() const noexcept;
    NODISCARD UInt32 IP4() const noexcept;
    NODISCARD TFixedMemory<KIP6Size> IP6() const noexcept;
    NODISCARD UInt16 Port() const noexcept;

    ~ZUDPEndpoint() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZUDPSocket;
    friend class ZIOContext;
    friend class ZUDPSingleSessionClient;
    friend class ZUDPMultipleSessionClient;
    friend class ZUDPSingleSessionServer;
    friend class ZUDPMultipleSessionServer;

private:
    TFixedMemory<KEndpointSize> endpoint_data_;
};

}//socket
}//zengine