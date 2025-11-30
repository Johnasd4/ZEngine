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

namespace zengine {
namespace socket {
namespace internal {

struct ZTCPSocketData : public ZObject {
public:
    ZTCPSocketData(boost::asio::io_context* _io_context_ptr) noexcept;

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::endpoint bind_endpoint_;
    //WTF??? Getting remote endpoint after socket disconnect will throw exception??? 
    boost::asio::ip::tcp::endpoint remote_endpoint_;
    Bool if_endpoint_bind_;
    TFunction<Void(ReturnType)> async_error_handle_func_;
};

}//internal
}//socket
}//zengine
