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

#include "z_udp_peer_manager.h"

namespace zengine {
namespace socket {

ZUDPPeerManagerP2P::ZUDPPeerManagerP2P(ZIOContext& _io_context) noexcept
    : SuperType_()
    , peer_ptr_array_()
    , worker_cv_()
    , worker_mutex_()
    , io_context_(_io_context)
    , is_running_(true)
	, tick_interval_ms_(kDefaultManagerTickIntervalMs)
    , request_queue_(kRequestQueueSize)
    , request_queue_consumer_token_(request_queue_.MakeConsumerToken())
    , worker_thread_(ZUDPPeerManagerP2P::WorkerThreadFuncP, this)
{}

ZUDPPeerManagerP2P::~ZUDPPeerManagerP2P() noexcept {
    is_running_ = false;
    worker_cv_.NotifyOne();
    if(worker_thread_.IsJoinable()) {
        worker_thread_.Join();
    }
	for (auto& peer_iter : peer_ptr_array_) {
		peer_iter->Close();
	}
}

NODISCARD TSharedPointer<ZUDPPeerP2P> ZUDPPeerManagerP2P::CreatePeer(
    TFunction<Void(ZUDPPeerP2P&, ZMemory&)>&& _receive_message_data_handle_func
) noexcept {
    auto peer_ptr = MakeShared<ZUDPPeerP2P>(
        io_context_, 
        std::forward<TFunction<Void(ZUDPPeerP2P&, ZMemory&)>>(_receive_message_data_handle_func)
    );
    request_queue_.PushBack(Request_(RequestEnum_::kAddPeer, peer_ptr));
    worker_cv_.NotifyOne();
    return peer_ptr;
}

Void ZUDPPeerManagerP2P::DestroyPeer(const TSharedPointer<ZUDPPeerP2P>& _peer_ptr) noexcept {
    request_queue_.PushBack(Request_(RequestEnum_::kRemovePeer, _peer_ptr));
	worker_cv_.NotifyOne();
}

Void ZUDPPeerManagerP2P::WorkerThreadFuncP(ZUDPPeerManagerP2P* _manager_ptr) noexcept {
    ReturnType link_code = kOK;
	TimeType last_tick_time_ms;

    while (_manager_ptr->is_running_) { 
		// Update the last tick time to the current steady time in milliseconds.
        last_tick_time_ms = ZSystemTime::GetSteadyTimeMs();

		// Process any pending requests in the request queue.
        _manager_ptr->ProcessRequestsP();

        // If the peer array is empty, wait for a notification before continuing.
        if(_manager_ptr->peer_ptr_array_.IsEmpty()) {
            {
                TUniqueLock<ZMutex> lock(_manager_ptr->worker_mutex_);
                _manager_ptr->worker_cv_.Wait(
                    lock, 
                    [_manager_ptr](){ 
                        return 
                            _manager_ptr->is_running_ == false || 
                            _manager_ptr->request_queue_.GetApproximateSize() > 0;
                    }
                );
            }
            continue;
        }

        // Iterate through the peer array and perform necessary operations on each peer.
		for (auto& peer_ptr_iter : _manager_ptr->peer_ptr_array_) {

            // Get a reference to the current peer in the iteration.
            ZUDPPeerP2P& peer = *peer_ptr_iter;
            
			// If the peer is not in the connected state, skip to the next peer.
            if (peer.state_ != ZUDPPeerP2P::StateEnum_::kConnected) {
                continue;
            }

            // Lock the peer's work mutex to ensure thread-safe access to its state.
            TLockGuard lock_guard(peer.work_mutex_);

            // Handle receiving packets for the current peer.
            link_code = peer.HandleReceivePacketsP();
			if (link_code != kOK) {
				Z_LOG_ERROR(
					error_code::kZUDPPeerManagerErrorCode_LinkError, link_code,
					"ZUDPPeerP2P::HandleReceivePacketsP() link error!"
				);
                continue;
			}

			// Send any pending messages for the current peer.
            link_code = peer.SendMessagesP();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZUDPPeerManagerErrorCode_LinkError, link_code,
                    "ZUDPPeerP2P::SendMessagesP() link error!"
                );
                continue;
            }

			// Check the connection status of the current peer to ensure it is still active.
            peer.ConnectionTimeoutCheckP();
        }

		// Calculate the time to wait until the next tick based on the last tick time and the configured tick interval.
		TimeType next_tick_time_ms = last_tick_time_ms + _manager_ptr->tick_interval_ms_;
		TimeType wait_time_ms = next_tick_time_ms - ZSystemTime::GetSteadyTimeMs();
		if (wait_time_ms > 0LL) {
            {
                TUniqueLock<ZMutex> lock(_manager_ptr->worker_mutex_);
                _manager_ptr->worker_cv_.WaitForMs(
                    lock, wait_time_ms,
                    [_manager_ptr]() {
                        return
                            _manager_ptr->is_running_ == false ||
                            _manager_ptr->request_queue_.GetApproximateSize() > 0;
                    }
                    
                );
            }
		}
    }
}

Void ZUDPPeerManagerP2P::ProcessRequestsP() noexcept {
    Request_ request_;

    // Process requests from the request queue
    while (request_queue_.TryPopFront(request_queue_consumer_token_, request_)) {
        switch (request_.type_) {
            // Add peer to the peer array
        case RequestEnum_::kAddPeer:
            peer_ptr_array_.PushBack(std::move(request_.peer_ptr_));
            break;

            // Remove peer from the peer array
        case RequestEnum_::kRemovePeer:
			// Close the peer if it is still open.
            if (request_.peer_ptr_->GetState() != ZUDPPeerP2P::StateEnum_::kClosed) {
                request_.peer_ptr_->Close();
            }
            peer_ptr_array_.EraseFirstUnordered(request_.peer_ptr_);
            break;

        default:
            Z_LOG_ERROR(
                error_code::kZUDPPeerManagerErrorCode_ParamOutOfRange, 0,
                "Parameter out of range! request_.type_: {}",
                static_cast<Int32>(request_.type_)
            );
            break;
        }
    }
}


} // socket
} // zengine