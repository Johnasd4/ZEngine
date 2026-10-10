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
#define LUA_DLLFILE
#include "drive/d_pch.h"

#include "z_lua_state_view.h"

namespace zengine {
namespace lua {

NODISCARD ZLuaResult ZLuaStateView::ExecuteScript(ZStringView _script) noexcept {
    return ZLuaResult(state_.safe_script(
        _script.GetSTDStringView(), 
        [](RawLuaState* _raw_lua_state_ptr, sol::protected_function_result _result) {
            Z_LOG_ERROR(
                error_code::kZLuaErrorCode_SystemOrLibraryError, 0,
                "Failed to execute script! err_msg: \n{}",
                _result.get<sol::error>().what()
            );
            return _result;
        }
    ));
}

NODISCARD ZLuaResult ZLuaStateView::ExecuteFile(ZStringView _file_path) noexcept {
    return ZLuaResult(state_.safe_script_file(
        std::string(_file_path.GetSTDStringView()),
        [](RawLuaState* _raw_lua_state_ptr, sol::protected_function_result _result) {
            Z_LOG_ERROR(
                error_code::kZLuaErrorCode_SystemOrLibraryError, 0,
                "Failed to execute file! err_msg: \n{}",
                _result.get<sol::error>().what()
            );
            return _result;
        }
    ));
}

NODISCARD ZLuaFunction ZLuaStateView::LoadScript(ZStringView _script) noexcept {
    auto load_result = state_.load(_script.GetSTDStringView());
    if(!load_result.valid()) {
        Z_LOG_ERROR(
            error_code::kZLuaErrorCode_SystemOrLibraryError, 0, 
            "Failed to load script! err_msg: \n{}", 
            load_result.get<sol::error>().what()
        );
        return ZLuaFunction();
    }
    return ZLuaFunction(load_result);
}

NODISCARD ZLuaFunction ZLuaStateView::LoadFile(ZStringView _file_path) noexcept {
    auto load_result = state_.load_file(std::string(_file_path.GetSTDStringView()));
    if (!load_result.valid()) {
        Z_LOG_ERROR(
            error_code::kZLuaErrorCode_SystemOrLibraryError, 0,
            "Failed to load file! _file_path: {}, err_msg: \n{}",
            _file_path.GetSTDStringView(),
            load_result.get<sol::error>().what()
        );
        return ZLuaFunction();
    }
    return ZLuaFunction(load_result);
}

Void ZLuaStateView::CollectGarbageByTimeMs(TimeType _time_ms) noexcept {
    TimeType finish_time = ZSystemTime::GetTimeMs() + _time_ms;
    while (ZSystemTime::GetTimeMs() < finish_time) {
        // Perform a small step (e.g., step size roughly corresponds to KB scanned)
        // LUA_GCSTEP returns 1 if the cycle is finished, 0 otherwise.
        // We use a small step size (e.g., 2) to allow frequent time checks.
        if (lua_gc(state_.lua_state(), LUA_GCSTEP, 2)) {
            break;
        }
    }
}

} // lua
} // zengine