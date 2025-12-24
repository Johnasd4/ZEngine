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

#include "f_memory_pool.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief The allocator for stl containers.
 * @tparam _ObjectType The type of object to be allocated.
 * @tparam _IfAllocFromThreadLocalMemoryPool Flag indicating if memory should be allocated from the thread-local pool.
 */
template<typename _ObjectType, Bool _IfAllocFromThreadLocalMemoryPool = false>
class TAllocator {
public:
    /** @brief Type alias for the object type managed by this allocator. */
    using value_type = _ObjectType;

    /**
     * @brief Structure to rebind the allocator to a different object type.
     * @tparam _OtherObjectType The new object type to rebind to.
     */
    template <typename _OtherObjectType>
    struct rebind {
        /** @brief The rebound allocator type. */
        using other = TAllocator<_OtherObjectType, _IfAllocFromThreadLocalMemoryPool>;
    };

    /**
     * @brief Gets the static instance of the allocator.
     * @return A reference to the allocator instance.
     */
    NODISCARD static TAllocator& Instance() noexcept {
        static TAllocator allocator;
        return allocator;
    }

    /** @brief Default constructor. */
    FORCEINLINE TAllocator() noexcept {}

    /**
     * @brief Copy constructor from another allocator type.
     * @tparam _OtherObjectType The object type of the source allocator.
     * @param _alocator The source allocator to copy from.
     */
    template<typename _OtherObjectType>
    FORCEINLINE TAllocator(const TAllocator<_OtherObjectType>& _alocator) noexcept {}

    /** @brief Destructor. */
    FORCEINLINE ~TAllocator() {}

    /**
     * @brief Allocates memory for a specified number of objects.
     * @param _capacity The number of objects to allocate memory for.
     * @return A pointer to the allocated memory block.
     */
    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        if constexpr (_IfAllocFromThreadLocalMemoryPool) {
            return reinterpret_cast<_ObjectType*>(memory_pool::ApplyThreadLocalMemory(
                (SizeType)(_capacity * sizeof(_ObjectType))
            ));
        }
        else {
            return reinterpret_cast<_ObjectType*>(memory_pool::ApplyGlobalMemory(
                (SizeType)(_capacity * sizeof(_ObjectType))
            ));
        }

    }

    /**
     * @brief Deallocates memory previously allocated.
     * @param _object_ptr Pointer to the memory block to deallocate.
     * @param _capacity The number of objects originally allocated (unused in logic but required by interface).
     */
    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        if constexpr (_IfAllocFromThreadLocalMemoryPool) {
            memory_pool::ReleaseThreadLocalMemory(reinterpret_cast<Void*>(_object_ptr));
        }
        else {
            memory_pool::ReleaseGlobalMemory(reinterpret_cast<Void*>(_object_ptr));
        }
    }

private:
    static Void* operator new(SizeType) = delete;
    static Void operator delete(Void*) = delete;
};

/**
 * @brief The allocator for smart pointers.
 * @tparam _ObjectType The type of object managed by the smart pointer.
 */
template<typename _ObjectType>
class TSmartPointerAllocator {
public:
    /** @brief Type alias for the object type managed by this allocator. */
    using value_type = _ObjectType;

    /** @brief Default constructor. */
    FORCEINLINE TSmartPointerAllocator() noexcept {}

    /**
     * @brief Copy constructor from another smart pointer allocator type.
     * @tparam _OtherObjectType The object type of the source allocator.
     * @param _alocator The source allocator to copy from.
     */
    template<typename _OtherObjectType>
    FORCEINLINE TSmartPointerAllocator(const TSmartPointerAllocator<_OtherObjectType>& _alocator) noexcept {}

    /** @brief Destructor. */
    FORCEINLINE ~TSmartPointerAllocator() {}

    /**
     * @brief Allocates memory for the smart pointer control block.
     * @param _capacity The number of objects to allocate (ignored as control blocks are fixed size per request).
     * @return A pointer to the allocated control block memory.
     */
    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        return reinterpret_cast<_ObjectType*>(memory_pool::ApplySmartPointerControlBlockMemory());
    }

    /**
     * @brief Deallocates memory for the smart pointer control block.
     * @param _object_ptr Pointer to the control block memory to deallocate.
     * @param _capacity The number of objects (ignored).
     */
    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        memory_pool::ReleaseSmartPointerControlBlockMemory(reinterpret_cast<Void*>(_object_ptr));
    }

private:
    static Void* operator new(SizeType) = delete;
    static Void operator delete(Void*) = delete;
};

}//zengine