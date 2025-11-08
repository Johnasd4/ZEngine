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
////using namespace std;
//
//
//Int32 TestFunc() {
//    Z_LOG_MESSAGE(L"1");
//    return 1;  // 返回值数量
//}
//
//Void TestThreadFunc() {
//    ReturnType link_code = kOK;
//    //ZApplication app;
//    ZWindow::SetVerticalSynchronization(1);
//    ZWindow test_window("Window1", GuiSize(1000, 1000), GuiPos(100, 100), ZWindow::kWindowScreenMode_Window);
//    ZFrame test_frame("frame1", GuiSize(500, 500), GuiPos(100, 200));
//    test_frame.SetBackgruondColour({ 1.0f, 0.0f, 0.0f, 1.0f });
//    test_window.Add(&test_frame);
//    ZFrame test_frame_2("frame2", GuiSize(200, 200), GuiPos(100, 200));
//    test_frame_2.SetBackgruondColour({ 0.0f, 1.0f, 0.0f, 1.0f });
//    //test_window.Add(&test_frame_2);  
//    test_frame.Add(&test_frame_2);
//    //test_window.Hide();
//    link_code = test_window.Execute();
//    //app.AddWindow(&test_window2);
//    //link_code = app.Execute();
//}
//
//// 普通函数
//int add(int a, int b) { return a + b; }
//
//// 函数对象
//struct Multiply {
//    int operator()(int a, int b) const { return a * b; }
//};
//
//Int32 count = 0;
//// 函数对象
//class TestClass {
//public:
//    TestClass() {
//        a = count++;
//        Z_LOG_MESSAGE(L"Construct %d", a);
//    }
//    ~TestClass() {
//        Z_LOG_MESSAGE(L"Destruct %d", a);
//    }
//
//private:
//    Int32 a;
//};
//
//#include<mutex>
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
//    zengine::socket::StartLogOutputClient([](const TCPLogOutputReplyLogData* _data_ptr) {
//        Z_LOG_MESSAGE(L"%ls", _data_ptr->log_string_.String());
//    });
//
//    ReturnType link_code = kOK;
//
//    ZIOContext io_context;
//    TFixedMemory<1024> buffer;
//
//    ZUDPSocket udp_socket(&io_context);
//    while (true) {
//        wscanf(L"%1024ls", buffer.DataPtr<WChar*>());
//        link_code = udp_socket.SendTo(buffer.DataPtr<Void*>(), 1024, "127.0.0.1", 8080);
//    };
//
//    ZTCPMultipleSessionClient test_client(&io_context);
//    link_code = test_client.AsyncConnect("127.0.0.1", "8080", [](ZTCPMultipleSessionClient* a, ZTCPSocket* b){}, 5);
//    link_code = io_context.Run();
//
//
//    ZTCPMultipleSessionServer tcp_server(&io_context);
//
//    link_code = tcp_server.BindEndpoint("127.0.0.1", 8080);
//    link_code = tcp_server.Listen();
//    
//    TFunction<Void(ZTCPSocket*, const Void*, SizeType)> read_handle_func;
//    TFunction<Void(ZTCPSocket*, const Void*, SizeType)> write_handle_func;
//    write_handle_func = [](ZTCPSocket* _socket_ptr, const Void* _data_buffer, SizeType _write_length) {};
//    TFunction< Void(ZTCPMultipleSessionServer*, ZTCPSocket*)> accept_handle_func;
//    read_handle_func = [&buffer, &tcp_server, &write_handle_func, &read_handle_func](ZTCPSocket* _socket_ptr, const Void* _data_buffer, SizeType _write_length) {
//        ReturnType link_code = kOK;
//        TFixedWString<512> str(
//            L"Client %ls(%d): %ls",
//            string::String2WString(_socket_ptr->RemoteAddress().String()).String(),
//            _socket_ptr->RemotePort(),
//            static_cast<const WChar*>(_data_buffer)
//        );
//        Z_LOG_MESSAGE(L"%ls", str.String());
//        link_code = tcp_server.AsyncBroadcast(str.DataPtr(), 1024, write_handle_func);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, 0, L"AsyncRead() error!");
//        }
//        link_code = _socket_ptr->AsyncRead(buffer.DataPtr<Void*>(), 1024, read_handle_func);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, 0, L"AsyncRead() error!");
//        }
//    };
//    
//    accept_handle_func = [&accept_handle_func, &buffer, &read_handle_func](ZTCPMultipleSessionServer* _server_ptr, ZTCPSocket* _socket_ptr) {
//        ReturnType link_code = kOK;
//        link_code = _socket_ptr->AsyncRead(buffer.DataPtr<Void*>(), 1024, read_handle_func);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, 0, L"AsyncRead() error!");
//        }
//        link_code = _server_ptr->AsyncAccept(accept_handle_func);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, 0, L"AsyncAccept() error!");
//        }
//    };
//
//    link_code = tcp_server.AsyncAccept(accept_handle_func);
//
//    link_code = io_context.Run();
//
//    while (true) {
//
//
//
//
//
//
//        //while (tcp_server.State() == ZTCPSingleSessionServer::ZTCPSingleSessionServerState_Connect) {
//        //    wscanf(L"%1024ls", buffer.DataPtr<WChar*>());
//        //    link_code = tcp_server.AsyncWrite(buffer.DataPtr<Void*>(), 1024, write_handle_func);
//        //    if (link_code != kOK) {
//        //        if (link_code != error_code::kPSocketErrorCode_Disconnected) {
//        //            Z_LOG_ERROR(0, 0, L"AsyncWrite() error!");
//        //        }
//        //        break;
//        //    }
//        //}
//
//        if (tcp_server.State() != ZTCPSingleSessionServer::ZTCPSingleSessionServerState_Listen) {
//            break;
//        }
//
//        //if (link_code != error_code::kPSocketErrorCode_Disconnected) {
//        //    break;
//        //}
//
//    }
//
//
//    //TFixedMemory<1024> buffer;
//    //ZTCPSingleSessionServer tcp_server;
//
//    //link_code = tcp_server.SetEndpoint("127.0.0.1", 8080);
//    //link_code = tcp_server.Listen();
//
//    //while (true) {
//    //    link_code = tcp_server.Accept();
//
//    //    TFunction<Void(Void*, SizeType)> read_handle_func;
//    //    TFunction<Void(Void*, SizeType)> write_handle_func;
//    //    read_handle_func = [&tcp_server, &buffer, &read_handle_func](Void* _data_buffer, SizeType _write_length) {
//    //        ReturnType link_code = kOK;
//    //        Z_LOG_MESSAGE(L"Client: %ls", static_cast<WChar*>(_data_buffer));
//    //        link_code = tcp_server.AsyncRead(buffer.DataPtr<Void*>(), 1024, read_handle_func);
//    //        if (link_code != kOK) {
//    //            Z_LOG_ERROR(0, 0, L"AsyncRead() error!");
//    //        }
//    //        };
//    //    write_handle_func = [&tcp_server, &buffer, &write_handle_func](Void* _data_buffer, SizeType _write_length) {
//    //        ReturnType link_code = kOK;
//    //        Z_LOG_MESSAGE(L"Send: %ls", static_cast<WChar*>(_data_buffer));
//    //        };
//
//    //    link_code = tcp_server.AsyncRead(buffer.DataPtr<Void*>(), 1024, read_handle_func);
//    //    if (link_code != kOK) {
//    //        Z_LOG_ERROR(0, 0, L"AsyncRead() error!");
//    //    }
//
//    //    link_code = tcp_server.Run();
//    //    //while (tcp_server.State() == ZTCPSingleSessionServer::ZTCPSingleSessionServerState_Connect) {
//    //    //    wscanf(L"%1024ls", buffer.DataPtr<WChar*>());
//    //    //    link_code = tcp_server.AsyncWrite(buffer.DataPtr<Void*>(), 1024, write_handle_func);
//    //    //    if (link_code != kOK) {
//    //    //        if (link_code != error_code::kPSocketErrorCode_Disconnected) {
//    //    //            Z_LOG_ERROR(0, 0, L"AsyncWrite() error!");
//    //    //        }
//    //    //        break;
//    //    //    }
//    //    //}
//
//    //    if (tcp_server.State() != ZTCPSingleSessionServer::ZTCPSingleSessionServerState_Listen) {
//    //        break;
//    //    }
//
//    //    //if (link_code != error_code::kPSocketErrorCode_Disconnected) {
//    //    //    break;
//    //    //}
//
//    //}
//
//
//
//    // 
//    //tcp_server->Write(&buffer, 1024);
//    //Int32 temp_int = 10;
//    //tcp_server.Write(&temp_int, 4);
//
//    //ZTCPClient tcp_client;
//    //link_code = tcp_client.SetEndpoints("zyl4090.f1.luyouxia.net:19216");
//    //link_code = tcp_client.Connect();
//
//    //Int32 temp_int = 10;
//    //tcp_client.Write(&temp_int, 4);
//
//
//    //link_code = StartLogOutputServer();
//    //if (link_code != kOK) {
//    //    Z_LOG_ERROR(
//    //        0, link_code,
//    //        L"socket::StartLogOutputServer() link error!"
//    //    );
//    //    return 0;
//    //}
//
//    //Int32 index = 0;
//    //while (true) {
//    //    Z_LOG_MESSAGE(L"Message... %d", index++);
//    //    Sleep(1000);
//    //}
//
//    //link_code = StopLogOutputServer();
//
//    //ZTCPClient tcp_client;
//    //link_code = tcp_client.SetEndpoint("127.0.0.1", 12345);
//    //link_code = tcp_client.Connect(3);
//    //if (link_code == kOK) {
//    //    for (;;) {
//    //        // 接收客户端消息
//    //        char data[1024] = { 0 };
//    //        SizeType length;
//    //        link_code = tcp_client.Read(static_cast<Void*>(data), 1024, &length);
//
//    //        std::cout << "Received: " << data;
//
//    //        std::string message;
//    //        std::getline(std::cin, message);
//
//    //        // 回显消息给客户端
//    //        link_code = tcp_client.Write(static_cast<const Void*>(message.c_str()), message.length() + 1);
//
//    //        if (tcp_client.State() != ZTCPClient::State_::ZTCPClientState_Connect) {
//    //            break;
//    //        }
//    //    }
//    //}
//     
//    //ZConfig z_config(L"config\\test.ini");
//    //z_config.AddMember("test", 123);
//    ////z_config.SetMemberValue("test", 456);
//    //TFixedMemory<123> fixed_memory;
//    //fixed_memory.At<Int32>(10) = 10;
//    //Z_LOG_MESSAGE(L"%d", fixed_memory.At<Int32>(9));
//    //ZJsonDocument json;
//    //json["12"] = 123;
//    //json["123"] = 123;
//    //json.AddMember("123", 4321, false);
//    //Z_LOG_MESSAGE(L"%ls", string::String2WString(json.GenerateJsonString().String()).String());
//    //ZTCPServer tcp_server;
//    //link_code = tcp_server.SetEndpoint("127.0.0.1", 12345);
//    ////link_code = tcp_server.SetEndpoint("127.012312312.0.1", 12345);
//    //link_code = tcp_server.Listen();
//    //link_code = tcp_server.Accept();
//
//
//    ////// 向客户端发送欢迎消息
//    ////ZString welcome_msg = "Welcome to the server!\n";
//    ////link_code = tcp_server.Write(static_cast<const Void*>(welcome_msg.String()), welcome_msg.Size() + 1);
//
//    //// 持续通信循环
//    //for (;;) {
//    //    // 接收客户端消息
//    //    char data[1024] = { 0 };
//    //    SizeType length;
//    //    link_code = tcp_server.Read(static_cast<Void*>(data), 1024, &length);
//
//    //    std::cout << "Received: " << data;
//
//    //    //// 回显消息给客户端
//    //    //link_code = tcp_server.Write(static_cast<const Void*>(data), length);
//
//    //    if (tcp_server.State() == ZTCPServer::State_::ZTCPServerState_Listen) {
//    //        link_code = tcp_server.Accept();
//    //    }
//    //}
//
//    /**/
//    SleepMs(kTimeBeforeProgramExit);
//    return 0;
//}
//  