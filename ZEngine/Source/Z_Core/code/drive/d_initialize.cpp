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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "drive/d_initialize.h"

#include "m_log.h"

namespace zengine {

CORE_DLLAPI Void Initialize() noexcept { 
    setlocale(LC_ALL, "");

#if USE_MEMORY_POOL_PERFORMANCE_TEST
    //initialize memory pool log file
    //ZFile& file = memory_pool::TMemoryPoolBase<MEMORY_POOL_THREAD_SAFE>::LogFile();
#endif //USE_MEMORY_POOL_PERFORMANCE_TEST

    //register log output function
#if USE_FILE_LOG
    log::RegisterLogOutputFunction<log::ZErrorLog>(log::ZErrorLog::FileOutputLog);
    log::RegisterLogOutputFunction<log::ZInfoLog>(log::ZInfoLog::FileOutputLog);
    log::RegisterLogOutputFunction<log::ZTraceLog>(log::ZTraceLog::FileOutputLog);
#endif
#if USE_CONSOLE_LOG
    log::RegisterLogOutputFunction<log::ZErrorLog>(log::ZErrorLog::ConsoleOutputLog);
    log::RegisterLogOutputFunction<log::ZInfoLog>(log::ZInfoLog::ConsoleOutputLog);
    log::RegisterLogOutputFunction<log::ZTraceLog>(log::ZTraceLog::ConsoleOutputLog);
#endif

    Z_LOG_ERROR(0, 0, "Error...");
    Z_LOG_TRACE("Trace...");
    Z_LOG_MESSAGE("Message...");
    Z_LOG_START("Start...");
    Z_LOG_PROCESS("Process...");
    Z_LOG_FINISH("Finish...");
    Z_LOG_FAILURE("Failure...");
    Z_LOG_SUCCESS("Success...");

    Z_LOG_SUCCESS("ZEngine initialize success!");
}

}//zengine
