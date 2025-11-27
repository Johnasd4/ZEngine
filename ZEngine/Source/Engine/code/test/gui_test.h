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

Int32 GuiTest() { 
    zengine::socket::StartLogOutputServer();

    ReturnType link_code = kOK;

    ZWindow window("Title", GuiSize(1000, 1000));
    window.SetScreenCenter();

    ZFrame frame1("Frame1");
    window.Add(&frame1);
    ZButton button("button");
    frame1.Add(&button);
    ZCheckBox check_box("check_box");
    frame1.Add(&check_box);
    frame1.SetPos(GuiPos(0,0));

    window.Execute();

    //Test_ImguiDemo();
    Test_000();
  
    return 0;
}
  
}//test