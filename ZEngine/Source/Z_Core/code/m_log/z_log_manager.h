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

/**
 * @brief The log manager, logs the error and info to the console, file and any place that needs to log.
 *
 * This singleton class manages the logging thread, processes log queues, and handles registration
 * of output functions.
 */
class ZLogManager : public ZObject<> {
public:
    /**
     * @brief Enumeration for internal request types handled by the log manager.
     */
    enum class RequestTypeEnum_ {
        /** @brief Request to register a log output function. */
        kLogOutputRegisterRequest,
        /** @brief Request to unregister a log output function. */
        kLogOutputUnregisterRequest
    };

private:
    /**
     * @brief Data structure for log output registration requests.
     */
    struct LogOutputRegisterRequestDataStruct_ {
        /** @brief The output function pointer. */
        ZLog::OutputFunction_ _output_func;
        /** @brief Pointer to the array holding output functions. */
        ZLog::OutputFunctionArray_* output_func_array_ptr_;
    };
    /**
     * @brief Data structure for log output unregistration requests.
     */
    struct LogOutputUnregisterRequestDataStruct_ {
        /** @brief The output function pointer. */
        ZLog::OutputFunction_ _output_func;
        /** @brief Pointer to the array holding output functions. */
        ZLog::OutputFunctionArray_* output_func_array_ptr_;
    };
    /**
     * @brief Union holding data for different log requests.
     */
    union LogRequestDataUnion_ {
        /** @brief Data for registration requests. */
        LogOutputRegisterRequestDataStruct_ register_data_;
        /** @brief Data for unregistration requests. */
        LogOutputUnregisterRequestDataStruct_ unregister_data_;
    };
    /**
     * @brief Represents a generic request to the log manager.
     */
    struct Request_ {
        /** @brief The type of request. */
        RequestTypeEnum_ request_type_;
        /** @brief The data associated with the request. */
        LogRequestDataUnion_ request_data_;
    };

public:
    /** @brief The maximum size of the fixed log queue. */
    static inline constexpr SizeType kLogQueueSize = 4096;

    /** @brief Minimum log port ID. */
    static inline constexpr SizeType kLogPortIDMin = 0;
    /** @brief Maximum log port ID. */
    static inline constexpr SizeType kLogPortIDMax = kLogMaxPortNum - 1;

public:
    /**
     * @brief Retrieves the singleton instance of the ZLogManager.
     * @return ZLogManager& Reference to the singleton instance.
     */
    NODISCARD static ZLogManager& Instance() noexcept;

    /**
     * @brief Submits a log entry to be processed.
     * @param _log Unique pointer to the log object to be processed.
     */
    Void Log(
        TUniquePointer<ZLog>&& _log
    ) noexcept;

    /**
     * @brief Registers a new output function for logging.
     * @param _output_func The function pointer to register.
     * @param _output_func_array_ptr Pointer to the specific array where the function should be stored.
     */
    Void RegisterLogOutputFunction(
        ZLog::OutputFunction_ _output_func,
        ZLog::OutputFunctionArray_* _output_func_array_ptr
    ) noexcept;

    /**
     * @brief Unregisters an existing output function.
     * @param _output_func The function pointer to unregister.
     * @param _output_func_array_ptr Pointer to the specific array where the function is stored.
     */
    Void UnregisterLogOutputFunction(
        ZLog::OutputFunction_ _output_func,
        ZLog::OutputFunctionArray_* _output_func_array_ptr
    ) noexcept;

    /**
     * @brief Flushes pending logs and stops the logging thread.
     * @param _max_wait_time_ms Maximum time to wait for the flush operation in milliseconds.
     */
    Void FinishFlush() noexcept;

    /**
     * @brief Checks if the logging thread has finished execution.
     * 
     * This function returns the current state of the background logging thread.
     * It is typically used after calling FinishFlush() to verify shutdown completion.
     * 
     * @return Bool True if the logging thread has finished, False otherwise.
     */
    FORCEINLINE Bool IsFinished() noexcept { return log_thread_finished_.Value(); }

protected:
    /** @brief Base class alias. */
    using SuperType_ = ZObject;

private:
    /** @brief Deleted copy constructor. */
    ZLogManager(const ZLogManager&) = delete;
    /** @brief Deleted move constructor. */
    ZLogManager(ZLogManager&&) = delete;
    /** @brief Deleted copy assignment operator. */
    ZLogManager& operator=(const ZLogManager&) = delete;
    /** @brief Deleted move assignment operator. */
    ZLogManager& operator=(ZLogManager&&) = delete;

    /**
     * @brief Static worker thread function that processes logs.
     * @param _log_manager_ptr Pointer to the log manager instance.
     */
    static Void LogThread(ZLogManager* _log_manager_ptr) noexcept;

    /**
     * @brief Private constructor for singleton pattern.
     */
    ZLogManager() noexcept;

    /**
     * @brief Destructor.
     */
    ~ZLogManager() noexcept;

    /** @brief Atomic flag indicating if the log thread has finished. */
    TAtom<Bool> log_thread_finished_;
    /** @brief Thread-safe queue for administrative requests (register/unregister). */
    TQueueSafe<TUniquePointer<Request_>> request_queue_;
    /** @brief The background thread instance processing logs. */
    ZThread log_thread_;
    /** @brief Mutex protecting the log queue. */
    ZMutex log_queue_mutex_;
    /** @brief Fixed-size queue storing pending log objects. */
    TFixedQueue<TUniquePointer<ZLog>, kLogQueueSize> log_queue_;
};

}//log
}//zengine