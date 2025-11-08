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
#include <boost/asio/ssl.hpp>

#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTLSStreamData : public ZObject {
public:
    ZTLSStreamData(boost::asio::ip::tcp::socket* _socket_ptr, boost::asio::ssl::context* _tls_context_ptr) noexcept
        : stream_(*_socket_ptr, *_tls_context_ptr)
    {}

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> stream_;
};

}//internal
}//socket
}//zengine