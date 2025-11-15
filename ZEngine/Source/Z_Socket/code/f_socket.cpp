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

#include "../z_core/z_json.h"
#include "../z_core/z_memory.h"
#include "../z_core/z_string.h"

#include "z_buffer.h"
#include "z_buffer_stream.h"
#include "z_io_context.h"
#include "z_tcp_client.h"


#include <boost/asio.hpp>

#include <map>

namespace zengine {
namespace socket {

ReturnType get_public_ip_and_port() {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZIOContext io_context;
    ZTCPClient client(&io_context);

    //connect
    link_code = client.Connect("ifconfig.me", "80");
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Connect() link error!"
        );
        return ret_val;
    }

    //send request
    static constexpr Char request[] =
        "GET /all.json HTTP/1.1\r\n"
        "Host: ifconfig.me\r\n"
        "Connection: close\r\n"
        "\r\n";
    link_code = client.Write(ZConstBuffer(request, sizeof(request)));
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Write() link error!"
        );
        return ret_val;
    }

    ZBufferStream buffer_stream(500);

    //read head
    SizeType size;
    link_code = client.ReadUntil(&buffer_stream, "\r\n\r\n", &size);
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::ReadUntil() link error!"
        );
        return ret_val;
    }

    Z_LOG_MESSAGE(
        L"%d",
        size
    );
    //dump head info
    //buffer_stream.DumpData(size);

    //read json string
    link_code = client.ReadUntil(&buffer_stream, '}', &size);
    //if (link_code != kOK) {
    //    ret_val = error_code::kPSocketErrorCode_LinkError;
    //    Z_LOG_ERROR(
    //        ret_val, link_code,
    //        L"ZTCPSocket::ReadUntil() link error!"
    //    );
    //    return ret_val;
    //}

    Z_LOG_MESSAGE(
        L"%d",
        size
    );

    //disconnect
    link_code = client.Close();
    if (link_code != kOK) {
        ret_val = error_code::kPSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZTCPSocket::Close() link error!"
        );
        return ret_val;
    }

    ZBuffer buffer = buffer_stream.ReadData(buffer_stream.Size());

    ZJsonDocument json_doc;
    //json_doc.Parse(buffer.BufferPtr<Char*>());

    Z_LOG_MESSAGE(
        L"%ls %d",
        string::String2WString(buffer.BufferPtr<const Char*>()).String(),
        buffer.Size()
    );

    //clear stream
    buffer_stream.Clear();

    //namespace asio = boost::asio;
    //using asio::ip::tcp;

    //std::map<std::string, std::string> result;
    //asio::io_context io_ctx;

    //try {
    //    // 解析主机地址和端口
    //    tcp::resolver resolver(io_ctx);
    //    auto endpoints = resolver.resolve("ifconfig.me", "80"); // HTTP默认端口

    //    // 建立连接
    //    tcp::socket socket(io_ctx);
    //    asio::connect(socket, endpoints);

    //    // 构造HTTP请求
    //    std::string request = 
    //        "GET /all HTTP/1.1\r\n"
    //        "Host: ifconfig.me\r\n"
    //        "Connection: close\r\n\r\n"; // 请求后关闭连接

    //    // 发送请求
    //    asio::write(socket, asio::buffer(request));

    //    // 读取响应
    //    asio::streambuf response_buf;
    //    boost::system::error_code error;

    //    // 1. 先读取HTTP头部，直到遇到空行
    //    asio::read_until(socket, response_buf, "\r\n\r\n", error);
    //    if(error && error != asio::error::eof) {
    //        std::cerr << "读取头部错误: " << error.message() << std::endl;
    //        return result;
    //    }

    //    // 2. 继续读取剩余的响应体数据
    //    while (asio::read(socket, response_buf, asio::transfer_at_least(1), error)) {}
    //    if(error != asio::error::eof) {
    //        std::cerr << "读取正文错误: " << error.message() << std::endl;
    //    }

    //    // 处理响应数据
    //    std::istream response_stream(&response_buf);
    //    std::string line;

    //    // 3. 关键：跳过已读的HTTP头部 (空行之前的内容)
    //    // 之前的 read_until 已经将流读到了头部结束的位置，所以我们直接开始读正文即可。
    //    // 为了确保万无一失，也可以在这里再次读取并跳过直到遇到一个空行。
    //    bool in_headers = true;
    //    while (std::getline(response_stream, line) && in_headers) {
    //        if (line == "\r" || line.empty()) {
    //            in_headers = false; // 头部结束，接下来是正文
    //        }
    //    }
    //    // 注意：上面的循环结束后，流指针已经位于正文开始处。

    //    // 4. 解析正文（键值对）
    //    do {
    //        // 去除行尾的换行符（如\r）
    //        if (!line.empty() && line.back() == '\r') {
    //            line.pop_back();
    //        }
    //        // 查找分隔符 ':'
    //        size_t separator_pos = line.find(':');
    //        if (separator_pos != std::string::npos) {
    //            std::string key = line.substr(0, separator_pos);
    //            // 值从冒号后开始，并去除前面的空格
    //            std::string value = line.substr(separator_pos + 1);
    //            value.erase(0, value.find_first_not_of(" "));
    //            // 存入map
    //            result[key] = value;
    //        }
    //    } while (std::getline(response_stream, line));

    //} catch (std::exception& e) {
    //    std::cerr << "操作异常: " << e.what() << std::endl;
    //}

    return ret_val;
}

/*
    Get current public ip.
*/
SOCKET_DLLAPI ReturnType GetPublicIP(
    ZString* public_ip_str_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    //std::cout << "正在从 ifconfig.me/all 获取公网IP和端口信息..." << std::endl;

    auto network_info = get_public_ip_and_port();

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