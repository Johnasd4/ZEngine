/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PSocket = 0x5U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_Socket = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_FSocket = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZAddress = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZIOContext = 0x3U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZUDPMessage = 0x4U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZUDPPeer = 0x5U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZUDPPeerManager = 0x6U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;
inline constexpr ReturnType kErrorCodeBase_ZKCPSession = 0x7U * kErrorCodeTypeMask + kErrorCodeBase_PSocket;

enum SocketErrorCodeEnum : ReturnType::ValueType {
    kSocketErrorCode_LinkError = kErrorCodeBase_Socket,
    kSocketErrorCode_SystemOrLibraryError,
    kSocketErrorCode_NullptrParam,
    kSocketErrorCode_ParamOutOfRange,
    kSocketErrorCode_StateError,
    kSocketErrorCode_OperationCanceled,
    kSocketErrorCode_Disconnected,
    kSocketErrorCode_UDPSocketNotConnected,
    kSocketErrorCode_ConnectFailed,
    kSocketErrorCode_ConnectRefused,
    kSocketErrorCode_SocketShutdown,
    kSocketErrorCode_ReadUntilSymbolNotFound,
    kSocketErrorCode_DestructedBeforeClose,
    kSocketErrorCode_TryResend,
};

} // error_code
} // zengine