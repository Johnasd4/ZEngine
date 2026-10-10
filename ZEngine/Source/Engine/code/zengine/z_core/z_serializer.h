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

#include "t_function.h"
#include "z_buffer.h"
#include "z_object.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration of error codes for serialization and deserialization operations.
 */
enum ZSerializerErrorCodeEnum : ReturnType::ValueType {
    /** @brief Indicates a linking error within the serializer's callback function. */
    kZSerializerErrorCode_LinkError = kErrorCodeBase_ZSerializer,
    /** @brief Indicates an error occurred in the underlying system or library. */
    kZSerializerErrorCode_SystemOrLibraryError,
    /** @brief Indicates a required parameter was a nullptr. */
    kZSerializerErrorCode_NullptrParam,
    /** @brief Indicates a parameter value is out of the valid range. */
    kZSerializerErrorCode_ParamOutOfRange,
    /** @brief Indicates the string length exceeds the maximum allowed limit. */
    kZSerializerErrorCode_StringTooLarge,
};

} // error_code
} // zengine

namespace zengine {

/**
 * @brief A class responsible for serializing data into a binary stream.
 *
 * Implementation Principle:
 * This class abstracts the destination of the binary data (e.g., memory, file, network) using a generic
 * `TFunction` callback (`output_func_`). It provides typed helper methods to write primitive types and
 * checks for trivial copyability for generic objects at compile time to ensure memory safety.
 * Strings are serialized by writing their size (UInt64) followed by the character data.
 */
class CORE_DLLAPI ZSerializer : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /** @brief The maximum allowed length for a string to be serialized, set to 64 MB. */
    static inline constexpr SizeType kMaxStringLength = 64ULL * kMB;

public:
    /**
     * @brief Constructs a ZSerializer with a specific output callback function.
     * @param _output_func The function object used to write data to the underlying medium.
     */
    ZSerializer(TFunction<ReturnType(ZConstBuffer)>&& _output_func) noexcept;

    /** @brief Deleted copy constructor. */
    ZSerializer(const ZSerializer&) = delete;

    /**
     * @brief Move constructor.
     * @param _serializer The other ZSerializer instance to move from.
     */
    ZSerializer(ZSerializer&& _serializer) noexcept;

    /**
     * @brief Destructor.
     */
    ~ZSerializer() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZSerializer& operator=(const ZSerializer&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _serializer The other ZSerializer instance to move from.
     * @return A reference to this ZSerializer instance.
     */
    ZSerializer& operator=(ZSerializer&& _serializer) noexcept;

    /**
     * @brief Writes a trivially copyable object to the stream.
     * @tparam _ObjectType The type of the object to write. Must be trivially copyable.
     * @param _obj The object to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE ReturnType Write(const _ObjectType& _obj) noexcept {
        STATIC_ASSERT(
            kIsTriviallyCopyable<_ObjectType>, 
            "ZSerializer::Write: _ObjectType must be trivially copyable."
        );
        return WriteData(ZConstBuffer(&_obj, sizeof(_ObjectType)));
    }

    /**
     * @brief Writes an 8-bit signed integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteInt8(Int8 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Int8)));
    }

    /**
     * @brief Writes a 16-bit signed integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteInt16(Int16 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Int16)));
    }

    /**
     * @brief Writes a 32-bit signed integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteInt32(Int32 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Int32)));
    }

    /**
     * @brief Writes a 64-bit signed integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteInt64(Int64 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Int64)));
    }

    /**
     * @brief Writes an 8-bit unsigned integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteUInt8(UInt8 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(UInt8)));
    }

    /**
     * @brief Writes a 16-bit unsigned integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteUInt16(UInt16 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(UInt16)));
    }

    /**
     * @brief Writes a 32-bit unsigned integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteUInt32(UInt32 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(UInt32)));
    }

    /**
     * @brief Writes a 64-bit unsigned integer to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteUInt64(UInt64 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(UInt64)));
    }

    /**
     * @brief Writes a 32-bit floating-point number to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteFloat32(Float32 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Float32)));
    }

    /**
     * @brief Writes a 64-bit floating-point number to the stream.
     * @param _data The data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType WriteFloat64(Float64 _data) noexcept {
        return WriteData(ZConstBuffer(&_data, sizeof(Float64)));
    }

    /**
     * @brief Writes a string to the stream.
     * @param _string The string view containing the data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD ReturnType WriteString(ZStringView _string) noexcept;

    /**
     * @brief Writes a raw block of memory to the stream.
     * @param _buffer The constant buffer containing the data to write.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD ReturnType WriteData(ZConstBuffer _buffer) noexcept;

private:
    /** @brief The callback function used to output data. */
    TFunction<ReturnType(ZConstBuffer)> output_func_;
};

//TODO

/**
 * @brief A class responsible for deserializing data from a binary stream.
 *
 * Implementation Principle:
 * This class abstracts the source of the binary data (e.g., memory, file, network) using a generic
 * `TFunction` callback (`input_func_`). It mirrors the operation of `ZSerializer` by reading primitive
 * types directly. When reading dynamic structures like strings, it first reads the size, ensures it is within
 * limits, resizes the target buffer, and then reads the data into it.
 */
class CORE_DLLAPI ZDeserializer : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /** @brief The maximum allowed length for a string to be deserialized, shared with ZSerializer. */
    static inline constexpr SizeType kMaxStringLength = ZSerializer::kMaxStringLength;

public:
    /**
     * @brief Constructs a ZDeserializer with a specific input callback function.
     * @param _input_func The function object used to read data from the underlying medium.
     */
    ZDeserializer(TFunction<ReturnType(ZBuffer)>&& _input_func) noexcept;

    /** @brief Deleted copy constructor. */
    ZDeserializer(const ZDeserializer&) = delete;

    /**
     * @brief Move constructor.
     * @param _deserializer The other ZDeserializer instance to move from.
     */
    ZDeserializer(ZDeserializer&& _deserializer) noexcept;

    /**
     * @brief Destructor.
     */
    ~ZDeserializer() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZDeserializer& operator=(const ZDeserializer&) = delete;

    /**
     * @brief Move assignment operator.
     * @param _deserializer The other ZDeserializer instance to move from.
     * @return A reference to this ZDeserializer instance.
     */
    ZDeserializer& operator=(ZDeserializer&& _deserializer) noexcept;

    /**
     * @brief Reads a trivially copyable object from the stream.
     * @tparam _ObjectType The type of the object to read. Must be trivially copyable.
     * @param _obj_ptr Pointer to the object where data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE ReturnType Read(_ObjectType* _obj_ptr) noexcept {
        STATIC_ASSERT(
            kIsTriviallyConstructible<_ObjectType>,
            "ZDeserializer::Read: _ObjectType must be trivially constructible."
        );
        STATIC_ASSERT(
            kIsTriviallyCopyable<_ObjectType>,
            "ZDeserializer::Read: _ObjectType must be trivially copyable."
        );
        STATIC_ASSERT(
            kIsTriviallyDestructible<_ObjectType>,
            "ZDeserializer::Read: _ObjectType must be trivially destructible."
        );
        return ReadData(ZBuffer(_obj_ptr, sizeof(_ObjectType)));
    }

    /**
         * @brief Reads an 8-bit signed integer from the stream.
         * @param _data Pointer to the variable where the data will be stored.
         * @return Returns kOK on success, otherwise returns an error code.
         */
    NODISCARD FORCEINLINE ReturnType ReadInt8(Int8* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Int8)));
    }

    /**
     * @brief Reads a 16-bit signed integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadInt16(Int16* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Int16)));
    }

    /**
     * @brief Reads a 32-bit signed integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadInt32(Int32* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Int32)));
    }

    /**
     * @brief Reads a 64-bit signed integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadInt64(Int64* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Int64)));
    }

    /**
     * @brief Reads an 8-bit unsigned integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadUInt8(UInt8* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(UInt8)));
    }

    /**
     * @brief Reads a 16-bit unsigned integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadUInt16(UInt16* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(UInt16)));
    }

    /**
     * @brief Reads a 32-bit unsigned integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadUInt32(UInt32* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(UInt32)));
    }

    /**
     * @brief Reads a 64-bit unsigned integer from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadUInt64(UInt64* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(UInt64)));
    }

    /**
     * @brief Reads a 32-bit floating-point number from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadFloat32(Float32* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Float32)));
    }

    /**
     * @brief Reads a 64-bit floating-point number from the stream.
     * @param _data Pointer to the variable where the data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD FORCEINLINE ReturnType ReadFloat64(Float64* _data) noexcept {
        return ReadData(ZBuffer(_data, sizeof(Float64)));
    }

    /**
     * @brief Reads a string from the stream.
     * @param _string_ptr Pointer to the ZString object where the read string will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD ReturnType ReadString(ZString* _string_ptr) noexcept;

    /**
     * @brief Reads a raw block of memory from the stream.
     * @param _buffer Pointer to the buffer where the read data will be stored.
     * @return Returns kOK on success, otherwise returns an error code.
     */
    NODISCARD ReturnType ReadData(ZBuffer _buffer) noexcept;

private:
    /** @brief The callback function used to input data. */
    TFunction<ReturnType(ZBuffer)> input_func_;
};

} // zengine