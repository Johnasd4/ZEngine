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

#include "../z_core/t_vector.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZIOContextData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    IO context.
*/
class SOCKET_DLLAPI ZIOContext : public ZObject {
public:
    enum State_ : Int32 {
        ZIOContextState_Idle,
        ZIOContextState_Run
    };

    ZIOContext() noexcept;
    ~ZIOContext() noexcept;

    NODISCARD FORCEINLINE State_ State() const noexcept { return state_; }

    /*
        Resolve the given address.
    */
    NODISCARD ReturnType ResolveTCPAddress(
        ZStringView _address_str,
        ZStringView _port_str,
        TVector<ZTCPEndpoint>* _endpoint_vector_ptr
    ) noexcept;

    /*
        Resolve the given address.
    */
    NODISCARD ReturnType ResolveUDPAddress(
        ZStringView _address_str,
        ZStringView _port_str,
        TVector<ZUDPEndpoint>* _endpoint_vector_ptr
    ) noexcept;

    /*
        Stop all sockets.
    */
    NODISCARD ReturnType Stop() noexcept;

    /*
        Deal with async operation until all operation finished.
        Suspend the current thread, returns until finish.
    */
    NODISCARD ReturnType Run() noexcept;

    /*
        Deal with async operation until all operation finished.
        Starts a new thread and returns immediately.
    */
    NODISCARD ReturnType AsyncRun() noexcept;

    /*
        If dealing with async operation.
    */
    NODISCARD FORCEINLINE Bool IsRunning() noexcept { return state_ == ZIOContextState_Run; }

    /*
        Suspend until async operation thread finish.
    */
    NODISCARD Void Join() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;
    friend class ZUDPSocket;
    friend class ZTCPSingleSessionClient;
    friend class ZTCPMultipleSessionClient;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:
    ZIOContext(const ZIOContext&) = delete;
    ZIOContext(ZIOContext&&) = delete;
    ZIOContext& operator=(const ZIOContext&) = delete;
    ZIOContext& operator=(ZIOContext&&) = delete;

private:
    TUniquePointer<internal::ZIOContextData> data_ptr_;
    State_ state_;
};

}//socket
}//zengine