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

#include "z_core/f_string.h"
#include "z_core/t_pair.h"
#include "z_core/t_vector.h"
#include "z_core/z_file.h"
#include "z_core/z_memory.h"
#include "z_core/z_string.h"
#include "z_core/z_string_view.h"

#include "z_buffer.h"
#include "z_io_context.h"
#include "z_udp_socket.h"


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

            static constexpr UInt8 kSTUNRequest[] = {
                0x00, 0x01, 0x00, 0x00,
                0x21, 0x12, 0xA4, 0x42,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00
            };

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
    ZUDPSocket udp_socket(&io_context);

    static constexpr UInt8 kSTUNRequest[] = {
        0x00, 0x01, 0x00, 0x00,
        0x21, 0x12, 0xA4, 0x42,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };



    return ret_val;
}

struct StunServerInfo : public ZObject {
    FORCEINLINE StunServerInfo(
        ZStringView _stun_address,
        UInt32 _stun_port,
        UInt32 _failed_count
    )
        : stun_address_(_stun_address)
        , stun_port_(_stun_port)
        , failed_count_(_failed_count)
    {
    }

    ZString stun_address_;
    UInt32 stun_port_;
    UInt32 failed_count_;
};

class StunServerInfoList : public TListSafe<StunServerInfo> {
public:
    NODISCARD static StunServerInfoList& Instance() noexcept {
        static StunServerInfoList stun_server_info_list;
        return stun_server_info_list;
    }

protected:
    using SuperType_ = TListSafe<StunServerInfo>;

private:
    StunServerInfoList() noexcept
        : SuperType_()
    {
        ReturnType link_code = kOK;
        link_code = LoadStunServerInfoP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kFSocketErrorCode_LinkError, link_code,
                L"StunServerInfoList::LoadStunServerInfoP() link error!"
            );
            return;
        }
    }

    ~StunServerInfoList() noexcept {
        ReturnType link_code = kOK;
        link_code = SaveStunServerInfoP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kFSocketErrorCode_LinkError, link_code,
                L"StunServerInfoList::SaveStunServerInfoP() link error!"
            );
            return;
        }
    }

    NODISCARD ReturnType LoadStunServerInfoP() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;
        link_code = file.OpenSafe(L"ZEngine/Socket/Stun_Address.txt", ZFile::kOpenTypeReadPlusBin);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZFile::OpenSafe() link error!"
            );
            return ret_val;
        }
        //read all data
        Int32 file_size = file.Size();
        ZMemory memory(file_size);
        link_code = file.Read(memory.DataPtr<Void*>(), file_size);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZFile::Read() link error!"
            );
            return ret_val;
        }
        auto stun_server_info_raw_list = string::SplitToStringView(
            ZStringView(memory.DataPtr<Char*>(), file_size), '\n'
        );
        for (
            auto stun_server_info_raw_iter = stun_server_info_raw_list.Begin();
            stun_server_info_raw_iter != stun_server_info_raw_list.End();
            ++stun_server_info_raw_iter
            ) {
            auto stun_server_info_string_view_list = string::SplitToStringView(
                *stun_server_info_raw_iter, ':'
            );
            if (stun_server_info_string_view_list.Size() >= 2ULL) {
                auto stun_server_info_string_view_list_iter = stun_server_info_string_view_list.Begin();
                ZStringView address_string_view = *stun_server_info_string_view_list_iter;
                ++stun_server_info_string_view_list_iter;
                UInt32 port;
                link_code = stun_server_info_string_view_list_iter->ToUInt32(&port);
                if (link_code != kOK) {
                    Z_LOG_ERROR(
                        error_code::kFSocketErrorCode_LinkError, link_code,
                        L"ZStringView::ToUInt32() link error!"
                    );
                    continue;
                }
                UInt32 failed_count = 0ULL;
                if (stun_server_info_string_view_list.Size() >= 3ULL) {
                    ++stun_server_info_string_view_list_iter;
                    link_code = stun_server_info_string_view_list_iter->ToUInt32(&failed_count);
                    if (link_code != kOK) {
                        Z_LOG_ERROR(
                            error_code::kFSocketErrorCode_LinkError, link_code,
                            L"ZStringView::ToUInt32() link error!"
                        );
                        continue;
                    }
                }
                EmplaceBack(
                    address_string_view,
                    port,
                    failed_count
                );
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType SaveStunServerInfoP() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;
        link_code = file.OpenSafe(L"ZEngine/Socket/Stun_Address.txt", ZFile::kOpenTypeWritePlusBin);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZFile::OpenSafe() link error!"
            );
            return ret_val;
        }
        //save all data
        for (
            auto stun_server_info_iter = Begin();
            stun_server_info_iter != End();
            ++stun_server_info_iter
        ) {
            ZString stun_server_info_string =  string::GenerateString(
                "%s:%d:%d\n",
                stun_server_info_iter->stun_address_.String(),
                stun_server_info_iter->stun_port_,
                stun_server_info_iter->failed_count_
            );
            file.Write(stun_server_info_string.String(), stun_server_info_string.Size());
            Z_LOG_MESSAGE(
                L"%ls",
                string::String2WString(stun_server_info_string.String()).String()
            );
        }
        return ret_val;
    }
};

/*
    Get current udp public address and port.
*/
SOCKET_DLLAPI ReturnType GetUDPPublicAddressAndPort(
    const Char* _local_udp_address,
    Int32 _local_udp_port,
    ZString* _public_udp_address_ptr,
    Int32* _public_udp_port_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZIOContext io_context;
    ZUDPSocket udp_socket(&io_context);

    StunServerInfoList& stun_server_info_list = StunServerInfoList::Instance();

    for (
        auto stun_server_info_iter = stun_server_info_list.Begin();
        stun_server_info_iter != stun_server_info_list.End();
        ++stun_server_info_iter
    ) {
        Z_LOG_MESSAGE(
            L"%ls %d %d",
            string::String2WString(stun_server_info_iter->stun_address_.String()).String(),
            stun_server_info_iter->stun_port_,
            stun_server_info_iter->failed_count_
        );
    }

    static constexpr UInt8 kSTUNRequest[] = {
        0x00, 0x01, 0x00, 0x00,
        0x21, 0x12, 0xA4, 0x42,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    return ret_val;
}

}//socket
}//zengine