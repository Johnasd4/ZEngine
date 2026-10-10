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

namespace zengine {

/**
 * @brief Memory view class.
 *
 * This class serves as a lightweight, non-owning wrapper around a contiguous region of memory.
 * It provides a read-only view into the data without managing the memory's lifecycle (no allocation or deallocation).
 * It is designed for efficient memory access and type reinterpretation without deep copying.
 */
class ZMemoryView {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor.
     * Initializes an empty memory view with a null pointer and zero size.
     */
    FORCEINLINE ZMemoryView() noexcept
        : data_ptr_(nullptr)
        , size_(0ULL)
    {}

    /**
     * @brief Copy constructor.
     * Creates a new view pointing to the same memory region as the source view.
     * @param _view The source memory view to copy from.
     */
    FORCEINLINE ZMemoryView(const ZMemoryView& _view) noexcept
        : data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {}

    /**
     * @brief Parameterized constructor.
     * Creates a memory view from a raw pointer and a size.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr Pointer to the start of the memory region.
     * @param _size The size of the memory region in bytes.
     */
    template<typename PointerType>
    FORCEINLINE ZMemoryView(const PointerType* _data_ptr, SizeType _size) noexcept
        : data_ptr_(reinterpret_cast<const Byte*>(_data_ptr))
        , size_(_size)
    {}

    /**
     * @brief Destructor.
     * Default destructor. Does not release the underlying memory.
     */
    FORCEINLINE ~ZMemoryView() noexcept = default;

    /**
     * @brief Assignment operator.
     * Updates the view to point to the same memory region as the source view.
     * @param _view The source memory view to assign from.
     * @return ZMemoryView& A reference to this updated memory view.
     */
    FORCEINLINE ZMemoryView& operator=(const ZMemoryView& _view) noexcept {
        data_ptr_ = _view.data_ptr_;
        size_ = _view.size_;
        return *this;
    }

    /**
     * @brief Assigns a new memory region to the view.
     * @tparam PointerType The type of the source pointer.
     * @param _data_ptr Pointer to the start of the new memory region.
     * @param _size The size of the new memory region in bytes.
     * @return Void
     */
    template<typename PointerType>
    NODISCARD FORCEINLINE Void Assign(const PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = reinterpret_cast<const Byte*>(_data_ptr);
        size_ = _size;
    }

    /**
     * @brief Gets the underlying data pointer cast to a specific type.
     * @tparam _ObjectType The type to cast the pointer to.
     * @return const _ObjectType* A constant pointer to the beginning of the memory view.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the size of the memory view.
     * @return SizeType The size of the view in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }

    /**
     * @brief Gets the capacity of the memory view.
     * For a view, the capacity is equivalent to its size.
     * @return SizeType The capacity of the view in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return size_; }

    /**
     * @brief Accesses a byte at a specific index.
     * @param _pos The index (offset) of the byte to access.
     * @return const Byte& A constant reference to the byte at the specified index.
     */
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _pos) const noexcept {
        ASSERT(_pos < size_, "ZMemoryView: _pos out of bounds!");
        return data_ptr_[_pos];
    }

    /**
     * @brief Accesses an object of a specific type at a byte offset.
     * Interprets the memory at the given offset as the specified type.
     * @tparam _ObjectType The type of object to retrieve.
     * @param _pos The byte offset from the start of the view.
     * @return const _ObjectType& A constant reference to the object at the specified offset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _pos) const noexcept {
        ASSERT(_pos + sizeof(_ObjectType) <= size_, "ZMemoryView: _pos out of bounds!");
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_ + _pos));
    }

private:
    /** @brief Pointer to the underlying memory region (read-only). */
    const Byte* data_ptr_;
    /** @brief Size of the memory region in bytes. */
    SizeType size_;
};

} // zengine