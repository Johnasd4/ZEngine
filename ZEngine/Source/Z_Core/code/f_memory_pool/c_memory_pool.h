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
namespace memory_pool {

/** @brief Alignment parameter for standard memory pages (exponent 16, representing 64KB). */
inline constexpr UInt32 kMemoryPageAlignmentParam = 16U;
/** @brief The size of standard memory page alignment in bytes (64KB). */
inline constexpr UInt32 kMemoryPageAlignmentSize = 1U << kMemoryPageAlignmentParam;
/** @brief Bitmask calculated from the alignment parameter, used for address alignment operations. */
inline constexpr UInt32 kMemoryPageAlignmentMask = kMemoryPageAlignmentSize - 1U;

/** @brief Alignment parameter for large memory pages (exponent 21, representing 2MB). */
inline constexpr UInt32 kLargeMemoryPageAlignmentParam = 21U;
/** @brief The size of large memory page alignment in bytes (2MB). */
inline constexpr UInt32 kLargeMemoryPageAlignmentSize = 1U << kLargeMemoryPageAlignmentParam;
/** @brief Bitmask calculated from the large alignment parameter, used for large page address alignment. */
inline constexpr UInt32 kLargeMemoryPageAlignmentMask = kLargeMemoryPageAlignmentSize - 1U;

/** @brief The size of a memory atom page in bytes (16KB). */
inline constexpr UInt32 kMemoryAtomPageSize = 16U * kKB;
/** @brief Bitmask calculated from the memory atom page size, used for address alignment operations. */
inline constexpr UInt32 kMemoryAtomPageMask = kMemoryAtomPageSize - 1U;

/** @brief Predefined array of memory block sizes supported by the small memory pool. */
inline constexpr UInt32 kSmallMemoryPoolMemoryBlockSizeArray[] = {
    16U, 32U, 48U, 64U, 
    80U, 96U, 112U, 128U,
    160U, 192U, 224U, 256U,
    320U, 384U, 448U, 512U,
    640U, 768U, 896U, 1024U,
    1280U, 1536U
};

/** @brief Predefined array of memory block sizes (including headers) supported by the medium memory pool. */
inline constexpr UInt32 kMediumMemoryPoolMemoryBlockWithHeadSizeArray[] = {
    2U * kKB, 3U * kKB, 4U * kKB, 6U * kKB,
    8U * kKB, 12U * kKB, 16U * kKB, 24U * kKB,
    32U * kKB, 48U * kKB, 64U * kKB
};

/** @brief Predefined array of memory block sizes (including headers) supported by the large memory pool. */
inline constexpr UInt32 kLargeMemoryPoolMemoryBlockWithHeadSizeArray[] = {
    96U * kKB, 128U * kKB, 
    192U * kKB, 256U * kKB,
    384U * kKB, 512U * kKB, 
    768U * kKB, 1024U * kKB, 
    1536U * kKB, 2048U * kKB
};

/** @brief The number of elements in the small memory pool block size array. */
inline constexpr UInt32 kSmallMemoryPoolNum =
    sizeof(kSmallMemoryPoolMemoryBlockSizeArray) / sizeof(UInt32);
/** @brief The number of elements in the medium memory pool block size array. */
inline constexpr UInt32 kMediumMemoryPoolNum =
    sizeof(kMediumMemoryPoolMemoryBlockWithHeadSizeArray) / sizeof(UInt32);
/** @brief The number of elements in the large memory pool block size array. */
inline constexpr UInt32 kLargeMemoryPoolNum =
    sizeof(kLargeMemoryPoolMemoryBlockWithHeadSizeArray) / sizeof(UInt32);

/** @brief Minimum size of a memory block in the small memory pool. */
inline constexpr UInt32 kSmallMemoryPoolMemoryBlockSizeMin =
    kSmallMemoryPoolMemoryBlockSizeArray[0];
/** @brief Maximum size of a memory block in the small memory pool. */
inline constexpr UInt32 kSmallMemoryPoolMemoryBlockSizeMax =
    kSmallMemoryPoolMemoryBlockSizeArray[kSmallMemoryPoolNum - 1];

/** @brief Minimum size of a memory block in the medium memory pool. */
inline constexpr UInt32 kMediumMemoryPoolMemoryBlockWithHeadSizeMin =
    kMediumMemoryPoolMemoryBlockWithHeadSizeArray[0];
/** @brief Maximum size of a memory block in the medium memory pool. */
inline constexpr UInt32 kMediumMemoryPoolMemoryBlockWithHeadSizeMax =
    kMediumMemoryPoolMemoryBlockWithHeadSizeArray[kMediumMemoryPoolNum - 1];
/** @brief The second to largest size of a memory block in the medium memory pool. */
inline constexpr UInt32 kMediumMemoryPoolMemoryBlockWithHeadSizeMax_1_5 =
    kMediumMemoryPoolMemoryBlockWithHeadSizeArray[kMediumMemoryPoolNum - 2];
/** @brief Alias for the maximum size of a memory block in the medium memory pool. */
inline constexpr UInt32 kMediumMemoryPoolMemoryBlockWithHeadSizeMax_2 =
    kMediumMemoryPoolMemoryBlockWithHeadSizeArray[kMediumMemoryPoolNum - 1];

/** @brief Minimum size of a memory block in the large memory pool. */
inline constexpr UInt32 kLargeMemoryPoolMemoryBlockWithHeadSizeMin =
    kLargeMemoryPoolMemoryBlockWithHeadSizeArray[0];
/** @brief Maximum size of a memory block in the large memory pool. */
inline constexpr UInt32 kLargeMemoryPoolMemoryBlockWithHeadSizeMax =
    kLargeMemoryPoolMemoryBlockWithHeadSizeArray[kLargeMemoryPoolNum - 1];

}//memory_pool
}//zengine