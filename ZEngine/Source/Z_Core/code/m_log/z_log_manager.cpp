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

NODISCARD ZLogManager& ZLogManager::Instance() noexcept {
    static ZLogManager log_manager;
    return log_manager;
}

Void ZLogManager::LogThread(ZLogManager* _log_manager_ptr) noexcept {
    Bool if_log = true;

    while (_log_manager_ptr->log_thread_finished_ == false) {
        if_log = false;

        //log manager request handle
        while (_log_manager_ptr->request_queue_.Size() > 0ULL) {
            TUniquePointer<Request_> request_ptr = std::move(_log_manager_ptr->request_queue_.Front());
            _log_manager_ptr->request_queue_.PopFront();
            auto& request_data = request_ptr->request_data_.register_data_;
            ZLog::OutputFunction_ request_func = request_data._output_func;
            ZLog::OutputFunctionArray_* request_func_array_ptr = request_data.output_func_array_ptr_;
            SizeType index;
            ZLog::OutputFunction_ index_func;

            switch (request_ptr->request_type_) {
            case RequestTypeEnum_::kLogOutputRegisterRequest:
                for (index = 0; index < kOutputFunctionMaxNum; ++index) {
                    index_func = (*request_func_array_ptr)[index];
                    if (index_func) {
                        //Check for same output.
                        if (index_func == request_func) {
                            Z_LOG_ERROR(
                                error_code::kMLogErrorCode_LogPortOutputFunctionAlreadyRegistered, 0, 
                                "Output function already registered!"
                            );
                            break;
                        }
                    }
                    else {
                        //find empty output.
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
            case RequestTypeEnum_::kLogOutputUnregisterRequest:
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

        //output log
        while (_log_manager_ptr->log_queue_.Size() > 0ULL) {
            if_log = true;
            TUniquePointer<ZLog> log = std::move(_log_manager_ptr->log_queue_.Front());
            _log_manager_ptr->log_queue_.PopFront();
            log->OutputLog();
        }

        //log str
        if (!if_log) {
            SleepMs(100);
        }
    }

    //output remaining log
    while (_log_manager_ptr->log_queue_.Size() > 0ULL) {
        if_log = true;
        _log_manager_ptr->log_queue_mutex_.Lock();
        TUniquePointer<ZLog> log = std::move(_log_manager_ptr->log_queue_.Front());
        _log_manager_ptr->log_queue_.PopFront();
        _log_manager_ptr->log_queue_mutex_.Unlock();
        log->OutputLog();
    }
}

Void ZLogManager::Log(
    TUniquePointer<ZLog>&& _log
) noexcept {
    TLockGuard lock_guard{log_queue_mutex_};
    if (!log_queue_.Full()) {
        log_queue_.PushBack(std::forward<TUniquePointer<ZLog>>(_log));
    }
}

Void ZLogManager::RegisterLogOutputFunction(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* _output_func_array_ptr
) noexcept {
    auto request_ptr = MakeUnique<Request_>();
    request_ptr->request_type_ = RequestTypeEnum_::kLogOutputRegisterRequest;
    auto& request_data = request_ptr->request_data_.register_data_;
    request_data._output_func = _output_func;
    request_data.output_func_array_ptr_ = _output_func_array_ptr;
    request_queue_.EmplaceBack(std::move(request_ptr));
}

Void ZLogManager::UnregisterLogOutputFunction(
    ZLog::OutputFunction_ _output_func,
    ZLog::OutputFunctionArray_* _output_func_array_ptr
) noexcept {
    auto request_ptr = MakeUnique<Request_>();
    request_ptr->request_type_ = RequestTypeEnum_::kLogOutputUnregisterRequest;
    auto& request_data = request_ptr->request_data_.unregister_data_;
    request_data._output_func = _output_func;
    request_data.output_func_array_ptr_ = _output_func_array_ptr;
    request_queue_.EmplaceBack(std::move(request_ptr));
}

Void ZLogManager::FinishFlush(TimeType _max_wait_time_ms) noexcept {
    log_thread_finished_ = true;
    if (log_thread_.Joinable()) {
        log_thread_.Join(_max_wait_time_ms);
    }
}

ZLogManager::ZLogManager() noexcept 
    : log_queue_()
    , log_queue_mutex_()
    , request_queue_()
    , log_thread_finished_(false)
    , log_thread_(ZLogManager::LogThread, this) 
{}

ZLogManager::~ZLogManager() noexcept {}

}//log
}//zengine