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

#include "z_tls_context_data.h"

namespace zengine {
namespace socket {
namespace internal {

ZTLSContextData::ZTLSContextData(TLSTypeEnum _tls_type) noexcept
    : SuperType_()
    , tls_context_(
        _tls_type == kTLSType_Client ? boost::asio::ssl::context::tls_client : boost::asio::ssl::context::tls_server
    )
{}

}//internal
}//socket
}//zengine