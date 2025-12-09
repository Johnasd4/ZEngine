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

#include "zengine/z_engine.h"
#include "zengine/z_gui/test.h"

using namespace zengine;
using namespace zengine::gui;
using namespace zengine::math;
using namespace zengine::file_system;
using namespace zengine::console;
using namespace zengine::tsrpg;
using namespace zengine::socket;

namespace test {

Int32 LogOutputServerTest() { 
    zengine::socket::StartLogOutputServer();

    ReturnType link_code = kOK;

    SizeType count = 0;
    while (true) {
        Z_LOG_ERROR(0, 0, "Error... {}", count);
        Z_LOG_TRACE("Trace... {}", count);
        Z_LOG_MESSAGE("Message... {}", count);
        Z_LOG_START("Start... {}", count);
        Z_LOG_PROCESS("Process {}", count);
        Z_LOG_FINISH("Finish... {}", count);
        Z_LOG_FAILURE("Failure... {}", count);
        Z_LOG_SUCCESS("Success... {}", count);
        ++count;
        SleepMs(1000);
    }

    /**/
    return 0;
}

}//test
  