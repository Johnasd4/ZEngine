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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_recursive_mutex.h"

#pragma warning(disable: 4834)

namespace zengine {

NODISCARD Bool ZRecursiveMutex::TryLockUntilMs(TimeType _abs_time_ms) noexcept {
    const ZThread::ThreadIDType_ current_thread_id = ZThread::GetCurrentThreadID();

    // Fast Path (Re-entrant)
    if (owner_thread_id_.Load(ZMemoryOrderEnum::kRelaxed) == current_thread_id) {
        ++count_;
        return true;
    }

    // Attempt timed wait on underlying mutex
    if (mutex_.TryLockUntilMs(_abs_time_ms)) {
        owner_thread_id_.Store(current_thread_id, ZMemoryOrderEnum::kRelaxed);
        count_ = 1U;
        return true;
    }

    return false;
}

Void ZRecursiveMutex::LockSlowPathP(const ZThread::ThreadIDType_ _current_thread_id) noexcept {
    mutex_.Lock();
    owner_thread_id_.Store(_current_thread_id, ZMemoryOrderEnum::kRelaxed);
    count_ = 1U;
}

} // zengine

#pragma warning(default: 4834)