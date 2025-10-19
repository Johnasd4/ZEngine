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

#include <boost/asio.hpp>

#include "z_core/t_allocator.h"
#include "z_core/t_function.h"
#include "z_core/t_smart_pointer.h"
#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSocketData : public ZObject {
public:
    FORCEINLINE ZTCPSocketData(boost::asio::io_context* _io_context_ptr) noexcept
        : socket_ptr_(MakeUnique<boost::asio::ip::tcp::socket>(*_io_context_ptr))
        , io_context_ptr_(_io_context_ptr)
        , address_()
        , port_()
        , async_error_handle_func_() {}

protected:
    using SuperType_ = ZObject;

public:
    TUniquePointer<boost::asio::ip::tcp::socket> socket_ptr_;
    boost::asio::io_context* io_context_ptr_;
    ZString address_;
    Int32 port_;
    TFunction<Void()> async_error_handle_func_;
};

}//internal
}//socket
}//zengine
