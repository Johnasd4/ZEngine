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

#include "t_smart_pointer.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A mutable memory buffer view that acts as a wrapper around a raw pointer and size.
 *
 * This class implements a non-owning memory view (similar to std::span). It does not allocate
 * or deallocate memory on the heap. It simply points to an existing memory block and provides
 * an interface to access and manipulate that memory safely (with bounds checking).
 * As a ZObject-style lightweight object, heap allocation of this class itself is prohibited.
 */
class ZBuffer {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor. Initializes an empty buffer view (nullptr, size 0).
     */
    FORCEINLINE ZBuffer() noexcept
        : data_ptr_(nullptr)
        , size_(0ULL)
    {}

    /**
     * @brief Copy constructor. Creates a new view pointing to the same memory as the source.
     * @param _buffer The source buffer view to copy from.
     */
    FORCEINLINE ZBuffer(const ZBuffer& _buffer) noexcept
        : data_ptr_(_buffer.data_ptr_)
        , size_(_buffer.size_)
    {}

    /**
     * @brief Explicit constructor from a raw pointer and size.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr The raw pointer to the memory block.
     * @param _size The size of the memory block in bytes.
     */
    template<typename PointerType>
    FORCEINLINE explicit ZBuffer(PointerType* _data_ptr, SizeType _size) noexcept
        : data_ptr_(reinterpret_cast<Byte*>(_data_ptr))
        , size_(_size)
    {}

    /**
     * @brief Default destructor.
     */
    ~ZBuffer() noexcept = default;

    /**
     * @brief Assignment operator. Updates this view to point to the same memory as the source.
     * @param _buffer The source buffer view.
     * @return A reference to this updated buffer object.
     */
    FORCEINLINE ZBuffer& operator=(const ZBuffer& _buffer) noexcept {
        data_ptr_ = _buffer.data_ptr_;
        size_ = _buffer.size_;
        return *this;
    }

    /**
     * @brief Re-assigns the buffer view to a new memory block.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr The raw pointer to the new memory block.
     * @param _size The new size of the memory block in bytes.
     * @return Void.
     */
    template<typename PointerType>
    NODISCARD FORCEINLINE Void Assign(PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = reinterpret_cast<Byte*>(_data_ptr);
        size_ = _size;
    }

    /**
     * @brief Retrieves the underlying raw pointer cast to a specific type.
     * @tparam _ObjectType The type to cast the pointer to.
     * @return A pointer of type _ObjectType to the start of the buffer.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<_ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the size of the buffer.
     * @return The size of the buffer in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }

    /**
     * @brief Accesses the byte at the specified position.
     * @param _pos The byte offset index.
     * @return A reference to the Byte at the specified position.
     */
    NODISCARD FORCEINLINE Byte& operator[](const SizeType _pos) const noexcept { 
        ASSERT(_pos < size_, "ZBuffer: _pos out of bounds!");
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Accesses an object of a specific type at the specified byte offset.
     * @tparam _ObjectType The type of the object to retrieve.
     * @param _pos The byte offset from the start of the buffer.
     * @return A reference to the object at the specified offset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType& At(const SizeType _pos) const noexcept {
        ASSERT(_pos + sizeof(_ObjectType) <= size_, "ZBuffer: _pos out of bounds!");
        return *(reinterpret_cast<_ObjectType*>(data_ptr_ + _pos));
    }

private:
    /** @brief Pointer to the start of the memory block. */
    Byte* data_ptr_;
    /** @brief Size of the memory block in bytes. */
    SizeType size_;
};

/**
 * @brief A constant memory buffer view that acts as a read-only wrapper around a raw pointer and size.
 *
 * This class implements a non-owning, read-only memory view. It allows safe access to memory
 * without permitting modification of the underlying data. Like ZBuffer, it does not manage
 * memory allocation and cannot be instantiated on the heap.
 */
class ZConstBuffer {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor. Initializes an empty constant buffer view.
     */
    FORCEINLINE ZConstBuffer() noexcept
        : data_ptr_(nullptr)
        , size_(0ULL)
    {}

    /**
     * @brief Copy constructor. Creates a new read-only view pointing to the same memory.
     * @param _buffer The source constant buffer view.
     */
    FORCEINLINE ZConstBuffer(const ZConstBuffer& _buffer) noexcept
        : data_ptr_(_buffer.data_ptr_)
        , size_(_buffer.size_)
    {}

    /**
     * @brief Conversion constructor. Creates a read-only view from a mutable ZBuffer.
     * @param _buffer The mutable source buffer.
     */
    FORCEINLINE ZConstBuffer(const ZBuffer& _buffer) noexcept
        : data_ptr_(_buffer.GetDataPtr<const Byte>())
        , size_(_buffer.GetSize())
    {}

    /**
     * @brief Explicit constructor from a raw pointer and size.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr The raw pointer to the memory block.
     * @param _size The size of the memory block in bytes.
     */
    template<typename PointerType>
    FORCEINLINE explicit ZConstBuffer(const PointerType* _data_ptr, SizeType _size) noexcept
        : data_ptr_(reinterpret_cast<const Byte*>(_data_ptr))
        , size_(_size)
    {}

    /**
     * @brief Default destructor.
     */
    ~ZConstBuffer() noexcept = default;

    /**
     * @brief Assignment operator. Updates this view to point to the same memory as the source.
     * @param _buffer The source constant buffer view.
     * @return A reference to this updated constant buffer object.
     */
    FORCEINLINE ZConstBuffer& operator=(const ZConstBuffer& _buffer) noexcept {
        data_ptr_ = _buffer.data_ptr_;
        size_ = _buffer.size_;
        return *this;
    }

    /**
     * @brief Assignment operator from mutable buffer. Updates this view to point to the mutable buffer's memory.
     * @param _buffer The source mutable buffer view.
     * @return A reference to this updated constant buffer object.
     */
    FORCEINLINE ZConstBuffer& operator=(const ZBuffer& _buffer) noexcept {
        data_ptr_ = _buffer.GetDataPtr<const Byte>();
        size_ = _buffer.GetSize();
        return *this;
    }

    /**
     * @brief Re-assigns the buffer view to a new memory block.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr The raw pointer to the new memory block.
     * @param _size The new size of the memory block in bytes.
     * @return Void.
     */
    template<typename PointerType>
    NODISCARD FORCEINLINE Void Assign(const PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = reinterpret_cast<const Byte*>(_data_ptr);
        size_ = _size;
    }

    /**
     * @brief Retrieves the underlying raw pointer cast to a specific type (read-only).
     * @tparam _ObjectType The type to cast the pointer to.
     * @return A const pointer of type _ObjectType to the start of the buffer.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the size of the buffer.
     * @return The size of the buffer in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }


    /**
     * @brief Accesses the byte at the specified position (read-only).
     * @param _pos The byte offset index.
     * @return A const reference to the Byte at the specified position.
     */
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _pos) const noexcept { 
        ASSERT(_pos < size_, "ZConstBuffer: _pos out of bounds!"); 
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Accesses an object of a specific type at the specified byte offset (read-only).
     * @tparam _ObjectType The type of the object to retrieve.
     * @param _pos The byte offset from the start of the buffer.
     * @return A const reference to the object at the specified offset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _pos) const noexcept {
        ASSERT(_pos + sizeof(_ObjectType) <= size_, "ZConstBuffer: _pos out of bounds!");
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_ + _pos));
    }

private:
    /** @brief Pointer to the start of the memory block (read-only). */
    const Byte* data_ptr_;
    /** @brief Size of the memory block in bytes. */
    SizeType size_;
};

} // zengine