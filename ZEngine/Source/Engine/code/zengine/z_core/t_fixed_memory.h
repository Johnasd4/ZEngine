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

#include "z_object.h"

namespace zengine {

/**
 * @brief A memory piece with fixed size and word alignment.
 * @tparam _Size The size of the memory block in bytes.
 */
template<SizeType _Size>
class WORD_ALIGN TFixedMemory : public ZObject {
    STATIC_ASSERT(_Size > 0U, "TFixedMemory: _Size must be greater than 0!");
public:
    /** @brief The base class type. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor. Leaves memory uninitialized for performance.
     */
    FORCEINLINE TFixedMemory() noexcept 
        : SuperType_()
    {}

    /**
     * @brief Copy constructor. Copies data from another memory block.
     * @param _mem The source memory block to copy from.
     */
    FORCEINLINE TFixedMemory(const TFixedMemory& _mem) noexcept : SuperType_(_mem) { 
        Copy(data_ptr_, _mem.data_ptr_, _Size);
    }

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~TFixedMemory() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _mem The source memory block to assign from.
     * @return Reference to this memory block.
     */
    FORCEINLINE TFixedMemory& operator=(const TFixedMemory& _mem) noexcept {
        if (this == &_mem) {
            return *this;
        }
        Copy(data_ptr_, _mem.data_ptr_, _Size);
        return *this;
    }

    /**
     * @brief Gets the raw data pointer cast to a specific type.
     * @tparam _ObjectType The type to cast the pointer to.
     * @return Pointer to the data.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept {
        return reinterpret_cast<_ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the raw data pointer cast to a specific type (const).
     * @tparam _ObjectType The type to cast the pointer to.
     * @return Const pointer to the data.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the size of the memory block.
     * @return The size of the memory block in bytes.
     */
    FORCEINLINE static constexpr SizeType GetSize() noexcept { return _Size; }

    /**
     * @brief Accesses the byte at the specified index.
     * @param _pos The index to access.
     * @return Reference to the byte at the specified index.
     */
    NODISCARD FORCEINLINE Byte& operator[](const SizeType _pos) noexcept { 
        ASSERT(_pos < _Size, "TFixedMemory: _pos out of bounds!");
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Accesses the byte at the specified index (const).
     * @param _pos The index to access.
     * @return Const reference to the byte at the specified index.
     */
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _pos) const noexcept { 
        ASSERT(_pos < _Size, "TFixedMemory: _pos out of bounds!"); 
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Interprets the memory at a specific byte offset as a specific object type.
     * @tparam _ObjectType The type to interpret the memory as.
     * @param _pos The byte offset to start reading from.
     * @return Reference to the interpreted object.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType& At(const SizeType _pos) {
        ASSERT(_pos + sizeof(_ObjectType) <= _Size, "TFixedMemory: _pos out of bounds!");
        return *(reinterpret_cast<_ObjectType*>(data_ptr_ + _pos));
    }

    /**
     * @brief Interprets the memory at a specific byte offset as a specific object type (const).
     * @tparam _ObjectType The type to interpret the memory as.
     * @param _pos The byte offset to start reading from.
     * @return Const reference to the interpreted object.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _pos) const {
        ASSERT(_pos + sizeof(_ObjectType) <= _Size, "TFixedMemory: _pos out of bounds!");
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_ + _pos));
    }

    /**
     * @brief Clears the memory block by setting all bytes to zero.
     */
    FORCEINLINE Void Clear() noexcept { memset(data_ptr_, 0, _Size); }

private:
    /** @brief The internal raw byte storage. */
    Byte data_ptr_[_Size];
};

} // zengine