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
    Contact: 1152325286@qq.com85
*/
#pragma once

#include "drive.h"

#include "../z_core/t_atom.h"
#include "../z_core/t_function.h"
#include "../z_core/t_pool.h"
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

#include "z_tcp_endpoint.h"
#include "z_tcp_socket.h"

namespace zengine {
namespace socket {

/**
 * @brief Single session TCP server class.
 * 
 * Implementation Principle:
 * This class wraps an ASIO TCP acceptor and a single ZTCPSocket instance to manage a 
 * server that allows only one client connection at a time. The state transitions follow 
 * Open -> BindEndpoint -> Listen -> Accept -> Read/Write -> Close. It handles the lifecycle 
 * of the acceptor and delegates data transmission to the underlying connected socket.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZTCPSingleSessionServer : public ZObject {
public:
    /** @brief Type alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /**
     * @brief Represents the internal state of the single session server.
     */
    enum class StateEnum_ : Int32 {
        /** @brief The server is closed and inactive. */
        kClosed,
        /** @brief The server is opened but the endpoint is not yet bound. */
        kOpened,
        /** @brief The server has successfully bound to a local endpoint. */
        kEndpointBind,
        /** @brief The server is currently listening for incoming connections. */
        kListen,
        /** @brief A client is currently connected to the server. */
        kConnected,
        /** @brief The server has encountered an error. */
        kError
    };

    /** @brief The default maximum number of clients that can wait in the connection queue. */
    static inline constexpr SizeType kDefaultMaxWaitConnectClientNum = 128ULL;

public:
    /**
     * @brief Constructs a new ZTCPSingleSessionServer object.
     * @param _io_context The IO context used to drive this server's operations.
     */
    ZTCPSingleSessionServer(ZIOContext& _io_context) noexcept;

    ZTCPSingleSessionServer(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer(ZTCPSingleSessionServer&&) = delete;

    /**
     * @brief Destroys the ZTCPSingleSessionServer object and cleans up resources.
     */
    ~ZTCPSingleSessionServer() noexcept;

    ZTCPSingleSessionServer& operator=(const ZTCPSingleSessionServer&) = delete;
    ZTCPSingleSessionServer& operator=(ZTCPSingleSessionServer&&) = delete;

    /**
     * @brief Gets the current state of the server.
     * @return StateEnum_ The current state of the server.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_; }

    /**
     * @brief Returns the size of the data that can be read without blocking.
     * @return SizeType The number of bytes available to read.
     */
    NODISCARD FORCEINLINE SizeType GetReadableDataSize() noexcept {
        return socket_ptr_->GetReadableDataSize();
    }

    /**
     * @brief Gets the reference to the associated IO context.
     * @return ZIOContext& Reference to the IO context.
     */
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return io_context_; }

    /**
     * @brief Gets the reference to the underlying connected socket.
     * @return ZTCPSocket& Reference to the TCP socket.
     */
    NODISCARD FORCEINLINE ZTCPSocket& GetSocket() noexcept { return *socket_ptr_; }

    /**
     * @brief Retrieves the linked object pointer from the underlying socket.
     * @tparam _ObjectType The type to cast the linked object pointer to.
     * @return _ObjectType* The linked object pointer.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetLinkObjectPtr() const noexcept {
        return socket_ptr_->GetLinkObjectPtr<_ObjectType>();
    }

    /**
     * @brief Sets the OS write buffer size. Must be called after a client is connected.
     * @param _size The desired size of the write buffer in bytes.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType SetOSWriteBufferSize(Int32 _size) noexcept;

    /**
     * @brief Sets the OS read buffer size. Must be called after a client is connected.
     * @param _size The desired size of the read buffer in bytes.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType SetOSReadBufferSize(Int32 _size) noexcept;

    /**
     * @brief Sets if the address is reusable. If true, can bind multiple sockets to the same address.
     * @param _if_reuse Boolean indicating whether to enable address reuse.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /**
     * @brief Sets the extra pointer linked to the underlying socket object.
     * @tparam _ObjectType The type of the object being linked.
     * @param _obj_ptr The pointer to the object to be linked.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE Void SetLinkObjectPtr(_ObjectType* _obj_ptr) noexcept {
        socket_ptr_->SetLinkObjectPtr(_obj_ptr);
    }

    /**
     * @brief Opens the server with the specified IP protocol type.
     * @param _ip_type The IP type (IPv4 or IPv6) to open the server with.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /**
     * @brief Binds the server to a local endpoint. Call before Listen() or after Close().
     * @param _endpoint The local TCP endpoint to bind to.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _endpoint) noexcept;

    /**
     * @brief Starts listening for incoming connections. Clients can connect after this.
     * @param _max_wait_connect_client_num The maximum number of pending connections.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Listen(Int32 _max_wait_connect_client_num = kDefaultMaxWaitConnectClientNum) noexcept;

    /**
     * @brief Closes the server and disconnects any connected client.
     */
    Void Close() noexcept;

    /**
     * @brief Suspends the current thread until a client is connected.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Accept() noexcept;

    /**
     * @brief Reads data synchronously. Will suspend the current thread until data is read.
     * @param _buffer The buffer to store the read data.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Read(
        ZBuffer _buffer,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Reads data asynchronously. Will not suspend the current thread.
     * @param _buffer The buffer to store the read data.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncRead(
        ZBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

    /**
     * @brief Reads data until a match character is found. Will suspend the thread until read completes.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_char The character to match for stopping the read.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Reads data until a match string is found. Will suspend the thread until read completes.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_str The string to match for stopping the read.
     * @param _data_size_ptr Optional pointer to store the size of the data read.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType ReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Reads data asynchronously until a match character is found. Will not suspend the thread.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_char The character to match for stopping the read.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        Char _match_char,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /**
     * @brief Reads data asynchronously until a match string is found. Will not suspend the thread.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _match_str The string to match for stopping the read.
     * @param _handle_func The callback function handling the read data and errors.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncReadUntil(
        ZBufferStream* _buffer_ptr,
        const Char* _match_str,
        const TFunction<Void(ReturnType, ZTCPSocket*, ZBufferStream*)>& _handle_func
    ) noexcept;

    /**
     * @brief Reads data until the connection is closed. Will suspend the current thread until close.
     * @param _buffer_ptr Pointer to the buffer stream to store the read data.
     * @param _data_size_ptr Optional pointer to store the total size of the data read.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType ReadUntilClose(
        ZBufferStream* _buffer_ptr,
        SizeType* _data_size_ptr = nullptr
    ) noexcept;

    /**
     * @brief Writes data synchronously. Will suspend the current thread until data is written.
     * @param _buffer Const buffer containing the data to write.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Write(
        ZConstBuffer _buffer
    ) noexcept;

    /**
     * @brief Writes data asynchronously. Will not suspend the current thread.
     * @param _buffer Const buffer containing the data to write.
     * @param _handle_func The callback function called after the data is sent.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncWrite(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

private:
    /** @brief The atomic state of the single session TCP server. */
    TAtom<StateEnum_> state_;
    /** @brief The underlying TCP socket used for the single client connection. */
    TUniquePointer<ZTCPSocket> socket_ptr_;
    /** @brief The ASIO TCP acceptor used to listen and accept incoming connections. */
    asio::ip::tcp::tcp::acceptor acceptor_;
    /** @brief Reference to the IO context driving this server. */
    ZIOContext& io_context_;
};

/**
 * @brief Multiple session TCP server class.
 * 
 * Implementation Principle:
 * This class uses an ASIO TCP acceptor to manage a server that allows multiple clients 
 * to connect at the same time. The state transitions follow Open -> BindEndpoint -> 
 * Listen -> Async accept -> Handle socket -> Close. Connected sockets are managed and 
 * stored in a thread-safe pool list to facilitate broadcasting and concurrent operations.
 */
class SOCKET_DLLAPI CACHE_ALIGN ZTCPMultipleSessionServer : public ZObject {
public:
    /** @brief Type alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /**
     * @brief Represents the internal state of the multiple session server.
     */
    enum class StateEnum_ : Int32 {
        /** @brief The server is closed and inactive. */
        kClosed,
        /** @brief The server is opened but the endpoint is not yet bound. */
        kOpened,
        /** @brief The server has successfully bound to a local endpoint. */
        kEndpointBind,
        /** @brief The server is currently listening for incoming connections. */
        kListen,
        /** @brief The server has encountered an error. */
        kError
    };

    /** @brief The default maximum number of clients that can wait in the connection queue. */
    static constexpr SizeType kDefaultMaxWaitConnectClientNum = 128ULL;

public:
    /**
     * @brief Constructs a new ZTCPMultipleSessionServer object.
     * @param _io_context The IO context used to drive this server's operations.
     */
    ZTCPMultipleSessionServer(ZIOContext& _io_context) noexcept;

    ZTCPMultipleSessionServer(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer(ZTCPMultipleSessionServer&&) = delete;

    /**
     * @brief Destroys the ZTCPMultipleSessionServer object and cleans up resources.
     */
    ~ZTCPMultipleSessionServer() noexcept;

    ZTCPMultipleSessionServer& operator=(const ZTCPMultipleSessionServer&) = delete;
    ZTCPMultipleSessionServer& operator=(ZTCPMultipleSessionServer&&) = delete;

    /**
     * @brief Gets the current state of the server.
     * @return StateEnum_ The current state of the server.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_; }

    /**
     * @brief Gets the thread-safe pool list of connected sockets.
     * WARNING: Moving the socket data might cause fatal errors.
     * @return TDoublyLinkedPool<ZTCPSocket, true>& Reference to the socket pool list.
     */
    NODISCARD FORCEINLINE TDoublyLinkedPool<ZTCPSocket, true>& GetSocketPoolList() noexcept { return socket_pool_; }

    /**
     * @brief Gets the reference to the associated IO context.
     * @return ZIOContext& Reference to the IO context.
     */
    NODISCARD FORCEINLINE ZIOContext& GetIOContext() const noexcept { return io_context_; }

    /**
     * @brief Sets if the address is reusable. If true, can bind multiple sockets to the same address.
     * Call before binding endpoint. Must be called on all sockets that bind to the same address.
     * @param _if_reuse Boolean indicating whether to enable address reuse.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType SetIfReuseAddress(Bool _if_reuse) noexcept;

    /**
     * @brief Opens the server with the specified IP protocol type.
     * @param _ip_type The IP type (IPv4 or IPv6) to open the server with.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Open(IPTypeEnum _ip_type) noexcept;

    /**
     * @brief Binds the server to a local endpoint. Call before Listen() or after Close().
     * @param _endpoint The local TCP endpoint to bind to.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType BindEndpoint(const ZTCPEndpoint& _endpoint) noexcept;

    /**
     * @brief Starts the server listening for incoming connections.
     * @param _max_wait_connect_client_num The maximum number of pending connections.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType Listen(Int32 _max_wait_connect_client_num = kDefaultMaxWaitConnectClientNum) noexcept;

    /**
     * @brief Closes the server and disconnects any connected clients.
     */
    Void Close() noexcept;

    /**
     * @brief Waits for a client to connect asynchronously. Will not suspend the current thread.
     * @param _handle_func The callback function called after a client is connected.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncAccept(
        const TFunction<Void(ReturnType, ZTCPMultipleSessionServer*, ZTCPSocket*)>& _handle_func
    ) noexcept;

    /**
     * @brief Writes data to all connected clients asynchronously. Will not suspend the current thread.
     * @param _buffer Const buffer containing the data to broadcast.
     * @param _handle_func The callback function called after the data is sent.
     * @return ReturnType returns kOK if successful, or an error code if it fails.
     */
    NODISCARD ReturnType AsyncBroadcast(
        ZConstBuffer _buffer,
        const TFunction<Void(ReturnType, ZTCPSocket*, const ZConstBuffer)>& _handle_func
    ) noexcept;

private:
    /** @brief The atomic state of the multiple session TCP server. */
    TAtom<StateEnum_> state_;
    /** @brief The ASIO TCP acceptor used to listen and accept incoming connections. */
    asio::ip::tcp::acceptor acceptor_;
    /** @brief Mutex to protect the socket list from concurrent access. */
    CACHE_ALIGN ZMutex socket_list_mutex_;
    /** @brief The raw list of socket nodes. */
    TPoolList<ZTCPSocket, TDoublyLinkedPool<ZTCPSocket, true>> socket_list_;
    /** @brief The thread-safe pool list managing connected sockets. */
    TDoublyLinkedPool<ZTCPSocket, true> socket_pool_;
    /** @brief Reference to the IO context driving this server. */
    ZIOContext& io_context_;
};

} // socket
} // zengine