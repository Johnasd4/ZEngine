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
#pragma once

#include "drive.h"

#include "internal/l_kcp.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to KCP session operations.
 */
enum ZKCPSessionErrorCodeEnum : ReturnType::ValueType {
    /** @brief The KCP link encountered an unrecoverable connection failure. */
    kZKCPSessionErrorCode_LinkError = kErrorCodeBase_ZKCPSession,
    /** @brief An underlying system or third-party library internal error occurred. */
    kZKCPSessionErrorCode_SystemOrLibraryError,
    /** @brief A null pointer was unexpectedly passed as an argument. */
    kZKCPSessionErrorCode_NullptrParam,
    /** @brief A provided parameter is out of the valid range. */
    kZKCPSessionErrorCode_ParamOutOfRange,
    /** @brief The send queue is congested and waiting for packets to be flushed. */
    kZKCPSessionErrorCode_WaitSend,
    /** @brief The KCP session context is uninitialized or does not exist. */
    kZKCPSessionErrorCode_SessionNotExist,
    /** @brief Packet retransmission count exceeded the dead link threshold. */
    kZKCPSessionErrorCode_RetryTimesToMuch,
    /** @brief No complete message is currently available to read. */
    kZKCPSessionErrorCode_NoMessageExist,
    /** @brief Message segments are incomplete; waiting for remaining clips. */
    kZKCPSessionErrorCode_MessageMissingClip,
    /** @brief The destination buffer is smaller than the incoming message size. */
    kZKCPSessionErrorCode_BufferSmallerThenMessage,
    /** @brief The received raw payload is not a valid KCP packet. */
    kZKCPSessionErrorCode_NotKCPPacket,
};

} // error_code
} // zengine

namespace zengine {
namespace socket {

/**
 * @brief High-performance reliable UDP session encapsulation based on the KCP protocol.
 *
 * Implementation Principles:
 * 1. Algorithmic State Machine: KCP is a pure ARQ algorithm layer without operating system dependencies or background
 *    threads. This class encapsulates the low-level `ikcpcb` context into an object-oriented session model.
 * 2. Passive Clock Driving: All protocol activities (ACK generation, fast retransmissions, sliding window updates,
 *    and timeout detection) are driven externally via explicit invocations of Update() and GetNextUpdateTimeMs().
 * 3. Flow Control and Reliability: Operates on top of unreliable UDP datagrams to provide ordered, packet-boundary-
 *    preserving data transmission. Includes sender backpressure detection via waitsnd queue monitoring.
 * 4. Memory Management: Memory allocation is intercepted using global memory pool allocators (_IsGlobal) to prevent
 *    heap fragmentation caused by high-frequency allocation and deallocation of transmission segments.
 */
class ZKCPSession : public ZObject {
public:
    using SuperType_ = ZObject;

    /**
     * @brief Logging event masks used to filter internal KCP trace outputs.
     */
    enum LogMaskEnum_ : Int32 {
        /** @brief Log low-level raw packet output events. */
        kKCPLogMask_Output = IKCP_LOG_OUTPUT,
        /** @brief Log low-level raw packet input events. */
        kKCPLogMask_Input = IKCP_LOG_INPUT,
        /** @brief Log upper-layer data send events. */
        kKCPLogMask_Send = IKCP_LOG_SEND,
        /** @brief Log upper-layer data receive events. */
        kKCPLogMask_Recv = IKCP_LOG_RECV,
        /** @brief Log incoming data segment events. */
        kKCPLogMask_InData = IKCP_LOG_IN_DATA,
        /** @brief Log incoming acknowledgment (ACK) segment events. */
        kKCPLogMask_InACK = IKCP_LOG_IN_ACK,
        /** @brief Log incoming window probe/advertisement events. */
        kKCPLogMask_InWins = IKCP_LOG_IN_WINS,
        /** @brief Log outgoing data segment events. */
        kKCPLogMask_OutData = IKCP_LOG_OUT_DATA,
        /** @brief Log outgoing acknowledgment (ACK) segment events. */
        kKCPLogMask_OutACK = IKCP_LOG_OUT_ACK,
        /** @brief Log outgoing window probe events. */
        kKCPLogMask_OutProbe = IKCP_LOG_OUT_PROBE,
        /** @brief Log outgoing window advertisement events. */
        kKCPLogMask_OutWins = IKCP_LOG_OUT_WINS,
    };

    /** @brief Maximum allowable KCP packet size in bytes. */
    static inline constexpr SizeType kMaxPacketSize = 1400;
    /** @brief Minimum KCP packet size in bytes (corresponding to the standard 24-byte protocol header). */
    static inline constexpr SizeType kPacketMinSize = 24;
    /** @brief Default threshold of pending send packets permitted before triggering backpressure. */
    static inline constexpr Int32 kDefaultMaxPendingPacketNum = 1024;

public:
    /**
     * @brief Constructs an uninitialized KCP session and ensures the global memory pool (_IsGlobal) is hooked.
     */
    ZKCPSession() noexcept;

    ZKCPSession(const ZKCPSession&) = delete;
    ZKCPSession(ZKCPSession&&) = delete;

    /**
     * @brief Destructor. Closes the session and releases the underlying KCP control block.
     */
    ~ZKCPSession() noexcept;

    ZKCPSession& operator=(const ZKCPSession&) = delete;
    ZKCPSession& operator=(ZKCPSession&&) = delete;

    /**
     * @brief Retrieves the current KCP update interval in milliseconds.
     * @return UInt32 Current update interval in milliseconds.
     */
    NODISCARD FORCEINLINE UInt32 GetUpdateIntervalMs() noexcept { return update_interval_ms_; }

    /**
     * @brief Retrieves the unique conversation ID associated with this KCP session.
     * @return UInt32 Conversation ID.
     */
    NODISCARD UInt32 GetConversationID() noexcept;

    /**
     * @brief Configures KCP fast mode and low-latency timing parameters. Open() must be called first.
     * @param _nodelay Whether to enable nodelay mode (0: disabled, 1: enabled for lower minimum RTO).
     * @param _interval Internal clock refresh interval in milliseconds (clamped between 10 ms and 5000 ms).
     * @param _resend Fast retransmit trigger threshold count (e.g., 2 skipped ACKs trigger retransmission).
     * @param _nc Whether to disable congestion control (0: normal congestion control, 1: disabled).
     */
    Void SetNoDelay(Int32 _nodelay, Int32 _interval, Int32 _resend, Int32 _nc) noexcept;

    /**
     * @brief Configures send and receive sliding window capacities. Open() must be called first.
     * @param _snd_wnd Maximum send window capacity measured in packet segments.
     * @param _rcv_wnd Maximum receive window capacity measured in packet segments.
     */
    Void SetWindowSize(Int32 _snd_wnd, Int32 _rcv_wnd) noexcept;

    /**
     * @brief Configures the Maximum Transmission Unit (MTU) size for packetization. Open() must be called first.
     * @param _mtu Maximum transmission unit size in bytes (defaults to 1400 bytes).
     */
    Void SetMaxTransmissionUnitSize(Int32 _mtu) noexcept;

    /**
     * @brief Configures the KCP session to operate in stream mode or message mode. Open() must be called first.
     * @param _mode 0 for message mode (default), 1 for stream mode (continuous byte stream).
     */
    Void SetStreamMode(Int32 _mode) noexcept;

    /**
     * @brief Sets the bitmask filter controlling internal KCP logging. Open() must be called first.
     * @param _log_mask Bitwise combination of LogMaskEnum_ flags.
     */
    Void SetLogMask(Int32 _log_mask) noexcept;

    /**
     * @brief Set the max retransmission attempts before considering the link dead. Open() must be called first.
     * @param _dead_link_times Maximum retransmission attempts before marking the link as dead.
     */
    Void SetDeadLinkTimes(UInt32 _dead_link_times) noexcept;

    /**
     * @brief Configures the maximum allowable backlog packets in the send queue before triggering backpressure.
     * @param _max_pending_packet_num Backlog threshold packet count.
     */
    FORCEINLINE Void SetMaxPendingPacketNum(Int32 _max_pending_packet_num) noexcept {
        max_pending_packet_num_ = _max_pending_packet_num;
    }

    /**
     * @brief Checks if the KCP session is closed.
     * @return Bool Returns true if the session is closed, false otherwise.
     */
    FORCEINLINE NODISCARD Bool IsClosed() noexcept {
		return kcp_context_ptr_ == nullptr;
    }


    /**
     * @brief Initializes and opens a new KCP session context with a conversation ID and output callback.
     * @param _conv_id Unique conversation identifier shared by both communicating endpoints.
     * @param _user_ptr Opaque user context pointer passed into low-level callbacks.
     * @param _output_callback_func Function pointer invoked when KCP needs to transmit raw UDP packets.
     */
    Void Open(
        UInt32 _conv_id,
        Void* _user_ptr,
        Int32(*_output_callback_func)(const Char*, Int32, ikcpcb*, Void*)
    ) noexcept;

    /**
     * @brief Closes the session and safely releases the internal KCP protocol control block.
     */
    Void Close() noexcept;

    /**
     * @brief Resets the dead link state of the KCP session.
     */
    Void ResetDeadLink() noexcept;

    /**
     * @brief Retrieves the current steady time in milliseconds for driving KCP timers.
     * @return UInt32 Current steady time in milliseconds.
     */
    static NODISCARD FORCEINLINE UInt32 GetCurrentTimeMs() noexcept { 
        return static_cast<UInt32>(ZSystemTime::GetSteadyTimeMs()); 
    }

    /**
     * @brief Determines the earliest future timestamp at which Update() must be invoked.
     * @return UInt32 Next scheduled update timestamp in milliseconds, or 0 if uninitialized.
     */
    NODISCARD UInt32 GetNextUpdateTimeMs() noexcept;

    /**
     * @brief Drives the KCP protocol state machine, flushing pending ACKs and handling timeout retransmissions.
     */
    Void Update() noexcept;

    /**
     * @brief Immediately flushes all queued outgoing messages and acknowledgments without waiting for timers.
     */
    Void Flush() noexcept;

    /**
     * @brief Submits user payload data into the transmission queue for fragmentation and reliable delivery.
     * @param _buffer Buffer containing the application data payload to send.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType Send(ZConstBuffer _buffer) noexcept;

    /**
     * @brief Demultiplexes and feeds a raw incoming UDP datagram from the network into the KCP protocol engine.
     * @param _buffer Buffer containing the raw received datagram bytes.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType ResolveReceivedPacket(ZConstBuffer _buffer) noexcept;

    /**
     * @brief Peeks at the byte size of the next fully assembled message awaiting in the receive queue.
     * @return Int32 Payload size in bytes if a full message is available, or a negative value if incomplete or empty.
     */
    NODISCARD Int32 GetNextMessageSize() noexcept;

    /**
     * @brief Retrieves and reassembles the next available user message from the receive queue into a buffer.
     * @param _buffer Destination buffer to receive the reassembled message bytes.
     * @param _msg_size_ptr Optional output pointer to store the actual received message size in bytes.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType GetNextMessage(ZBuffer _buffer, Int32* _msg_size_ptr = nullptr) noexcept;

    /**
     * @brief Discards the next message in the receive queue without reading it, useful for skipping unwanted messages.
     */
    Void SkipNextMessage() noexcept;

private:
    /** @brief Pointer to the underlying C-style KCP protocol control block. */
    ikcpcb* kcp_context_ptr_;
    /** @brief Maximum number of pending packets permitted in the send queue before backpressure rejection. */
    Int32 max_pending_packet_num_;
    /** @brief Current KCP update interval in milliseconds, used for driving the protocol state machine. */
    UInt32 update_interval_ms_;
};

} // socket
} // zengine