///*
//    Copyright (c) YuLin Zhu
//
//    This code file is licensed under the Creative Commons
//    Attribution-NonCommercial 4.0 International License.
//
//    You may obtain a copy of the License at
//    https://creativecommons.org/licenses/by-nc/4.0/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    Author: YuLin Zhu
//    Contact: 1152325286@qq.com
//*/
//#define PROJECT_NAME L"Include"
//
//#include "zengine/z_engine.h"
//#include "zengine/z_gui/test.h"
//
//using namespace zengine;
//using namespace zengine::gui;
//using namespace zengine::math;
//using namespace zengine::file_system;
//using namespace zengine::console;
//using namespace zengine::tsrpg;
//using namespace zengine::socket;
//
////Int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//Int32 main() { 
//    zengine::Initialize();
//    zengine::socket::StartLogOutputServer();
//
//    Z_LOG_ERROR(1, 2, L"Error...");
//    Z_LOG_TRACE(L"Trace...");
//    Z_LOG_MESSAGE(L"Message...");
//    Z_LOG_START(L"Start...");
//    Z_LOG_PROCESS(L"Process 1...");
//    Z_LOG_PROCESS(L"Process 2...");
//    Z_LOG_PROCESS(L"Process 3...");
//    Z_LOG_FINISH(L"Finish...");
//    Z_LOG_FAILURE(L"Failure...");
//    Z_LOG_SUCCESS(L"Success...");
//
//    ReturnType link_code = kOK;
//
//    //ZWindow window("Title", GuiSize(1000, 1000));
//    //window.SetScreenCenter();
//
//    //ZFrame frame1("Frame1");
//    //window.Add(&frame1);
//    //ZFrame frame2("Frame2");
//    //frame1.Add(&frame2);
//    //frame2.SetHeight(180);
//    //frame2.SetWidth(300);
//    //frame1.SetPos(GuiPos(0,0));
//    //frame2.SetPos(GuiPos(0,0));
//
//    //window.Execute();
//
//    //Test_ImguiDemo();
//    Test_000();
//  
//
//    /**/
//    SleepMs(kTimeBeforeProgramExit);
//    return 0;
//}
//  