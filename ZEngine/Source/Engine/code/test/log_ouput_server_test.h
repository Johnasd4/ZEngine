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
    zengine::Initialize();
    zengine::socket::StartLogOutputServer();

    ReturnType link_code = kOK;

    SizeType count = 0;
    while (true) {
        Z_LOG_ERROR(1, 2, L"Error... %d", count);
        Z_LOG_TRACE(L"Trace... %d", count);
        Z_LOG_MESSAGE(L"Message... %d", count);
        Z_LOG_START(L"Start... %d", count);
        Z_LOG_PROCESS(L"Process %d", count);
        Z_LOG_FINISH(L"Finish... %d", count);
        Z_LOG_FAILURE(L"Failure... %d", count);
        Z_LOG_SUCCESS(L"Success... %d", count);
        ++count;
        SleepMs(1000);
    }

    /**/
    return 0;
}

}//test
  