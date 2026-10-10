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

#include "z_log_manager.h"

#include "m_log.h"
#include "t_lock_guard.h"

namespace zengine {
namespace log {

NODISCARD ZLogManager& ZLogManager::GetInstance() noexcept {
    static ZLogManager log_manager;
    return log_manager;
}

Void ZLogManager::LogThread(ZLogManager* _log_manager_ptr) noexcept {
    _log_manager_ptr->log_thread_state_ = StateEnum_::kRunning;
    // tell the main thread that log thread is running
    _log_manager_ptr->log_thread_state_.NotifyOne();   

    while (_log_manager_ptr->log_thread_state_.Load() == StateEnum_::kRunning) {
        Bool if_log = false;

        // log manager request handle
        TUniquePointer<Request_> request_ptr;
        while (_log_manager_ptr->request_queue_.TryPopFront(request_ptr)) {
            auto& request_data = request_ptr->data_.register_;
            ZLog::OutputFunction_ request_func = request_data._output_func;
            ZLog::OutputFunctionArray_* request_func_array_ptr = request_data.output_func_array_ptr_;
            SizeType index;
            ZLog::OutputFunction_ index_func;

            switch (request_ptr->id_) {
            case RequestIDEnum_::kLogOutputRegisterRequest:
                for (index = 0; index < kOutputFunctionMaxNum; ++index) {
                    index_func = (*request_func_array_ptr)[index];
                    if (index_func) {
                        // Check for same output.
                        if (index_func == request_func) {
                            Z_LOG_ERROR(
                                error_code::kMLogErrorCode_LogPortOutputFunctionAlreadyRegistered, 0, 
                                "Output function already registered!"
                            );
                            break;
                        }
                    }
                    else {
                        // find empty output.
                        (*request_func_array_ptr)[index] = request_func;
                        break;
                    }
                }
                if (index == kOutputFunctionMaxNum) {
                    Z_LOG_ERROR(
                        error_code::kMLogErrorCode_LogPortOutputFunctionFull, 0, 
                        "Register failed, output function array full!"
                    );
                }
                break;
            case RequestIDEnum_::kLogOutputUnregisterRequest:
                for (index = 0; index < kOutputFunctionMaxNum; ++index) {
                    index_func = (*request_func_array_ptr)[index];
                    if (index_func == request_func) {
                        (*request_func_array_ptr)[index] = nullptr;
                        break;
                    }
                }
                break;
            }
        }

        // output log
        TUniquePointer<ZLog> log_ptr;
        while (_log_manager_ptr->log_queue_.TryPopFront(log_ptr)) {
            if_log = true;
            log_ptr->OutputLog();
        }

        // sleep if no log to output
        if (!if_log) {
            ZThread::SleepMs(100);
        }
    }

    // output remaining log
    TUniquePointer<ZLog> log_ptr;
    while (_log_manager_ptr->log_queue_.TryPopFront(log_ptr)) {
        log_ptr->OutputLog();
    }
}

Void ZLogManager::Log(
    TUniquePointer<ZLog>&& _log
) noexcept {
    log_queue_.PushBack(std::move(_log));
}

Void ZLogManager::RegisterLogOutputFunction(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* _output_func_array_ptr
) noexcept {
    auto request_ptr = MakeUnique<Request_>();
    request_ptr->id_ = RequestIDEnum_::kLogOutputRegisterRequest;
    auto& request_data = request_ptr->data_.register_;
    request_data._output_func = _output_func;
    request_data.output_func_array_ptr_ = _output_func_array_ptr;
    request_queue_.EmplaceBack(std::move(request_ptr));
}

Void ZLogManager::UnregisterLogOutputFunction(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* _output_func_array_ptr
) noexcept {
    auto request_ptr = MakeUnique<Request_>();
    request_ptr->id_ = RequestIDEnum_::kLogOutputUnregisterRequest;
    auto& request_data = request_ptr->data_.unregister_;
    request_data._output_func = _output_func;
    request_data.output_func_array_ptr_ = _output_func_array_ptr;
    request_queue_.EmplaceBack(std::move(request_ptr));
}

Void ZLogManager::StartThread() noexcept {
    log_thread_ = ZThread(ZLogManager::LogThread, this);
    // wait for log thread to start
    log_thread_state_.Wait(StateEnum_::kIdle);
}

Void ZLogManager::FinishFlush() noexcept {
    log_thread_state_ = StateEnum_::kFinished;
    if (log_thread_.IsJoinable()) {
        log_thread_.Join();
    }
}

ZLogManager::ZLogManager() noexcept 
    : log_queue_()
    , log_thread_state_(StateEnum_::kIdle)
    , request_queue_()
    , log_thread_() 
{}

} // log
} // zengine