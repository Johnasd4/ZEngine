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

#include "z_core/z_object.h"
#include "z_core/z_thread.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSingleSessionClientData : public ZObject {
public:
    ZTCPSingleSessionClientData(boost::asio::io_context* _io_context_ptr) noexcept
        : resolver_(*_io_context_ptr) {}

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::ip::tcp::resolver resolver_;
};

struct ZTCPMultipleSessionClientData : public ZObject {
public:
    ZTCPMultipleSessionClientData(boost::asio::io_context* _io_context_ptr) noexcept
        : resolver_(*_io_context_ptr) {}

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::ip::tcp::resolver resolver_;
};

}//internal
}//socket
}//zengine