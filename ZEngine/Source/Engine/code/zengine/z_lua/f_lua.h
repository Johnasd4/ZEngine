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
#pragma once

#include "drive.h"

#include "../z_core/m_log.h"
#include "../z_core/z_string.h"

namespace zengine {
namespace error_code {
enum FLuaErrorCode : ReturnType {
    kFLuaErrorCode_LinkError = kErrorCodeBase_FLua,
    kFLuaErrorCode_SystemError,
    kFLuaErrorCode_NullptrParam,
    kFLuaErrorCode_ParamOutOfRange,
    kFLuaErrorCode_LuaFunctionNotExist,
    kFLuaErrorCode_LuaFileNotLoaded,
    kFLuaErrorCode_LuaCrash,
    kFLuaErrorCode_LuaObjectTypeWrongOrNotExist,
    kFLuaErrorCode_LuaTableNotExist,
    kFLuaErrorCode_LuaLogError
};
}//error_code
}//zengine

namespace zengine {
namespace lua { 

using LuaHandle = sol::state;
using LuaResult = sol::unsafe_function_result;
using LuaSafeResult = sol::protected_function_result;
using LuaFunction = sol::unsafe_function;
using LuaSafeFunction = sol::safe_function;
using LuaObject = sol::object;
using LuaTable = sol::table;

constexpr sol::lua_nil_t kLuaNull = sol::nil;

/*
    Gets the lua state, each thread gets a different state.
*/
LUA_DLLAPI NODISCARD LuaHandle& Lua() noexcept;

/*
    If lua object valid.
*/
template<typename LuaObject>
NODISCARD FORCEINLINE Bool Valid(const LuaObject& _obj) noexcept { return _obj.valid(); }

/*
    Runs the given lua script.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaScript(const Char* _script) noexcept;
/*
    Runs the given lua script.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaScript(LuaResult* _result_ptr, const Char* _script) noexcept;

/*
    Runs the given lua script safely.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaScriptSafe(const Char* _script) noexcept;
/*
    Runs the given lua script safely.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaScriptSafe(LuaResult* _result_ptr, const Char* _script) noexcept;

/*
    Run lua file.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaFile(const Char* _path_dir) noexcept;
/*
    Run lua file.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaFile(LuaResult* _result_ptr, const Char* _path_dir) noexcept;

/*
    Run lua file safe.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaFileSafe(const Char* _path_dir) noexcept;

/*
    Run lua file safe.
*/
LUA_DLLAPI NODISCARD ReturnType RunLuaFileSafe(LuaSafeResult* _result_ptr, const Char* _path_dir) noexcept;

/*
    Load lua file.
*/
LUA_DLLAPI NODISCARD ReturnType LoadLuaFile(const Char* _path_dir) noexcept;

/*
    Unload lua file.
*/
LUA_DLLAPI NODISCARD Void UnloadLuaFile(const Char* _path_dir) noexcept;

/*
    Run the loaded file.
*/
LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFile(const Char* _path_dir) noexcept;
/*
    Run the loaded file.
*/
LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFile(LuaResult* _result_ptr, const Char* _path_dir) noexcept;

/*
    Run the loaded file safe.
*/
LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFileSafe(const Char* _path_dir) noexcept;
/*
    Run the loaded file safe.
*/
LUA_DLLAPI NODISCARD ReturnType RunLoadedLuaFileSafe(LuaSafeResult* _result_ptr, const Char* _path_dir) noexcept;


/*
    Gets the lua table.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaTable(const Char* _table_name, LuaTable* _table_ptr) noexcept;

/*
    Gets the lua function by name, the program will crash if an error happens, fast but unsafe.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaFunction(const Char* _func_name, LuaFunction* _func_ptr) noexcept;

/*
    Gets the lua safe function by name, the program won't crash if an error happens.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunction(const Char* _func_name, LuaSafeFunction* _func_ptr) noexcept;

/*
    Gets the lua object.
*/
template<typename _ObjectType>
NODISCARD FORCEINLINE ReturnType GetLuaObject(const Char* _obj_name, _ObjectType* _obj_ptr) noexcept {
    ReturnType ret_val = kOK;
    LuaObject obj = Lua()[_obj_name];
    if (obj.is<_ObjectType>()) {
        *_obj_ptr = obj.as<_ObjectType>();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaObjectTypeWrongOrNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua object type wrong or not exist!");
    }
    return ret_val;
}

/*
    Gets the lua table from lua table.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaTableFromLuaTable(
    const LuaTable& _table,
    const Char* _table_name,
    LuaTable* _table_ptr) noexcept;

/*
    Gets the lua table from lua table.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaTableFromLuaTable(
    const LuaTable& _table,
    SizeType _index,
    LuaTable* _table_ptr) noexcept;


/*
    Gets the lua function by name from the lua table, the program will crash if an error happens, fast but unsafe.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaFunctionFormLuaTable(
    const LuaTable& _table,
    const Char* _func_name, 
    LuaFunction* _func_ptr) noexcept;

/*
    Gets the lua function by name from the lua table, the program will crash if an error happens, fast but unsafe.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaFunctionFormLuaTable(
    const LuaTable& _table,
    SizeType _index,
    LuaFunction* _func_ptr) noexcept;

/*
    Gets the lua safe function by name from the lua table, the program won't crash if an error happens.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunctionFormLuaTable(
    const LuaTable& _table,
    const Char* _func_name, 
    LuaSafeFunction* _func_ptr) noexcept;

/*
    Gets the lua safe function by name from the lua table, the program won't crash if an error happens.
*/
LUA_DLLAPI NODISCARD ReturnType GetLuaSafeFunctionFormLuaTable(
    const LuaTable& _table,
    SizeType _index,
    LuaSafeFunction* _func_ptr) noexcept;

/*
    Gets the lua object from lua table.
*/
template<typename _ObjectType>
NODISCARD FORCEINLINE ReturnType GetLuaObjectFormLuaTable(
    const LuaTable& _table, 
    const Char* _obj_name, 
    _ObjectType* _obj_ptr
) noexcept {
    ReturnType ret_val = kOK;
    LuaObject obj = _table[_obj_name];
    if (obj.is<_ObjectType>()) {
        *_obj_ptr = obj.as<_ObjectType>();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaObjectTypeWrongOrNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua object type wrong or not exist!");
    }
    return ret_val;
}

/*
    Gets the lua object from lua table.
*/
template<typename _ObjectType>
NODISCARD FORCEINLINE ReturnType GetLuaObjectFormLuaTable(
    const LuaTable& _table,
    SizeType _index,
    _ObjectType* _obj_ptr
) noexcept {
    ReturnType ret_val = kOK;
    LuaObject obj = _table[_index];
    if (obj.is<_ObjectType>()) {
        *_obj_ptr = obj.as<_ObjectType>();
    }
    else {
        ret_val = error_code::kFLuaErrorCode_LuaObjectTypeWrongOrNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Lua object type wrong or not exist!");
    }
    return ret_val;
}

/*
    Call the lua function.
*/
template<typename... _ArgsType>
NODISCARD FORCEINLINE ReturnType CallLuaFunction(
    LuaFunction& _lua_func,
    _ArgsType&&... _args
) noexcept {
    ReturnType ret_val = kOK;
    try {
        _lua_func(std::forward<_ArgsType>(_args)...);
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LuaCrash;
        Z_LOG_ERROR(ret_val, 0, L"Lua functon crashs! Error info: %ls", string::String2WString(e.what()).String());
    }
    return ret_val;
}

/*
    Call the lua function.
*/
template<typename... _ArgsType>
NODISCARD FORCEINLINE ReturnType CallLuaFunction(
    LuaResult* _result_ptr,
    LuaFunction& _lua_func,
    _ArgsType&&... _args
) noexcept {
    ReturnType ret_val = kOK;
    try {
        *_result_ptr = std::move(_lua_func(std::forward<_ArgsType>(_args)...));
    }
    catch (const sol::error& e) {
        ret_val = error_code::kFLuaErrorCode_LuaCrash;
        Z_LOG_ERROR(ret_val, 0, L"Lua functon crashs! Error info: %ls", string::String2WString(e.what()).String());
    }
    return ret_val;
}

/*
    Call the lua safe function.
*/
template<typename... _ArgsType>
NODISCARD FORCEINLINE ReturnType CallLuaSafeFunction(
    LuaSafeFunction& _lua_func,
    _ArgsType&&... _args
) noexcept {
    ReturnType ret_val = kOK;
    LuaSafeResult result = _lua_func(std::forward<_ArgsType>(_args)...);
    if (!result.valid()) {
        ret_val = error_code::kFLuaErrorCode_LuaCrash;
        sol::error e = result;
        Z_LOG_ERROR(ret_val, 0, L"Lua crashs! Error info: %ls", string::String2WString(e.what()).String());
    }
    return ret_val;
}

/*
    Call the lua safe function.
*/
template<typename... _ArgsType>
NODISCARD FORCEINLINE ReturnType CallLuaSafeFunction(
    LuaSafeResult* _result_ptr,
    LuaSafeFunction& _lua_func,
    _ArgsType&&... _args
) noexcept {
    ReturnType ret_val = kOK;
    *_result_ptr = std::move(_lua_func(std::forward<_ArgsType>(_args)...));
    if (!_result_ptr->valid()) {
        ret_val = error_code::kFLuaErrorCode_LuaCrash;
        sol::error e = *_result_ptr;
        Z_LOG_ERROR(ret_val, 0, L"Lua crashs! Error info: %ls", string::String2WString(e.what()).String());
    }
    return ret_val;
}

/*
    Register non-class function to lua.
    Cpy: RegisterCObject("ObjectName", object)
    Ref: RegisterCObject("ObjectName", &object)
*/
template<typename _ObjectType>
NODISCARD FORCEINLINE Void RegisterCObject(const Char* _obj_name, _ObjectType&& _obj) noexcept {
    Lua()[_obj_name] = _obj;
}

/*
    Register non-class function to lua.
*/
template<typename _Function>
NODISCARD FORCEINLINE Void RegisterCFunction(const Char* _func_name, _Function && _func) noexcept {
    Lua()[_func_name] = &_func;
}

/*
    Register class to lua.
    Example:  
    RegisterCClass("ClassName", "Func1Name", &Class::Func1, "Member1Name", &Class::Member1)
*/
template<typename _ClassType, typename... _ArgsType>
NODISCARD FORCEINLINE Void RegisterCClass(const Char* _class_name, _ArgsType&&... _args) noexcept {
    Lua().new_usertype<_ClassType>(_class_name, std::forward<_ArgsType>(_args)...);
}

}//lua
}//zengine