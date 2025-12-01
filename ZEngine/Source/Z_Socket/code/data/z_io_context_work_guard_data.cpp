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

#include "z_io_context_work_guard_data.h"

namespace zengine {
namespace socket {
namespace internal {

ZIOContextWorkGuardData::ZIOContextWorkGuardData(boost::asio::io_context* _io_context_ptr) noexcept
    : SuperType_()
    , work_guard_(_io_context_ptr->get_executor())
{}

}//internal
}//socket
}//zengine