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

#include "d_lib.h"

namespace zengine {
namespace socket {

enum class TLSTypeEnum : Int32 {
    kClient,
    kServer
};

enum class TLSVerifyModeEnum : Int32 {
    kNone,
    kPeer
};

enum class CertificateFileFormatTypeEnum : Int32 {
    kASN_1,
    kPEM
};

enum class IPTypeEnum : Int32 {
    IP4,
    IP6
};

}//socket
}//zengine

namespace zengine {
namespace socket {

class ZSocketBufferStream;
class ZFixedBuffer;
class ZIOContext;
class ZTCPEndpoint;
class ZUDPEndpoint;
class ZTCPSocket;
class ZUDPSocket;
class ZTCPSingleSessionClient;
class ZTCPMultipleSessionClient;
class ZTCPSingleSessionServer;
class ZTCPMultipleSessionServer;

class ZTLSContext;
class ZTLSStream;

}//socket
}//zengine