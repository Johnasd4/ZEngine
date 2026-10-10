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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_tick_manager.h"

#include "t_lock_guard.h"
#include "t_unique_lock.h"
#include "z_system_time.h"

namespace zengine {

NODISCARD ZTickManager& ZTickManager::GetInstance() noexcept {
    static ZTickManager manager;
    return manager;
}

Void ZTickManager::SetTickMode(TickModeEnum_ _mode) noexcept {
    TLockGuard lock_guard(tick_mode_change_mutex_);

    switch (_mode) {
    case TickModeEnum_::kSelfTick:
        if (state_ == StateEnum_::kExternalTicking) {
            state_ = StateEnum_::kSelfTickInitializing;
            tick_thread_ = ZThread(SelfTickThreadFuncP, this);
            // Wait for the self-tick thread to initialize before allowing the main thread to proceed.
            state_.Wait(StateEnum_::kSelfTickInitializing);
        }
        break;
    case TickModeEnum_::kExternalTick:
        // If currently in self-tick mode, signal the self-tick thread to stop and wait for it to finish.
        state_ = StateEnum_::kSelfTickStopping;
        // Notify the self-tick thread to wake up and check the stop signal.
        self_tick_timer_cv_.NotifyOne();
        if (tick_thread_.IsJoinable()) {
            tick_thread_.Join();
        }
        state_ = StateEnum_::kExternalTicking;
        break;
    default:
        Z_LOG_ERROR(
            error_code::kZTickManagerErrorCode_ParamOutOfRange, 0,
            "Invalid tick mode: {}", static_cast<Int32>(_mode)
        );
        return;
    }
}

Void ZTickManager::SetSelfTickIntervalMs(TimeType _ms) noexcept {
    Request_ request;
    request.id_ = RequestIDEnum_::kSetSelfTickInterval;
    request.data_.set_self_tick_interval_ms_.interval_ms_ = _ms;
    request_queue_.EmplaceBack(request);
}

Void ZTickManager::AddTickFunction(TSimpleFunction<Void(Float32)> _callback_func) noexcept {
    Request_ request;
    request.id_ = RequestIDEnum_::kAddTickFunction;
    request.data_.add_tick_function_.callback_func_ = _callback_func;
    request_queue_.EmplaceBack(request);
}

Void ZTickManager::RemoveTickFunction(TSimpleFunction<Void(Float32)> _callback_func) noexcept {
    Request_ request;
    request.id_ = RequestIDEnum_::kRemoveTickFunction;
    request.data_.remove_tick_function_.callback_func_ = _callback_func;
    request_queue_.EmplaceBack(request);
}

Void ZTickManager::RemoveTickFunctionImmediately(TSimpleFunction<Void(Float32)> _callback_func) noexcept {
    TLockGuard lock_guard(tick_mutex_);
    for (size_t index = 0; index < callback_func_data_array_.GetSize(); ++index) {
        CallbackFunctionData_& func_data = callback_func_data_array_[index];
        if (
            !func_data.is_delete_ &&
            func_data.callback_func_ == _callback_func
        ) {
            func_data.is_delete_ = true;
            deleted_callback_func_data_index_stack_.PushBack(index);
            break;
        }
    }
}

Void ZTickManager::OnTick(Float32 _delta_sec) noexcept {
    TLockGuard lock_guard(tick_mutex_);

    // Handle all pending requests in the queue before processing tick functions.
    Request_ request = {};

    while (request_queue_.TryPopFront(request)) {
        switch (request.id_) {
        case RequestIDEnum_::kAddTickFunction:
            // Reuse deleted function slots if available, otherwise add a new entry.
            if (!deleted_callback_func_data_index_stack_.IsEmpty()) {
                SizeType index = deleted_callback_func_data_index_stack_.Back();
                deleted_callback_func_data_index_stack_.PopBack();
                callback_func_data_array_[index].callback_func_ = request.data_.add_tick_function_.callback_func_;
                callback_func_data_array_[index].is_delete_ = false;
            }
            else {
                callback_func_data_array_.EmplaceBack(CallbackFunctionData_{
                    .callback_func_ = request.data_.add_tick_function_.callback_func_,
                    .is_delete_ = false
                });
            }
            break;
        case RequestIDEnum_::kRemoveTickFunction:
            for (size_t index = 0; index < callback_func_data_array_.GetSize(); ++index) {
                CallbackFunctionData_& func_data = callback_func_data_array_[index];
                if (
                    !func_data.is_delete_ &&
                    func_data.callback_func_ == request.data_.remove_tick_function_.callback_func_
                ){
                    func_data.is_delete_ = true;
                    deleted_callback_func_data_index_stack_.PushBack(index);
                    break;
                }
            }
            break;
        case RequestIDEnum_::kSetSelfTickInterval:
            self_tick_interval_ms_ = request.data_.set_self_tick_interval_ms_.interval_ms_;
            break;
        default:
            Z_LOG_ERROR(
                error_code::kZTickManagerErrorCode_ParamOutOfRange, 0,
                "Invalid request ID: {}", static_cast<Int32>(request.id_)
            );
            break;
        }
    }

    // Process all tick functions.
    for (size_t index = 0; index < callback_func_data_array_.GetSize(); ++index) {
        if (!callback_func_data_array_[index].is_delete_) {
            callback_func_data_array_[index].callback_func_(_delta_sec);
        }
    }
}

Void ZTickManager::SelfTickThreadFuncP(ZTickManager* _manager_ptr) noexcept {
    TimeType next_tick_time = ZSystemTime::GetTimeMs();
    TimeType previous_tick_time = next_tick_time;
    Bool stop_signal_received = false;

    _manager_ptr->state_ = StateEnum_::kSelfTicking;
    _manager_ptr->state_.NotifyAll(); // Notify that the self-tick thread has initialized.

    while (_manager_ptr->state_ == StateEnum_::kSelfTicking) {
        TimeType current_time = ZSystemTime::GetTimeMs();

        // Wait until the next tick time or until a stop signal is received.
        {
            TUniqueLock unique_lock(_manager_ptr->self_tick_timer_mutex_);

            while (next_tick_time >= current_time) {
                // end
                if (_manager_ptr->self_tick_timer_cv_.WaitForMs(unique_lock, next_tick_time - current_time)) {
                    if (_manager_ptr->state_ != StateEnum_::kSelfTicking) {
                        stop_signal_received = true;
                        break;
                    }
                }
                current_time = ZSystemTime::GetTimeMs();
            }

            // Check if a stop signal was received.
            if (stop_signal_received) {
                break;
            }
        }

        // Update current time.
        current_time = ZSystemTime::GetTimeMs();

        // Calculate next tick time. 
        // Ensure it is always in the future to prevent immediate ticks if the processing time exceeds the interval.
        next_tick_time += _manager_ptr->self_tick_interval_ms_;
        if (next_tick_time <= current_time) {
            next_tick_time = current_time + _manager_ptr->self_tick_interval_ms_;
        }

        // Calculate delta time in seconds.
        Float32 delta_sec = 
            static_cast<Float32>(current_time - previous_tick_time) / static_cast<Float32>(ZSystemTime::kMsPerSec);
        previous_tick_time = current_time;

        // Tick the manager.
        _manager_ptr->OnTick(delta_sec);
    }
}

ZTickManager::ZTickManager() noexcept
    : SuperType_()
    , state_(StateEnum_::kSelfTickInitializing)
    , tick_mode_change_mutex_()
    , self_tick_timer_mutex_()
    , self_tick_timer_cv_()
    , self_tick_interval_ms_(kDefaultSelfTickIntervalMs)
    , callback_func_data_array_()
    , request_queue_()
    , deleted_callback_func_data_index_stack_()
    , tick_thread_(SelfTickThreadFuncP, this)
{
    // Wait for the self-tick thread to initialize before allowing the main thread to proceed.
    state_.Wait(StateEnum_::kSelfTickInitializing);
}

ZTickManager::~ZTickManager() noexcept {
    TLockGuard lock_guard(tick_mode_change_mutex_);
    // If the self-tick thread is still running, signal it to stop and wait for it to finish.
    if (tick_thread_.IsJoinable()) {
        state_ = StateEnum_::kSelfTickStopping;
        self_tick_timer_cv_.NotifyOne();
        tick_thread_.Join();
    }
}

} // zengine