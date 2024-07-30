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

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

using namespace zengine;
using namespace zengine::gui;
using namespace std;

int main() {
    ReturnType link_code = kOK;
    ZString TEST_STRING("123");
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%x", 3, 4, 5, 6, ~7);
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_MESSAGE(L"Message...");
    Z_LOG_START(L"Start...");
    Z_LOG_PROCESS(L"Process 1...");
    Z_LOG_PROCESS(L"Process 2...");
    Z_LOG_PROCESS(L"Process 3...");
    Z_LOG_FINISH(L"Finish...");
    Z_LOG_SUCCESS(L"Success...");
    Z_LOG_FAILURE(L"Failure...");
    ZApplication app;
    ZWindow::SetVerticalSynchronization(1);
    ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenModeWindow);
    ZFrame test_frame("frame1", GuiSize(500, 500), GuiPos(100, 200));
    test_window.Add(&test_frame);
    ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
    test_frame.Add(&test_frame_2);
    //test_window.SetSize(GuiSize(1000,1000));
    //test_window.SetPos(GuiPos(100, 100));
    app.AddWindow(&test_window);
    //ZWindow test_window2("Window2", GuiSize(100, 100), GuiPos(100, 100), ZWindow::kWindowScreenModeWindow);
    //test_window2.SetSize(GuiSize(1000, 1000));
    //test_window2.SetPos(GuiPos(1000, 1000));
    //test_window2.SetBackgruondColour(GuiColour(1, 1, 1, 1));
    //app.AddWindow(&test_window2);
    link_code = app.Execute();
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
