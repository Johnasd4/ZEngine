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

#include "../z_core/t_fixed_string.h"

/*
    This module allows logs over tcp. 
    After a client is connected to the server, when a log is being output, 
    the module will copy the output and store it in a queue. In a new thread, 
    logs will be popped out and send to the client.
    When the client disconnects over a certain time, the log queue will be emptied.
*/

namespace zengine {
namespace error_code {

enum FTCPLogOutputErrorCode : ReturnType {
    kFTCPLogOutputErrorCode_LinkError = kErrorCodeBase_FTCPLogOutput,
    kFTCPLogOutputErrorCode_SystemError,
    kFTCPLogOutputErrorCode_NullptrParam,
    kFTCPLogOutputErrorCode_ParamOutOfRange,
    kFTCPLogOutputErrorCode_LogServerStateError,
    kFTCPLogOutputErrorCode_CommandIDNotExist,
    kFTCPLogOutputErrorCode_ReplyIDNotExist,
};

}//zengine
}//error_code

namespace zengine {
namespace socket {

constexpr Char kDefaultTCPLogAddressString[] = "127.0.0.1";
constexpr Char kDefaultTCPLogPortString[] = "10000";
constexpr Int32 kDefaultTCPLogPort = 10000;
constexpr TimeType kLogServerDisconnectLogWaitMaxTime = 30;
constexpr SizeType kLogServerDisconnectLogWaitMaxNum = 1000;

enum TCPLogOutputCommandIDEnum : Int32 {
    TCPLogOutputCommandID_GetNextLog
};

enum TCPLogOutputReplyIDEnum : Int32 {
    TCPLogOutputReplyID_Log,
    TCPLogOutputReplyID_CommandIDNotExist
};

//command

struct TCPLogOutputCommandGetNextLogData {
    Int32 temp_;
};

union TCPLogOutputCommandDataUnion {
    TCPLogOutputCommandGetNextLogData get_next_log_data_;
};

struct TCPLogOutputCommand {
    TCPLogOutputCommandIDEnum command_id_;
    TCPLogOutputCommandDataUnion command_data_;

};

//reply

struct TCPLogOutputErrorLogInfo {
    TimeType temp_;
};

struct TCPLogOutputTraceLogInfo {
    TimeType temp_;
};

struct TCPLogOutputInfoLogInfo {
    zengine::log::InfoLogTypeEnum info_type_;
};

union TCPLogOutputLogInfoUnion {
    TCPLogOutputErrorLogInfo error_log_info_;  
    TCPLogOutputTraceLogInfo trace_log_info_;
    TCPLogOutputInfoLogInfo info_log_info_;
};

struct TCPLogOutputReplyLogData {
    zengine::log::LogTypeEnum log_type_;
    TimeType log_time_;
    TCPLogOutputLogInfoUnion log_info_;
    TFixedWString<zengine::log::kOutputStringMaxSize> log_string_;
};

struct TCPLogOutputReplyCommandNotExistData {
    TCPLogOutputCommandIDEnum command_id_;
};

union TCPLogOutputReplyDataUnion {
    TCPLogOutputReplyLogData log_data_;
    TCPLogOutputReplyCommandNotExistData command_not_exist_data_;
};

struct TCPLogOutputReply {
    TCPLogOutputReplyIDEnum reply_id_;
    TCPLogOutputReplyDataUnion reply_data_;
};

/*
    Start log server.
*/
SOCKET_DLLAPI ReturnType StartLogOutputServer(
    const Char* _address_str = kDefaultTCPLogAddressString,
    Int32 _port = kDefaultTCPLogPort
) noexcept;

/*
* 
    Stop log server.
*/
SOCKET_DLLAPI ReturnType StopLogOutputServer() noexcept;

/*
    Start log handle client.
*/
SOCKET_DLLAPI ReturnType StartLogOutputClient( 
    Void (*_handle_func)(const TCPLogOutputReplyLogData*),
    const Char* _address_str = kDefaultTCPLogAddressString,
    const Char* _port_str = kDefaultTCPLogPortString
) noexcept;

}//socket
}//zengine