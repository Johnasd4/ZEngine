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
namespace error_code {

/**
 * @brief Enumeration of error codes related to memory pool operations.
 */
enum FMemoryPoolErrorCodeEnum : ReturnType {
    /** @brief Error indicating a linking issue within the memory pool. */
    kFMemoryPoolErrorCode_LinkError = kErrorCodeBase_FMemoryPool,
    /** @brief Error indicating a system-level failure. */
    kFMemoryPoolErrorCode_SystemOrLibraryError,
    /** @brief Error indicating a null pointer was passed as a parameter. */
    kFMemoryPoolErrorCode_NullptrParam,
    /** @brief Error indicating a parameter is out of the valid range. */
    kFMemoryPoolErrorCode_ParamOutOfRange,
    /** @brief Error indicating the memory pool has run out of memory. */
    kFMemoryPoolErrorCode_OutOfMemory,
    /** @brief Error indicating that the memory being accessed does not belong to the global memory pool. */
    kFMemoryPoolErrorCode_NotGlobalMemory,
    /** @brief Error indicating that the memory being accessed does not belong to the current thread's memory pool. */
    kFMemoryPoolErrorCode_NotCurrentThreadMemory
};

}//error_code
}//zengine

namespace zengine {
namespace memory_pool {

/**
 * @brief Applys thread-local memory by the size given and returns the pointer of the memory.
 * @param _size The size of memory to allocate.
 * @return Void* Pointer to the allocated memory, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ApplyThreadLocalMemory(SizeType _size) noexcept;

/**
 * @brief Applys thread-local memory by the size given and returns the pointer of the memory.
 * @param _size The size of memory to allocate.
 * @param _memory_size_ptr Output pointer to store the actual size of the allocated memory block.
 * @return Void* Pointer to the allocated memory, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ApplyThreadLocalMemory(SizeType _size, SizeType* _memory_size_ptr) noexcept;

/**
 * @brief Applys global memory by the size given and returns the pointer of the memory.
 * @param _size The size of memory to allocate.
 * @return Void* Pointer to the allocated global memory, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ApplyGlobalMemory(SizeType _size) noexcept;

/**
 * @brief Applys global memory by the size given and returns the pointer of the memory.
 * @param _size The size of memory to allocate.
 * @param _memory_size_ptr Output pointer to store the actual size of the allocated memory block.
 * @return Void* Pointer to the allocated global memory, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ApplyGlobalMemory(SizeType _size, SizeType* _memory_size_ptr) noexcept;

/**
 * @brief Reapplys the thread-local memory and copies the data to the new memory.
 * @param _old_memory_ptr Pointer to the existing thread-local memory block.
 * @param _size The new size of memory to allocate.
 * @return Void* Pointer to the new memory block, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ReapplyThreadLocalMemory(Void* _old_memory_ptr, SizeType _size) noexcept;

/**
 * @brief Reapplys the global memory and copies the data to the new memory.
 * @param _old_memory_ptr Pointer to the existing global memory block.
 * @param _size The new size of memory to allocate.
 * @return Void* Pointer to the new global memory block, or nullptr if allocation fails.
 */
CORE_DLLAPI NODISCARD Void* ReapplyGlobalMemory(Void* _old_memory_ptr, SizeType _size) noexcept;

/**
 * @brief Calculate the minimum size of the memory block that satisfies the memory requirement.
 * @param _size The requested memory size.
 * @return SizeType The aligned or actual size that will be allocated.
 */
CORE_DLLAPI NODISCARD SizeType CalculateMemory(SizeType _size) noexcept;

/**
 * @brief Releases the memory block allocated from the thread-local memory pool.
 *
 * This function is strictly used for releasing memory allocated by thread-local allocators.
 * It cannot be used to release memory allocated from the global memory pool.
 *
 * @param _memory_ptr The pointer to the thread-local memory to release. The pointer is allowed to be nullptr.
 */
CORE_DLLAPI Void ReleaseThreadLocalMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Releases the memory block allocated from the global memory pool.
 *
 * This function is strictly used for releasing memory allocated by global allocators.
 * It cannot be used to release memory allocated from the thread-local memory pool.
 *
 * @param _memory_ptr The pointer to the global memory to release. The pointer is allowed to be nullptr.
 */
CORE_DLLAPI Void ReleaseGlobalMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Applys memory for the smart pointers control block.
 * @return Void* Pointer to the allocated control block memory.
 */
CORE_DLLAPI NODISCARD Void* ApplySmartPointerControlBlockMemory() noexcept;

/**
 * @brief Release the memory by the memory pointer. Only can be used by smart pointers control blocks.
 * @param _memory_ptr The pointer to the memory to release.
 */
CORE_DLLAPI Void ReleaseSmartPointerControlBlockMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Applys memory for log strings.
 * @return Void* Pointer to the allocated log string memory.
 */
CORE_DLLAPI NODISCARD Void* ApplyLogStringMemory() noexcept;

/**
 * @brief Release the memory by the memory pointer. Only can be used by log strings.
 * @param _memory_ptr The pointer to the memory to release.
 */
CORE_DLLAPI Void ReleaseLogStringMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Applys memory for error logs.
 * @return Void* Pointer to the allocated error log memory.
 */
CORE_DLLAPI NODISCARD Void* ApplyErrorLogMemory() noexcept;

/**
 * @brief Release the memory by the memory pointer. Only can be used by error logs.
 * @param _memory_ptr The pointer to the memory to release.
 */
CORE_DLLAPI Void ReleaseErrorLogMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Applys memory for info logs.
 * @return Void* Pointer to the allocated info log memory.
 */
CORE_DLLAPI NODISCARD Void* ApplyInfoLogMemory() noexcept;

/**
 * @brief Release the memory by the memory pointer. Only can be used by info logs.
 * @param _memory_ptr The pointer to the memory to release.
 */
CORE_DLLAPI Void ReleaseInfoLogMemory(Void* _memory_ptr) noexcept;

/**
 * @brief Applys memory for trace logs.
 * @return Void* Pointer to the allocated trace log memory.
 */
CORE_DLLAPI NODISCARD Void* ApplyTraceLogMemory() noexcept;

/**
 * @brief Release the memory by the memory pointer. Only can be used by trace logs.
 * @param _memory_ptr The pointer to the memory to release.
 */
CORE_DLLAPI Void ReleaseTraceLogMemory(Void* _memory_ptr) noexcept;

}//memory_pool
}//zengine