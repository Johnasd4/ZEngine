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

#include "../z_core/t_queue_mpmc.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_condition_variable.h"
#include "../z_core/z_mutex.h"

#include "z_io_context.h"
#include "z_udp_peer.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to the UDP peer manager.
 * Returns kOK on success, or one of these error codes on failure when using ReturnType.
 */
enum ZUDPPeerManagerErrorCodeEnum : ReturnType::ValueType {
    /** @brief Communication link error encountered during packet handling or message sending. */
    kZUDPPeerManagerErrorCode_LinkError = kErrorCodeBase_ZUDPPeerManager,
    /** @brief Underlying system or socket network library error occurred. */
    kZUDPPeerManagerErrorCode_SystemOrLibraryError,
    /** @brief An unexpected nullptr argument was provided. */
    kZUDPPeerManagerErrorCode_NullptrParam,
    /** @brief Provided parameter value is out of the valid range. */
    kZUDPPeerManagerErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine
namespace zengine {
namespace socket {

/**
 * @brief Manages the lifecycle, scheduling, and network I/O servicing of peer-to-peer UDP peers.
 *
 * Implementation Principles:
 * - Threading Architecture: Drives network operations through a dedicated background worker thread (`worker_thread_`)
 *   running a periodic tick loop at a configurable time interval (`tick_interval_ms_`).
 * - Lock-Free Request Decoupling: External operations such as CreatePeer() and DestroyPeer() do not mutate the
 *   internal peer array directly. Instead, they enqueue lightweight command structs into a lock-free MPMC queue
 *   (`request_queue_`), fully decoupling calling threads from the worker thread.
 * - Consumer Token Optimization: The worker thread utilizes a thread-bound `request_queue_consumer_token_` to pop
 *   requests in batches without contending with producer threads on global queue metadata.
 * - Adaptive Sleep and Low Latency: When no peers exist, the worker thread suspends indefinitely on `worker_cv_`.
 *   When peers are active, it calculates the remaining frame time and enters a timed wait (`WaitForMs`), which wakes up
 *   immediately if new requests arrive or shutdown is signaled.
 * - Granular Peer Synchronization: The worker thread sequentially processes peers, acquiring each peer's individual
 *   `work_mutex_` only during packet reception and message transmission, ensuring thread-safe peer state access.
 * - O(1) Peer Removal: Peers are removed using `EraseFirstUnordered`, eliminating the O(N) memory shift penalty
 *   associated with sequential vector deletion.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZUDPPeerManagerP2P : public ZObject {
public:
    /** @brief Super class type alias. */
    using SuperType_ = ZObject;

    /** @brief Default tick interval of the manager thread in milliseconds. */
    static inline constexpr TimeType kDefaultManagerTickIntervalMs = 5;

private:
    /** @brief Initial capacity of the internal request queue. */
    static inline constexpr SizeType kRequestQueueSize = 1024;

public:
    /**
     * @brief Constructs the peer manager and launches the background worker thread.
     * @param _io_context Reference to the I/O context used for socket operations.
     */
    ZUDPPeerManagerP2P(ZIOContext& _io_context) noexcept;

    ZUDPPeerManagerP2P(const ZUDPPeerManagerP2P&) = delete;
    ZUDPPeerManagerP2P(ZUDPPeerManagerP2P&&) = delete;

    /**
     * @brief Destructs the peer manager, notifying and joining the worker thread.
     */
    ~ZUDPPeerManagerP2P() noexcept;

    ZUDPPeerManagerP2P& operator=(const ZUDPPeerManagerP2P&) = delete;
    ZUDPPeerManagerP2P& operator=(ZUDPPeerManagerP2P&&) = delete;

    /**
     * @brief Retrieves the current tick interval of the manager worker thread.
     * @return TimeType The tick interval in milliseconds.
     */
    NODISCARD FORCEINLINE TimeType GetTickInterval() noexcept { return tick_interval_ms_; }

    /**
     * @brief Sets the tick interval of the manager worker thread.
     * @param _tick_interval_ms Target tick interval in milliseconds.
     * @return Void None.
     */
    FORCEINLINE Void SetTickInterval(TimeType _tick_interval_ms) noexcept {
        tick_interval_ms_ = _tick_interval_ms;
    }

    /**
     * @brief Creates a new UDP peer, enqueues an addition request, and returns its shared pointer.
     * @param _receive_message_data_handle_func Callback function invoked when message data is received by the peer.
     * @return TSharedPointer<ZUDPPeerP2P> Shared pointer to the newly created peer instance.
     */
    NODISCARD TSharedPointer<ZUDPPeerP2P> CreatePeer(
        TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _receive_message_data_handle_func
    ) noexcept;

    /**
     * @brief Enqueues a removal request to disconnect and destroy the specified peer.
     * @param _peer_ptr Shared pointer to the peer to be destroyed.
     * @return Void None.
     */
    Void DestroyPeer(const TSharedPointer<ZUDPPeerP2P>& _peer_ptr) noexcept;

private:
    /**
     * @brief Types of asynchronous requests handled by the manager worker thread.
     */
    enum class RequestEnum_ : Int32 {
        /** @brief Request to add a peer to the active array. */
        kAddPeer,
        /** @brief Request to remove a peer from the active array. */
        kRemovePeer
    };

    /**
     * @brief Request command structure transferred through the lock-free queue.
     */
    struct Request_ {
        /** @brief Type of the queued request operation. */
        RequestEnum_ type_;
        /** @brief Target peer instance associated with this request. */
        TSharedPointer<ZUDPPeerP2P> peer_ptr_;
    };

private:
    /**
     * @brief Entry routine executed by the background worker thread.
     * @param _manager_ptr Pointer to the ZUDPPeerManagerP2P instance to service.
     * @return Void None.
     */
    static Void WorkerThreadFuncP(ZUDPPeerManagerP2P* _manager_ptr) noexcept;

    /**
     * @brief Dequeues and processes all pending requests from the request queue.
     * @return Void None.
     */
    Void ProcessRequestsP() noexcept;

private:
    /** @brief Array of active UDP peer shared pointers managed by this instance. */
    TArray<TSharedPointer<ZUDPPeerP2P>> peer_ptr_array_;
    /** @brief Condition variable used to wake or sleep the worker thread. */
    ZConditionVariable worker_cv_;
    /** @brief Mutex used in conjunction with worker_cv_ for synchronization. */
    ZMutex worker_mutex_;
    /** @brief Reference to the I/O context powering socket operations. */
    ZIOContext& io_context_;
    /** @brief Atomic flag indicating whether the background worker thread should continue running. */
    CACHE_ALIGN TAtom<Bool> is_running_;
    /** @brief Atomic tick interval in milliseconds between update iterations. */
    TAtom<TimeType> tick_interval_ms_;
    /** @brief Lock-free MPMC queue buffering asynchronous requests from calling threads. */
    CACHE_ALIGN TQueueMPMC<Request_> request_queue_;
    /** @brief Thread-bound consumer token optimizing request queue pop throughput. */
    TQueueMPMC<Request_>::ConsumerToken_ request_queue_consumer_token_;
    /** @brief Background worker thread executing the manager servicing loop. */
    ZThread worker_thread_;
};

} // socket
} // zengine