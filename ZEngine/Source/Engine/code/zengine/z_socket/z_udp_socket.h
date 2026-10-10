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

    2. Commercial Use: Commercial use of th
    
    is software is STRICTLY PROHIBITED
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

#include <new>

#include "../z_core/t_function.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "internal/l_asio.h"
#include "z_io_context.h"
#include "z_udp_endpoint.h"

namespace zengine {
namespace socket {
namespace internal {

class ZUDPSocketDeleteManager;

}// internal
}// socket
}// zengine

namespace zengine {
namespace socket {

/**
 * @brief UDP socket type optimized for P2P networking and high performance with zero hot-path allocation.
 * 
 * WARNING: To use async operations, the object must be allocated on the heap (using new) instead of stack allocation.
 * Using stack allocation may cause fatal errors or undefined behavior.
 * 
 * Implementation Principles:
 * This class implements a "ping-pong" asynchronous receive model, utilizing a single member variable 
 * (`async_receive_endpoint_`) to store the sender's endpoint. This achieves absolute zero heap memory 
 * allocation and lock-free execution on the hot-path.
 * 
 * WARNING - Buffer Lifetime:
 * Memory pointers passed via ZBuffer to any Async* functions MUST remain valid and un-destroyed until 
 * the corresponding asynchronous callback function is fully triggered.
 * 
 * WARNING - Concurrent Overlapped Reads:
 * Do NOT issue overlapping or concurrent `AsyncReceiveFrom` calls on the same socket. Due to the 
 * zero-allocation design, multiple concurrent receives will cause data races on the internal endpoint 
 * variable. Always wait for the previous read callback to fire before issuing the next read.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZUDPSocket : public ZObject {
public:
    /** @brief Parent type definition. */
    using SuperType_ = ZObject;

    /** @brief Constant representing infinite retry attempts for connecting operations. */
    static inline constexpr Int32 kConnectRetryForever = kInt32Max;

    /** @brief Enumeration representing the internal state machine of the UDP socket. */
    enum class StateEnum_ : Int32 {
        /** @brief The socket is closed and inactive. */
        kClosed,
        /** @brief The socket is opened and ready for IO operations. */
        kOpened,
        /** @brief The socket is shutting down, no further operations are allowed. */
        kShutdown,
        /** @brief The socket has encountered a fatal error. */
        kError
    };

public:
    /**
     * @brief Moves the object to a queue to wait for async operation to complete, then deallocates the memory.
     * @param _ptr Pointer to the memory to be deallocated.
     */
    static Void operator delete(ZUDPSocket* _socket_ptr, std::destroying_delete_t) noexcept;

    /**
     * @brief Constructs a ZUDPSocket bound to a specific IO context.
     * @param _io_context The IO context used to drive asynchronous operations.
     */
    ZUDPSocket(ZIOContext& _io_context) noexcept;

    /** @brief Deleted copy constructor. */
    ZUDPSocket(const ZUDPSocket&) = delete;

    /** @brief Deleted move constructor. */
    ZUDPSocket(ZUDPSocket&&) = delete;

    /**
     * @brief Destructor for ZUDPSocket. Safely cleans up resources.
     */
    ~ZUDPSocket() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZUDPSocket& operator=(const ZUDPSocket&) = delete;

    /** @brief Deleted move assignment operator. */
    ZUDPSocket& operator=(ZUDPSocket&&) = delete;

    /**
     * @brief Retrieves the current state of the socket.
     * @return The current state enum value of the socket.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_.Load(); }

    /**
     * @brief Retrieves the local UDP endpoint the socket is currently bound to.
     * @return The local ZUDPEndpoint.
     */
    NODISCARD FORCEINLINE ZUDPEndpoint GetLocalEndpoint() const noexcept { return socket_.local_endpoint(); }

    /**
     * @brief Retrieves the size of the data currently available to be read without blocking.
     * @return The size of the readable data in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetReadableDataSize() noexcept { return socket_.available(); }

    /**
     * @brief Retrieves the IO context associated with this socket.
     * @return A reference to the ZIOContext.
     */
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return io_context_; }

    /**
     * @brief Retrieves the pointer to the linked custom object.
     * @tparam _ObjectType The type of the linked object to cast to.
     * @return A pointer to the linked object.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetLinkObjectPtr() const noexcept { return link_object_ptr_; }

    /**
     * @brief Sets the operating system's write buffer size for this socket. Call after connection or bind.
     * @param _size The desired size of the OS write buffer in bytes.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;

    /**
     * @brief Sets the operating system's read buffer size for this socket. Call after connection or bind.
     * @param _size The desired size of the OS read buffer in bytes.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;

    /**
     * @brief Sets whether the local address can be reused. Must be called before binding. Default is true.
     * @param _if_reuse True to allow address reuse, false otherwise.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /**
     * @brief Sets the callback function that will be triggered when an asynchronous error occurs.
     * @param _handle_func The function to handle asynchronous errors.
     */
    NODISCARD FORCEINLINE Void SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
        async_error_handle_func_ = std::forward<TFunction<Void(ReturnType)>>(_handle_func);
    }

    /**
     * @brief Links a custom external object pointer to this socket for convenient context retrieval.
     * @tparam _ObjectType The type of the linked object.
     * @param _obj_ptr Pointer to the custom object to link.
     */
    template<typename _ObjectType>
    NODISCARD Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept { 
        link_object_ptr_ = _obj_ptr; 
    }

    /**
     * @brief Opens the socket with the specified IP protocol type.
     * @param _ip_type The IP protocol type (IPv4 or IPv6) to open the socket with.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /**
     * @brief Binds the socket to a specific local UDP endpoint. Must be called before receiving data.
     * @param _udp_endpoint The local UDP endpoint to bind to.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType BindEndpoint(const ZUDPEndpoint& _udp_endpoint) noexcept;

    /**
     * @brief Cancels all currently pending asynchronous operations on the socket.
     */
    Void Cancel() noexcept;

    /**
     * @brief Shuts down the socket for both sending and receiving, preventing further operations.
     */
    Void Shutdown() noexcept;

    /**
     * @brief Closes the socket safely, unbinding callbacks and cancelling all pending asynchronous operations.
     */
    Void Close() noexcept;

    /**
     * @brief Sets a target endpoint. After connecting, the socket can only receive from the specified endpoint.
     * @param _udp_endpoint The target UDP endpoint to connect to.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType Connect(const ZUDPEndpoint& _udp_endpoint) noexcept;

    /**
     * @brief Synchronously receives a message. Will suspend the current thread until a message is received.
     * @param _buffer The buffer used to store the received data.
     * @param _message_size_ptr Optional pointer to store the actual size of the received message.
     * @param _udp_endpoint_ptr Optional pointer to store the sender's UDP endpoint.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType ReceiveFrom(
        ZBuffer _buffer,
        SizeType* _message_size_ptr = nullptr,
        ZUDPEndpoint* _udp_endpoint_ptr = nullptr
    ) noexcept;

    /**
     * @brief Asynchronously receives a message. Will not suspend the thread. 
     * @param _buffer The buffer for incoming data. WARNING: Memory must remain valid until callback triggers.
     * @param _handle_func Callback function triggered after data is received or an error occurs.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType AsyncReceiveFrom(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer, const ZUDPEndpoint&)>& _handle_func
    ) noexcept;

    /**
     * @brief Synchronously receives a message from a connected endpoint. Will suspend the current thread.
     * @param _buffer The buffer used to store the received data.
     * @param _message_size_ptr Optional pointer to store the actual size of the received message.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType Receive(
        ZBuffer _buffer,
        SizeType* _message_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Asynchronously receives a message from a connected endpoint. Will not suspend the thread.
     * @param _buffer The buffer for incoming data. WARNING: Memory must remain valid until callback triggers.
     * @param _handle_func Callback function triggered after data is received or an error occurs.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType AsyncReceive(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /**
     * @brief Synchronously sends a message to the specified endpoint. Will suspend the thread until sent.
     * @param _udp_endpoint The target UDP endpoint to send data to.
     * @param _buffer The constant buffer containing the data to send.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType SendTo(
        const ZUDPEndpoint& _udp_endpoint,
        ZConstBuffer _buffer
    ) noexcept;

    /**
     * @brief Asynchronously sends a message to the specified endpoint. Will not suspend the thread.
     * @param _udp_endpoint The target UDP endpoint to send data to.
     * @param _buffer The constant buffer containing the data to send.
     * @param _handle_func Callback function triggered after the message is sent or an error occurs.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType AsyncSendTo(
        const ZUDPEndpoint& _udp_endpoint,
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /**
     * @brief Synchronously sends a message to the connected endpoint. Call Connect() before this function.
     * @param _buffer The constant buffer containing the data to send.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType Send(
        ZConstBuffer _buffer
    ) noexcept;

    /**
     * @brief Asynchronously sends a message to the connected endpoint. Call Connect() before this function.
     * @param _buffer The constant buffer containing the data to send.
     * @param _handle_func Callback function triggered after the message is sent or an error occurs.
     * @return Returns kOK if successful, otherwise it will return an error code.
     */
    NODISCARD ReturnType AsyncSend(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZUDPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

private:
    friend class zengine::socket::internal::ZUDPSocketDeleteManager;

private:
    /** @brief Atomic variable tracking the current operational state of the socket. */
    TAtom<StateEnum_> state_;
    /** @brief The atomic count of ongoing asynchronous operations. */
    TAtom<Int32> async_operation_count_;
    /** @brief Flag indicating if the UDP socket is exclusively connected to a specific endpoint. */
    Bool if_connected_;
    /** @brief Flag defining whether the socket address is allowed to be reused (SO_REUSEADDR). */
    Bool if_reuse_address_;
    /** @brief The underlying ASIO UDP socket handle managing OS-level network operations. */
    asio::ip::udp::socket socket_;
    /** @brief Pre-allocated endpoint variable safely used sequentially to receive sender IP information. */
    asio::ip::udp::endpoint async_receive_endpoint_;
    /** @brief Registered callback function to notify higher-level systems of asynchronous socket errors. */
    TFunction<Void(ReturnType)> async_error_handle_func_;
    /** @brief Reference to the IO context running the asynchronous event loop for this socket. */
    ZIOContext& io_context_;
    /** @brief Type-erased raw pointer storing a link to an external object associated with this socket. */
    Void* link_object_ptr_;

};

} // socket
} // zengine