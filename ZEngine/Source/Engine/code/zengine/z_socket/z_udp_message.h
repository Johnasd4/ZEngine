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
#include "../z_core/z_memory.h"
#include "../z_core/z_object.h"
#include "../z_core/z_serializer.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration of error codes for UDP message operations.
 */
enum ZUDPMessageErrorCodeEnum : ReturnType::ValueType {
    /** @brief Indicates a linking error within the message processing. */
    kZUDPMessageErrorCode_LinkError = kErrorCodeBase_ZUDPMessage,
    /** @brief Indicates an error occurred in the underlying system or library. */
    kZUDPMessageErrorCode_SystemOrLibraryError,
    /** @brief Indicates a required parameter was a nullptr. */
    kZUDPMessageErrorCode_NullptrParam,
    /** @brief Indicates a parameter value is out of the valid range. */
    kZUDPMessageErrorCode_ParamOutOfRange,
    /** @brief Indicates an error occurred while deserializing data. */
    kZUDPMessageErrorCode_DeserializeDataError,
};

} // error_code
} // zengine

namespace zengine {
namespace socket {

/**
 * @brief Message base class for UDP communication.
 * 
 * @details Implementation Principle:
 * This class serves as the polymorphic base for high-level business messages. Unlike strictly mapped 
 * low-level packets, messages represent complete logical units of data. ZMemory safely handles deep 
 * copying and dynamic payloads. After parsing or serialization, these messages are fragmented into 
 * smaller ZUDPPacket slices for actual transmission. The virtual destructor is made public to allow 
 * safe polymorphic deletion, preventing memory leaks when managed by base pointers.
 */
struct ZUDPMessage : public ZObject {
public:
    /** @brief Type alias for the base object class. */
    using SuperType_ = ZObject;

    /** @brief The fixed byte size of the message header (size + type + id). */
    static inline constexpr SizeType kMessageHeadSize = 16ULL;
    /** @brief The offset for the message size field in the header. */
    static inline constexpr SizeType kMessageSizeOffset = 0ULL;
    /** @brief The offset for the message type field in the header. */
    static inline constexpr SizeType kMessageTypeOffset = 8ULL;
    /** @brief The offset for the message ID field in the header. */
    static inline constexpr SizeType kMessageIdOffset = 12ULL;
    /** @brief The message type value used for keep-alive messages. */
    static inline constexpr UInt32 kKeepAliveMessageType = kUInt32Max;

public:
    /**
     * @brief Construct a new ZUDPMessage object.
     * @param _message_type The identifier for the message type, used for routing and handling logic.
     * @param _message_id The unique identifier specifying the exact type of this message.
     */
    FORCEINLINE ZUDPMessage(UInt32 _message_type, UInt32 _message_id) noexcept
        : SuperType_()
        , message_type_(_message_type)
        , message_id_(_message_id)
    {}

    /**
     * @brief Default copy constructor. Safely relies on ZMemory's deep copy implementation.
     * @param other The other message object to copy from.
     */
    ZUDPMessage(const ZUDPMessage&) noexcept = default;

    /**
     * @brief Default move constructor. Safely relies on ZMemory's move semantics.
     * @param other The other message object to move from.
     */
    ZUDPMessage(ZUDPMessage&&) noexcept = default;

    /**
     * @brief Default copy assignment operator.
     * @param other The other message object to copy assign from.
     * @return ZUDPMessage& Returns a reference to this object.
     */
    ZUDPMessage& operator=(const ZUDPMessage&) noexcept = default;

    /**
     * @brief Default move assignment operator.
     * @param other The other message object to move assign from.
     * @return ZUDPMessage& Returns a reference to this object.
     */
    ZUDPMessage& operator=(ZUDPMessage&&) noexcept = default;

    /**
     * @brief Destroy the ZUDPMessage object.
     */
    virtual ~ZUDPMessage() noexcept = default;

    /**
     * @brief Serialize the complete message including the header into the output memory.
     * @param _output_memory The memory buffer where the serialized data will be stored.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    NODISCARD ReturnType Serialize(ZMemory& _output_memory) noexcept;

    /**
     * @brief Deserialize the complete message including the header from the input buffer.
     * @param _input_buffer The buffer containing the binary data to deserialize.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    NODISCARD ReturnType Deserialize(ZConstBuffer _input_buffer) noexcept;

protected:
    /**
     * @brief Get the maximum possible size of the message body.
     * @return SizeType Returns the maximum possible size in bytes.
     */
    NODISCARD virtual SizeType GetMessageBodyPossibleMaxSize() const noexcept;

    /**
     * @brief Serialize the message data into a binary stream.
     * @param _out The serializer instance used to write the message fields into binary format.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    NODISCARD virtual ReturnType OnSerialize(ZSerializer& _out) const noexcept;

    /**
     * @brief Deserialize the message data from a binary stream.
     * @param _in The deserializer instance used to read and populate the message fields.
     * @return ReturnType Returns kOK if successful, otherwise returns an error code.
     */
    NODISCARD virtual ReturnType OnDeserialize(ZDeserializer& _in) noexcept;

public:
    /** @brief The message type field, used to identify the specific type of message. */
    UInt32 message_type_;
    /** @brief The unique identifier determining the actual message structure. */
    UInt32 message_id_;
};

} // socket
} // zengine