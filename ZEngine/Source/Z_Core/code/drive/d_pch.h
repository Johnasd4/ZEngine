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

#include "drive.h"

/* Link Library */

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"L_WhereAmI_x64_Debug.lib")
#pragma comment(lib,"L_Simdutf_x64_Debug.lib")
#pragma comment(lib,"L_Fmt_11_1_0_x64_Debug.lib")
#else
#pragma comment(lib,"L_WhereAmI_x64_Release.lib")
#pragma comment(lib,"L_Simdutf_x64_Release.lib")
#pragma comment(lib,"L_Fmt_11_1_0_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"L_WhereAmI_Win32_Debug.lib")
#pragma comment(lib,"L_Simdutf_Win32_Debug.lib")
#pragma comment(lib,"L_Fmt_11_1_0_Win32_Debug.lib")
#else
#pragma comment(lib,"L_WhereAmI_Win32_Release.lib")
#pragma comment(lib,"L_Simdutf_Win32_Release.lib")
#pragma comment(lib,"L_Fmt_11_1_0_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64

#ifdef _WIN32
#pragma comment(lib,"onecore.lib")
#pragma comment(lib,"DbgHelp.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Userenv.lib")
#pragma comment(lib,"Psapi.lib")
#endif

#pragma comment(lib,"libuv_x64_Release.lib")

/* Standard Library */

#include <iostream>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <memoryapi.h>
#include <shobjidl.h> //filesystem
#else
#include <sys/mman.h> // Linux/macOS mmap
#endif

/* Third Party Isolate Library */

#include <simdutf/simdutf.h>
#include <whereami/whereami.h>
#include <uv.h>

/* Third Party Include Library */

#include "internal/l_fmt.h"
#include "internal/l_rapidjson.h"

/* ZEngine Include */