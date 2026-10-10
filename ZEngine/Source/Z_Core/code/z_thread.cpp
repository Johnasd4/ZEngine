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

#include "z_thread.h"

namespace zengine {

NODISCARD ZThread::ThreadIDType_ ZThread::GetCurrentThreadID() noexcept { return std::this_thread::get_id(); }

NODISCARD ZThread::ThreadIDType_ ZThread::GetMainThreadID() noexcept {
    static ThreadIDType_ id = std::this_thread::get_id();
    return id; 
}

NODISCARD Bool ZThread::IsMainThread() noexcept {
    return GetCurrentThreadID() == GetMainThreadID();
}

} // zengine
