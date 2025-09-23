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
#define PROJECT_NAME L"Include"

#include "z_engine.h"
#include "z_gui/test.h"

using namespace zengine;
using namespace zengine::gui;
using namespace zengine::math;
using namespace zengine::file_system;
using namespace zengine::console;
using namespace zengine::tsrpg;
using namespace zengine::socket;
//using namespace std;

class TestClass {
public:
    int a = 1;
    Void Print() {
        Z_LOG_MESSAGE(L"%d", a);
    }
};

Int32 TestFunc() {
    Z_LOG_MESSAGE(L"1");
    return 1;  // 返回值数量
}

Void TestThreadFunc() {
    ReturnType link_code = kOK;
    //ZApplication app;
    ZWindow::SetVerticalSynchronization(1);
    ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenMode_Window);
    ZFrame test_frame("frame1", GuiSize(500, 500), GuiPos(100, 200));
    test_frame.SetBackgruondColour({ 1.0f, 0.0f, 0.0f, 1.0f });
    test_window.Add(&test_frame);
    ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
    test_frame_2.SetBackgruondColour({ 0.0f, 1.0f, 0.0f, 1.0f });
    //test_window.Add(&test_frame_2);  
    test_frame.Add(&test_frame_2);
    //test_window.Hide();
    link_code = test_window.Execute();
    //app.AddWindow(&test_window2);
    //link_code = app.Execute();
}

//Int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
Int32 main() { 
    zengine::Initialize();

    Z_LOG_ERROR(1, 2, L"Error...");
    Z_LOG_TRACE(L"Trace...");
    Z_LOG_MESSAGE(L"Message...");
    Z_LOG_START(L"Start...");
    Z_LOG_PROCESS(L"Process 1...");
    Z_LOG_PROCESS(L"Process 2...");
    Z_LOG_PROCESS(L"Process 3...");
    Z_LOG_FINISH(L"Finish...");
    Z_LOG_FAILURE(L"Failure...");
    Z_LOG_SUCCESS(L"Success...");

    sizeof(wchar_t);

    ReturnType link_code;

    ZTCPClient tcp_client;
    link_code = tcp_client.SetEndpoint("127.0.0.1", 12345);
    link_code = tcp_client.Connect(3);
    if (link_code == kOK) {
        for (;;) {
            // 接收客户端消息
            char data[1024] = { 0 };
            SizeType length;
            link_code = tcp_client.Read(static_cast<Void*>(data), 1024, &length);

            std::cout << "Received: " << data;

            std::string message;
            std::getline(std::cin, message);

            // 回显消息给客户端
            link_code = tcp_client.Write(static_cast<const Void*>(message.c_str()), message.length() + 1);

            if (tcp_client.State() != ZTCPClient::State_::ZTCPClientState_Connect) {
                break;
            }
        }
    }
    //ZTCPServer tcp_server;
    //link_code = tcp_server.SetEndpoint("127.0.0.1", 12345);
    ////link_code = tcp_server.SetEndpoint("127.012312312.0.1", 12345);
    //link_code = tcp_server.Listen();
    //link_code = tcp_server.Accept();


    //// 向客户端发送欢迎消息
    //ZString welcome_msg = "Welcome to the server!\n";
    //link_code = tcp_server.Write(static_cast<const Void*>(welcome_msg.String()), welcome_msg.Size() + 1);

    //// 持续通信循环
    //for (;;) {
    //    // 接收客户端消息
    //    char data[1024] = { 0 };
    //    SizeType length;
    //    link_code = tcp_server.Read(static_cast<Void*>(data), 1024, &length);

    //    std::cout << "Received: " << data;

    //    // 回显消息给客户端
    //    link_code = tcp_server.Write(static_cast<const Void*>(data), length);

    //    if (tcp_server.State() == ZTCPServer::State_::ZTCPServerState_Listen) {
    //        link_code = tcp_server.Accept();
    //    }
    //}

    /**/
    SleepMs(kTimeBeforeProgramExit);
    return 0;
}
  