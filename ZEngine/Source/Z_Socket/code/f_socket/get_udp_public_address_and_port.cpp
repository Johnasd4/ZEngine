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
#include "z_core/t_array.h"
#include "z_core/z_file.h"
#include "z_core/z_memory.h"
#include "z_core/z_string.h"
#include "z_core/z_string_view.h"

#include "z_buffer.h"
#include "z_io_context.h"
#include "z_udp_socket.h"

namespace zengine {
namespace socket {

struct StunServerInfo : public ZObject {
    FORCEINLINE StunServerInfo(
        ZStringView _stun_address,
        UInt32 _stun_port,
        UInt32 _failed_count
    )
        : stun_address_(_stun_address)
        , stun_port_(_stun_port)
        , failed_count_(_failed_count)
    {}

    ZString stun_address_;
    UInt32 stun_port_;
    UInt32 failed_count_;
};

class StunServerInfoList : public TListSafe<StunServerInfo*> {
public:
    static constexpr UInt32 kMaxFailedTimeAllowed = 5;

    NODISCARD static StunServerInfoList& Instance() noexcept {
        static StunServerInfoList stun_server_info_list;
        return stun_server_info_list;
    }

protected:
    using SuperType_ = TListSafe<StunServerInfo*>;

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
        //release
        for (
            auto stun_server_info_iter = Begin();
            stun_server_info_iter != End();
            ++stun_server_info_iter
        ) {
            delete* stun_server_info_iter;
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
                PushBack(new StunServerInfo(address_string_view, port, failed_count));
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
                (*stun_server_info_iter)->stun_address_.String(),
                (*stun_server_info_iter)->stun_port_,
                (*stun_server_info_iter)->failed_count_
            );
            link_code = file.Write(stun_server_info_string.String(), stun_server_info_string.Size());
            if (link_code != kOK) {
                ret_val = error_code::kFSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZFile::Write() link error!"
                );
                return ret_val;
            }
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
    link_code = udp_socket.BindEndpoint(_local_udp_address, _local_udp_port);
    if (link_code != kOK) {
        ret_val = error_code::kFSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZUDPSocket::BindEndpoint() link error!"
        );
        return ret_val;
    }

    StunServerInfoList& stun_server_info_list = StunServerInfoList::Instance();

    static constexpr UInt8 kSTUNRequest[] = {
        0x00, 0x01, 0x00, 0x00,
        0x21, 0x12, 0xA4, 0x42,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    TArray<UInt8, 512ULL> receive_buffer;

    SizeType info_num = stun_server_info_list.Size();
    for (SizeType count = 0; count < info_num; ++info_num) {
        StunServerInfo* stun_server_info_ptr = stun_server_info_list.Front();
        link_code = udp_socket.SendTo(
            stun_server_info_ptr->stun_address_.String(),
            stun_server_info_ptr->stun_port_,
            ZConstBuffer(kSTUNRequest, sizeof(kSTUNRequest))
        );
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZUDPSocket::SendTo() link error!"
            );
            return ret_val;
        }

        SizeType receive_size = 0ULL;
        link_code = udp_socket.ReceiveFrom(
            ZBuffer(receive_buffer.DataPtr(), receive_buffer.Capacity()),
            &receive_size
        );

        if (receive_size >= 28 && receive_buffer[0] == 0x01 && receive_buffer[1] == 0x01) {
            SizeType buffer_index = 20;
            while (buffer_index + 8 < receive_size) {
                UInt16 type = (receive_buffer[buffer_index] << 8) | receive_buffer[buffer_index + 1];
                UInt16 length = (receive_buffer[buffer_index + 2] << 8) | receive_buffer[buffer_index + 3];
                //success
                if (
                    type == 0x0020 && 
                    length >= 8 && 
                    receive_buffer[buffer_index + 4] == 0 && 
                    receive_buffer[buffer_index + 5] == 1
                ) {  
                    // XOR-MAPPED-ADDRESS IPv4
                    UInt16 port_raw = (receive_buffer[buffer_index + 6] << 8) | receive_buffer[buffer_index + 7];
                    port_raw ^= 0x2112;
                    UInt32 address_raw = 
                        (UInt32(receive_buffer[buffer_index + 8]) << 24) | 
                        (UInt32(receive_buffer[buffer_index + 9]) << 16) |
                        (UInt32(receive_buffer[buffer_index + 10]) << 8) | 
                        receive_buffer[buffer_index + 11];
                    address_raw ^= 0x2112A442;

                    *_public_udp_address_ptr = std::move(IP4ToString(address_raw));
                    *_public_udp_port_ptr = port_raw;
                    //reset failed count
                    stun_server_info_ptr->failed_count_ = 0U;
                    return ret_val;
                }
                buffer_index += 4 + ((length + 3) & ~3);
            }
        }

        //failed
        stun_server_info_list.PopFront();
        ++stun_server_info_ptr->failed_count_;
        if (stun_server_info_ptr->failed_count_ < StunServerInfoList::kMaxFailedTimeAllowed) {
            stun_server_info_list.PushBack(stun_server_info_ptr);
        }
    }

    return ret_val;
}

}//socket
}//zengine