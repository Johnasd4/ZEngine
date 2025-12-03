/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define MATH_DLLFILE
#include "drive/d_pch.h"

#include "f_basic_math/f_basic_math_random.h"

namespace zengine {
namespace math {
namespace internal {

class RandManager {
public:
    static Void RegisterRefillFunc(Void(*_refill_func)()) {
        static RandManager& instance = Instance();
        TLockGuard<ZMutex> lock_guard(instance.refill_func_array_mutex_);
        instance.refill_func_array_.PushBack(_refill_func);
    }

private:
    RandManager(const RandManager&) = delete;
    RandManager(RandManager&&) = delete;
    RandManager& operator=(const RandManager&) = delete;
    RandManager& operator=(RandManager&&) = delete;

    NODISCARD static RandManager& Instance() noexcept {
        static RandManager rand_manager;
        return rand_manager;
    }

    RandManager() noexcept
        : refill_func_array_()
        , refill_func_array_mutex_()
        , refill_timer_() 
    {
        ReturnType link_code = kOK;
        refill_timer_.SetIntervalMs(100);
        refill_timer_.SetTickFunc(RefillTimerTickFunc);
        link_code = refill_timer_.Start();
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFBasicMathRandom_LinkError, link_code, L"ZTimer::Start() link error!");
            return;
        }
    };

    ~RandManager() noexcept {}

    NODISCARD static Void RefillTimerTickFunc() noexcept {
        static TArray<Void(*)()>& manager_refill_func_array = RandManager::Instance().refill_func_array_;
        static ZMutex& manager_refill_func_array_mutex = RandManager::Instance().refill_func_array_mutex_;
        static TArray<Void(*)()> refill_func_array;

        //add the new refill func
        if (!manager_refill_func_array.Empty()) {
            manager_refill_func_array_mutex.Lock();
            for (
                auto refill_func = manager_refill_func_array.Begin()
                ; refill_func != manager_refill_func_array.End()
                ; ++refill_func
            ) {
                refill_func_array.PushBack(*refill_func);
            }
            manager_refill_func_array.Clear();
            manager_refill_func_array_mutex.Unlock();
        }

        //refill the rand pool
        for (auto refill_func = refill_func_array.Begin(); refill_func != refill_func_array.End(); ++refill_func) {
            (*refill_func)();
        }
    }

    TArray<Void(*)()> refill_func_array_;
    ZMutex refill_func_array_mutex_;
    ZTimer refill_timer_;
};

template<typename _NumberType, typename _RandFunction>
class RandPool {
public:
    static inline constexpr SizeType kDefaultPoolSize = 1024;
    static inline constexpr SizeType kArrayExtendMultFactor = 2;
    static inline constexpr Float32 kArrayExtendLimitFactor = 0.5;

    NODISCARD static _NumberType Apply() noexcept {
        static RandPool& instance = Instance();
        TLockGuard<ZMutex> lock_guard(instance.apply_mutex_);
        if (++instance.apply_num_ >= instance.pool_array_.Size()) {
            instance.ExtendP();
        }
        return instance.pool_array_[instance.current_index_++ % instance.pool_array_.Size()];
    }

    NODISCARD static Void RefillTimerFunc() noexcept {
        RandPool& instance = Instance();

        SizeType begin_index;
        SizeType end_index;
        Bool if_resize = false;
        SizeType apply_num;
        SizeType array_size = instance.pool_array_.Size();

        {
            TLockGuard<ZMutex> lock_guard(instance.apply_mutex_);
            apply_num = instance.apply_num_;
            instance.apply_num_ = 0;
            end_index = instance.current_index_;
        }

        if (apply_num == 0) {
            return;
        }

        if (apply_num >= array_size * kArrayExtendLimitFactor) {
            if_resize = true;
            TArray<_NumberType> new_pool_array;
            new_pool_array.Resize(array_size * 2);
            RefillP(&new_pool_array, 1, 0);
            //reset the pool
            {
                TLockGuard<ZMutex> lock_guard(instance.apply_mutex_);
                instance.pool_array_ = std::move(new_pool_array);
                instance.current_index_ = 0;
            }
                instance.refill_index_ = 0;
        }
        else {
            begin_index = instance.refill_index_;
            instance.refill_index_ = end_index;
            RefillP(&instance.pool_array_, begin_index, end_index);
        }
    }

private:
    NODISCARD static RandPool& Instance() noexcept {
        static RandPool rand_pool;
        return rand_pool;
    }

    RandPool() noexcept
        : refill_index_(0)
        , current_index_(0)
        , apply_num_(0)
        , pool_array_()
        , apply_mutex_()
    {
        pool_array_.Resize(kDefaultPoolSize);
        RefillP(&pool_array_, 1ULL, 0ULL);
        RandManager::RegisterRefillFunc(RefillTimerFunc);
    }

    static Void RefillP(TArray<_NumberType>* _rand_array_ptr, SizeType _begin_index, SizeType _end_index) noexcept {
        SizeType size = _rand_array_ptr->Size();
        SizeType index = _begin_index;
        //refill size always > 0
        do {
            (*_rand_array_ptr)[index] = _RandFunction::GenerateRandNum();
            index = ++index % size;
        } while (index != _end_index);
        (*_rand_array_ptr)[index] = _RandFunction::GenerateRandNum();
    }

    Void ExtendP() noexcept {
        pool_array_.Resize(pool_array_.Size() * 2);
        RefillP(&pool_array_, 1, 0);
        current_index_ = 0;
        refill_index_ = 0;
    }

    SizeType refill_index_;
    SizeType current_index_;
    Int32 apply_num_;
    TArray<_NumberType> pool_array_;
    ZMutex apply_mutex_;
};

NODISCARD static const std::random_device::result_type GetRandSeed() noexcept {
    static std::random_device::result_type rand_seed = std::invoke([]() {
        std::random_device rand_seed_generator;
        return rand_seed_generator();
    });
    return rand_seed;
}

struct RandInt32P {
    NODISCARD FORCEINLINE static Int32 GenerateRandNum() noexcept {
        static std::mt19937 rand_device(GetRandSeed());
        static std::uniform_int_distribution<Int32> rand_genrator(0, kInt32Max);
        return rand_genrator(rand_device);
    }
};

struct RandInt64P {
    NODISCARD FORCEINLINE static Int64 GenerateRandNum() noexcept {
        static std::mt19937_64 rand_device(GetRandSeed());
        static std::uniform_int_distribution<Int64> rand_genrator(0, kInt64Max);
        return rand_genrator(rand_device);
    }
};

struct RandFloat32P {
    NODISCARD FORCEINLINE static Float32 GenerateRandNum() noexcept {
        static std::mt19937 rand_device(GetRandSeed());
        static std::uniform_real_distribution<Float32> rand_genrator(0.0f, 1.0f);
        return rand_genrator(rand_device);
    }
};

struct RandFloat64P {
    NODISCARD FORCEINLINE static Float64 GenerateRandNum() noexcept {
        static std::mt19937_64 rand_device(GetRandSeed());
        static std::uniform_real_distribution<Float64> rand_genrator(0.0, 1.0);
        return rand_genrator(rand_device);
    }
};

}//internal
}//math
}//zengine

namespace zengine {
namespace math {

MATH_DLLAPI NODISCARD const Int32 RandInt32(const Int32 _lower_bound, const Int32 _upper_bound) noexcept {
    return _lower_bound + (internal::RandPool<Int32, internal::RandInt32P>::Apply() % (_upper_bound - _lower_bound + 1));
}

MATH_DLLAPI NODISCARD const Int64 RandInt64(const Int64 _lower_bound, const Int64 _upper_bound) noexcept {
    return _lower_bound + (internal::RandPool<Int64, internal::RandInt64P>::Apply() % (_upper_bound - _lower_bound + 1));
}

MATH_DLLAPI NODISCARD const Float32 RandFloat32(const Float32 _lower_bound, const Float32 _upper_bound) noexcept {
    return _lower_bound + (internal::RandPool<Float32, internal::RandFloat32P>::Apply() * (_upper_bound - _lower_bound));
}

MATH_DLLAPI NODISCARD const Float64 RandFloat64(const Float64 _lower_bound, const Float64 _upper_bound) noexcept {
    return _lower_bound + (internal::RandPool<Float64, internal::RandFloat64P>::Apply() * (_upper_bound - _lower_bound));
}

}//math
}//zengine