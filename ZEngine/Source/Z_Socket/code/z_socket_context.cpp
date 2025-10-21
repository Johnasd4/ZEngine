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

#include "z_socket_context.h"

#include <boost/asio.hpp>

#include "z_core/m_log.h"
#include "z_core/z_string.h"

#include "data/z_context_data.h"


namespace zengine {
namespace socket {

ZSocketContext::ZSocketContext() noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZContextData>())
    , state_(ZSocketContextState_Idle)
{}

ZSocketContext::~ZSocketContext() noexcept {}

NODISCARD ReturnType ZSocketContext::Stop() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        state_ != ZSocketContextState_Run,
        error_code::kZSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZSocketContextState_Run
    );

    data_ptr_->io_context_.stop();

    return ret_val;
}

NODISCARD ReturnType ZSocketContext::Run() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZSocketContextState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZSocketContextState_Idle
    );

    state_ = ZSocketContextState_Run;

    data_ptr_->io_context_.run();
    data_ptr_->io_context_.restart();

    state_ = ZSocketContextState_Idle;

    return ret_val;
}

NODISCARD ReturnType ZSocketContext::AsyncRun() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        state_ != ZSocketContextState_Idle,
        error_code::kZSocketErrorCode_StateError,
        L"Context state error! state: %d expect state: %d",
        state_, ZSocketContextState_Idle
    );

    state_ = ZSocketContextState_Run;

    data_ptr_->aysnc_thread_ = ZThread(
        [this]() {
            //start dealing with async operation.
            data_ptr_->io_context_.run();
            data_ptr_->io_context_.restart();

            state_ = ZSocketContextState_Idle;
        }
    );

    return ret_val;
}

}//socket
}//zengine