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

inline constexpr ReturnType kErrorCodeBase_Socket = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_HTTP = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_URL = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_UDPMessage = 0x3U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

inline constexpr ReturnType kErrorCodeBase_ZSocket = 0x10U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZHTTP = 0x11U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZURL = 0x12U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

inline constexpr ReturnType kErrorCodeBase_FTCPLogOutput = 0x10U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_FSocket = 0x11U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

enum SocketErrorCodeEnum : ReturnType {
    kSocketErrorCode_LinkError = kErrorCodeBase_Socket,
    kSocketErrorCode_SystemError,
    kSocketErrorCode_NullptrParam,
    kSocketErrorCode_ParamOutOfRange,
    kSocketErrorCode_StateError,
    kSocketErrorCode_OperationCanceled,
    kSocketErrorCode_SocketNotOpen,
    kSocketErrorCode_AddressNotVaild,
    kSocketErrorCode_IPNotVaild,
    kSocketErrorCode_PortNotVaild,
    kSocketErrorCode_Disconnected,
    kSocketErrorCode_EndpointNotBind,
    kSocketErrorCode_UDPSocketNotConnected,
    kSocketErrorCode_ConnectFailed,
    kSocketErrorCode_ReadUntilSymbolNotFound,
    kSocketErrorCode_TLSTypeNotValid,
    kSocketErrorCode_TLSVerifyModeNotValid,
    kSocketErrorCode_CertificateFileFormatNotValid,
    kSocketErrorCode_ServerCertificateNotLoaded,
    kSocketErrorCode_ServerPrivateKeyNotLoaded,
};

enum HTTPErrorCodeEnum : ReturnType {
    kHTTPErrorCode_LinkError = kErrorCodeBase_HTTP,
    kHTTPErrorCode_SystemError,
    kHTTPErrorCode_NullptrParam,
    kHTTPErrorCode_ParamOutOfRange,
    kHTTPErrorCode_HTTP11ResponseResolverAlreadyUsed,
};

enum URLErrorCodeEnum : ReturnType {
    kURLErrorCode_LinkError = kErrorCodeBase_URL,
    kURLErrorCode_SystemError,
    kURLErrorCode_NullptrParam,
    kURLErrorCode_ParamOutOfRange,
    kURLErrorCode_URLResolveFailed,
};

}//error_code
}//zengine