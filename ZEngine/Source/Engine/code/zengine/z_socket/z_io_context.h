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

#include "../z_core/t_array.h"
#include "../z_core/t_atom.h"
#include "../z_core/t_optional.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "internal/l_asio.h"
#include "z_tcp_endpoint.h"
#include "z_udp_endpoint.h"

namespace zengine {
namespace error_code {

enum ZIOContextErrorCodeEnum : ReturnType::ValueType {
    kZIOContextErrorCode_LinkError = kErrorCodeBase_ZIOContext,
    kZIOContextErrorCode_SystemOrLibraryError,
    kZIOContextErrorCode_NullptrParam,
    kZIOContextErrorCode_ParamOutOfRange,
    kZIOContextErrorCode_OperationNotAllowed,
};

} // error_code
} // zengine

namespace zengine {
namespace socket {

/**
 * @brief A wrapper class for asio::io_context to manage asynchronous I/O operations and thread pooling.
 *
 * Implementation Principle:
 * This class encapsulates the standard asio::io_context to integrate it into the ZEngine architecture.
 * It maintains an atomic counter (running_thread_count_) to track the number of threads currently
 * executing the event loop (Run()). This allows for safe shutdown (Stop()) and restart (Restart())
 * mechanisms. The resolution functions utilize asio's resolvers to translate hostnames and ports
 * into compatible endpoints for TCP and UDP sockets. The class inherits from ZObject for memory management.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZIOContext : public ZObject {
public:
    /** @brief Type alias for the base class ZObject. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor.
     *
     * Initializes the underlying IO context and sets the running thread count to zero.
     */
    ZIOContext() noexcept;

    /** @brief Deleted copy constructor. */
    ZIOContext(const ZIOContext&) = delete;

    /** @brief Deleted move constructor. */
    ZIOContext(ZIOContext&&) = delete;

    /**
     * @brief Default destructor.
     */
    ~ZIOContext() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZIOContext& operator=(const ZIOContext&) = delete;

    /** @brief Deleted move assignment operator. */
    ZIOContext& operator=(ZIOContext&&) = delete;

    /**
    * @brief Checks if any threads are currently running the IO context.
    * @return True if the running thread count is greater than zero, false otherwise.
    */
    NODISCARD FORCEINLINE Bool IsRunning() noexcept { return running_thread_count_ != 0; }

    /**
     * @brief Gets the number of threads currently running the IO context.
     * @return The count of active threads executing Run().
     */
    NODISCARD FORCEINLINE SizeType GetRunningThreadCount() noexcept { return running_thread_count_; }

    /**
     * @brief Synchronously resolves a TCP hostname and port to a list of endpoints.
     * @param _host_str The hostname or IP address string.
     * @param _port_str The port number or service name string.
     * @return An optional containing an array of ZTCPEndpoint on success, or an empty optional on failure.
     */
    NODISCARD TOptional<TArray<ZTCPEndpoint>> ResolveTCP(
        ZStringView _host_str,
        ZStringView _port_str
    ) noexcept;

    /**
     * @brief Synchronously resolves a UDP hostname and port to a list of endpoints.
     * @param _host_str The hostname or IP address string.
     * @param _port_str The port number or service name string.
     * @return An optional containing an array of ZUDPEndpoint on success, or an empty optional on failure.
     */
    NODISCARD TOptional<TArray<ZUDPEndpoint>> ResolveUDP(
        ZStringView _host_str,
        ZStringView _port_str
    ) noexcept;

    /**
     * @brief Asynchronously resolves a TCP hostname and port.
     * @param _host_str The hostname or IP address string.
     * @param _port_str The port number or service name string.
     * @param _handle_func The callback function to be invoked with the resolution result (endpoints or error).
     */
    Void AsyncResolveTCP(
        ZStringView _host_str,
        ZStringView _port_str,
        TFunction<Void(TOptional<TArray<ZTCPEndpoint>>&&)> _handle_func
    ) noexcept;

    /**
     * @brief Asynchronously resolves a UDP hostname and port.
     * @param _host_str The hostname or IP address string.
     * @param _port_str The port number or service name string.
     * @param _handle_func The callback function to be invoked with the resolution result (endpoints or error).
     */
    Void AsyncResolveUDP(
        ZStringView _host_str,
        ZStringView _port_str,
        TFunction<Void(TOptional<TArray<ZUDPEndpoint>>&&)> _handle_func
    ) noexcept;

    /**
     * @brief Stops the IO context and cancels all pending operations.
     */
    Void Stop() noexcept;

    /**
     * @brief Restarts the IO context to allow it to be run again after being stopped.
     * @return The result of the restart operation.
     */
    NODISCARD ReturnType Restart() noexcept;

    /**
     * @brief Runs the IO context's event loop on the current thread.
     *
     * This function blocks until all work has finished or the context is stopped.
     * It increments the running thread count upon entry and decrements it upon exit.
     */
    Void Run() noexcept;

    /**
     * @brief Blocks the current thread until all other threads running the IO context have finished.
     */
    Void Join() noexcept;

private:
    friend class ZIOContextWorkGuard;
    friend class ZTCPSocket;
    friend class ZUDPSocket;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:
    /** @brief The underlying ASIO io_context object handling the event loop. */
    asio::io_context io_context_;
    /** @brief Atomic counter tracking the number of threads currently running the IO context. */
    TAtom<Int32> running_thread_count_;
};

} // socket
} // zengine