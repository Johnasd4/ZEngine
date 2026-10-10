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

#include <new>

#include "../z_core/t_atom.h"
#include "../z_core/t_function.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "internal/l_asio.h"
#include "z_buffer_stream.h"
#include "z_io_context.h"
#include "z_tcp_endpoint.h"

namespace zengine {
namespace socket {
namespace internal {

class ZTCPSocketDeleteManager;

}// internal
}// socket
}// zengine

namespace zengine {
namespace socket {

/**
 * @brief TCP socket wrapper type based on ASIO.
 * 
 * WARNING: To use async operations, the object must be allocated on the heap (using new) instead of stack allocation.
 * Using stack allocation may cause fatal errors or undefined behavior.
 * 
 * Implementation Principle:
 * This class encapsulates an ASIO TCP socket to provide a unified object-oriented interface. 
 * It manages the network lifecycle state machine (Closed -> Opened -> Connecting -> Connected -> Error) 
 * and maps ASIO's low-level system error codes to the engine's standardized ReturnType. 
 * It supports both synchronous and asynchronous operations, relying on an internal ZIOContext. 
 * Additionally, it contains an extra pointer that can be safely linked to any user object for 
 * session or context management using GetLinkObjectPtr and SetLinkObjectPtr templates.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZTCPSocket : public ZObject {
public:
    /** @brief Type alias for the base class ZObject. */
    using SuperType_ = ZObject;
    /** @brief Constant representing an infinite number of connection retries. */
    static inline constexpr Int32 kConnectRetryForever = kInt32Max;

    /** @brief Represents the internal state of the TCP socket. */
    enum class StateEnum_ : Int32 {
        /** @brief The socket is closed and inactive. */
        kClosed,
        /** @brief The socket is opened but not yet connected. */
        kOpened,
        /** @brief The socket is currently attempting to connect to a remote endpoint. */
        kConnecting,
        /** @brief The socket is successfully connected and ready for data transfer. */
        kConnected,
        /** @brief The socket has encountered an error. */
        kError
    };

public:
    /**
     * @brief Moves the object to a queue to wait for async operation to complete, then deallocates the memory.
     * @param _ptr Pointer to the memory to be deallocated.
     */
    static Void operator delete(ZTCPSocket* _socket_ptr, std::destroying_delete_t) noexcept;

    /**
     * @brief Constructs a new ZTCPSocket object.
     * @param _io_context The IO context used to drive this socket's operations.
     */
    ZTCPSocket(ZIOContext& _io_context) noexcept;

    /** @brief Deleted copy constructor. */
    ZTCPSocket(const ZTCPSocket&) = delete;

    /** @brief Deleted move constructor. */
    ZTCPSocket(ZTCPSocket&&) = delete;

    /**
     * @brief Destroys the ZTCPSocket object, closing the underlying socket if open.
     */
    ~ZTCPSocket() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZTCPSocket& operator=(const ZTCPSocket&) = delete;

    /** @brief Deleted move assignment operator. */
    ZTCPSocket& operator=(ZTCPSocket&&) = delete;

    /**
     * @brief Gets the current state of the socket.
     * @return StateEnum_ The current socket state.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_.Load(); }

    /**
     * @brief Gets the local endpoint of the socket.
     * @return ZTCPEndpoint The local TCP endpoint.
     */
    NODISCARD FORCEINLINE ZTCPEndpoint GetLocalEndpoint() const noexcept { return socket_.local_endpoint(); }

    /**
     * @brief Gets the remote endpoint of the socket.
     * @return ZTCPEndpoint The remote TCP endpoint.
     */
    NODISCARD FORCEINLINE ZTCPEndpoint GetRemoteEndpoint() const noexcept { return remote_endpoint_; }

    /**
     * @brief Returns the size of the data that can be read without blocking.
     * @return SizeType The number of bytes available to read.
     */
    NODISCARD FORCEINLINE SizeType GetReadableDataSize() noexcept {
        return socket_.available();
    }

    /**
     * @brief Gets the reference to the associated IO context.
     * @return ZIOContext& Reference to the IO context.
     */
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return io_context_; }

    /**
     * @brief Retrieves the extra pointer linked to this socket object.
     * @tparam _ObjectType The type to cast the linked object pointer to.
     * @return _ObjectType* The linked object pointer.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetLinkObjectPtr() const noexcept { return link_object_ptr_; }

    /**
     * @brief Sets the asynchronous error handling function, called when an async error happens.
     * @param _handle_func The function to call when an error occurs during an asynchronous operation.
     */
    NODISCARD FORCEINLINE Void SetAsyncErrorHandleFunction(TFunction<Void(ReturnType)>&& _handle_func) noexcept {
        async_error_handle_func_ = std::forward<TFunction<Void(ReturnType)>>(_handle_func);
    }

    /**
     * @brief Sets the OS write buffer size. Must be called after the socket is connected.
     * @param _size The desired size of the write buffer in bytes.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;

    /**
     * @brief Sets the OS read buffer size. Must be called after the socket is connected.
     * @param _size The desired size of the read buffer in bytes.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;

    /**
     * @brief Set if address is reuseable. If true, can bind multiple sockets to the same address.
     * @param _if_reuse Boolean indicating whether to enable address reuse. Default is true in usage.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /**
     * @brief Sets the extra pointer linked to this socket object.
     * @tparam _ObjectType The type of the object being linked.
     * @param _obj_ptr The pointer to the object to be linked.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        link_object_ptr_ = _obj_ptr;
    }

    /**
     * @brief Open the socket with the specified IP protocol type.
     * @param _ip_type The IP type (IPv4 or IPv6) to open the socket with.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /**
     * @brief Bind the socket to a local endpoint. Must be called before connecting.
     * @param _tcp_endpoint The local TCP endpoint to bind to.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _tcp_endpoint) noexcept;

    /**
     * @brief Cancel all pending asynchronous operations.
     */
    Void Cancel() noexcept;

    /**
     * @brief Close the socket, cleaning up internal states and terminating connections.
     */
    Void Close() noexcept;

    /**
     * @brief Connect to the target socket endpoint. Will suspend the current thread until finished.
     * @param _tcp_endpoint The remote TCP endpoint to connect to.
     * @param _repeat_times The maximum number of reconnect attempts.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType Connect(
        const ZTCPEndpoint& _tcp_endpoint,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /**
     * @brief Connect to the target socket endpoint asynchronously. Will not suspend the current thread.
     * @param _tcp_endpoint The remote TCP endpoint to connect to.
     * @param _handle_func The callback function upon completion or error.
     * @param _repeat_times The maximum number of reconnect attempts.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncConnect(
        const ZTCPEndpoint& _tcp_endpoint,
        const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times = kConnectRetryForever
    ) noexcept;

    /**
     * @brief Stops an ongoing connection attempt.
     */
    NODISCARD Void StopConnect() noexcept;

    /**
     * @brief Read data synchronously. Will suspend the current thread until data is read.
     * @param _buffer The buffer to store the read data.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType Read(
        ZBuffer _buffer,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Read data asynchronously. Will not suspend the current thread.
     * @param _buffer The buffer to store the read data.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncRead(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /**
     * @brief Read data until a match character is found. Will suspend the thread until read completes.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_char The character to match for stopping the read.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Read data until a match string is found. Will suspend the thread until read completes.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_str The string to match for stopping the read.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Read data asynchronously until a match character is found. Will not suspend the current thread.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_char The character to match for stopping the read.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /**
     * @brief Read data asynchronously until a match string is found. Will not suspend the current thread.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_str The string to match for stopping the read.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /**
     * @brief Read data until the connection is closed. Will suspend the current thread until close.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _data_size_ptr Optional pointer to store the total size of the data read.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType ReadUntilClose(
        ZBufferStream* _buffer_ptr,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Write data synchronously. Will suspend the current thread until data is written.
     * @param _buffer Const buffer containing the data to write.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType Write(
        ZConstBuffer _buffer
    ) noexcept;

    /**
     * @brief Write data asynchronously. Will not suspend the current thread.
     * @param _buffer Const buffer containing the data to write.
     * @param _handle_func The callback function called after the data is sent.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncWrite(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

private:
    /**
     * @brief Internal callback executed when the socket successfully connects.
     */
    Void OnConnectP() noexcept;

    /**
     * @brief Internal asynchronous connect execution mechanism.
     * @param _tcp_endpoint The remote TCP endpoint to connect to.
     * @param _handle_func The callback function upon completion.
     * @param _repeat_times The total maximum number of reconnect attempts.
     * @param _reconnect_times The current number of reconnect attempts made.
     * @return ReturnType Returns kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType AsyncConnectExecuteP(
        const ZTCPEndpoint& _tcp_endpoint,
        const TFunction<Void(ReturnType, ZTCPSocket*)>& _handle_func,
        Int32 _repeat_times,
        Int32 _reconnect_times
    ) noexcept;

private:
    friend class zengine::socket::internal::ZTCPSocketDeleteManager;
    friend class ZTCPSingleSessionServer;
    friend class ZTCPMultipleSessionServer;

private:    
    /** @brief The atomic state of the TCP socket. */
    TAtom<StateEnum_> state_;
    /** @brief The atomic count of ongoing asynchronous operations. */
    TAtom<Int32> async_operation_count_;
    /** @brief Flag indicating whether a local endpoint has been bound. */
    Bool if_endpoint_bind_;
    /** @brief Flag indicating whether the address should be reusable. */
    Bool if_reuse_address_;
    /** @brief The underlying ASIO TCP socket instance. */
    asio::ip::tcp::socket socket_;
    /** @brief The local endpoint to bind to. */
    ZTCPEndpoint bind_endpoint_;
    /** @brief The remote endpoint connected to. Getting remote endpoint after socket disconnect throws exception. */
    ZTCPEndpoint remote_endpoint_;
    /** @brief The callback function triggered upon asynchronous errors. */
    TFunction<Void(ReturnType)> async_error_handle_func_;
    /** @brief Reference to the IO context driving this socket. */
    ZIOContext& io_context_;
    /** @brief Extra pointer linked to an external object for user logic. */
    Void* link_object_ptr_;
};

} // namespace socket
} // namespace zengine