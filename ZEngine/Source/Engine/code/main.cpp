/*
    Copyright (c) YuLin Zhu (朱雨林)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (朱雨林)
    Contact: 1152325286@qq.com
*/
#define PROJECT_NAME L"Include"

#include "z_engine.h"
#include "z_gui/test.h"

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

using namespace zengine;
using namespace zengine::gui;
using namespace std;

Void TestThreadFunc() {
    ReturnType link_code = kOK;
    //ZApplication app;
    ZWindow::SetVerticalSynchronization(1);
    ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenModeWindow);
    ZFrame test_frame("frame1", GuiSize(500, 500), GuiPos(100, 200));
    test_frame.SetBackgruondColour({ 1.0f, 0.0f, 0.0f, 1.0f });
    test_window.Add(&test_frame);
    ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
    test_frame_2.SetBackgruondColour({ 0.0f, 1.0f, 0.0f, 1.0f });
    ZButton test_button("button1", GuiSize(200, 50), GuiPos(0, 50));
    //test_window.Add(&test_frame_2);  
    test_frame.Add(&test_frame_2);
    test_frame_2.Add(&test_button);
    //test_window.Hide();
    link_code = test_window.Execute();
    //app.AddWindow(&test_window2);
    //link_code = app.Execute();
}

//Int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
Int32 main() {

    TVector<Float64> factors = { -0.0000485636136383, 0.0004692812857553, 0.0005043464816358, -0.0008932405433630, -0.0007136956208987, 0.0008609541181935 };
    Float64 ans =
        factors[0]
        - factors[1]
        + factors[2]
        - factors[3]
        + factors[4]
        - factors[5];
    Float64 start_pos = 0.0;
    Float64 end_pos = ans;
    Float64 time = 1.5028;
    Float64 vel = (end_pos - start_pos) / time;
    Float64 act_start_pos = -0.001095629646;
    Float64 act_end_pos = -0.001184739755;
    Float64 act_time = 376 * 0.0004 * 10;
    Float64 act_vel = (act_end_pos - act_start_pos) / act_time;

    Z_LOG_MESSAGE(L"start_pos %lf", start_pos);
    Z_LOG_MESSAGE(L"end_pos %lf", end_pos);
    Z_LOG_MESSAGE(L"time %lf", time);
    Z_LOG_MESSAGE(L"vel %lf", vel);
    Z_LOG_MESSAGE(L"act_start_pos %lf", act_start_pos);
    Z_LOG_MESSAGE(L"act_end_pos %lf", act_end_pos);
    Z_LOG_MESSAGE(L"act_time %lf", act_time);
    Z_LOG_MESSAGE(L"act_vel %lf", act_vel);


    //ReturnType link_code = kOK;
    //ZString TEST_STRING("123");
    //Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%x", 3, 4, 5, 6, ~7);
    //Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    //Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    //Z_LOG_MESSAGE(L"Message...");
    //Z_LOG_START(L"Start...");
    //Z_LOG_PROCESS(L"Process 1...");
    //Z_LOG_PROCESS(L"Process 2...");
    //Z_LOG_PROCESS(L"Process 3...");
    //Z_LOG_FINISH(L"Finish...");
    //Z_LOG_SUCCESS(L"Success...");
    //Z_LOG_FAILURE(L"Failure...");
    //ZWindow::SetVerticalSynchronization(1);
    //ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenModeWindow);
    //ZFrame test_frame("frame1", GuiSize(500, 500), GuiPos(100, 200));
    //test_frame.SetBackgruondColour({ 1.0f, 0.0f, 0.0f, 1.0f });
    //test_window.Add(&test_frame);
    //test_window.BindMoveEvent([](ZGuiAdjustableObject* _this_ptr, GuiPos _pre_pos, GuiPos cur_pos) {
    //    std::cout << cur_pos.x_ << " " << cur_pos.y_ << endl;
    //});
    //ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
    //test_frame_2.SetBackgruondColour({ 0.0f, 1.0f, 0.0f, 1.0f });
    //test_window.Add(&test_frame_2);
    ////test_window.SetSize(GuiSize(1000,1000));
    ////test_window.SetPos(GuiPos(100, 100));
    //link_code = test_window.Execute();
    ////app.AddWindow(&test_window);
    //ZWindow test_window2("Window2", GuiSize(100, 100), GuiPos(100, 100), ZWindow::kWindowScreenModeWindow);
    //test_window2.SetSize(GuiSize(1000, 1000));
    //test_window2.SetPos(GuiPos(1000, 1000));
    //test_window2.SetBackgruondColour(GuiColour(1, 1, 1, 1));
    //link_code = test_window2.Execute();
    //app.AddWindow(&test_window2);
    //link_code = app.Execute();
    //WSADATA wsaData;
    //SOCKET serverSocket;
    //sockaddr_in serverAddr, clientAddr;
    //int clientAddrSize = sizeof(clientAddr);
    //char buffer[BUFFER_SIZE];
    //int recvLen;

    //// 初始化Winsock
    //if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    //    std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
    //    return 1;
    //}

    //// 创建UDP套接字
    //if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    //    std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
    //    WSACleanup();
    //    return 1;
    //}

    //// 设置服务器地址
    //serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = INADDR_ANY;
    //serverAddr.sin_port = htons(SERVER_PORT);

    //// 绑定套接字
    //if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
    //    std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //std::cout << "Server is running on port " << SERVER_PORT << std::endl;

    //// 接收数据
    //while (true) {
    //    recvLen = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
    //    if (recvLen == SOCKET_ERROR) {
    //        std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
    //        break;
    //    }
    //    buffer[recvLen] = '\0';
    //    std::cout << "Received message: " << buffer << std::endl;
    //}

    //// 清理
    //closesocket(serverSocket);
    //WSACleanup();
    
    //gui::Test();

    Sleep(50);



    return 0;
}
  