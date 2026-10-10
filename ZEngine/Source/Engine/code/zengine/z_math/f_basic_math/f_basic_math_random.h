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

#include "../drive.h"

#include <random>

namespace zengine {
namespace math {

namespace internal {

/**
 * @brief Retrieves the thread-local instance of the Mersenne Twister random number generator.
 * @return A reference to the thread-local std::mt19937 engine.
 */
NODISCARD FORCEINLINE std::mt19937& GetRandomDeviceInstance() noexcept {
    thread_local std::mt19937 rand_device(std::random_device{}());
    return rand_device;
}

} // internal

/**
 * @brief Generates a non-deterministic random seed using the hardware random device.
 * @return A random 32-bit unsigned integer to be used as a seed.
 */
NODISCARD FORCEINLINE UInt32 GenerateRandSeed() noexcept {
    thread_local std::random_device device;
    return device();
}

/**
 * @brief Sets the seed for the current thread's random number generator engine.
 * @param _rand_seed The seed value to initialize the random engine with.
 */
FORCEINLINE Void SetThreadLocalRandSeed(UInt32 _rand_seed) noexcept {
    internal::GetRandomDeviceInstance().seed(_rand_seed);
}

/**
 * @brief Generates a pseudo-random number within the specified closed range [_lower_bound, _upper_bound].
 * @details This function uses thread-local random engines and distributions for performance.
 *          It handles 1-byte integers by promoting them to Int16 to comply with standard library requirements.
 * @tparam _NumberType The numeric type of the random number (integer or floating-point).
 * @param _lower_bound The inclusive lower bound of the range.
 * @param _upper_bound The inclusive upper bound of the range.
 * @return A random value of type _NumberType between _lower_bound and _upper_bound (inclusive).
 */
template<typename _NumberType>
NODISCARD FORCEINLINE _NumberType Rand(_NumberType _lower_bound, _NumberType _upper_bound) noexcept {
    ASSERT(_lower_bound <= _upper_bound, "Rand: Invalid range!");
    if constexpr (kIsFloat<_NumberType>) {
        thread_local std::uniform_real_distribution<_NumberType> rand_genrator;
        rand_genrator.param(typename std::uniform_real_distribution<_NumberType>::param_type(
            _lower_bound, 
            _upper_bound
        ));
        return rand_genrator(internal::GetRandomDeviceInstance());
    }
    else {
        if constexpr (sizeof(_NumberType) == 1) {
            thread_local std::uniform_int_distribution<Int16> rand_genrator;
            rand_genrator.param(typename std::uniform_int_distribution<Int16>::param_type(
                    static_cast<Int16>(_lower_bound),
                    static_cast<Int16>(_upper_bound)
            ));
            return static_cast<_NumberType>(rand_genrator(internal::GetRandomDeviceInstance()));
        }
        else {
            thread_local std::uniform_int_distribution<_NumberType> rand_genrator;
            rand_genrator.param(typename std::uniform_int_distribution<_NumberType>::param_type(
                _lower_bound, 
                _upper_bound
            ));
            return rand_genrator(internal::GetRandomDeviceInstance());
        }
    }
}

} // math
} // zengine