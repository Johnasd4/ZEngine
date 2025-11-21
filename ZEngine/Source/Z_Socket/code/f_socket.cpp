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
#define SOCKET_DLLFILE

#include "f_socket.h"

#include "../z_core/z_string.h"

#include "z_buffer.h"
#include "z_buffer_stream.h"
#include "z_http_request_generator.h"
#include "z_http_response_resolver.h"
#include "z_io_context.h"
#include "z_tcp_client.h"
#include "z_tls_context.h"
#include "z_tls_stream.h"
#include "z_url_resolver.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
using ssl_socket = ssl::stream<tcp::socket>;

namespace zengine {
namespace socket {

using boost::asio::ip::tcp;

using boost::asio::ip::udp;
using namespace std::chrono_literals;

// 中国大陆最稳定的 STUN 服务器列表（2025 年实测）
const std::vector<std::pair<std::string, uint16_t>> STUN_SERVERS = {
    {"stun.12voip.com", 3478},
    {"stun.aa.net.uk", 3478},
    {"stun.acrobits.cz", 3478},
    {"stun.actionvoip.com", 3478},
    {"stun.annatel.net", 3478},
    {"stun.antisip.com", 3478},
};

struct PublicEP { std::string ip; uint16_t port; };

PublicEP get_public_ip_port() {
    boost::asio::io_context io;
    udp::socket sock(io);
    sock.open(udp::v4());

    for (const auto& [host, port] : STUN_SERVERS) {
        try {
            std::cout << "正在尝试 " << host << ":" << port << " ... ";

            udp::resolver resolver(io);
            udp::endpoint stun_ep = *resolver.resolve(host, std::to_string(port)).begin();

            // 最简 Binding Request (20 字节，兼容所有服务器)
            std::array<uint8_t, 20> req{ {
                0x00, 0x01, 0x00, 0x00,
                0x21, 0x12, 0xA4, 0x42,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00
            } };

            sock.send_to(boost::asio::buffer(req), stun_ep);

            std::array<uint8_t, 512> buf{};
            udp::endpoint from;
            size_t len = sock.receive_from(boost::asio::buffer(buf), from);

            if (len >= 28 && buf[0] == 0x01 && buf[1] == 0x01) {
                size_t i = 20;
                while (i + 8 < len) {
                    uint16_t type = (buf[i] << 8) | buf[i + 1];
                    uint16_t length = (buf[i + 2] << 8) | buf[i + 3];
                    if (type == 0x0020 && length >= 8 && buf[i + 4] == 0 && buf[i + 5] == 1) {  // XOR-MAPPED-ADDRESS IPv4
                        uint16_t xport = (buf[i + 6] << 8) | buf[i + 7];
                        xport ^= 0x2112;
                        uint32_t xip = (uint32_t(buf[i + 8]) << 24) | (uint32_t(buf[i + 9]) << 16) |
                            (uint32_t(buf[i + 10]) << 8) | buf[i + 11];
                        xip ^= 0x2112A442;

                        boost::asio::ip::address_v4 ip(xip);
                        std::cout << "成功！" << std::endl;
                        return { ip.to_string(), xport };
                    }
                    i += 4 + ((length + 3) & ~3);
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "失败 (" << e.what() << ")" << std::endl;
        }
    }
    throw std::runtime_error("所有服务器都失败了（极小概率）");
}

ReturnType get_public_ip_and_port() {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZIOContext io_context;
    ZTCPClient client(&io_context);
    ZTLSContext tls_context(TLSTypeEnum::kTLSType_Client);
    link_code = tls_context.SetVerifyMode(kTLSVerifyType_Peer);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTLSContext::LoadSystemVerifyFiles() link error!"
        );
        return ret_val;
    }
    link_code = tls_context.LoadSystemVerifyFiles();
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTLSContext::LoadSystemVerifyFiles() link error!"
        );
        return ret_val;
    }

    link_code = client.BindEndpoint("192.168.1.1",10000);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPClient::BindEndpoint() link error!"
        );
        return ret_val;
    }

    //generate request
    ZHTTP11RequestGenerator http_generator;
    http_generator.SetRequestType(ZHTTP11RequestGenerator::RequestType_Get);
    http_generator.SetTarget("/");
    http_generator.SetHost("ifconfig.me");
    http_generator.SetConnection("close");
    http_generator.SetAccept("*/*");
    http_generator.SetUserAgent("test_user");

    //http_generator.SetRequestType(ZHTTP11RequestGenerator::RequestType_Get);
    //http_generator.SetTarget("/");
    //http_generator.SetHost("183.192.65.101");
    //http_generator.SetConnection("close");
    //http_generator.SetAccept("*/*");
    //http_generator.SetUserAgent("test_user");

    ZString request = http_generator.GenerateString();

    Z_LOG_MESSAGE(
        L"1\n%ls",
        string::String2WString(request.String()).String()
    );

    //connect
    link_code = client.Connect("ifconfig.me", "https");
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Connect() link error!"
        );
        return ret_val;
    }

    ZTLSStream tls_stream(&client.GetSocket(), &tls_context);
    link_code = tls_stream.Handshake();
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Handshake() link error!"
        );
        return ret_val;
    }


    //send request
    link_code = client.Write(ZConstBuffer(request.String(), request.Size()));
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Write() link error!"
        );
        return ret_val;
    }

    ZBufferStream buffer_stream(5000);

    //read head
    SizeType size;
    link_code = client.ReadUntil(&buffer_stream, "\r\n\r\n", &size);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::ReadUntil() link error!"
        );
        return ret_val;
    }

    //link_code = client.ReadUntilClose(&buffer_stream, &size);
    //if (link_code != kOK) {
    //    ret_val = error_code::kFSocketErrorCode_LinkError;
    //    Z_LOG_ERROR(
    //        ret_val, link_code,
    //        L"ZTCPSocket::ReadUntilClose() link error!"
    //    );
    //    return ret_val;
    //}

    //reslove head buffer
    ZConstBuffer head_buffer = buffer_stream.ReadData(size);
    ZHTTP11ResponseResolver http_resover;
    link_code = http_resover.Resolve(head_buffer);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZHTTP11ResponseResolver::Resolve() link error!"
        );
        return ret_val;
    }
    ZString buffer_string(head_buffer.BufferPtr<const Char*>(), head_buffer.Size());
    Z_PRINT(
        "2\n%s",
        buffer_string.String()
    );

    //dump head info
    buffer_stream.DumpData(size);

    //302 Found
    if (http_resover.GetResult() == 302U) {
        link_code = client.Close();
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPClient::Close() link error!"
            );
            return ret_val;
        }

        client.BindEndpoint("192.168.1.1", 10000);


        ZStringView location_string_view;
        link_code = http_resover.GetStringView(&location_string_view, "Location");
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZHTTP11ResponseResolver::GetStringView() link error!"
            );
            return ret_val;
        }
        
        ZURLResolver url_resolver;
        link_code = url_resolver.Resolve(location_string_view);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZURLResolver::Resolve() link error!"
            );
            return ret_val;
        }

        ZString host_string = url_resolver.GetHost();
        ZString port_string = url_resolver.GetPort();

        Z_LOG_MESSAGE(
            L"host %ls", 
            string::String2WString(host_string.String()).String()
        );
        Z_LOG_MESSAGE(
            L"port %ls",
            string::String2WString(port_string.String()).String()
        );

        http_generator.SetTarget("/");
        http_generator.SetHost("183.192.65.101");
        //http_generator.SetHost(host_string.String());

        ZString found_request = http_generator.GenerateString();

        Z_LOG_MESSAGE(
            L"3\n%ls",
            string::String2WString(found_request.String()).String()
        );

        link_code = client.Connect(host_string.String(), port_string.String());
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZClient::Connect() link error!"
            );
            return ret_val;
        }

        //send request
        link_code = client.Write(ZConstBuffer(found_request.String(), found_request.Size()));
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::Write() link error!"
            );
            return ret_val;
        }

        buffer_stream.Clear();

        //read head
        SizeType size;
        link_code = client.ReadUntil(&buffer_stream, "\r\n\r\n", &size);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZTCPSocket::ReadUntil() link error!"
            );
            return ret_val;
        }

        //reslove head buffer
        ZConstBuffer head_buffer = buffer_stream.ReadData(buffer_stream.Size());
        buffer_string.Assign(head_buffer.BufferPtr<const Char*>(), size);
        Z_LOG_MESSAGE(
            L"4 %d \n%ls",
            buffer_stream.Size(),
            string::String2WString(buffer_string.String()).String()
        );

        ZHTTP11ResponseResolver http_resover;
        link_code = http_resover.Resolve(head_buffer);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZHTTP11ResponseResolver::http_resover() link error!"
            );
            return ret_val;
        }

    }

    ////read json string
    //link_code = client.ReadUntil(&buffer_stream, "\r\n\r\n", &size);
    //if (link_code != kOK) {
    //    ret_val = error_code::kFSocketErrorCode_LinkError;
    //    Z_LOG_ERROR(
    //        ret_val, link_code,
    //        L"ZTCPSocket::ReadUntil() link error!"
    //    );
    //    return ret_val;
    //}

    ////disconnect
    //link_code = client.Close();
    //if (link_code != kOK) {
    //    ret_val = error_code::kFSocketErrorCode_LinkError;
    //    Z_LOG_ERROR(
    //        ret_val, link_code,
    //        L"ZTCPSocket::Close() link error!"
    //    );
    //    return ret_val;
    //}

    //ZConstBuffer buffer = buffer_stream.ReadData(buffer_stream.Size());

    //http_resover.Resolve(buffer);


    //ZJsonDocument json_doc;
    //json_doc.Parse(buffer.BufferPtr<Char*>());

    //Z_LOG_MESSAGE(
    //    L"%ls %d",
    //    string::String2WString(buffer.BufferPtr<const Char*>()).String(),
    //    buffer.Size()
    //);

    //clear stream
    buffer_stream.Clear();

    return ret_val;
}

/*
    Get current udp public ip and port.
*/
SOCKET_DLLAPI ReturnType GetPublicUDPIPAndPort(
    const Char* _local_udp_address,
    Int32 _local_udp_port,
    ZString* _public_udp_address_ptr,
    Int32* _public_udp_port_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    try {
        auto ep = get_public_ip_port();
        std::cout << "公网 IP   : " << ep.ip << std::endl;
        std::cout << "公网端口  : " << ep.port << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "最终失败: " << e.what() << std::endl;
    }
    get_public_ip_and_port();
    //auto network_info = get_public_ip_and_port();

    //if (!network_info.empty()) {
    //    std::cout << "\n=== 网络信息解析结果 ===" << std::endl;
    //    // 打印所有获取到的信息
    //    for (const auto& pair : network_info) {
    //        std::cout << pair.first << ": " << pair.second << std::endl;
    //    }

    //    // 特别输出IP地址
    //    if (network_info.find("ip_addr") != network_info.end()) {
    //        std::cout << "\n>>> 你的公网IP地址是: " << network_info["ip_addr"] << std::endl;
    //    }
    //    else {
    //        std::cout << "\n>>> 未能解析到IP地址。" << std::endl;
    //    }

    //    // 特别输出端口信息 (注意：ifconfig.me/all 返回的通常是远程端口)
    //    if (network_info.find("remote_port") != network_info.end()) {
    //        std::cout << ">>> 远程端口是: " << network_info["remote_port"] << std::endl;
    //    }
    //    else {
    //        std::cout << ">>> 未能解析到远程端口信息。" << std::endl;
    //    }
    //}
    //else {
    //    std::cout << "未能获取到网络信息。" << std::endl;
    //}

    return ret_val;
}

}//socket
}//zengine