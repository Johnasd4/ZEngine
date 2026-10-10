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

#include "z_core/t_function.h"
#include "z_core/t_list.h"
#include "z_core/t_stack.h"

namespace zengine {
namespace socket {

class ZTCPSocket;
class ZUDPSocket;

}// socket
}// zengine

namespace zengine {
namespace socket {
namespace internal {

/**
 * @brief Manager for delayed and thread-safe destruction of TCP sockets.
 * 
 * Implementation Principle:
 * To safely manage socket lifecycles in asynchronous network programming, this class employs a 
 * delayed deletion mechanism. Newly requested deletions are pushed to a thread-safe stack. A scheduled 
 * tick function then periodically polls these sockets, moving them to a tracking list. It continuously 
 * checks the asynchronous operation count of each socket. Once the count drops to zero (meaning no 
 * pending read/write callbacks), the socket is safely destructed and its memory is reclaimed or handled 
 * by a provided custom callback, thereby strictly avoiding use-after-free and data race conditions.
 */
class CACHE_ALIGN ZTCPSocketDeleteManager {
private:
    /**
     * @brief Internal structure to encapsulate the socket pointer and its destruction callback.
     */
    struct SocketData_ {
        /** @brief Pointer to the TCP socket to be managed and deleted. */
        ZTCPSocket* socket_ptr_;
        /** @brief Custom destruction function invoked after the socket's destructor is called. */
        TFunction<Void()> destruct_func_;
    };

public:
    /**
     * @brief Retrieves the singleton instance of the TCP socket delete manager.
     * @return ZTCPSocketDeleteManager& Reference to the global singleton manager instance.
     */
    NODISCARD static ZTCPSocketDeleteManager& GetInstance() noexcept;

    /**
     * @brief Tick function executed periodically to process pending socket deletions.
     * @param _delta_sec The time elapsed since the previous tick, in seconds.
     * @return Void No return value.
     */
    NODISCARD static Void DeleteTickFunc(Float32 _delta_sec) noexcept;

    /**
     * @brief Default constructor for initializing the TCP socket delete manager.
     */
    ZTCPSocketDeleteManager() noexcept;
    
    /**
     * @brief Default destructor for the TCP socket delete manager.
     */
    ~ZTCPSocketDeleteManager() noexcept;

    /**
     * @brief Enqueues a TCP socket for delayed thread-safe deletion.
     * @param _socket_ptr Pointer to the TCP socket that needs to be deleted.
     * @param _destruct_func Optional custom callback function to execute for memory deallocation.
     * @return Void No return value.
     */
    Void DeleteSocket(
        ZTCPSocket* _socket_ptr, 
        TFunction<Void()>&& _destruct_func = TFunction<Void()>()
    ) noexcept;

private:
    /** @brief Thread-safe stack buffering sockets newly submitted for deletion. */
    TStackSafe<SocketData_> new_socket_ptr_stack_;
    /** @brief Internal list storing sockets actively being monitored for zero async operations. */
    TList<SocketData_> socket_ptr_list_;
};

/**
 * @brief Manager for delayed and thread-safe destruction of UDP sockets.
 * 
 * Implementation Principle:
 * Similar to the TCP socket manager, this class ensures safe memory lifecycle management for UDP sockets. 
 * It buffers incoming deletion requests in a thread-safe stack. During each scheduled tick, it transfers 
 * these requests to a tracking list and verifies the asynchronous operation count. A UDP socket is only 
 * destructed and its memory released or handed over to a custom callback when all pending asynchronous IO 
 * operations (count reaches zero) are fully resolved, thus eliminating use-after-free risks.
 */
class CACHE_ALIGN ZUDPSocketDeleteManager {
private:
    /**
     * @brief Internal structure to encapsulate the socket pointer and its destruction callback.
     */
    struct SocketData_ {
        /** @brief Pointer to the UDP socket to be managed and deleted. */
        ZUDPSocket* socket_ptr_;
        /** @brief Custom destruction function invoked after the socket's destructor is called. */
        TFunction<Void()> destruct_func_;
    };

public:
    /**
     * @brief Retrieves the singleton instance of the UDP socket delete manager.
     * @return ZUDPSocketDeleteManager& Reference to the global singleton manager instance.
     */
    NODISCARD static ZUDPSocketDeleteManager& GetInstance() noexcept;

    /**
     * @brief Tick function executed periodically to process pending socket deletions.
     * @param _delta_sec The time elapsed since the previous tick, in seconds.
     * @return Void No return value.
     */
    NODISCARD static Void DeleteTickFunc(Float32 _delta_sec) noexcept;

    /**
     * @brief Default constructor for initializing the UDP socket delete manager.
     */
    ZUDPSocketDeleteManager() noexcept;

    /**
     * @brief Default destructor for the UDP socket delete manager.
     */
    ~ZUDPSocketDeleteManager() noexcept;

    /**
     * @brief Enqueues a UDP socket for delayed thread-safe deletion.
     * @param _socket_ptr Pointer to the UDP socket that needs to be deleted.
     * @param _destruct_func Optional custom callback function to execute for memory deallocation.
     * @return Void No return value.
     */
    Void DeleteSocket(
        ZUDPSocket* _socket_ptr,
        TFunction<Void()>&& _destruct_func = TFunction<Void()>()
    ) noexcept;

private:
    /** @brief Thread-safe stack buffering sockets newly submitted for deletion. */
    TStackSafe<SocketData_> new_socket_ptr_stack_;
    /** @brief Internal list storing sockets actively being monitored for zero async operations. */
    TList<SocketData_> socket_ptr_list_;
};

}// internal
}// socket
}// zengine