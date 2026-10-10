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

#include "z_kcp_session.h"

namespace zengine {
namespace socket {
namespace internal {

static Void KcpLogCallbackFunc(const Char* _log_str, ikcpcb* _kcp_ptr, Void* _user) {
    Z_LOG_TRACE("{}", _log_str);
}

} // internal
} // socket
} // zengine

namespace zengine {
namespace socket {

ZKCPSession::ZKCPSession() noexcept
    : SuperType_()
    , kcp_context_ptr_(nullptr)
    , max_pending_packet_num_(kDefaultMaxPendingPacketNum)
    , update_interval_ms_(0U)
{
    static Bool set_allocator = []() {
        ikcp_allocator(memory_pool::ApplyGlobalMemory, memory_pool::ReleaseGlobalMemory);
        return true;
    }();
}

ZKCPSession::~ZKCPSession() noexcept {
    if (kcp_context_ptr_) {
        ikcp_release(kcp_context_ptr_);
        kcp_context_ptr_ = nullptr;
    }
}

NODISCARD UInt32 ZKCPSession::GetConversationID() noexcept { 
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    return kcp_context_ptr_->conv; 
}

Void ZKCPSession::SetNoDelay(Int32 _nodelay, Int32 _interval, Int32 _resend, Int32 _nc) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_nodelay(kcp_context_ptr_, _nodelay, _interval, _resend, _nc);
    update_interval_ms_ = kcp_context_ptr_->interval;
}

Void ZKCPSession::SetWindowSize(Int32 _snd_wnd, Int32 _rcv_wnd) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_wndsize(kcp_context_ptr_, _snd_wnd, _rcv_wnd);
}

Void ZKCPSession::SetMaxTransmissionUnitSize(Int32 _mtu) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_setmtu(kcp_context_ptr_, _mtu);
}

Void ZKCPSession::SetStreamMode(Int32 _mode) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    kcp_context_ptr_->stream = _mode;
}

Void ZKCPSession::SetLogMask(Int32 _log_mask) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    kcp_context_ptr_->logmask = _log_mask;
}

Void ZKCPSession::SetDeadLinkTimes(UInt32 _dead_link_times) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    kcp_context_ptr_->dead_link = _dead_link_times;
}

Void ZKCPSession::Open(
    UInt32 _conv_id,
    Void* _user_ptr,
    Int32(*_output_callback_func)(const Char*, Int32, ikcpcb*, Void*)
) noexcept {
    // Rlese the existing KCP context if it exists to avoid memory leaks.
    if (kcp_context_ptr_ != nullptr) {
        ikcp_release(kcp_context_ptr_);
    }
    // Create a new KCP context with the specified conversation ID and user pointer.
    kcp_context_ptr_ = ikcp_create(_conv_id, _user_ptr);
    // Bind the output callback function.
    ikcp_setoutput(kcp_context_ptr_, _output_callback_func);
    // Bind the log callback function.
    kcp_context_ptr_->writelog = internal::KcpLogCallbackFunc;
}

Void ZKCPSession::Close() noexcept {
    if (kcp_context_ptr_ == nullptr) {
        return;
    }
    ikcp_release(kcp_context_ptr_);
    kcp_context_ptr_ = nullptr;
}

Void ZKCPSession::ResetDeadLink() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    kcp_context_ptr_->state = 0;
}

NODISCARD UInt32 ZKCPSession::GetNextUpdateTimeMs() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    return ikcp_check(kcp_context_ptr_, GetCurrentTimeMs());
}

Void ZKCPSession::Update() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_update(kcp_context_ptr_, GetCurrentTimeMs());
}

Void ZKCPSession::Flush() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_flush(kcp_context_ptr_);
}

NODISCARD ReturnType ZKCPSession::Send(ZConstBuffer _buffer) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ReturnType ret_val = kOK;

    // Check if the KCP context is valid. If not, return an error indicating that the session does not exist.
    if (kcp_context_ptr_ == nullptr) {
        ret_val = error_code::kZKCPSessionErrorCode_SessionNotExist;
        Z_LOG_ERROR(
            ret_val, kOK,
            "KCP session context is nullptr! Please call Open() to create a KCP session first."
        );
        return ret_val;
    }

    // Check the number of pending packets in the KCP send queue. If it exceeds the maximum allowed, return an error.
    Int32 pending_packets = ikcp_waitsnd(kcp_context_ptr_);
    if (pending_packets >= max_pending_packet_num_) {
        ret_val = error_code::kZKCPSessionErrorCode_WaitSend;
        return ret_val;
    }

    // Attempt to send the data through the KCP context. If it fails, log an error.
    if (ikcp_send(kcp_context_ptr_, _buffer.GetDataPtr<Char>(), _buffer.GetSize()) < 0) {
        ret_val = error_code::kZKCPSessionErrorCode_RetryTimesToMuch;
        Z_LOG_ERROR(
            ret_val, kOK,
            "KCP session send failed! Please check the network connection or KCP configuration."
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZKCPSession::ResolveReceivedPacket(ZConstBuffer _buffer) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ReturnType ret_val = kOK;

    if (ikcp_input(kcp_context_ptr_, _buffer.GetDataPtr<Char>(), _buffer.GetSize()) < 0) {
        ret_val = error_code::kZKCPSessionErrorCode_NotKCPPacket;
        return ret_val;
    }

    return ret_val;
}

NODISCARD Int32 ZKCPSession::GetNextMessageSize() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    return ikcp_peeksize(kcp_context_ptr_);
}

NODISCARD ReturnType ZKCPSession::GetNextMessage(ZBuffer _buffer, Int32* _msg_size_ptr) noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ReturnType ret_val = kOK;

    Int32 msg_size = ikcp_recv(kcp_context_ptr_, _buffer.GetDataPtr<Char>(), _buffer.GetSize());   

    if(msg_size < 0) {
        if (msg_size == -1) {
            ret_val = error_code::kZKCPSessionErrorCode_NoMessageExist;
        }
        else if (msg_size == -2) {
            ret_val = error_code::kZKCPSessionErrorCode_MessageMissingClip;
        }
        else if (msg_size == -3) {
            ret_val = error_code::kZKCPSessionErrorCode_BufferSmallerThenMessage;
            Z_LOG_ERROR(
                ret_val, 0,
                "KCP session receive failed! The provided buffer is smaller than the next message size."
            );
        }
        else {
            ret_val = error_code::kZKCPSessionErrorCode_SystemOrLibraryError;
            Z_LOG_ERROR(
                ret_val, 0,
                "KCP session receive failed! Unknown error."
            );
        }
        return ret_val;
    }
    
    if (_msg_size_ptr != nullptr) {
        *_msg_size_ptr = msg_size;
    }

    return ret_val;
}

Void ZKCPSession::SkipNextMessage() noexcept {
    ASSERT(kcp_context_ptr_ != nullptr, "KCP context pointer is nullptr.");
    ikcp_recv(kcp_context_ptr_, nullptr, kInt32Max);
}

} // socket
} // zengine