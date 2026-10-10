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

#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"

#include "internal/l_asio.h"

namespace zengine {
namespace socket {

/**
 * @brief Buffer type used for managing network I/O data streams.
 * 
 * Implementation principles: This class encapsulates `asio::basic_streambuf` to provide a robust, 
 * stream-based buffering mechanism. It utilizes a sliding window memory model where incoming network 
 * data is stored in the buffer, and processed data is explicitly explicitly consumed (removed from the 
 * front) to free up space. This effectively reuses physical memory and prevents memory bloat during 
 * continuous data streaming operations without the need for frequent dynamic memory reallocations.
 */
class SOCKET_DLLAPI ZBufferStream : public ZObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor. Initializes an empty socket buffer stream.
     */
    FORCEINLINE ZBufferStream() noexcept = default;

    /** @brief Deleted copy constructor. */
    ZBufferStream(const ZBufferStream&) = delete;

    /**
     * @brief Move constructor. Transfers ownership of the internal buffer.
     * @param <unnamed> Rvalue reference of another ZSocketBufferStream object to be moved.
     */
    FORCEINLINE ZBufferStream(ZBufferStream&&) noexcept = default;

    /**
     * @brief Constructor with maximum size limitation.
     * @param _max_size The maximum allowed size for the underlying buffer to prevent memory exhaustion.
     */
    FORCEINLINE ZBufferStream(SizeType _max_size) noexcept
        : SuperType_()
        , buffer_(_max_size)
    {}

    /**
     * @brief Default destructor. Cleans up the buffer resources.
     */
    FORCEINLINE ~ZBufferStream() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZBufferStream& operator=(const ZBufferStream&) = delete;

    /**
     * @brief Move assignment operator. Transfers ownership of the internal buffer.
     * @param <unnamed> Rvalue reference of another ZSocketBufferStream object to be moved.
     * @return Reference to this ZSocketBufferStream instance.
     */
    FORCEINLINE ZBufferStream& operator=(ZBufferStream&&) noexcept = default;

    /**
     * @brief Gets the total size of the currently readable data in the buffer.
     * @return The size of the valid readable data in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return buffer_.size(); }

    /**
     * @brief Gets a constant view of the currently readable data in the buffer.
     * @return A ZConstBuffer object representing the valid readable memory region.
     */
    NODISCARD FORCEINLINE ZConstBuffer GetData() const noexcept { 
        return ZConstBuffer(buffer_.data().data(), buffer_.size()); 
    }

    /**
     * @brief Consumes and removes all currently readable data from the buffer.
     */
    FORCEINLINE Void ConsumeAll() noexcept {
        buffer_.consume(buffer_.size());
    }

    /**
     * @brief Consumes and removes a specific amount of readable data from the buffer's front.
     * @param _size The number of bytes to consume and discard.
     */
    FORCEINLINE Void Consume(SizeType _size) noexcept {
        buffer_.consume(_size);
    }

private:
    friend class ZTCPSocket;
    friend class ZTLSStream;

private:
    /** @brief The underlying asio streambuf managing the physical memory of the stream. */
    asio::basic_streambuf<TAllocator<Char>> buffer_;
};

} // socket
} // zengine