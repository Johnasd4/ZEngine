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

#include <cassert>

// Windows (MSVC, MinGW, Cygwin)
#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef DLLEXPORT
/** @brief Exports a symbol from a dynamic link library (DLL) on Windows. */
#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
/** @brief Imports a symbol from a dynamic link library (DLL) on Windows. */
#define DLLIMPORT __declspec(dllimport)
#endif

// Linux / macOS (GCC, Clang)
#else
#ifndef DLLEXPORT
#if __GNUC__ >= 4 || defined(__clang__)
/** @brief Exports a symbol with default visibility on GCC/Clang. */
#define DLLEXPORT __attribute__((visibility("default")))
#else
/** @brief Defines an empty export macro for older compilers. */
#define DLLEXPORT
#endif
#endif
#ifndef DLLIMPORT
/** @brief Imports a symbol on Linux/Unix (usually empty or default visibility). */
#define DLLIMPORT
#endif
#endif

#ifndef NODISCARD
/** @brief Indicates that the return value of a function should not be discarded. */
#define NODISCARD [[nodiscard]]
#endif

#ifndef FORCEINLINE
// MSVC
#if defined(_MSC_VER)
/** @brief Forces the compiler to inline the function (MSVC specific). */
#define FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Forces the compiler to inline the function (GCC/Clang specific). */
#define FORCEINLINE inline __attribute__((always_inline))
// Fallback
#else
/** @brief Fallback to standard inline if force inlining is not supported. */
#define FORCEINLINE inline
#endif
#endif

#ifndef NOINLINE
// MSVC
#if defined(_MSC_VER)
/** @brief Forces the compiler NOT to inline the function (MSVC specific). */
#define NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Forces the compiler NOT to inline the function (GCC/Clang specific). */
#define NOINLINE __attribute__((noinline))
// Fallback
#else
/** @brief Fallback to nothing if no-inline is not supported. */
#define NOINLINE
#endif
#endif

#ifndef ALIGN
// MSVC (Visual Studio)
#if defined(_MSC_VER)
/** @brief Aligns a type or variable to the specified byte boundary (MSVC). */
#define ALIGN(_align_bytes) __declspec(align(_align_bytes))

// GCC / Clang
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Aligns a type or variable to the specified byte boundary (GCC/Clang). */
#define ALIGN(_align_bytes) __attribute__((aligned(_align_bytes)))

// C++11 Standard Fallback
#elif __cplusplus >= 201103L
/** @brief Aligns a type or variable using the C++11 alignas keyword. */
#define ALIGN(_align_bytes) alignas(_align_bytes)

// Fallback
#else
/** @brief Empty macro if alignment is not supported. */
#define ALIGN(_align_bytes)
#endif
#endif

/** @brief Defines the cache line size for alignment purposes. */
#define CACHE_LINE_SIZE 64
/** @brief Macro to align data structures to cache line size. */
#define CACHE_ALIGN ALIGN(CACHE_LINE_SIZE)
/**
 * @brief Aligns to the machine word boundary (CPU native register width).
 * 4 bytes on 32-bit systems, 8 bytes on 64-bit systems.
 * This ensures single-cycle read/write atomicity for pointers and size_t.
 */
#define WORD_ALIGN ALIGN(alignof(Void*))

/** @brief Aligns to 16 bytes for SIMD operations. */
#define SIMD_ALIGN ALIGN(16)

#ifndef STATIC_ASSERT
/** @brief Compile-time assertion using C++11 static_assert. */
#define STATIC_ASSERT(_condition, _message) static_assert(_condition, _message)
#endif

/** @brief Assertion macro for runtime checks. */
#ifndef ASSERT
#define ASSERT(cond, msg) assert((cond) && (msg))
#endif

/** @brief Macro to disable heap allocation for a class by deleting */
#ifndef NO_NEW_DELETE
#define NO_NEW_DELETE \
public: \
    static FORCEINLINE Void* operator new(SizeType _size, Void* _ptr) noexcept { return _ptr; } \
private: \
    static Void* operator new(SizeType) = delete; \
    static Void operator delete(Void*) = delete;
#endif
