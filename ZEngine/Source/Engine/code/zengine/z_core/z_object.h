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

namespace zengine {

/**
 * @brief The base class for objects that require custom memory management via memory pools.
 *
 * Inheriting from this class overrides the new and delete operators to use the engine's memory pool system
 * instead of the default system heap.
 *
 * @warning Adding any member variables to this class will cause fatal errors due to size mismatches in derived classes
 * or strict memory layout requirements!
 *
 * @tparam _IsGlobal If true, memory is allocated from the global memory pool. If false, it uses the thread-local pool.
 */
template<Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class ZObject {
public:
    /**
     * @brief Overloads the new operator to allocate memory from the memory pool.
     * @param _size The size of the memory to allocate.
     * @return Void* Pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) noexcept {
        if constexpr (_IsGlobal) {
            return memory_pool::ApplyGlobalMemory(static_cast<SizeType>(_size));
        }
        else {
            return memory_pool::ApplyThreadLocalMemory(static_cast<SizeType>(_size));
        }
    }

    /**
     * @brief Overloads the placement new operator.
     * @param _size The size of the memory (unused).
     * @param _memory_ptr The pointer to the memory to construct the object in.
     * @return Void* The passed memory pointer.
     */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size, Void* _memory_ptr) noexcept {
        return _memory_ptr;
    }

    /**
     * @brief Overloads the delete operator to release memory back to the memory pool.
     * @param _memory_ptr The pointer to the memory to release.
     */
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) noexcept {
        if constexpr (_IsGlobal) {
            memory_pool::ReleaseGlobalMemory(_memory_ptr);
        }
        else {
            memory_pool::ReleaseThreadLocalMemory(_memory_ptr);
        }
    }

    /**
     * @brief Overloads the placement delete operator (corresponding to placement new).
     * @param _delete_memory The pointer to the memory being deleted.
     * @param _memory_ptr The pointer passed to placement new.
     */
    NODISCARD FORCEINLINE static Void operator delete(Void* _delete_memory, Void* _memory_ptr) noexcept {}

    /**
     * @brief Copy assignment operator.
     * @param _obj The object to copy from.
     * @return const ZObject& Reference to this object.
     */
    FORCEINLINE constexpr const ZObject& operator=(const ZObject& _obj) noexcept { return *this; }

    /**
     * @brief Move assignment operator.
     * @param _obj The object to move from.
     * @return const ZObject& Reference to this object.
     */
    FORCEINLINE constexpr const ZObject& operator=(ZObject&& _obj) noexcept { return *this; }

    /**
     * @brief Checks if two objects are the same instance based on address.
     * @param _obj The object to compare with.
     * @return Bool True if the objects are the same instance, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const ZObject& _obj) noexcept {
        return this == &_obj;
    }

    /**
     * @brief Checks if two objects are different instances based on address.
     * @param _obj The object to compare with.
     * @return Bool True if the objects are different instances, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const ZObject& _obj) noexcept {
        return this != &_obj;
    }

protected:
    /** @brief Default constructor. */
    FORCEINLINE constexpr ZObject() noexcept {}

    /** @brief Copy constructor. */
    FORCEINLINE constexpr ZObject(const ZObject& _obj) noexcept {}

    /** @brief Move constructor. */
    FORCEINLINE constexpr ZObject(ZObject&& _obj) noexcept {}

    /** @brief Destructor. */
    FORCEINLINE constexpr ~ZObject() noexcept {}

private:
    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;
};

}//zengine