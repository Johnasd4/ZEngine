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
#define LUA_DLLFILE

#include "f_lua.h"

#include "z_core/z_string.h"

namespace zengine {
namespace lua {

namespace internal{

namespace log {

static Void LuaLogError(const Char* _str) noexcept {
    Z_LOG_ERROR(error_code::kFLuaErrorCode_LuaLogError, 0, string::String2WString(_str).String());
}

static Void LuaLogTrace(const Char* _str) noexcept {
    Z_LOG_TRACE(string::String2WString(_str).String());
}

static Void LuaLogMessage(const Char* _str) noexcept {
    Z_LOG_MESSAGE(string::String2WString(_str).String());
}

static Void LuaLogStart(const Char* _str) noexcept {
    Z_LOG_START(string::String2WString(_str).String());
}

static Void LuaLogProcess(const Char* _str) noexcept {
    Z_LOG_PROCESS(string::String2WString(_str).String());
}

static Void LuaLogFinish(const Char* _str) noexcept {
    Z_LOG_FINISH(string::String2WString(_str).String());
}

static Void LuaLogSuccess(const Char* _str) noexcept {
    Z_LOG_SUCCESS(string::String2WString(_str).String());
}

static Void LuaLogFailure(const Char* _str) noexcept {
    Z_LOG_FAILURE(string::String2WString(_str).String());
}

}

class ZLuaCore : public ZObject {
public:
    ZLuaCore() noexcept : SuperType_() {
        RegisterLogFuncP();
    }
    ~ZLuaCore() noexcept {}

    NODISCARD FORCEINLINE LuaHandle& Handle() { return handle_; }

protected:
    using SuperType_ = ZObject;

private:
    Void RegisterLogFuncP() noexcept {
        RegisterCFunction("LogError", log::LuaLogError);
        RegisterCFunction("LogTrace", log::LuaLogTrace);
        RegisterCFunction("LogMessage", log::LuaLogMessage);
        RegisterCFunction("LogStart", log::LuaLogStart);
        RegisterCFunction("LogProcess", log::LuaLogProcess);
        RegisterCFunction("LogFinish", log::LuaLogFinish);
        RegisterCFunction("LogSuccess", log::LuaLogSuccess);
        RegisterCFunction("LogFailure", log::LuaLogFailure);
    }

    ZLuaCore(const ZLuaCore&) = delete;
    ZLuaCore(ZLuaCore&&) = delete;
    ZLuaCore& operator=(const ZLuaCore&) = delete;
    ZLuaCore& operator=(ZLuaCore&&) = delete;

    LuaHandle handle_;
};

}

LUA_DLLAPI NODISCARD LuaHandle& Lua() noexcept {
    thread_local internal::ZLuaCore lua_core;
    thread_local LuaHandle& lua = lua_core.Handle();
    return lua;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaScript(const Char* _script) noexcept {
    ReturnType ret_val = kOK;
    try {
        Lua().unsafe_script(_script);
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Lua().unsafe_script() link error! Error info: %ls", 
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaScript(LuaResult* _result_ptr, const Char* _script) noexcept {
    ReturnType ret_val = kOK;
    try {
        *_result_ptr = std::move(Lua().unsafe_script(_script));
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Lua().unsafe_script() link error! Error info: %ls", 
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaScriptSafe(const Char* _script) noexcept {
    ReturnType ret_val = kOK;
    LuaSafeResult result = Lua().safe_script("user_script()", sol::script_pass_on_error);
    if (!Valid(result)) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        sol::error e = result;
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Lua().safe_script() link error! Error info: %ls", 
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaScriptSafe(LuaResult* _result_ptr, const Char* _script) noexcept {
    ReturnType ret_val = kOK;
    *_result_ptr = std::move(Lua().safe_script("user_script()", sol::script_pass_on_error));
    if (!Valid(*_result_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        sol::error e = *_result_ptr;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Lua().safe_script() link error! Error info: %ls",
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaFile(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        Lua().unsafe_script_file(_path_dir);
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, 0, 
            L"Lua().unsafe_script_file() link error! Error info: %ls", 
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaFile(LuaResult* _result_ptr, const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        *_result_ptr = Lua().unsafe_script_file(_path_dir);
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Lua().unsafe_script_file() link error! Error info: %ls",
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaFileSafe(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    LuaSafeResult result = Lua().safe_script_file(_path_dir, sol::script_pass_on_error);
    if (!Valid(result)) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        sol::error e = result;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Lua().safe_script_file() link error! Error info: %ls",
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLuaFileSafe(LuaSafeResult* _result_ptr, const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    *_result_ptr = std::move(Lua().safe_script_file(_path_dir, sol::script_pass_on_error));
    if (!Valid(*_result_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        sol::error e = *_result_ptr;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Lua().safe_script_file() link error! Error info: %ls",
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType LoadLuaFile(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    sol::load_result result = Lua().load_file(_path_dir);
    if (Valid(result)) {
        Lua()[_path_dir] = result;
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LinkError;
        sol::error e = result;
        Z_LOG_ERROR(
            ret_val, 0,
            L"Lua().load_file() link error! Error info: %ls",
            string::String2WString(e.what()).String());
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD Void UnloadLuaFile(const Char* _path_dir) noexcept {
    Lua()[_path_dir] = kLuaNull;
    Lua().collect_garbage();
}

LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFile(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    LuaFunction func = Lua()[_path_dir];
    if (Valid(func)) {
        func();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaFileNotLoaded;
        Z_LOG_ERROR(ret_val, 0, L"Lua file not loaded!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFile(LuaResult* _result_ptr, const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    LuaFunction func = Lua()[_path_dir];
    if (Valid(func)) {
        *_result_ptr = func();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaFileNotLoaded;
        Z_LOG_ERROR(ret_val, 0, L"Lua file not loaded!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFileSafe(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    LuaSafeFunction func = Lua()[_path_dir];
    if (Valid(func)) {
        func();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaFileNotLoaded;
        Z_LOG_ERROR(ret_val, 0, L"Lua file not loaded!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFileSafe(LuaSafeResult* _result_ptr, const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    LuaSafeFunction func = Lua()[_path_dir];
    if (Valid(func)) {
        *_result_ptr = func();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaFileNotLoaded;
        Z_LOG_ERROR(ret_val, 0, L"Lua file not loaded!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaTable(const Char* _table_name, LuaTable* _table_ptr) noexcept {
    ReturnType ret_val = kOK;
    *_table_ptr = std::move(Lua()[_table_name]);
    if (!Valid(*_table_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaTableNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua table not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaFunction(const Char* _func_name, LuaFunction* _func_ptr) noexcept {
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(Lua()[_func_name]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunction(const Char* _func_name, LuaSafeFunction* _func_ptr) noexcept {
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(Lua()[_func_name]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaTableFromLuaTable(
    const LuaTable& _table,
    const Char* _table_name,
    LuaTable* _table_ptr) noexcept
{
    ReturnType ret_val = kOK;
    *_table_ptr = std::move(_table[_table_name]);
    if (!Valid(*_table_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaTableNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua table not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaTableFromLuaTable(
    const LuaTable& _table,
    IndexType _index,
    LuaTable* _table_ptr) noexcept
{
    ReturnType ret_val = kOK;
    *_table_ptr = std::move(_table[_index]);
    if (!Valid(*_table_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaTableNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua table not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaFunctionFormLuaTable(
    const LuaTable& _table,
    const Char* _func_name,
    LuaFunction* _func_ptr) noexcept 
{
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(_table[_func_name]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaFunctionFormLuaTable(
    const LuaTable& _table,
    IndexType _index,
    LuaFunction* _func_ptr) noexcept
{
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(_table[_index]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunctionFormLuaTable(
    const LuaTable& _table,
    const Char* _func_name,
    LuaSafeFunction* _func_ptr) noexcept 
{
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(_table[_func_name]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunctionFormLuaTable(
    const LuaTable& _table,
    IndexType _index,
    LuaSafeFunction* _func_ptr) noexcept
{
    ReturnType ret_val = kOK;
    *_func_ptr = std::move(_table[_index]);
    if (!Valid(*_func_ptr)) {
        ret_val = error_code::kFLuaErrorCode_LuaFunctionNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua function not exist!");
    }
    return ret_val;
}

}//lua
}//zengine