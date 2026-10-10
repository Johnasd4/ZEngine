/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "f_socket.h"

#include "z_io_context.h"
#include "z_io_context_work_guard.h"
#include "z_udp_socket.h"

namespace zengine {
namespace socket {

struct CACHE_ALIGN StunServerInfo : public ZObject {
    FORCEINLINE StunServerInfo(
        ZStringView _stun_address,
        ZStringView _stun_port,
        UInt32 _success_count
    )
        : stun_address_str_(_stun_address)
        , stun_port_str_(_stun_port)
        , success_count_(_success_count)
        , is_valid_(true)
    {}

    ZString stun_address_str_;
    ZString stun_port_str_;
    TAtom<UInt32> success_count_;
    TAtom<Bool> is_valid_;
};

class StunServerInfoList : public TList<StunServerInfo> {
public:
    using SuperType_ = TList<StunServerInfo>;

    static constexpr Char kStunServerInfoFilePath[] = "ZEngine/Socket/Stun_Address.txt";

public:
    NODISCARD static StunServerInfoList& GetInstance() noexcept {
        static StunServerInfoList stun_server_info_list;
        return stun_server_info_list;
    }

private:
    StunServerInfoList() noexcept
        : SuperType_()
    {
        ReturnType link_code = kOK;
        link_code = LoadStunServerInfoP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kFSocketErrorCode_LinkError, link_code,
                "StunServerInfoList::LoadStunServerInfoP() link error!"
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
                "StunServerInfoList::SaveStunServerInfoP() link error!"
            );
            return;
        }
    }

    NODISCARD ReturnType LoadStunServerInfoP() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;
        link_code = file.OpenSafe(kStunServerInfoFilePath, ZFile::kReadOnly | ZFile::kBinaryMode);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::OpenSafe() link error!"
            );
            return ret_val;
        }
        // read all data
        Int32 file_size = static_cast<Int32>(file.ComputeSize());
        ZMemory memory(file_size);
        auto read_result = file.Read(ZBuffer(memory.GetDataPtr<Void>(), file_size), file_size);
        if (!read_result) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, read_result.GetErrorCode(),
                "ZFile::Read() link error!"
            );
            return ret_val;
        }
        SizeType read_size = read_result.GetValue();
        auto stun_server_info_raw_list = string::SplitToStringViewSkipEmpty(
            ZStringView(memory.GetDataPtr<Char>(), read_size), '\n'
        );
        for (
            auto stun_server_info_raw_iter = stun_server_info_raw_list.Begin();
            stun_server_info_raw_iter != stun_server_info_raw_list.End();
            ++stun_server_info_raw_iter
            ) {
            // if stun server info is empty, skip it
            if(stun_server_info_raw_iter->GetSize() == 0ULL) {
                continue;
            }

            // remove \r if exist
            ZStringView stun_server_info_raw_string_view;
            if ((*stun_server_info_raw_iter)[stun_server_info_raw_iter->GetSize() - 1] == '\r') {
                stun_server_info_raw_string_view.Assign(
                    stun_server_info_raw_iter->GetDataPtr(), 
                    stun_server_info_raw_iter->GetSize() - 1
                );
            }
            else {
                stun_server_info_raw_string_view = *stun_server_info_raw_iter;
            }

            // split to address, port and failed count
            auto stun_server_info_string_view_list = string::SplitToStringViewSkipEmpty(
                stun_server_info_raw_string_view, ':'
            );
            if (stun_server_info_string_view_list.GetSize() >= 2ULL) {
                auto stun_server_info_string_view_list_iter = stun_server_info_string_view_list.Begin();
                ZStringView address_string_view = *stun_server_info_string_view_list_iter;
                ++stun_server_info_string_view_list_iter;
                ZStringView port_string_view = *stun_server_info_string_view_list_iter;

                UInt32 success_count = 0ULL;
                // if failed count exist, load it, otherwise use 0 as default value
                if (stun_server_info_string_view_list.GetSize() >= 3ULL) {
                    ++stun_server_info_string_view_list_iter;
                    auto result = string::StringToUInt32(*stun_server_info_string_view_list_iter);
                    if (!result) {
                        success_count = 0;
                        Z_LOG_ERROR(
                            error_code::kFSocketErrorCode_LinkError, result.GetErrorCode(),
                            "ZStringView::ToUInt32() link error!"
                        );
                    }
                }
                // add to list
                EmplaceBack(address_string_view, port_string_view, success_count);
            }
        }
        return ret_val;
    }

    NODISCARD ReturnType SaveStunServerInfoP() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;
        ZFile file;
        link_code = file.OpenSafe(kStunServerInfoFilePath, ZFile::kWriteOnly | ZFile::kTruncate);
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::OpenSafe() link error!"
            );
            return ret_val;
        }
        // save all data
        for (
            auto stun_server_info_ptr_iter = Begin();
            stun_server_info_ptr_iter != End();
            ++stun_server_info_ptr_iter
        ) {
            StunServerInfo& stun_server_info = *stun_server_info_ptr_iter;
            // if stun server info is not valid, skip it
            if (!stun_server_info.is_valid_) {
                continue;
            }

            ZString stun_server_info_string = string::GenerateString(
                "{}:{}:{}\n",
                stun_server_info.stun_address_str_.GetDataPtr(),
                stun_server_info.stun_port_str_.GetDataPtr(),
                stun_server_info.success_count_.Load()
            );
            link_code = file.Write(
                ZConstBuffer(stun_server_info_string.GetDataPtr(), stun_server_info_string.GetSize())
            );
            if (link_code != kOK) {
                ret_val = error_code::kFSocketErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    "ZFile::Write() link error!"
                );
                return ret_val;
            }
        }
        return ret_val;
    }

};

SOCKET_DLLAPI TOptional<ZUDPEndpoint> GetUDPSocketPublicIP4AndPort(ZUDPSocket& _socket) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // get stun server info list
    StunServerInfoList& stun_server_info_list = StunServerInfoList::GetInstance();

    // stun request data
    TFixedArray<UInt8, 20> stun_request_header = {
        0x00, 0x01, 0x00, 0x00,
        0x21, 0x12, 0xA4, 0x42,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    Bool if_success = false;
    Int32 async_count = 0;
    ZMutex async_mutex;
    ZMutex success_mutex;
    ZMutex finish_mutex;
    UInt32 public_udp_ip;
    UInt16 public_udp_port;

    success_mutex.Lock();
    TList<TFixedArray<UInt8, 1000ULL>> buffer_list;
    for (
        auto stun_server_info_ptr_iter = stun_server_info_list.Begin(); 
        stun_server_info_ptr_iter != stun_server_info_list.End();
    ) {
        StunServerInfo& stun_server_info = *stun_server_info_ptr_iter;

        // if stun server info is not valid, skip it
        if (stun_server_info.is_valid_ == false) {
            ++stun_server_info_ptr_iter;
            continue;
        }

        // resolve address
        auto resolve_result = _socket.GetIOContext().ResolveUDP(
            stun_server_info.stun_address_str_,
            stun_server_info.stun_port_str_
        );

        // if resolve failed, mark stun server info as invalid and skip it
        if (!resolve_result) {
            stun_server_info.is_valid_ = false;
            Z_LOG_ERROR(
                ret_val, resolve_result.GetErrorCode(),
                "ZIOContext::ResolveUDPAddress() link error!"
            );
            continue;
        }

        // set success count pointer in stun request header, use copy to avoid strict aliasing violation
        StunServerInfo* info_address = &stun_server_info;
        Copy(
            stun_request_header.GetDataPtr() + 8ULL,
            &info_address,
            sizeof(StunServerInfo*)
        );

        // send stun request to first resolved endpoint
        link_code = _socket.SendTo(
            resolve_result->Front(),
            ZConstBuffer(stun_request_header.GetDataPtr(), sizeof(stun_request_header))
        );
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::SendTo() link error!"
            );
            return ret_val;
        }

        // add count and lock
        {
            TLockGuard async_lock_guard(async_mutex);
            if (async_count == 0) {
                finish_mutex.Lock();
            }
            ++async_count;
        }

        buffer_list.EmplaceBack();
        //wait for response
        link_code = _socket.AsyncReceiveFrom(
            ZBuffer(
                buffer_list.Back().GetDataPtr(),
                buffer_list.Back().GetCapacity()
            ),
            [
                &public_udp_ip, &public_udp_port, &if_success, &async_mutex, &success_mutex, &async_count,
                &finish_mutex, &stun_server_info_list
            ] (
                ReturnType _error_code,
                ZUDPSocket* _socket_ptr,
                const ZConstBuffer _buffer,
                const ZUDPEndpoint& _udp_endpoint
            ) {
                Bool is_last_async = false;
                if (
                    _error_code == kOK &&
                    _buffer.GetSize() >= 28 && 
                    _buffer.GetDataPtr<const UInt8>()[0] == 0x01 && 
                    _buffer.GetDataPtr<const UInt8>()[1] == 0x01
                ) {
                    SizeType buffer_index = 20;
                    while (buffer_index + 8 < _buffer.GetSize()) {
                        UInt16 type = 
                            (_buffer.GetDataPtr<const UInt8>()[buffer_index] << 8)
                            | _buffer.GetDataPtr<const UInt8>()[buffer_index + 1];
                        UInt16 length = 
                            (_buffer.GetDataPtr<const UInt8>()[buffer_index + 2] << 8) |
                            _buffer.GetDataPtr<const UInt8>()[buffer_index + 3];
                        //success
                        if (
                            type == 0x0020 &&
                            length >= 8 &&
                            _buffer.GetDataPtr<const UInt8>()[buffer_index + 4] == 0 &&
                            _buffer.GetDataPtr<const UInt8>()[buffer_index + 5] == 1
                        ) {
                            // XOR-MAPPED-ADDRESS IPv4
                            UInt16 port = 
                                (_buffer.GetDataPtr<const UInt8>()[buffer_index + 6] << 8) |
                                _buffer.GetDataPtr<const UInt8>()[buffer_index + 7];
                            port ^= 0x2112;
                            UInt32 ip =
                                (UInt32(_buffer.GetDataPtr<const UInt8>()[buffer_index + 8]) << 24) |
                                (UInt32(_buffer.GetDataPtr<const UInt8>()[buffer_index + 9]) << 16) |
                                (UInt32(_buffer.GetDataPtr<const UInt8>()[buffer_index + 10]) << 8) |
                                _buffer.GetDataPtr<const UInt8>()[buffer_index + 11];
                            ip ^= 0x2112A442;

                            // use copy to avoid strict aliasing violation
                            StunServerInfo* target_server_info_ptr;
                            Copy(
                                &target_server_info_ptr,
                                _buffer.GetDataPtr<const UInt8>() + 8ULL,
                                sizeof(StunServerInfo*)
                            );

                            // update stun server info, compare all to defend against attack
                            for (
                                auto stun_server_info_iter = stun_server_info_list.Begin();
                                stun_server_info_iter != stun_server_info_list.End();
                                ++stun_server_info_iter
                                ) {
                                StunServerInfo& stun_server_info = *stun_server_info_iter;
                                if (&stun_server_info == target_server_info_ptr) {
                                    ++stun_server_info.success_count_;
                                    break;
                                }
                            }

                            // set success result                                                   
                            {
                                TLockGuard async_lock_guard(async_mutex);
                                if (if_success == false) {
                                    if_success = true;
                                    success_mutex.Unlock();
                                    public_udp_ip = ip;
                                    public_udp_port = port;
                                }
                                --async_count;
                                if (async_count == 0) {
                                    is_last_async = true;
                                }
                            }
                            if (is_last_async) {
                                finish_mutex.Unlock();
                            }
                            return;
                        }
                        buffer_index += 4 + ((length + 3) & ~3);
                    }
                }

                // failed
                {
                    TLockGuard async_lock_guard(async_mutex);
                    --async_count;
                    if (async_count == 0) {
                        is_last_async = true;
                    }
                }
                if (is_last_async) {
                    finish_mutex.Unlock();
                }
            }
        );
        if (link_code != kOK) {
            ret_val = error_code::kFSocketErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZUDPSocket::ReceiveFrom() link error!"
            );
            return ret_val;
        }

        ++stun_server_info_ptr_iter;

        // if success, break loop
        {
            TLockGuard success_lock_guard(async_mutex);
            if (if_success == true) {
                break;
            }
        }
    }

    // wait max 500ms for success
    if (!success_mutex.TryLockForMs(500LL)) {
        ZUDPEndpoint local_udp_endpoint = _socket.GetLocalEndpoint();
        Z_LOG_FAILURE(
            "Wait for stun response timeout!  local_ip: {} local_port: {}",
            local_udp_endpoint.GetIP4Address().GenerateIPString(),
            local_udp_endpoint.GetPort()
        );
    }
    success_mutex.Unlock();

    // cancel other async receive
    _socket.Cancel();

    // wait until all operation finished
    finish_mutex.Lock();
    finish_mutex.Unlock();

    if (!if_success) {
        ZUDPEndpoint local_udp_endpoint = _socket.GetLocalEndpoint();
        Z_LOG_ERROR(
            error_code::kFSocketErrorCode_LinkError, link_code,
            "Get udp public ip4 and prot failed! local_ip: {} local_port: {}",
            local_udp_endpoint.GetIP4Address().GenerateIPString(),
            local_udp_endpoint.GetPort()
        );
        return ret_val;
    }

    return TOptional<ZUDPEndpoint>(public_udp_ip, public_udp_port);
}

} // socket
} // zengine