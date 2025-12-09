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

#include "drive/d_pch.h"

#include "m_log/z_log/z_log.h"
#include "t_atom.h"
#include "t_fixed_queue.h"
#include "t_queue.h"
#include "t_smart_pointer.h"
#include "z_thread.h"

namespace zengine {
namespace log {

/*
    The log manager, log's the error and info to the console, file and any place that needs to log.
*/
class ZLogManager : public ZObject {
public:
    enum class RequestTypeEnum_ {
        kLogOutputRegisterRequest,
        kLogOutputUnregisterRequest
    };

private:
    struct LogOutputRegisterRequestDataStruct_ {
        ZLog::OutputFunction_ _output_func;
        ZLog::OutputFunctionArray_* output_func_array_ptr_;
    };
    struct LogOutputUnregisterRequestDataStruct_ {
        ZLog::OutputFunction_ _output_func;
        ZLog::OutputFunctionArray_* output_func_array_ptr_;
    };
    union LogRequestDataUnion_ {
        LogOutputRegisterRequestDataStruct_ register_data_;
        LogOutputUnregisterRequestDataStruct_ unregister_data_;
    };
    struct Request_ {
        RequestTypeEnum_ request_type_;
        LogRequestDataUnion_ request_data_;
    };

public:
    static inline constexpr SizeType kLogQueueSize = 4096;

    static inline constexpr SizeType kLogPortIDMin = 0;
    static inline constexpr SizeType kLogPortIDMax = kLogMaxPortNum - 1;

public:
    NODISCARD static ZLogManager& Instance() noexcept;

    Void Log(
        TUniquePointer<ZLog>&& _log
    ) noexcept;

    Void RegisterLogOutputFunction(
        ZLog::OutputFunction_ _output_func,
        ZLog::OutputFunctionArray_* _output_func_array_ptr
    ) noexcept;

    Void UnregisterLogOutputFunction(
        ZLog::OutputFunction_ _output_func,
        ZLog::OutputFunctionArray_* _output_func_array_ptr
    ) noexcept;

    Void FinishFlush(TimeType _max_wait_time_ms) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZLogManager(const ZLogManager&) = delete;
    ZLogManager(ZLogManager&&) = delete;
    ZLogManager& operator=(const ZLogManager&) = delete;
    ZLogManager& operator=(ZLogManager&&) = delete;

    static Void LogThread(ZLogManager* _log_manager_ptr) noexcept;

    ZLogManager() noexcept;

    ~ZLogManager() noexcept;

    TFixedQueue<TUniquePointer<ZLog>, kLogQueueSize> log_queue_;
    ZMutex log_queue_mutex_;
    TQueueSafe<TUniquePointer<Request_>> request_queue_;
    TAtom<Bool> log_thread_finished_;
    ZThread log_thread_;
};

}//log
}//zengine