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
 * @brief A standard allocator implementation for the engine.
 * @tparam _ObjectType The type of object to be managed by the allocator.
 * @tparam _IsGlobal Indicates whether to use the global memory pool (true) or thread-local memory pool (false).
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TAllocator {
    NO_NEW_DELETE
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
        using other = TAllocator<_OtherObjectType, _IsGlobal>;
    };

    /**
     * @brief Gets the singleton instance of the allocator.
     * @return A reference to the static allocator instance.
     */
    NODISCARD static TAllocator& GetInstance() noexcept {
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
    FORCEINLINE TAllocator(const TAllocator<_OtherObjectType, _IsGlobal>& _alocator) noexcept {}

    /** @brief Destructor. */
    FORCEINLINE ~TAllocator() = default;

    /**
     * @brief Allocates memory for a specified number of objects.
     * @param _capacity The number of objects to allocate.
     * @return A pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        return reinterpret_cast<_ObjectType*>(memory_pool::ApplyMemory<_IsGlobal>(
            static_cast<SizeType>(_capacity * sizeof(_ObjectType))
        ));
    }

    /**
     * @brief Deallocates the memory for the specified object pointer.
     * @param _object_ptr A pointer to the memory to deallocate.
     * @param _capacity The number of objects originally allocated (unused).
     */
    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        memory_pool::ReleaseMemory<_IsGlobal>(_object_ptr);
    }
};

/**
 * @brief An allocator specialized for smart pointer control blocks.
 * @tparam _ObjectType The type of object managed by the smart pointer.
 */
template<typename _ObjectType>
class TSmartPointerAllocator {
    NO_NEW_DELETE
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
    FORCEINLINE ~TSmartPointerAllocator() = default;

    /**
     * @brief Allocates memory for the smart pointer control block.
     * @param _capacity The number of objects to allocate (ignored for control blocks).
     * @return A pointer to the allocated control block memory.
     */
    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        return reinterpret_cast<_ObjectType*>(memory_pool::ApplySmartPointerControlBlockMemory());
    }

    /**
     * @brief Deallocates the memory for the smart pointer control block.
     * @param _object_ptr A pointer to the control block memory to deallocate.
     * @param _capacity The number of objects (ignored).
     */
    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        memory_pool::ReleaseSmartPointerControlBlockMemory(reinterpret_cast<Void*>(_object_ptr));
    }
};

} // zengine