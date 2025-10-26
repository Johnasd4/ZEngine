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

#include "../internal/z_drive.h"

#include "z_log.h"

namespace zengine {
namespace error_code {

enum MLogErrorCode : ReturnType {
    kMLogErrorCode_LinkError = kErrorCodeBase_MLog,
    kMLogErrorCode_LogQueueOverflow,
    kMLogErrorCode_PortIDOutOfRange,
    kMLogErrorCode_LogPortOutputFunctionFull,
    kMLogErrorCode_LogPortOutputFunctionAlreadyRegistered,
    kMLogErrorCode_LogPortInputFunctionAlreadyRegistered,
    kMLogErrorCode_LogPortInputFunctionUnregisteredFailed,
    kMLogErrorCode_LogPortFull
};

}//error_code
}//zengine

namespace zengine {
namespace log {

enum LogTypeEnum : IndexType {
    kLogType_Min = 0,
    kLogType_Error = kLogType_Min,
    kLogType_Trace,
    kLogType_Info,
    kLogType_Max
};

enum InfoLogTypeEnum : IndexType {
    kInfoLogType_Min = 0,
    kInfoLogType_Message = kLogType_Min,
    kInfoLogType_Start,
    kInfoLogType_Process,
    kInfoLogType_Finish,
    kInfoLogType_Success,
    kInfoLogType_Failure,
    kInfoLogType_Max
};

static constexpr Int32 kLogMaxPortNum = 8;
static constexpr Int32 kLogPortMaxOutputNum = 8;
static constexpr IndexType kErrorLogPortID = kLogMaxPortNum - 1;
static constexpr IndexType kTraceLogPortID = kLogMaxPortNum - 2;
static constexpr IndexType kInfoLogPortID = kLogMaxPortNum - 3;

//max size of the log message string.
static constexpr Int32 kFormatStringMaxSize = 1024;
//max size of the output log string.
static constexpr Int32 kOutputStringMaxSize = 2048;
//max size of the log message string.
static constexpr Int32 kLogFileMaxNum = 10;

}//log
}//zengine