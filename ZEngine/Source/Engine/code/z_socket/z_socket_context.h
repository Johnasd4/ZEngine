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

#include "z_core/t_function.h"
#include "z_core/t_smart_pointer.h"
#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZContextData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    TCP socket type.
    Contains a extra ptr that can be linked to any object. Call LinkObjectPtr<_ObjectType>() to get the object ptr.
*/
class SOCKET_DLLAPI ZSocketContext : public ZObject {
public:
    enum State_ {
        ZSocketContextState_Idle,
        ZSocketContextState_Run
    };

    ZSocketContext() noexcept;
    ~ZSocketContext() noexcept;

    NODISCARD FORCEINLINE State_ State() noexcept { return state_; }

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
    NODISCARD FORCEINLINE Bool IsRunning() noexcept { return state_ == ZSocketContextState_Run; }

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;
    friend class ZUDPSocket;
    friend class ZTCPSingleSessionClient;
    friend class ZTCPMultipleSessionClient;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:
    ZSocketContext(const ZSocketContext&) = delete;
    ZSocketContext(ZSocketContext&&) = delete;
    ZSocketContext& operator=(const ZSocketContext&) = delete;
    ZSocketContext& operator=(ZSocketContext&&) = delete;

private:
    TUniquePointer<internal::ZContextData> data_ptr_;
    State_ state_;
};

}//socket
}//zengine