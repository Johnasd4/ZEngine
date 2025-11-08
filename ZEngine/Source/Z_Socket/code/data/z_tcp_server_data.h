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

#include <boost/asio.hpp>

#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSingleSessionServerData : public ZObject {
public:
    ZTCPSingleSessionServerData(boost::asio::io_context* _io_context_ptr) noexcept
        : acceptor_(*_io_context_ptr)
        , server_endpoint_()
        , if_endpoint_bind_(false) {}

public:
    boost::asio::ip::tcp::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::endpoint server_endpoint_;
    Bool if_endpoint_bind_;
};

struct ZTCPMultipleSessionServerData : public ZObject {
public:
    ZTCPMultipleSessionServerData(boost::asio::io_context* _io_context_ptr) noexcept
        : acceptor_(*_io_context_ptr)
        , server_endpoint_()
        , if_endpoint_bind_(false) {
    }

public: 
    boost::asio::ip::tcp::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::endpoint server_endpoint_;
    Bool if_endpoint_bind_;
};

}//internal
}//socket
}//zengine
