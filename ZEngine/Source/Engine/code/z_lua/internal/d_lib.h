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

#include "z_core/internal/z_drive.h"

#pragma warning(disable: 26439)
#pragma warning(disable: 26495)
#pragma warning(disable: 26800)

#include "sol/sol.hpp"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"L_Lua_5_4_7_x64_Debug.lib")
#else
#pragma comment(lib,"L_Lua_5_4_7_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"L_Lua_5_4_7_Win32_Debug.lib")
#else
#pragma comment(lib,"L_Lua_5_4_7_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64

//Define if at the start of the z_lua cpp file.
#ifdef LUA_DLLFILE
#define LUA_DLLAPI DLLEXPORT
#define PROJECT_NAME L"ZLUA"
#else
#define LUA_DLLAPI DLLIMPORT
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"Z_Lua_x64_Debug.lib")
#else
#pragma comment(lib,"Z_Lua_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"Z_Lua_Win32_Debug.lib")
#else
#pragma comment(lib,"Z_Lua_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#endif//LUA_DLLFILE