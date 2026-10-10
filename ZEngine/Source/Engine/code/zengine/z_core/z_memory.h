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
namespace internal {

/**
 * @brief A wrapper class for raw memory management.
 * 
 * Principle: This class encapsulates a raw memory pointer, managing its lifecycle via a custom memory pool.
 * It enforces exclusive ownership (move-only semantics) to prevent expensive implicit copies.
 * It supports dynamic resizing by tracking size and capacity, reallocating only when necessary.
 * 
 * @tparam _IsGlobal If true, the memory is allocated from the global memory pool.
 */
template<Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>    
class TMemory : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor.
     * Initializes an empty memory block with null pointer and zero size.
     */
    TMemory() noexcept
        : SuperType_()
        , size_(0ULL)
        , capacity_(0ULL)
        , data_ptr_(nullptr)
    {}

    /**
     * @brief Copy constructor.
     * Creates a new memory block and copies the data from the source object.
     */
    TMemory(const TMemory& _memory) noexcept 
        : SuperType_(_memory)
        , size_(_memory.size_)
        , data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory<_IsGlobal>(_memory.size_, &capacity_)))
    {
        if (size_ != 0ULL) {
            Copy(data_ptr_, _memory.data_ptr_, size_);
        }
    }

    /**
     * @brief Move constructor.
     * Transfers ownership of the memory resource from another object.
     * 
     * @param _memory The source memory object to move from.
     */
    TMemory(TMemory&& _memory) noexcept
        : SuperType_(std::move(_memory))
        , data_ptr_(_memory.data_ptr_)
        , size_(_memory.size_)
        , capacity_(_memory.capacity_)
    {
        _memory.data_ptr_ = nullptr;
        _memory.size_ = 0ULL;
        _memory.capacity_ = 0ULL;
    }

    /**
     * @brief Constructor with initial size.
     * Allocates a memory block of the specified size from the memory pool.
     * 
     * @param _size The initial size of the memory in bytes.
     */
    TMemory(SizeType _size) noexcept
        : SuperType_()
        , data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory<_IsGlobal>(_size, &capacity_)))
        , size_(_size)
    {}

    /**
     * @brief Destructor.
     * Releases the held memory back to the memory pool if it is valid.
     */
    ~TMemory() noexcept {
        if (data_ptr_ != nullptr) {
            memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);
        }
    }

    /**
     * @brief Copy assignment operator.
     * Resizes the current memory block and copies data from the source object.
     * 
     * @param _memory The source memory object to copy from.
     * @return Reference to this object.
     */
    TMemory& operator=(const TMemory& _memory) noexcept {
        if (this == &_memory) {
            return *this;
        }
        if (_memory.size_ > capacity_) {
            Byte* data_ptr = static_cast<Byte*>(memory_pool::ApplyMemory<_IsGlobal>(_memory.size_, &capacity_));
            if (data_ptr_ != nullptr) {
                memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);
            }
            data_ptr_ = data_ptr;
        }
        size_ = _memory.size_;  
        if (size_ != 0ULL) {
            Copy(data_ptr_, _memory.data_ptr_, _memory.size_);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * Releases current memory and takes ownership of the memory from the source object.
     * 
     * @param _memory The source memory object to move from.
     * @return Reference to this object.
     */
    TMemory& operator=(TMemory&& _memory) noexcept {
        if (this == &_memory) {
            return *this;
        }
        if (data_ptr_ != nullptr) {
            memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);
        }
        data_ptr_ = _memory.data_ptr_;
        size_ = _memory.size_;
        capacity_ = _memory.capacity_;
        _memory.data_ptr_ = nullptr;
        _memory.size_ = 0ULL;
        _memory.capacity_ = 0ULL;
        return *this;
    }

    /**
     * @brief Gets the raw memory pointer cast to the specified type.
     *
     * @tparam _ObjectType The type to cast the pointer to.
     * @return Pointer to the memory block.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept {
        return reinterpret_cast<_ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the raw memory pointer cast to the specified type (read-only).
     *
     * @tparam _ObjectType The type to cast the pointer to.
     * @return Const pointer to the memory block.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    /**
     * @brief Gets the current size of the memory block.
     *
     * @return The size in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }

    /**
     * @brief Gets the current allocated capacity of the memory block.
     *
     * @return The capacity in bytes.
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return capacity_; }

    /**
     * @brief Accesses the byte at the specified position.
     * 
     * @param _pos The index of the byte to access.
     * @return Reference to the byte at the specified position.
     */
    NODISCARD FORCEINLINE Byte& operator[](const SizeType _pos) noexcept { 
        ASSERT(_pos < size_, "TMemory: _pos out of bounds!");
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Accesses the byte at the specified position (read-only).
     * 
     * @param _pos The index of the byte to access.
     * @return Const reference to the byte at the specified position.
     */
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _pos) const noexcept { 
        ASSERT(_pos < size_, "TMemory: _pos out of bounds!");
        return data_ptr_[_pos]; 
    }

    /**
     * @brief Accesses an object of type _ObjectType at the specified byte offset.
     * Performs strict checks for boundary and memory alignment.
     * 
     * @tparam _ObjectType The type of the object to retrieve.
     * @param _pos The byte offset where the object is located.
     * @return Reference to the object at the specified offset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType& At(const SizeType _pos) noexcept {
        ASSERT(
            (reinterpret_cast<PointerType>(data_ptr_ + _pos) % alignof(_ObjectType)) == 0, 
            "TMemory: Alignment violation!"
        );
        ASSERT(_pos + sizeof(_ObjectType) <= size_, "TMemory: _pos out of bounds!");
        return *(reinterpret_cast<_ObjectType*>(data_ptr_ + _pos));
    }

    /**
     * @brief Accesses an object of type _ObjectType at the specified byte offset (read-only).
     * Performs strict checks for boundary and memory alignment.
     * 
     * @tparam _ObjectType The type of the object to retrieve.
     * @param _pos The byte offset where the object is located.
     * @return Const reference to the object at the specified offset.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _pos) const noexcept {
        ASSERT(
            (reinterpret_cast<PointerType>(data_ptr_ + _pos) % alignof(_ObjectType)) == 0,
            "TMemory: Alignment violation!"
        );
        ASSERT(_pos + sizeof(_ObjectType) <= size_, "TMemory: _pos out of bounds!");
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_ + _pos));
    }

    /**
     * @brief Resizes the memory block to the specified size.
     * If the new size is larger than the current capacity, a reallocation occurs, and existing data is copied.
     * 
     * @param _size The new size in bytes.
     */
    Void Resize(SizeType _size) noexcept {
        if (_size > capacity_) {
            Byte* data_ptr = static_cast<Byte*>(memory_pool::ApplyMemory<_IsGlobal>(_size, &capacity_));
            if (size_ != 0ULL) {
                Copy(data_ptr, data_ptr_, size_);
            }
            if (data_ptr_ != nullptr) {
                memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);
            }
            data_ptr_ = data_ptr;
        }
        size_ = _size;
    }

    /**
     * @brief Swaps the contents of this memory object with another.
     * 
     * @param _memory The other memory object to swap with.
     */
    FORCEINLINE Void Swap(TMemory& _memory) noexcept { ::zengine::Swap(this, &_memory); }

private:
    /** @brief Current used size of the memory block. */
    SizeType size_;
    /** @brief Current allocated capacity of the memory block. */
    SizeType capacity_;
    /** @brief Pointer to the raw memory block. */
    Byte* data_ptr_;

};

} // internal
} // zengine

namespace zengine {

using ZMemory = internal::TMemory<true>;
using ZMemoryLocal = internal::TMemory<false>;

} // zengine