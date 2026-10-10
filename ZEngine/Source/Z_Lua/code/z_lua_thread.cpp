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

#include "z_lua_thread.h"

namespace zengine {
namespace lua {

NODISCARD ZString ZLuaThread::ToString() const noexcept {
    if (thread_.valid()) {
        return ZString();
    }

    lua_State* lua_state = thread_.lua_state();

    // check lua_state validity
    if (!lua_state) return ZString("error_state");

    Int32 top = lua_gettop(lua_state);

    // push the object onto the stack
    thread_.push();

    SizeType len = 0;
    // call luaL_tolstring to convert the object at the top of the stack to a string
    const Char* str = luaL_tolstring(lua_state, -1, &len);

    // construct ZString from the returned C-string and length
    ZString result(str, len);

    // clear the two values from the stack (the string and the original object)
    lua_settop(lua_state, top);

    return result;
}

} // lua
} // zengine