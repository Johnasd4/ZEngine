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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_udp_message.h"

namespace zengine {
namespace socket {

class ZUDPMessageSerializer {
    NO_NEW_DELETE
public:
    static NODISCARD FORCEINLINE ZUDPMessageSerializer& GetInstance() noexcept {
        thread_local ZUDPMessageSerializer serializer;
        return serializer;
    }

    NODISCARD FORCEINLINE ZSerializer& SetOutputMemoryAndGetSerializer(ZMemory& _output_memory) noexcept {
        output_memory_ptr_ = &_output_memory;
        message_size_ = ZUDPMessage::kMessageHeadSize;
        return serializer_;
    }

    NODISCARD FORCEINLINE SizeType GetSerializedMessageSize() noexcept { return message_size_; }

private:
    ZUDPMessageSerializer() noexcept
        : output_memory_ptr_(nullptr)
        , message_size_(ZUDPMessage::kMessageHeadSize)
        , serializer_(
            [this](ZConstBuffer _buffer) -> ReturnType {
                ReturnType ret_val = kOK;

                SizeType new_size = message_size_ + _buffer.GetSize();

                // Resize the buffer if the new size exceeds the current capacity.
                if (new_size > output_memory_ptr_->GetSize()) {
                    output_memory_ptr_->Resize(new_size);
                }

                // Copy the serialized data into the buffer and update the message size.
                Copy(
                    output_memory_ptr_->GetDataPtr<Byte>() + message_size_,
                    _buffer.GetDataPtr<Void>(),
                    _buffer.GetSize()
                );

                // Update the message size to reflect the newly added data.
                message_size_ = new_size;

                return ret_val;
            }
        )
    {}

    FORCEINLINE ~ZUDPMessageSerializer() noexcept = default;

private:
    ZMemory* output_memory_ptr_;
    SizeType message_size_;
    ZSerializer serializer_;
};

class ZUDPMessageDeserializeManager {
    NO_NEW_DELETE
public:
    static NODISCARD FORCEINLINE ZUDPMessageDeserializeManager& GetInstance() noexcept {
        thread_local ZUDPMessageDeserializeManager deserializer;
        return deserializer;
    }

    NODISCARD FORCEINLINE ZDeserializer& SetInputBufferAndGetDeserializer(ZConstBuffer _input_buffer) noexcept {
        input_buffer_ = _input_buffer;
        return deserializer_;
    }

    NODISCARD FORCEINLINE SizeType GetUndeserializedSize() noexcept { return input_buffer_.GetSize(); }

private:
    ZUDPMessageDeserializeManager() noexcept
        : input_buffer_()
        , deserializer_(
            [this](ZBuffer _output_buffer) -> ReturnType {
                ReturnType ret_val = kOK;

                // Check if the buffer contains enough data to read.
                if (_output_buffer.GetSize() > input_buffer_.GetSize()) {
                    ret_val = error_code::kZUDPMessageErrorCode_DeserializeDataError;
                    Z_LOG_ERROR(
                        ret_val,
                        0,
                        "ZUDPMessageDeserializer::DeserializerFunction() Not enough data to deserialize!"
                    );
                    return ret_val;
                }

                // Copy the deserialized data from the buffer into the message's data buffer at the correct offset.
                Copy(
                    _output_buffer.GetDataPtr<Void>(),
                    input_buffer_.GetDataPtr<Byte>(),
                    _output_buffer.GetSize()
                );

                // Update the input buffer to reflect the remaining data after deserialization.
                input_buffer_ = ZConstBuffer(
                    input_buffer_.GetDataPtr<Byte>() + _output_buffer.GetSize(),
                    input_buffer_.GetSize() - _output_buffer.GetSize()
                );

                return ret_val;
            }
        )
    {}

    FORCEINLINE ~ZUDPMessageDeserializeManager() noexcept = default;

private:
    ZConstBuffer input_buffer_;
    ZDeserializer deserializer_;
};

} // socket
} // zengine

namespace zengine {
namespace socket {

NODISCARD ReturnType ZUDPMessage::Serialize(ZMemory& _output_memory) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZUDPMessageSerializer& serialize_manager = ZUDPMessageSerializer::GetInstance();

    // Resize the output memory to accommodate the maximum possible size of the message body plus the fixed header size.
    _output_memory.Resize(GetMessageBodyPossibleMaxSize() + kMessageHeadSize);

    // Serialize the message fields into the binary format using the serializer.
    link_code = OnSerialize(serialize_manager.SetOutputMemoryAndGetSerializer(_output_memory));
    if (link_code != kOK) {
        ret_val = error_code::kZUDPMessageErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPMessage::OnSerialize() link error!"
        );
        return ret_val;
    }

    UInt64 serialized_size = serialize_manager.GetSerializedMessageSize();

    // After serialization, update the message header fields in the message data buffer.
    _output_memory.At<UInt64>(kMessageSizeOffset) = serialized_size;
    _output_memory.At<UInt32>(kMessageTypeOffset) = message_type_;
    _output_memory.At<UInt32>(kMessageIdOffset) = message_id_;

    // Resize the output memory to the actual serialized size of the message, which includes the header and body.
    _output_memory.Resize(serialized_size);

    return ret_val;
}

NODISCARD ReturnType ZUDPMessage::Deserialize(ZConstBuffer _input_buffer) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZUDPMessageDeserializeManager& deserialize_manager = ZUDPMessageDeserializeManager::GetInstance();

    Z_CHECK(
        _input_buffer.GetSize() >= kMessageHeadSize, 
        error_code::kZUDPMessageErrorCode_ParamOutOfRange,
        "ZUDPMessage::Deserialize() Input buffer size is smaller than message header size!"
    );

    // Read the message header fields from the message data buffer.
    message_type_ = _input_buffer.At<UInt32>(kMessageTypeOffset);
    message_id_ = _input_buffer.At<UInt32>(kMessageIdOffset);

    // Set the input buffer for the deserializer. Buffer starts after the fixed header size.
    ZDeserializer& deserializer = deserialize_manager.SetInputBufferAndGetDeserializer(
        ZConstBuffer(_input_buffer.GetDataPtr<Byte>() + kMessageHeadSize, _input_buffer.GetSize() - kMessageHeadSize)
    );

    // Deserialize the message fields from the binary format using the deserializer.
    link_code = OnDeserialize(deserializer);
    if (link_code != kOK) {
        ret_val = error_code::kZUDPMessageErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZUDPMessage::OnDeserialize() link error!"
        );
        return ret_val;
    }

    // Check if all data has been deserialized. If there is remaining data, it indicates a deserialization error.
    if (deserialize_manager.GetUndeserializedSize() > 0ULL) {
        ret_val = error_code::kZUDPMessageErrorCode_DeserializeDataError;
        Z_LOG_ERROR(
            ret_val,
            0,
            "ZUDPMessage::OnDeserialize() Not all data was deserialized!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD SizeType ZUDPMessage::GetMessageBodyPossibleMaxSize() const noexcept { return 0; }

NODISCARD ReturnType ZUDPMessage::OnSerialize(ZSerializer& _out) const noexcept { return kOK; }

NODISCARD ReturnType ZUDPMessage::OnDeserialize(ZDeserializer& _in) noexcept { return kOK; }

} // socket
} // zengine