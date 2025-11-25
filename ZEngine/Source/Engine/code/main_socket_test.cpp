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

#include "zengine/z_engine.h"
//#include "zengine/z_gui/test.h"

using namespace zengine;
using namespace zengine::gui;
using namespace zengine::math;
using namespace zengine::file_system;
using namespace zengine::console;
using namespace zengine::tsrpg;
using namespace zengine::socket;
using namespace std;

//Void SocketServer() noexcept {
//    ZIOContext context;
//    ZTCPServer server(&context);
//    ReturnType link_code = kOK;
//    link_code = server.BindEndpoint("127.0.0.1", 10000);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::BindEndpoint link error!");
//        return;
//    }
//    link_code = server.Listen();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::Listen link error!");
//        return;
//    }
//    link_code = server.Accept();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::Accept link error!");
//        return;
//    }
//
//    ZTLSContext tls_context(TLSTypeEnum::kTLSType_Server);
//    link_code = tls_context.SetVerifyMode(TLSVerifyModeEnum::kTLSVerifyType_None);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::SetVerifyMode link error!");
//        return;
//    }
//    link_code = tls_context.UseCertificateFile(
//        "H:\\HTTPS\\server.crt", 
//        CertificateFileFormatTypeEnum::kCertificateFileFormatTypeType_PEM 
//    );
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::UseCertificateFile link error!");
//        return;
//    }
//    link_code = tls_context.UsePrivateKeyFile(
//        "H:\\HTTPS\\server.key",
//        CertificateFileFormatTypeEnum::kCertificateFileFormatTypeType_PEM
//    );
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::UsePrivateKeyFile link error!");
//        return;
//    }
//    ZTLSStream stream(&server.GetSocket(), &tls_context);
//    link_code = stream.Handshake();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::Handshake link error!");
//        return;
//    }
//
//    while (true) {      
//        TFixedMemory<1024> data;
//        SizeType length;
//        link_code = stream.Read(data.DataPtr<Void*>(), 1024, &length);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, link_code, L"ZTLSStream::Read link error!");
//            break;
//        }
//        Z_LOG_MESSAGE(L"Server(%d): %ls", length, data);
//    }
//}
//Void SocketClient() noexcept {
//    ZIOContext io_context;
//    ZTCPClient client(&io_context);
//    ReturnType link_code = kOK;
//    link_code = client.Connect("127.0.0.1", "10000");
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPClient::Connect link error!");
//        return;
//    }
//    ZTLSContext tls_context(TLSTypeEnum::kTLSType_Client);
//    link_code = tls_context.SetVerifyMode(TLSVerifyModeEnum::kTLSVerifyType_None);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::SetVerifyMode link error!");
//        return;
//    }
//    ZTLSStream stream(&client.GetSocket(), &tls_context);
//    Sleep(3000);
//    link_code = stream.Handshake();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::Handshake link error!");
//        return;
//    }
//
//    SizeType index = 0;
//    while (index < 10) {
//        TFixedWString<1024> data;
//        data.SetString(L"This is a client message: %u", ++index);
//        link_code = stream.Write(data.DataPtr(), wcslen(data.DataPtr()) * 2 + 2);
//        if (link_code != kOK) {
//            Z_LOG_ERROR(0, link_code, L"ZTLSStream::Write link error!");
//            break;
//        }
//        SleepMs(1000);
//    }
//
//    //SizeType index = 0;
//    //while (index < 10) {
//    //    TFixedWString<1024> data;
//    //    data.SetString(L"This is a client message: %u", ++index);
//    //    link_code = client.Write(data.DataPtr(), wcslen(data.DataPtr()) * 2 + 2);
//    //    if (link_code != kOK) {
//    //        Z_LOG_ERROR(0, link_code, L"ZTCPClient::Write link error!");
//    //        break;
//    //    }
//    //    SleepMs(1000);
//    //}
//}
//
//Void AysncSocketServer() noexcept {
//    ZIOContext io_context;
//    ZTCPServer server(&io_context);
//    ReturnType link_code = kOK;
//    link_code = server.BindEndpoint("127.0.0.1", 10000);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::BindEndpoint link error!");
//        return;
//    }
//    link_code = server.Listen();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::Listen link error!");
//        return;
//    }
//    link_code = server.Accept();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPServer::Accept link error!");
//        return;
//    }
//
//    ZTLSContext tls_context(TLSTypeEnum::kTLSType_Server);
//    link_code = tls_context.SetVerifyMode(TLSVerifyModeEnum::kTLSVerifyType_None);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::SetVerifyMode link error!");
//        return;
//    }
//    link_code = tls_context.UseCertificateFile(
//        "H:\\HTTPS\\server.crt",
//        CertificateFileFormatTypeEnum::kCertificateFileFormatTypeType_PEM
//    );
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::UseCertificateFile link error!");
//        return;
//    }
//    link_code = tls_context.UsePrivateKeyFile(
//        "H:\\HTTPS\\server.key",
//        CertificateFileFormatTypeEnum::kCertificateFileFormatTypeType_PEM
//    );
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::UsePrivateKeyFile link error!");
//        return;
//    }
//
//    TFixedMemory<1024> data;
//
//    TFunction<Void(ZTLSStream*, const Void*, SizeType)> read_handle_func =
//        [&data, &read_handle_func](ZTLSStream* _stream_ptr, const Void* _buffer_ptr, SizeType _size) {
//            ReturnType link_code = kOK;
//            Z_LOG_MESSAGE(L"Server(%d): %ls", _size, static_cast<const WChar*>(_buffer_ptr));
//            link_code = _stream_ptr->AsyncRead(data.DataPtr<Void*>(), 1024, read_handle_func);
//            if (link_code != kOK) {
//                Z_LOG_ERROR(0, link_code, L"ZTLSStream::AsyncRead link error!");
//                return;
//            }
//        };
//
//    ZTLSStream stream(&server.GetSocket(), &tls_context);
//    link_code = stream.AsyncHandshake(
//        [&data, &read_handle_func](ZTLSStream* _stream_ptr, Bool _success) {
//            if (!_success) {
//                Z_LOG_ERROR(0, 0, L"ZTLSStream::AsyncHandshake error!");
//                return;
//            }
//
//            ReturnType link_code = kOK;
//            link_code = _stream_ptr->AsyncRead(data.DataPtr<Void*>(), 1024, read_handle_func);
//            if (link_code != kOK) {
//                Z_LOG_ERROR(0, link_code, L"ZTLSStream::Read link error!");
//                return;
//            }
//        }
//    );
//
//    link_code = io_context.Run();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZIOContext::Run link error!");
//        return;
//    }
//}
//Void AysncSocketClient() noexcept {
//    ZIOContext io_context;
//    ZTCPClient client(&io_context);
//    ReturnType link_code = kOK;
//    link_code = client.Connect("127.0.0.1", "10000");
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTCPClient::Connect link error!");
//        return;
//    }
//    ZTLSContext tls_context(TLSTypeEnum::kTLSType_Client);
//    link_code = tls_context.SetVerifyMode(TLSVerifyModeEnum::kTLSVerifyType_None);
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZTLSStream::SetVerifyMode link error!");
//        return;
//    }
//
//    TFixedWString<1024> data;
//
//    SizeType index = 0;
//    TFunction<Void(ZTLSStream*, const Void*, SizeType)> write_handle_func =
//        [&data, &write_handle_func, &index](ZTLSStream* _stream_ptr, const Void* _buffer_ptr, SizeType _size) {
//            if (index == 10) {
//                return;
//            }
//            ReturnType link_code = kOK;
//            //SleepMs(1000);
//            data.SetString(L"This is a client message: %u", ++index);
//            link_code = _stream_ptr->AsyncWrite(data.DataPtr(), wcslen(data.DataPtr()) * 2 + 2, write_handle_func);
//            if (link_code != kOK) {
//                Z_LOG_ERROR(0, link_code, L"ZTLSStream::AsyncWrite link error!");
//                return;
//            }
//        };
//
//    ZTLSStream stream(&client.GetSocket(), &tls_context);
//    link_code = stream.AsyncHandshake(
//        [&data, &write_handle_func, &index](ZTLSStream* _stream_ptr, Bool _success) {
//            if (!_success) {
//                Z_LOG_ERROR(0, 0, L"ZTLSStream::AsyncHandshake error!");
//                return;
//            }
//
//            ReturnType link_code = kOK;
//            data.SetString(L"This is a client message: %u", ++index);
//            link_code = _stream_ptr->AsyncWrite(data.DataPtr(), wcslen(data.DataPtr()) * 2 + 2, write_handle_func);
//            if (link_code != kOK) {
//                Z_LOG_ERROR(0, link_code, L"ZTLSStream::AsyncWrite link error!");
//                return;
//            }
//        }
//    );
//
//    link_code = io_context.Run();
//    if (link_code != kOK) {
//        Z_LOG_ERROR(0, link_code, L"ZIOContext::Run link error!");
//        return;
//    }
//}

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


    ReturnType link_code = kOK;

    //GetPublicIP(nullptr);
    ZWString test_str(nullptr, 0);


    std::string a = "123";
    ZStringView view = a;

    GetUDPPublicAddressAndPort("", 1, nullptr, nullptr);

    //ZThread server_thread(SocketServer);
    //ZThread client_thread(SocketClient);

    //ZThread server_thread(AysncSocketServer);
    //ZThread client_thread(AysncSocketClient);

    //server_thread.Join();
    //client_thread.Join();

    /**/
    log::FinishFlush();
    return 0;
}
  