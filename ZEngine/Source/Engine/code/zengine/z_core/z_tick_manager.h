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

#include "t_array.h"
#include "t_atom.h"
#include "t_function.h"
#include "t_queue.h"
#include "t_stack.h"
#include "z_condition_variable.h"
#include "z_mutex.h"
#include "z_object.h"
#include "z_thread.h"

namespace zengine {
namespace error_code {

/**
 * @brief Error codes specific to the ZTickManager class.
 */
enum ZTickManagerErrorCodeEnum : ReturnType::ValueType {
    /** @brief Error occurred during task linking or execution. */
    kZTickManagerErrorCode_LinkError = kErrorCodeBase_ZTickManager,
    /** @brief System or external library error. */
    kZTickManagerErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was provided. */
    kZTickManagerErrorCode_NullptrParam,
    /** @brief A parameter was out of the valid range. */
    kZTickManagerErrorCode_ParamOutOfRange
};
} // error_code
} // zengine

namespace zengine {

/**
 * @brief Manager for managing and executing tick callback functions.
 * @details Principles of implementation:
 *          - Driving Modes: Supports self-tick (driven by an internal dedicated thread) 
 *            and external tick (manually invoked via OnTick).
 *          - Thread Safety: Queue-based deferred request mechanism handles adding/removing 
 *            callbacks and changing intervals safely across threads.
 *          - Memory Efficiency: Active callbacks are stored in an array. Deleted callback 
 *            slots are marked and their indices are recycled via a stack to minimize reallocations.
 */
class CORE_DLLAPI CACHE_ALIGN ZTickManager : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

private:
    /** @brief The default self-tick interval in milliseconds. */
    static constexpr TimeType kDefaultSelfTickIntervalMs = 50LL;

public:
    /**
     * @brief Enum representing the execution mode of the tick manager.
     */
    enum class TickModeEnum_ : Int32 {
        /** @brief Ticking is driven by an internal dedicated thread. */
        kSelfTick,
        /** @brief Ticking is driven by an external caller invoking OnTick. */
        kExternalTick
    };

public:
    /**
     * @brief Gets the singleton instance of the ZTickManager.
     * @return Reference to the singleton instance of ZTickManager.
     */
    NODISCARD static ZTickManager& GetInstance() noexcept;

    /**
     * @brief Sets the tick mode of the manager.
     * @param _mode The tick mode to set.
     */
    Void SetTickMode(TickModeEnum_ _mode) noexcept;

    /**
     * @brief Sets the interval of the self-tick thread in milliseconds.
     * @param _ms The interval in milliseconds.
     */
    Void SetSelfTickIntervalMs(TimeType _ms) noexcept;

    /**
     * @brief Adds a callback function to be executed on each tick.
     * @param _callback_func The simple function callback taking delta time in seconds.
     */
    Void AddTickFunction(TSimpleFunction<Void(Float32)> _callback_func) noexcept;

    /**
     * @brief Removes a registered tick callback function.
     * @param _callback_func The callback function to be removed.
     */
    Void RemoveTickFunction(TSimpleFunction<Void(Float32)> _callback_func) noexcept;

    /**
     * @brief Removes a registered tick callback function immediately, will block the thread until removed.
     * @param _callback_func The callback function to be removed.
     */
    Void RemoveTickFunctionImmediately(TSimpleFunction<Void(Float32)> _callback_func) noexcept;

    /**
     * @brief Executes a single tick step, invoking all registered callbacks.
     * @param _delta_sec The time elapsed since the last tick in seconds.
     */
    Void OnTick(Float32 _delta_sec) noexcept;

private:
    /**
     * @brief Static entry function for the self-tick thread.
     * @param _manager_ptr Pointer to the ZTickManager instance.
     */
    static Void SelfTickThreadFuncP(ZTickManager* _manager_ptr) noexcept;

    /**
     * @brief Default constructor.
     */
    ZTickManager() noexcept;

    /** @brief Deleted copy constructor. */
    ZTickManager(const ZTickManager&) = delete;

    /** @brief Deleted move constructor. */
    ZTickManager(ZTickManager&&) = delete;

    /**
     * @brief Destructor.
     */
    ~ZTickManager() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZTickManager& operator=(const ZTickManager&) = delete;

    /** @brief Deleted move assignment operator. */
    ZTickManager& operator=(ZTickManager&&) = delete;

private:
    /**
     * @brief Enum representing the internal state of the tick manager.
     */
    enum class StateEnum_ : Int32 {
        /** @brief The self-tick thread is initializing. */
        kSelfTickInitializing,
        /** @brief The self-tick thread is actively running. */
        kSelfTicking,
        /** @brief The self-tick thread is being stopped. */
        kSelfTickStopping,
        /** @brief The manager is in external tick mode. */
        kExternalTicking
    };

    /**
     * @brief Structure holding registered callback function details.
     */
    struct CallbackFunctionData_ {
        /** @brief The callback function wrapper. */
        TSimpleFunction<Void(Float32)> callback_func_;
        /** @brief Flag indicating if this callback has been marked for deletion. */
        Bool is_delete_;
    };

    /**
     * @brief Enum representing different types of deferred requests.
     */
    enum class RequestIDEnum_ : Int32 {
        /** @brief Request to add a tick function. */
        kAddTickFunction,
        /** @brief Request to remove a tick function. */
        kRemoveTickFunction,
        /** @brief Request to set self-tick interval. */
        kSetSelfTickInterval
    };

    /**
     * @brief Data structure for the add tick function request.
     */
    struct RequestDataAddTickFunction_ {
        /** @brief The callback function to add. */
        TSimpleFunction<Void(Float32)> callback_func_;
    };

    /**
     * @brief Data structure for the remove tick function request.
     */
    struct RequestDataRemoveTickFunction_ {
        /** @brief The callback function to remove. */
        TSimpleFunction<Void(Float32)> callback_func_;
    };

    /**
     * @brief Data structure for setting the self-tick interval request.
     */
    struct RequestDataSetSelfTickIntervalMs_ {
        /** @brief The target interval in milliseconds. */
        TimeType interval_ms_;
    };

    /**
     * @brief Union containing data for various request types.
     */
    union RequestDataUnion_ {
        /** @brief Data for adding a tick function. */
        RequestDataAddTickFunction_ add_tick_function_;
        /** @brief Data for removing a tick function. */
        RequestDataRemoveTickFunction_ remove_tick_function_;
        /** @brief Data for setting the self-tick interval. */
        RequestDataSetSelfTickIntervalMs_ set_self_tick_interval_ms_;

        FORCEINLINE RequestDataUnion_() noexcept {}
    };

    /**
     * @brief Structure representing a deferred request to be processed on the next tick.
     */
    struct Request_ {
        /** @brief The unique request identifier. */
        RequestIDEnum_ id_;
        /** @brief The payload data for the request. */
        RequestDataUnion_ data_;

        FORCEINLINE Request_() noexcept = default;

        FORCEINLINE Request_(const Request_& _request) noexcept {
            Copy(this, &_request, sizeof(Request_));
        }

        FORCEINLINE Request_(Request_&& _request) noexcept {
            Copy(this, &_request, sizeof(Request_));
        }

        FORCEINLINE Request_& operator=(const Request_& _request) noexcept {
            Copy(this, &_request, sizeof(Request_));
            return *this;
        }

        FORCEINLINE Request_& operator=(Request_&& _request) noexcept {
            Copy(this, &_request, sizeof(Request_));
            return *this;
        }
    };

private:
    /** @brief The current state of the tick manager. */
    TAtom<StateEnum_> state_;
    /** @brief Mutex to synchronize changes to the tick mode. */
    ZMutex tick_mode_change_mutex_;
    /** @brief Mutex to synchronize the self-tick timer. */
    ZMutex self_tick_timer_mutex_;
    /** @brief Mutex to protect access to the tick data. */
    ZMutex tick_mutex_;
    /** @brief Condition variable for the self-tick thread sleep-wake cycle. */
    ZConditionVariable self_tick_timer_cv_;
    /** @brief The interval for self-ticking in milliseconds. */
    TimeType self_tick_interval_ms_;
    /** @brief Array containing registered callbacks. */
    TArray<CallbackFunctionData_> callback_func_data_array_;
    /** @brief Thread-safe queue for buffering pending registration requests. */
    TQueueSafe<Request_> request_queue_;
    /** @brief Stack tracking indices of deleted elements to reuse them. */
    TStack<SizeType> deleted_callback_func_data_index_stack_;
    /** @brief The thread used for self-ticking mode. */
    ZThread tick_thread_;
};

} // zengine