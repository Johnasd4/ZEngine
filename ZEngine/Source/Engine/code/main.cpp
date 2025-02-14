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
using namespace zengine::file_system;
using namespace zengine::console;
using namespace std;

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
    SetLanguage(kLanguageChinese);
    ZString a("1230000000000000000000000000000000000000000000000000000000000000000000000000");
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%x", 3, 4, 5, 6, ~7);
    Z_LOG_ERROR(1, 2, L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_MESSAGE(L"Message...");
    Z_LOG_START(L"Start...");
    Z_LOG_PROCESS(L"Process 1...");
    Z_LOG_PROCESS(L"Process 2...");
    Z_LOG_PROCESS(L"Process 3...");
    Z_LOG_FINISH(L"Finish...");
    Z_LOG_FAILURE(L"Failure...");
    Z_LOG_SUCCESS(L"Success...");
    //TList<ZWString> file_list;
    //TList<ZFileInfo> file_info_list;
    //GetFilesByPath(L"J:/夸克网盘/J 4k JOJO的奇妙冒险/JOJO的奇妙冒险 S3 不灭钻石 [JOJO&UHA-WING&Kamigami&MAI.] [Ma10p_2160p]" ,  &file_list);
    //file_list.Sort([](ZWString& _str_1, ZWString& _str_2) {
    //    return _str_1 < _str_2;
    //});
    //GetFileInfoListByPathList(&file_list, &file_info_list);
    //Int32 season_num = 3;
    //Int32 episode_num = 1;
    //TWFixedString<1024> new_path;

    //for (auto path_info = file_info_list.Begin(); path_info != file_info_list.End(); ++path_info) {
    //    new_path.SetString(L"%ls/%d-%02d%ls", path_info->directory_.String(), season_num, episode_num, path_info->extension_.String());
    //    //Z_LOG_MESSAGE(L"path: %ls", new_path.String());
    //    //Z_LOG_MESSAGE(L"path: %ls", path_info->path_.String());
    //    //Z_LOG_MESSAGE(L"name: %ls", path_info->name_.String());
    //    //Z_LOG_MESSAGE(L"extension: %ls", path_info->extension_.String());
    //    //Z_LOG_MESSAGE(L"directory: %ls", path_info->directory_.String());
    //    RenameFileByPath(path_info->path_.String(), new_path.String());
    //    ++episode_num;
    //}

    


    //Test_000();
    //ZThread thread_1(TestThreadFunc);
    //Sleep(100);
    //ZThread thread_2(TestThreadFunc);
    //thread_1.Join();
    //thread_2.Join();
    ReturnType link_code = kOK;
    ZString TEST_STRING("123");
    //ZApplication app;
    ZWindow::SetVerticalSynchronization(1);
    ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenMode_Window);
    ZFrame test_frame("##frame1", GuiSize(500, 500), GuiPos(100, 200));
    //test_frame.SetBackgruondColour({ 1.0f, 0.0f, 0.0f, 1.0f });
    test_window.Add(&test_frame);
    //ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
    ZText test_text("123");
    //test_text.SetPos(GuiPos(50,50));
    test_frame.Add(&test_text);
    test_frame.Add(&test_text);
    ZInputText test_input_text("##1234");
    ZInputText test_input_text_2("##123");
    ZInputText test_input_text_3("##234");
    ZInputText test_input_text_4("##12345");
    test_input_text.SetInputTextFlag(ZInputText::kInputTextFlag_CallbackEdit, true);
    test_frame.Add(&test_input_text);
    test_frame.Add(&test_input_text_2);
    test_frame.Add(&test_input_text_3);
    test_frame.Add(&test_input_text_4);
    //test_frame_2.SetBackgruondColour({ 0.0f, 1.0f, 0.0f, 1.0f });
    //test_window.Add(&test_frame_2);
    //test_window.SetSize(GuiSize(1000,1000));
    //test_window.SetPos(GuiPos(100, 100));
    link_code = test_window.Execute();
    //app.AddWindow(&test_window);
    //ZWindow test_window2("Window2", GuiSize(100, 100), GuiPos(100, 100), ZWindow::kWindowScreenMode_Window);
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



    /**/
    Sleep(1000);
    return 0;
}
  