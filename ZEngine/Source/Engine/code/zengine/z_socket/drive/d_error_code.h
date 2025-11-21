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
#include "d_type.h"

namespace zengine {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PSocket = 0x4U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_ZSocket = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZHTTP = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZURL = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

inline constexpr ReturnType kErrorCodeBase_FTCPLogOutput = 0x10U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_FSocket = 0x11U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

enum ZSocketErrorCode : ReturnType {
    kPSocketErrorCode_LinkError = kErrorCodeBase_PSocket,
    kPSocketErrorCode_SystemError,
    kPSocketErrorCode_NullptrParam,
    kPSocketErrorCode_ParamOutOfRange,
    kPSocketErrorCode_StateError,
    kPSocketErrorCode_SocketNotOpen,
    kPSocketErrorCode_AddressNotVaild,
    kPSocketErrorCode_PortNotVaild,
    kPSocketErrorCode_Disconnected,
    kPSocketErrorCode_EndpointNotBind,
    kPSocketErrorCode_UDPSocketNotConnected,
    kPSocketErrorCode_ConnectFailed,
    kPSocketErrorCode_ReadUntilSymbolNotFound,
    kPSocketErrorCode_TLSTypeNotValid,
    kPSocketErrorCode_TLSVerifyModeNotValid,
    kPSocketErrorCode_CertificateFileFormatNotValid,
    kPSocketErrorCode_ServerCertificateNotLoaded,
    kPSocketErrorCode_ServerPrivateKeyNotLoaded,
};

enum ZHTTPErrorCode : ReturnType {
    kPHTTPErrorCode_LinkError = kErrorCodeBase_PSocket,
    kPHTTPErrorCode_SystemError,
    kPHTTPErrorCode_NullptrParam,
    kPHTTPErrorCode_ParamOutOfRange,
    kPHTTPErrorCode_HTTP11ResponseResolverAlreadyUsed,
};

enum ZURLErrorCode : ReturnType {
    kPURLErrorCode_LinkError = kErrorCodeBase_PSocket,
    kPURLErrorCode_SystemError,
    kPURLErrorCode_NullptrParam,
    kPURLErrorCode_ParamOutOfRange,
    kPURLErrorCode_URLResolveFailed,
};

}//error_code
}//zengine