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

#include "z_lua_state.h"

namespace zengine {
namespace lua {
namespace internal {

template<Bool _IfGlobal>
static Void* LuaGlobalAllocator(Void*, Void* _mem_ptr, SizeType _old_size, SizeType _new_size) {
    // release the memory pool associated with the Lua state
    if (_new_size == 0) {
        if (_mem_ptr != nullptr) {
            memory_pool::ReleaseMemory<_IfGlobal>(_mem_ptr);
        }
        return nullptr;
    }

    return memory_pool::ReapplyMemory<_IfGlobal>(_mem_ptr, _new_size);
}

static RawLuaState* CreateLuaStateRaw(Bool _use_global_memory_pool) {
    RawLuaState* raw_state_ptr;
    if (_use_global_memory_pool) {
        raw_state_ptr = lua_newstate(LuaGlobalAllocator<true>, nullptr);
    }
    else {
        raw_state_ptr = lua_newstate(LuaGlobalAllocator<false>, nullptr);
    }
    return raw_state_ptr;
   
}

} // internal
} // lua
} // zengine

namespace zengine {
namespace lua {

ZLuaState::ZLuaState(Bool _use_global_memory_pool) noexcept 
    : SuperType_(internal::CreateLuaStateRaw(_use_global_memory_pool))
{}

ZLuaState::~ZLuaState() noexcept {
    RawLuaState* raw_state_ptr = state_.lua_state();
    if (raw_state_ptr != nullptr) {
        lua_close(raw_state_ptr);
    }
}

NODISCARD ReturnType ZLuaState::Initialize() noexcept {
    ReturnType link_code = kOK;

    link_code = InitializeStandardLibrariesP();
    if (link_code != kOK) {
        return link_code;
    }

    link_code = InitializePanicHandlerP();
    if (link_code != kOK) {
        return link_code;
    }

    link_code = InitializeErrorHandlerP();
    if (link_code != kOK) {
        return link_code;
    }

    link_code = InitializeLogFunctionP();
    if (link_code != kOK) {
        return link_code;
    }

    return kOK;
}

NODISCARD ReturnType ZLuaState::InitializeStandardLibrariesP() noexcept {
    ReturnType ret_val = kOK;

    // open standard libraries
    try {
        state_.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::table,
            sol::lib::string,
            sol::lib::math,
            sol::lib::io,
            sol::lib::os,
            sol::lib::coroutine,
            sol::lib::debug
        );
    }
    catch (const std::exception& error) {
        ret_val = error_code::kZLuaErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(ret_val, 0, "System error! error_msg: {}", error.what());
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZLuaState::InitializePanicHandlerP() noexcept {
    ReturnType ret_val = kOK;

    state_.set_panic([](RawLuaState* _lua_state_ptr) -> Int32 {
        // get the error message from the top of the stack
        const Char* error_msg = lua_tostring(_lua_state_ptr, -1);
        if (error_msg == nullptr) {
            error_msg = "Unknown Lua Panic Error";
        }

        Z_LOG_ERROR(
            error_code::kZLuaErrorCode_LuaPanic, 0,
            "Lua Panic occurred! error_msg:\n{}",
            error_msg
        );

        std::abort(); // terminate the program

        return 0; 
    });

    return ret_val;
}

NODISCARD ReturnType ZLuaState::InitializeErrorHandlerP() noexcept {
    ReturnType ret_val = kOK;

    state_.safe_script(R"(
        function __err_handler__(msg)
            return "Lua Error: " .. tostring(msg) .. "\n" .. debug.traceback()
        end
    )");
    sol::function handler = state_["__err_handler__"];
    sol::protected_function::set_default_handler(handler);

    return ret_val;
}

NODISCARD ReturnType ZLuaState::InitializeLogFunctionP() noexcept {
    ReturnType ret_val = kOK;

    state_.set_function(
        "__z_log_error__", 
        [](
            std::string_view _file_path,
            std::string_view _func_name,
            Int32 _err_line,
            ReturnType::ValueType _link_code,
            std::string_view _msg
        ) {
            log::LogError(
                zengine::ZSystemTime::GetTimeNs(),
                PROJECT_NAME,
                _file_path,
                _func_name,
                _err_line,
                error_code::kZLuaErrorCode_LuaError,
                _link_code,
                _msg
            );
        }
    );

    state_.set_function(
        "__z_log_trace__", 
        [](
            std::string_view _file_path,
            std::string_view _func_name,
            Int32 _trace_line,
            std::string_view _msg
        ) {
            log::LogTrace(
                zengine::ZSystemTime::GetTimeNs(),
                PROJECT_NAME,
                _file_path,
                _func_name,
                _trace_line,
                _msg
            );
        }
    );

    state_.set_function("Z_LOG_MESSAGE", [](std::string_view _msg) {Z_LOG_MESSAGE(_msg); });
    state_.set_function("Z_LOG_START", [](std::string_view _msg) {Z_LOG_START(_msg); });
    state_.set_function("Z_LOG_PROCESS", [](std::string_view _msg) {Z_LOG_PROCESS(_msg); });
    state_.set_function("Z_LOG_FINISH", [](std::string_view _msg) {Z_LOG_FINISH(_msg); });
    state_.set_function("Z_LOG_SUCCESS", [](std::string_view _msg) {Z_LOG_SUCCESS(_msg); });
    state_.set_function("Z_LOG_FAILURE", [](std::string_view _msg) {Z_LOG_FAILURE(_msg); });

    state_.safe_script(R"(
        function Z_LOG_ERROR(_error_code, _msg)
            local info = debug.getinfo(2, "Snl")                
            local src = "local script" 
            if info and info.source then
                local s = info.source
                if string.sub(s, 1, 1) == "@" then
                    src = string.sub(s, 2)
                end
            end

            local func_name = (info and info.name) or "unknown"
            local line = (info and info.currentline) or -1
            local msg_str = tostring(_msg)

            __z_log_error__(
                src,
                func_name,
                line,
                _error_code,
                msg_str
            )
        end

        function Z_LOG_TRACE(_msg)
            local info = debug.getinfo(2, "Snl")                
            local src = "local script" 
            if info and info.source then
                local s = info.source
                if string.sub(s, 1, 1) == "@" then
                    src = string.sub(s, 2)
                end
            end

            local func_name = (info and info.name) or "unknown"
            local line = (info and info.currentline) or -1
            local msg_str = tostring(_msg)

            __z_log_trace__(
                src,
                func_name,
                line,
                msg_str
            )
        end
    )");

    return ret_val;
}

} // lua
} // zengine