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
#include "z_core/t_atom.h"
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
        ZStringView _stun_port,
        UInt32 _failed_count
    )
        : stun_address_str_(_stun_address)
        , stun_port_str_(_stun_port)
        , failed_count_(_failed_count)
        , failed_(false)
    {}

    ZString stun_address_str_;
    ZString stun_port_str_;
    UInt32 failed_count_;
    Bool failed_;
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
            //remove \r if exist
            ZStringView stun_server_info_raw_string_view;
            if ((*stun_server_info_raw_iter)[stun_server_info_raw_iter->Size() - 1] == '\r') {
                stun_server_info_raw_string_view.SetViewString(
                    stun_server_info_raw_iter->DataPtr(), 
                    stun_server_info_raw_iter->Size() - 1
                );
            }
            else {
                stun_server_info_raw_string_view = *stun_server_info_raw_iter;
            }

            auto stun_server_info_string_view_list = string::SplitToStringView(
                stun_server_info_raw_string_view, ':'
            );
            if (stun_server_info_string_view_list.Size() >= 2ULL) {
                auto stun_server_info_string_view_list_iter = stun_server_info_string_view_list.Begin();
                ZStringView address_string_view = *stun_server_info_string_view_list_iter;
                ++stun_server_info_string_view_list_iter;
                ZStringView port_string_view = *stun_server_info_string_view_list_iter;

                UInt32 failed_count = 0ULL;
                if (stun_server_info_string_view_list.Size() >= 3ULL) {
                    ++stun_server_info_string_view_list_iter;
                    link_code = stun_server_info_string_view_list_iter->ToUInt32(&failed_count);
                    if (link_code != kOK) {
                        failed_count = 0;
                        Z_LOG_ERROR(
                            error_code::kFSocketErrorCode_LinkError, link_code,
                            L"ZStringView::ToUInt32() link error!"
                        );
                    }
                }
                PushBack(new StunServerInfo(address_string_view, port_string_view, failed_count));
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
                "%s:%s:%u\n",
                (*stun_server_info_iter)->stun_address_str_.String(),
                (*stun_server_info_iter)->stun_port_str_.String(),
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
SOCKET_DLLAPI ReturnType GetUDPPublicIP4AndPort(
    const ZUDPEndpoint& _local_udp_endpoint,
    UInt32* _public_udp_ip_ptr,
    UInt16* _public_udp_port_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZIOContext io_context;
    ZUDPSocket udp_socket(&io_context);
    link_code = udp_socket.BindEndpoint(_local_udp_endpoint);
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

    TVector<ZUDPEndpoint> endpoint_vector;
    Bool finished = false;
    ZMutex finished_mutex;
    TList<TArray<UInt8, 1000ULL>> buffer_list;
    for (
        auto stun_server_info_ptr_iter = stun_server_info_list.Begin(); 
        (stun_server_info_ptr_iter != stun_server_info_list.End()) && (finished == false);
    ) {
        StunServerInfo* stun_server_info_ptr = *stun_server_info_ptr_iter;

        //resolve address
        link_code = io_context.ResolveUDPAddress(
            stun_server_info_ptr->stun_address_str_, 
            stun_server_info_ptr->stun_port_str_,
            &endpoint_vector
        );
        if (link_code != kOK) {
            stun_server_info_ptr_iter = stun_server_info_list.Erase(stun_server_info_ptr_iter);
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZIOContext::ResolveUDPAddress() link error!"
            );
            continue;
        }

        link_code = udp_socket.SendTo(
            endpoint_vector.Front(),
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

        buffer_list.EmplaceBack();
        link_code = udp_socket.AsyncReceiveFrom(
            ZBuffer(
                buffer_list.Back().DataPtr(),
                buffer_list.Back().Capacity()
            ),
            [
                _public_udp_ip_ptr, _public_udp_port_ptr, &finished, &finished_mutex,
                stun_server_info_ptr ,&stun_server_info_list
            ](
                ReturnType _error_code,
                ZUDPSocket* _socket_ptr,
                const ZConstBuffer _buffer,
                const ZUDPEndpoint& _udp_endpoint
            ) {
                if (
                    _error_code == kOK &&
                    _buffer.Size() >= 28 && 
                    _buffer.BufferPtr<const UInt8*>()[0] == 0x01 && 
                    _buffer.BufferPtr<const UInt8*>()[1] == 0x01
                ) {
                    SizeType buffer_index = 20;
                    while (buffer_index + 8 < _buffer.Size()) {
                        UInt16 type = 
                            (_buffer.BufferPtr<const UInt8*>()[buffer_index] << 8)
                            | _buffer.BufferPtr<const UInt8*>()[buffer_index + 1];
                        UInt16 length = 
                            (_buffer.BufferPtr<const UInt8*>()[buffer_index + 2] << 8) |
                            _buffer.BufferPtr<const UInt8*>()[buffer_index + 3];
                        //success
                        if (
                            type == 0x0020 &&
                            length >= 8 &&
                            _buffer.BufferPtr<const UInt8*>()[buffer_index + 4] == 0 &&
                            _buffer.BufferPtr<const UInt8*>()[buffer_index + 5] == 1
                        ) {
                            // XOR-MAPPED-ADDRESS IPv4
                            UInt16 port = 
                                (_buffer.BufferPtr<const UInt8*>()[buffer_index + 6] << 8) |
                                _buffer.BufferPtr<const UInt8*>()[buffer_index + 7];
                            port ^= 0x2112;
                            UInt32 ip =
                                (UInt32(_buffer.BufferPtr<const UInt8*>()[buffer_index + 8]) << 24) |
                                (UInt32(_buffer.BufferPtr<const UInt8*>()[buffer_index + 9]) << 16) |
                                (UInt32(_buffer.BufferPtr<const UInt8*>()[buffer_index + 10]) << 8) |
                                _buffer.BufferPtr<const UInt8*>()[buffer_index + 11];
                            ip ^= 0x2112A442;
                            
                            finished_mutex.Lock();
                            if (finished == false) {
                                finished = true;
                                *_public_udp_ip_ptr = ip;
                                *_public_udp_port_ptr = port;
                            }
                            finished_mutex.Unlock();

                            //reset failed count
                            stun_server_info_ptr->failed_count_ = 0U;

                            return;
                        }
                        buffer_index += 4 + ((length + 3) & ~3);
                    }
                }

                //failed
                stun_server_info_ptr->failed_ = true;
                ++stun_server_info_ptr->failed_count_;
            }
        );
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                L"ZUDPSocket::ReceiveFrom() link error!"
            );
            return ret_val;
        }

        if (!io_context.IsRunning()) {
            link_code = io_context.AsyncRun();
            if (link_code != kOK) {
                ret_val = error_code::kFSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZIOContext::AsyncRun() link error!"
                );
                return ret_val;
            }
        }
        ++stun_server_info_ptr_iter;

        SleepMs(100);
    }
      
    //cancel other
    link_code = udp_socket.Cancel();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFSocketErrorCode_LinkError, link_code,
            L"ZUDPSocket::Cancel() link error!"
        );
        return ret_val;
    }

    //Wait until finish.
    io_context.Join();

    //handle failed servers
    for (
        auto stun_server_info_ptr_iter = stun_server_info_list.Begin();
        stun_server_info_ptr_iter != stun_server_info_list.End();
    ) {
        StunServerInfo* stun_server_info_ptr = *stun_server_info_ptr_iter;
        if (stun_server_info_ptr->failed_) {
            stun_server_info_ptr->failed_ = false;
            if (stun_server_info_ptr->failed_count_ < StunServerInfoList::kMaxFailedTimeAllowed) {
                stun_server_info_list.PushBack(stun_server_info_ptr);
            }
            stun_server_info_ptr_iter = stun_server_info_list.Erase(stun_server_info_ptr_iter);
            continue;
        }
        ++stun_server_info_ptr_iter;
    }

    return ret_val;
}

}//socket
}//zengine